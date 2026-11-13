#!/usr/bin/make -f
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# omx-test-note-echo, a TEST plugin (its declaration says "role": "test"): a CLAP face only, linked into
# libomxplugins.a by the top Makefile and installed by no target. Written by hand; tools/gen.mjs writes
# generated/omx_test_note_echo_decl.h from the declaration.
#
#   make            build/omx-test-note-echo.clap (for the oracle; never installed)
#   make test       the generated header is fresh, the echo oracle at every oracle rate
#                   (test/note-echo-oracle.c), and its sabotage arms (test/sabotage.sh)

NAME       := omx-test-note-echo
CLAP_ID    := org.openmixer.test-note-echo
ROOT       := $(abspath ../..)
GEN        := generated
B          := build

PKG_CONFIG ?= pkg-config
ABI_CFLAGS := $(shell $(PKG_CONFIG) --cflags clap 2>/dev/null)
DSP_CFLAGS := $(shell $(PKG_CONFIG) --cflags omxdsp 2>/dev/null)

CC      ?= cc
CFLAGS  ?= -O2 -g
WARN    := -std=gnu11 -Wall -Wextra -Werror
PLUGIN  := -fPIC -shared -Wl,-z,defs -fvisibility=hidden -I$(GEN)

all: $(B)/$(NAME).clap

$(B)/$(NAME).clap: omx_test_note_echo_clap.c $(GEN)/omx_test_note_echo_decl.h | $(B)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARN) $(PLUGIN) $(ABI_CFLAGS) -o $@ $< $(LDFLAGS) -lm

$(B)/note-echo-oracle: test/note-echo-oracle.c $(GEN)/omx_test_note_echo_decl.h | $(B)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARN) -I$(GEN) $(ABI_CFLAGS) $(DSP_CFLAGS) -o $@ $< -ldl -lm

$(B):
	@mkdir -p $@

check:
	node $(ROOT)/tools/gen.mjs --check $(CURDIR)

test: all check $(B)/note-echo-oracle
	$(B)/note-echo-oracle $(B)/$(NAME).clap
	CC="$(CC)" CFLAGS="$(CFLAGS) $(WARN)" PLUGIN="$(PLUGIN) $(ABI_CFLAGS)" sh test/sabotage.sh $(B)/sabotage $(B)/note-echo-oracle

clean:
	rm -rf $(B)

# a test plugin's own oracle and sabotage are its whole check and run in `test`: nothing of it is left for the
# heavy set, so its `test-heavy` (which make test-heavy reaches in every plugin) does nothing
test-heavy: ;

.PHONY: all check test test-heavy clean
