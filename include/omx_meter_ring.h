/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
 *
 * omx_meter_ring.h — the values of a plugin's meters, handed from the audio thread to any number of
 * readers so that one read never mixes two blocks: what clap.meters/1's CLAP_METER_IS_COHERENT
 * promises (free-audio/clap#521). Plain C11, header only, no allocation, no lock; the audio thread
 * and every reader finish in a bounded number of steps.
 *
 * The recipe the draft's header documents: three copies of the value array and one atomic word
 * naming the current copy. After each block the audio thread fills a copy no reader can hold and
 * then stores the word; a reader loads the word and copies that array. One step more makes the copy
 * provably whole: the reader loads the word again after copying and keeps the copy only if the
 * audio thread published at most once in between.
 *
 * WHY THAT CHECK IS ENOUGH (the three-copy argument). The word carries a publication count `s` and
 * the copy `c` that publication filled; copies are filled in turn, 0 1 2 0 1 2 …, so publication
 * s + k fills copy (c + k) mod 3, and the first one to write copy `c` again is s + 3. The audio
 * thread starts filling for s + 3 only after it stored the word of s + 2 (program order, and the
 * release fence at the start of publish() keeps that store ahead of the fill for every reader that
 * sees any value of the fill). A reader that saw word s, copied, issued an acquire fence and then
 * loaded word s' <= s + 1 has therefore not seen any store of s + 3 or later: if it had, the fence
 * pair would make the word of s + 2 visible to its second load. And it sees every value of s, which
 * the acquire load of the word ordered before its copy. So an accepted copy is exactly publication
 * s, whole. With two copies the overwrite would start after s + 1 and no publication at all could
 * land during a read; with three the reader survives one whole publication and a second one in
 * progress, which is two blocks of slack for a copy of a few doubles.
 *
 * THE HELD ACCUMULATOR (spec 7.9: the plugin holds, the reader recovers). Beside each value the audio
 * thread keeps its accumulator: the largest value published since the reader's last take() (the deepest,
 * i.e. the smallest, for a value the policy marks as a gain reduction), and for a clip word the longest run.
 * The accumulator travels in the same copy as the value, so a take is as whole as a read. The reader says
 * what it has seen by storing, for each value it took, the count of the copy; the audio thread, at its next
 * block, restarts a value's accumulator from the block's own value when that count is the last one it
 * published (one compare per value), and folds the block in otherwise. Taking a meter takes only its values. A block published between the reader's copy
 * and its store is therefore held once more: the error is one-sided, a burst is never lost, and a slow
 * reader sees a value a block too high at worst. ONE reader per instance may take(); read() stays for any
 * number of observers and takes nothing.
 *
 * A reader that finds s' >= s + 2 (it was preempted across two blocks) copies again, at most
 * OMX_METER_RING_TRIES times, and then reports nothing rather than a torn copy: both sides stay
 * wait-free. The count is 30 bits and skips 0 on wrapping, so 0 can mean "nothing published"; a
 * reader stalled for 2^30 blocks (hours, at the smallest block at the highest rate) could in theory
 * be fooled, and no other reader can.
 */
#ifndef OMX_METER_RING_H
#define OMX_METER_RING_H

#include <stdatomic.h>
#include <stdint.h>
#include <string.h>

/** The most values one ring carries: every channel of every meter of one plugin. */
#ifndef OMX_METER_RING_MAX
#define OMX_METER_RING_MAX 16u
#endif
/** How many times a reader copies before it gives up on a read (see above). */
#ifndef OMX_METER_RING_TRIES
#define OMX_METER_RING_TRIES 4u
#endif
#define OMX_METER_RING_COPIES 3u
/** A copy holds the values, then their held accumulators, then their clip words. */
#define OMX_METER_RING_HELD (OMX_METER_RING_MAX)
#define OMX_METER_RING_CLIP (2u * OMX_METER_RING_MAX)
#define OMX_METER_RING_STRIDE (3u * OMX_METER_RING_MAX)
#define OMX_METER_RING_COUNT_MASK 0x3fffffffu

_Static_assert(ATOMIC_INT_LOCK_FREE == 2 && ATOMIC_LLONG_LOCK_FREE == 2, "the ring needs lock-free atomics");
_Static_assert(sizeof(double) == sizeof(unsigned long long), "a value travels as its 64 bits");

struct omx_meter_ring {
  _Atomic unsigned int word;  /* (count << 2) | copy of the last publication; 0: none since clear() */
  unsigned int count, next;   /* the audio thread's: the last count it published, the copy it fills next */
  unsigned int deep_mask;     /* bit i: value i holds its smallest (a gain reduction), else its largest */
  unsigned int hold_mask;     /* bit i: value i is held until read; clear: the held word is the block's value */
  double acc[OMX_METER_RING_MAX], clip_acc[OMX_METER_RING_MAX]; /* the audio thread's accumulators */
  _Atomic unsigned int taken[OMX_METER_RING_MAX]; /* per value: the count of the copy the reader last took it from */
  _Atomic unsigned int reads; /* how many takes succeeded; the reader's */
  _Atomic unsigned long long copy[OMX_METER_RING_COPIES][OMX_METER_RING_STRIDE]; /* values, held, clip: bits */
};

/** Before any other call, with no reader yet. */
static inline void omx_meter_ring_init(struct omx_meter_ring *r) {
  memset(r, 0, sizeof *r);
  r->hold_mask = ~0u;
}

/** Which values hold their smallest (`deep_mask`) and which are held at all (`hold_mask`; the rest give the
 * block's value). After init(), before the first publish(). [main thread, audio thread stopped] */
static inline void omx_meter_ring_policy(struct omx_meter_ring *r, unsigned int deep_mask, unsigned int hold_mask) {
  r->deep_mask = deep_mask;
  r->hold_mask = hold_mask;
}

/** Nothing published: readers get no values until the next publish(). [main thread, audio thread stopped]
 * The count and the copy in turn go on, so a reader that straddles a clear() is still judged right. */
static inline void omx_meter_ring_clear(struct omx_meter_ring *r) {
  atomic_store_explicit(&r->word, 0u, memory_order_release);
}

/** One block's values, `n` of them (n <= OMX_METER_RING_MAX), and `clips`, each value's run of over-full-scale
 * samples in the block (NULL: none). [audio thread, one writer] */
static inline void omx_meter_ring_publish_clip(struct omx_meter_ring *r, const double *values, const double *clips,
                                               unsigned int n) {
  const unsigned int c = r->next;
  /* the previous word stays ahead of this fill, for any reader that sees a value of it */
  atomic_thread_fence(memory_order_release);
  for (unsigned int i = 0; i < n; i++) {
    const double v = values[i], k = clips ? clips[i] : 0.0;
    /* the reader has taken this value from the last block published: it starts again from this one */
    const int restart = atomic_load_explicit(&r->taken[i], memory_order_acquire) == r->count;
    if (restart || !((r->hold_mask >> i) & 1u)) {
      r->acc[i] = v;
      r->clip_acc[i] = k;
    } else {
      r->acc[i] = ((r->deep_mask >> i) & 1u) ? (v < r->acc[i] ? v : r->acc[i]) : (v > r->acc[i] ? v : r->acc[i]);
      if (k > r->clip_acc[i]) r->clip_acc[i] = k;
    }
    unsigned long long bits;
    memcpy(&bits, &v, sizeof bits);
    atomic_store_explicit(&r->copy[c][i], bits, memory_order_relaxed);
    memcpy(&bits, &r->acc[i], sizeof bits);
    atomic_store_explicit(&r->copy[c][OMX_METER_RING_HELD + i], bits, memory_order_relaxed);
    memcpy(&bits, &r->clip_acc[i], sizeof bits);
    atomic_store_explicit(&r->copy[c][OMX_METER_RING_CLIP + i], bits, memory_order_relaxed);
  }
  r->count = (r->count + 1u) & OMX_METER_RING_COUNT_MASK;
  if (r->count == 0u) r->count = 1u;
  atomic_store_explicit(&r->word, (r->count << 2) | c, memory_order_release);
  r->next = (c + 1u) % OMX_METER_RING_COPIES;
}

/** One block's values, `n` of them, with no clip word. [audio thread, one writer] */
static inline void omx_meter_ring_publish(struct omx_meter_ring *r, const double *values, unsigned int n) {
  omx_meter_ring_publish_clip(r, values, NULL, n);
}

/** Values `first` .. `first + n - 1` of the last publication, all of the same block, into `out`;
 * returns n, or 0 when nothing was published since clear() or no whole copy could be read in
 * OMX_METER_RING_TRIES tries. [any thread, any number of readers] */
static inline unsigned int omx_meter_ring_read(struct omx_meter_ring *r, unsigned int first, double *out,
                                               unsigned int n) {
  for (unsigned int t = 0; t < OMX_METER_RING_TRIES; t++) {
    const unsigned int w = atomic_load_explicit(&r->word, memory_order_acquire);
    if (w == 0u) return 0;
    for (unsigned int i = 0; i < n; i++) {
      const unsigned long long bits = atomic_load_explicit(&r->copy[w & 3u][first + i], memory_order_relaxed);
      memcpy(&out[i], &bits, sizeof bits);
    }
    atomic_thread_fence(memory_order_acquire);
    const unsigned int again = atomic_load_explicit(&r->word, memory_order_relaxed);
    if (again != 0u && (((again >> 2) - (w >> 2)) & OMX_METER_RING_COUNT_MASK) <= 1u) return n;
  }
  return 0;
}

/** What the reader has missed since its last take: values `first` .. `first + n - 1` of one block into
 * `values`, their held accumulators (the largest since the last take, the smallest for a deep value) into
 * `held`, and their clip words into `clip` (each may be NULL but `held`); returns n, or 0 as read() does,
 * and then takes nothing. A success tells the audio thread to start the accumulators again.
 * [any thread but the audio role; ONE reader per instance] */
static inline unsigned int omx_meter_ring_take(struct omx_meter_ring *r, unsigned int first, double *values,
                                               double *held, double *clip, unsigned int n) {
  for (unsigned int t = 0; t < OMX_METER_RING_TRIES; t++) {
    const unsigned int w = atomic_load_explicit(&r->word, memory_order_acquire);
    if (w == 0u) return 0;
    for (unsigned int i = 0; i < n; i++) {
      unsigned long long bits;
      if (values) {
        bits = atomic_load_explicit(&r->copy[w & 3u][first + i], memory_order_relaxed);
        memcpy(&values[i], &bits, sizeof bits);
      }
      bits = atomic_load_explicit(&r->copy[w & 3u][OMX_METER_RING_HELD + first + i], memory_order_relaxed);
      memcpy(&held[i], &bits, sizeof bits);
      if (clip) {
        bits = atomic_load_explicit(&r->copy[w & 3u][OMX_METER_RING_CLIP + first + i], memory_order_relaxed);
        memcpy(&clip[i], &bits, sizeof bits);
      }
    }
    atomic_thread_fence(memory_order_acquire);
    const unsigned int again = atomic_load_explicit(&r->word, memory_order_relaxed);
    if (again != 0u && (((again >> 2) - (w >> 2)) & OMX_METER_RING_COUNT_MASK) <= 1u) {
      for (unsigned int i = 0; i < n; i++) atomic_store_explicit(&r->taken[first + i], w >> 2, memory_order_release);
      atomic_fetch_add_explicit(&r->reads, 1u, memory_order_relaxed);
      return n;
    }
  }
  return 0;
}

/** How many takes have succeeded. [any thread] */
static inline unsigned int omx_meter_ring_reads(const struct omx_meter_ring *r) {
  return atomic_load_explicit(&((struct omx_meter_ring *)r)->reads, memory_order_relaxed);
}

#endif /* OMX_METER_RING_H */
