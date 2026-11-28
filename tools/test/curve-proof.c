// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
/*
 * curve-proof.c — the measurement proof of a display marked `basis: exact`: the curve a surface draws, omx-dsp's
 * omx_curve_points() over the terms the panel binds, against the plugin's own static response measured through
 * libomx-clap (omx-clap-host's library: open, activate, the parameter ring, the RT body), point for point.
 *
 *   curve-proof < plan
 *
 * The plan is what tools/test/curve-proof.mjs writes from the declarations' panel layouts, one TAB-separated record
 * a line:
 *
 *   case  <stem> <setting> <rate> <.clap path> <display kind> <family>
 *   param <CLAP parameter name> <value the host writes>
 *   term  <role> <index> <value> <unit word> <shape word, or * for none>
 *   end
 *
 * For each case the plugin is opened, activated at the rate, every listed parameter written and read back (the
 * plugin must hold what the host wrote, as the float it keeps), and the curve asked for over the terms:
 *
 *   - a transfer_curve over the level sweep CURVE_PROOF_LEVEL_MIN_DB to 0 dB in CURVE_PROOF_LEVEL_STEP_DB steps: a
 *     square wave at each level, whose mean square is its level (the detector's level, whatever its window), held
 *     until the detector settles and then measured as the output's level over whole periods;
 *   - an eq_response over its own frequency grid and anchors (the extents a surface draws): a sine at each x, held
 *     until the filters settle, the output's magnitude over the input's from one Hann-windowed DFT bin each.
 *
 * Every point must be within CURVE_PROOF_TOLERANCE_DB of the curve. An eq_response point the curve draws deeper than
 * CURVE_PROOF_DEPTH_DB (a notch's bottom: the double-precision curve runs on to its -120 dB floor, while the plugin's
 * float coefficients put the notch's zero just off the unit circle, a finite depth under -90 dB) holds when the
 * measurement is that deep too: at or under CURVE_PROOF_DEPTH_DB plus the tolerance.
 * One line per case: PASS, or FAIL naming the setting, the worst point and its error.
 *
 * Exit 0 every case holds; 1 a case fails (or its plugin, a parameter or the curve is refused, named); 2 the plan
 * is malformed.
 */
#include <omx-clap-host/clap_host.h>
#include <omxdsp/omx_curve.h>

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** The proof's tolerance, dB, at every point (the one-panel spec's lane 5). */
#define CURVE_PROOF_TOLERANCE_DB 0.5
/** The level sweep: CURVE_PROOF_LEVEL_MIN_DB to 0 dB in CURVE_PROOF_LEVEL_STEP_DB steps. */
#define CURVE_PROOF_LEVEL_MIN_DB (-80.0)
#define CURVE_PROOF_LEVEL_STEP_DB 2.0
/** The depth an eq_response is held to: the deepest level a level axis draws (CURVE_EXTENTS levelMinDb). */
#define CURVE_PROOF_DEPTH_DB ((double)OMX_CURVE_EXTENTS_LEVEL_MIN_DB)
/** The grid points of an eq_response, before its anchors. */
#define CURVE_PROOF_EQ_POINTS 48u
/** The square wave's frequency, Hz: a whole number of samples a half period at every rate the plan runs. */
#define CURVE_PROOF_SQUARE_HZ 500.0
/** How long a level is held before it is measured, and measured, seconds. */
#define CURVE_PROOF_LEVEL_SETTLE_S 0.5
#define CURVE_PROOF_LEVEL_MEASURE_S 0.2
/** How long a sine is held before it is measured, and measured, seconds. */
#define CURVE_PROOF_SINE_SETTLE_S 0.2
#define CURVE_PROOF_SINE_MEASURE_S 0.25
/** The block the host runs. */
#define CURVE_PROOF_BLOCK 256u

#define MAX_PARAMS 256
#define MAX_TERMS 384

typedef struct {
  char stem[64], setting[64], path[512];
  double rate;
  int kind, family;
  char pname[MAX_PARAMS][CLAP_NAME_SIZE];
  double pvalue[MAX_PARAMS];
  uint32_t nparams;
  struct omx_curve_term terms[MAX_TERMS];
  uint32_t nterms;
} Case;

/** One TAB-separated field of `line` at `*at`, cut in place; NULL past the last. */
static char *field(char **at) {
  if (!*at) return NULL;
  char *f = *at, *tab = strchr(f, '\t');
  if (tab) {
    *tab = '\0';
    *at = tab + 1;
  } else {
    f[strcspn(f, "\n")] = '\0';
    *at = NULL;
  }
  return f;
}

static int word(const char *const *words, uint32_t n, const char *w) { return omx_curve_word(words, n, w); }

/** One block through the hosted instance, in place: what the RT body runs. */
static void run_block(struct omx_clap_instance *in, float *l, float *r, uint32_t n) { omx_clap_host_run(in, l, r, n); }

/** A square wave held at `level_db` until settled, then measured: the output's level, dB. */
static double measure_level(struct omx_clap_instance *in, double rate, double level_db, uint64_t *phase) {
  const double amp = pow(10.0, level_db / 20.0);
  const uint64_t half = (uint64_t)llround(rate / CURVE_PROOF_SQUARE_HZ / 2.0);
  const uint64_t settle = (uint64_t)llround(CURVE_PROOF_LEVEL_SETTLE_S * rate);
  const uint64_t period = 2u * half, measure = (uint64_t)llround(CURVE_PROOF_LEVEL_MEASURE_S * rate) / period * period;
  float l[CURVE_PROOF_BLOCK], r[CURVE_PROOF_BLOCK];
  double in_sq = 0.0, out_sq = 0.0;
  for (uint64_t done = 0; done < settle + measure;) {
    const uint32_t n = (uint32_t)(settle + measure - done < CURVE_PROOF_BLOCK ? settle + measure - done : CURVE_PROOF_BLOCK);
    float x[CURVE_PROOF_BLOCK];
    for (uint32_t i = 0; i < n; i++) {
      x[i] = (float)(((*phase + i) / half) % 2u ? -amp : amp);
      l[i] = r[i] = x[i];
    }
    run_block(in, l, r, n);
    for (uint32_t i = 0; i < n; i++)
      if (done + i >= settle) {
        in_sq += (double)x[i] * x[i];
        out_sq += 0.5 * ((double)l[i] * l[i] + (double)r[i] * r[i]);
      }
    *phase += n;
    done += n;
  }
  return level_db + 10.0 * log10(out_sq / in_sq);
}

/** A sine at `hz` held until settled, then the output's magnitude over the input's at `hz`, dB. */
static double measure_sine(struct omx_clap_instance *in, double rate, double hz, uint64_t *t) {
  const uint64_t settle = (uint64_t)llround(CURVE_PROOF_SINE_SETTLE_S * rate);
  const uint64_t measure = (uint64_t)llround(CURVE_PROOF_SINE_MEASURE_S * rate);
  const double w = 2.0 * M_PI * hz / rate;
  float l[CURVE_PROOF_BLOCK], r[CURVE_PROOF_BLOCK];
  double ire = 0.0, iim = 0.0, ore = 0.0, oim = 0.0;
  for (uint64_t done = 0; done < settle + measure;) {
    const uint32_t n = (uint32_t)(settle + measure - done < CURVE_PROOF_BLOCK ? settle + measure - done : CURVE_PROOF_BLOCK);
    float x[CURVE_PROOF_BLOCK];
    for (uint32_t i = 0; i < n; i++) {
      x[i] = (float)(0.25 * sin(w * (double)(*t + i)));
      l[i] = r[i] = x[i];
    }
    run_block(in, l, r, n);
    for (uint32_t i = 0; i < n; i++)
      if (done + i >= settle) {
        const double k = (double)(done + i - settle);
        const double hann = 0.5 - 0.5 * cos(2.0 * M_PI * k / (double)(measure - 1u));
        const double c = cos(w * k), s = sin(w * k), o = 0.5 * ((double)l[i] + (double)r[i]);
        ire += hann * x[i] * c, iim -= hann * x[i] * s;
        ore += hann * o * c, oim -= hann * o * s;
      }
    *t += n;
    done += n;
  }
  return 10.0 * log10((ore * ore + oim * oim) / (ire * ire + iim * iim));
}

static int run_case(const Case *c) {
  const char *k = OMX_CURVE_DISPLAY_WORDS[c->kind];
  char why[OMX_CLAP_WHY_MAX] = "";
  struct omx_clap_instance *in = NULL;
  if (omx_clap_host_open(c->path, NULL, &in, why) != 0) {
    printf("FAIL curve-proof %s '%s' at %.0f Hz: the host did not open %s: %s\n", c->stem, c->setting, c->rate, c->path, why);
    return 1;
  }
  struct omx_clap_param_row *rows = NULL;
  const char *rwhy = NULL;
  const int nrows = omx_clap_host_param_roster(in, &rows, &rwhy);
  clap_id ids[MAX_PARAMS];
  int bad = 0;
  for (uint32_t p = 0; p < c->nparams; p++) {
    ids[p] = CLAP_INVALID_ID;
    for (int i = 0; i < nrows; i++)
      if (strcmp(rows[i].name, c->pname[p]) == 0) ids[p] = rows[i].id;
    if (ids[p] == CLAP_INVALID_ID) {
      printf("FAIL curve-proof %s '%s' at %.0f Hz: no parameter named '%s'\n", c->stem, c->setting, c->rate, c->pname[p]);
      bad = 1;
    }
  }
  free(rows);
  if (!bad && omx_clap_host_activate(in, c->rate, CURVE_PROOF_BLOCK, why) != 0) {
    printf("FAIL curve-proof %s '%s' at %.0f Hz: activate refused: %s\n", c->stem, c->setting, c->rate, why);
    bad = 1;
  }
  if (bad) {
    omx_clap_host_close(in);
    return 1;
  }
  for (uint32_t p = 0; p < c->nparams; p++) omx_clap_host_param_write(in, ids[p], c->pvalue[p]);
  omx_clap_host_param_flush(in);
  for (uint32_t p = 0; p < c->nparams; p++) {
    double held = NAN;
    /* the faces hold a parameter as a float: what the host wrote, rounded once */
    if (omx_clap_host_param_read(in, ids[p], &held) != 0 || (float)held != (float)c->pvalue[p]) {
      printf("FAIL curve-proof %s '%s' at %.0f Hz: '%s' holds %.17g, the host wrote %.17g\n", c->stem, c->setting, c->rate,
             c->pname[p], held, c->pvalue[p]);
      bad = 1;
    }
  }
  /* the curve a surface draws over these terms */
  const int eq = c->kind == OMX_CURVE_DISPLAYS_EQ_RESPONSE;
  const uint32_t levels = (uint32_t)llround(-CURVE_PROOF_LEVEL_MIN_DB / CURVE_PROOF_LEVEL_STEP_DB) + 1u;
  struct omx_curve_request rq = {.kind = (enum omx_curve_displays)c->kind,
                                 .family = (enum omx_curve_families)c->family,
                                 .sample_rate = c->rate,
                                 .points = eq ? CURVE_PROOF_EQ_POINTS : levels,
                                 .has_extents = !eq,
                                 .x_min = eq ? 0.0 : CURVE_PROOF_LEVEL_MIN_DB,
                                 .x_max = 0.0,
                                 .terms = c->terms,
                                 .nterms = c->nterms};
  static double x[OMX_CURVE_POINTS_MAX], y[OMX_CURVE_POINTS_MAX];
  int code = OMX_CURVE_OK;
  const uint32_t n = omx_curve_points(&rq, x, y, OMX_CURVE_POINTS_MAX, &code);
  if (code != OMX_CURVE_OK || n == 0) {
    printf("FAIL curve-proof %s '%s' at %.0f Hz: omx_curve_points refused the %s: code %d\n", c->stem, c->setting, c->rate, k, code);
    bad = 1;
  }
  if (bad) {
    omx_clap_host_close(in);
    return 1;
  }
  omx_clap_host_publish(in, pthread_self());
  uint64_t t = 0;
  double worst = -1.0, worst_x = 0.0, worst_y = 0.0, worst_m = 0.0;
  uint32_t deep = 0;
  for (uint32_t i = 0; i < n; i++) {
    const double m = eq ? measure_sine(in, c->rate, x[i], &t) : measure_level(in, c->rate, x[i], &t);
    /* a point drawn under the depth holds a measurement at or under the depth */
    const int under = eq && y[i] < CURVE_PROOF_DEPTH_DB;
    deep += (uint32_t)under;
    const double err = under ? (m > CURVE_PROOF_DEPTH_DB ? m - CURVE_PROOF_DEPTH_DB : 0.0) : fabs(m - y[i]);
    if (worst < 0.0 || !(err <= worst)) worst = isfinite(err) ? err : INFINITY, worst_x = x[i], worst_y = y[i], worst_m = m;
  }
  /* the RT body is this thread: ask it to stop, run the block that stops it, then unpublish finds it stopped */
  omx_clap_request_stop(&in->stage);
  float l[CURVE_PROOF_BLOCK] = {0}, r[CURVE_PROOF_BLOCK] = {0};
  run_block(in, l, r, CURVE_PROOF_BLOCK);
  if (omx_clap_host_unpublish(in, 100, 1000000) != 0) {
    printf("FAIL curve-proof %s '%s' at %.0f Hz: the stage did not stop\n", c->stem, c->setting, c->rate);
    return 1;
  }
  omx_clap_host_stop(in);
  omx_clap_host_close(in);
  const char *unit = eq ? "Hz" : "dB in";
  if (worst <= CURVE_PROOF_TOLERANCE_DB) {
    printf("PASS curve-proof %s '%s' at %.0f Hz: the %s within %.1f dB at %u points (%u drawn under %.0f dB, measured "
           "under it), worst %.3f dB at %.6g %s\n",
           c->stem, c->setting, c->rate, k, CURVE_PROOF_TOLERANCE_DB, n, deep, CURVE_PROOF_DEPTH_DB, worst, worst_x, unit);
    return 0;
  }
  printf("FAIL curve-proof %s '%s' at %.0f Hz: the %s is off by %.3f dB at %.6g %s (drawn %.3f dB, measured %.3f dB), over "
         "the %.1f dB the exact basis allows\n",
         c->stem, c->setting, c->rate, k, worst, worst_x, unit, worst_y, worst_m, CURVE_PROOF_TOLERANCE_DB);
  return 1;
}

int main(void) {
  struct omx_clap_host_config cfg;
  omx_clap_host_config_default(&cfg);
  cfg.clamp = 0;          /* a make-up may lift the output past full scale: measured, never clamped */
  cfg.idle_hold_us = OMX_CLAP_IDLE_HOLD_OFF;
  if (omx_clap_host_configure(&cfg) != 0) {
    fputs("curve-proof: libomx-clap refused the configuration\n", stderr);
    return 1;
  }
  static Case c;
  static char line[4096];
  int open = 0, bad = 0, cases = 0, lineno = 0;
  while (fgets(line, sizeof line, stdin)) {
    lineno++;
    char *at = line, *tag = field(&at);
    if (!tag || !*tag) continue;
    if (strcmp(tag, "case") == 0 && !open) {
      memset(&c, 0, sizeof c);
      const char *stem = field(&at), *setting = field(&at), *rate = field(&at), *path = field(&at), *kind = field(&at),
                 *family = field(&at);
      if (!family) goto malformed;
      snprintf(c.stem, sizeof c.stem, "%s", stem);
      snprintf(c.setting, sizeof c.setting, "%s", setting);
      snprintf(c.path, sizeof c.path, "%s", path);
      c.rate = strtod(rate, NULL);
      c.kind = word(OMX_CURVE_DISPLAY_WORDS, OMX_CURVE_DISPLAYS_COUNT, kind);
      c.family = word(OMX_CURVE_FAMILY_WORDS, OMX_CURVE_FAMILIES_COUNT, family);
      if (c.kind < 0 || c.family < 0 || !(c.rate > 0.0)) goto malformed;
      open = 1;
    } else if (strcmp(tag, "param") == 0 && open && c.nparams < MAX_PARAMS) {
      const char *name = field(&at), *value = field(&at);
      if (!value) goto malformed;
      snprintf(c.pname[c.nparams], CLAP_NAME_SIZE, "%s", name);
      c.pvalue[c.nparams++] = strtod(value, NULL);
    } else if (strcmp(tag, "term") == 0 && open && c.nterms < MAX_TERMS) {
      const char *role = field(&at), *index = field(&at), *value = field(&at), *unit = field(&at), *shape = field(&at);
      if (!shape) goto malformed;
      const int ro = word(OMX_CURVE_ROLE_WORDS, OMX_CURVE_ROLES_COUNT, role);
      if (ro < 0) continue; /* a role the curve does not read (attack, release): not a term, as a surface binds it */
      struct omx_curve_term *tm = &c.terms[c.nterms++];
      tm->role = (enum omx_curve_roles)ro;
      tm->index = (uint32_t)strtoul(index, NULL, 10);
      tm->value = strtod(value, NULL);
      const int u = word(OMX_CURVE_UNIT_WORDS, OMX_CURVE_UNITS_COUNT, unit);
      if (u < 0) {
        fprintf(stderr, "curve-proof: line %d: '%s' is no unit word of omx-dsp's curve\n", lineno, unit);
        return 2;
      }
      tm->unit = (enum omx_curve_units)u;
      tm->shape = strcmp(shape, "*") == 0 ? OMX_CURVE_NO_SHAPE : word(OMX_CURVE_SHAPE_WORDS, OMX_CURVE_SHAPES_COUNT, shape);
      if (tm->shape == -1 && strcmp(shape, "*") != 0) {
        fprintf(stderr, "curve-proof: line %d: '%s' is no shape word of omx-dsp's curve\n", lineno, shape);
        return 2;
      }
      tm->origin = OMX_CURVE_ORIGINS_USER;
    } else if (strcmp(tag, "end") == 0 && open) {
      bad |= run_case(&c);
      fflush(stdout);
      cases++;
      open = 0;
    } else {
      goto malformed;
    }
  }
  if (open || cases == 0) {
    fputs("curve-proof: the plan ends inside a case, or holds none\n", stderr);
    return 2;
  }
  return bad;
malformed:
  fprintf(stderr, "curve-proof: line %d of the plan is malformed\n", lineno);
  return 2;
}
