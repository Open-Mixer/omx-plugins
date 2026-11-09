#!/usr/bin/make -f
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# omx-fbs's LV2 face (spec 7.7, 7.13, 7.16): hand-written C over omx-dsp's live-shell header
# (omx_fbs_lv2.h), as omx-hrp's is: the first worker-threaded faces in this tree, which the generated
# faces (lib/omx_lv2_face.c) have no support for.
#
#   make test   build and run the report/live-set oracle
#
# Build deps: omx-dsp (pkg-config omxdsp, >= OMXDSP_MIN, the floor packaging/omx-plugins.spec declares),
# lv2.

OMXDSP_MIN ?= 0.32.0
PKG_CONFIG ?= pkg-config
ifeq ($(shell $(PKG_CONFIG) --atleast-version=$(OMXDSP_MIN) omxdsp && echo y),)
$(error omx-dsp >= $(OMXDSP_MIN) is required (packaging/omx-plugins.spec); `$(PKG_CONFIG) omxdsp` finds $(or $(shell $(PKG_CONFIG) --modversion omxdsp 2>/dev/null),none): install omx-dsp-devel or libomxdsp-dev from the channel (BUILDING.md))
endif

CC     ?= cc
CFLAGS ?= -O0 -g
WARN   := -std=gnu11 -Wall -Wextra -Werror
DSP_CFLAGS := $(shell $(PKG_CONFIG) --cflags omxdsp)
DSP_LIBS   := $(shell $(PKG_CONFIG) --libs omxdsp)
LV2_CFLAGS := $(shell $(PKG_CONFIG) --cflags lv2 2>/dev/null)

B := build

.PHONY: all test clean
all: $(B)/omx_fbs_lv2.o

$(B)/omx_fbs_lv2.o: omx_fbs_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(DSP_CFLAGS) $(LV2_CFLAGS) -fPIC -c $< -o $@

$(B)/fbs_report_matches_live_set.test: test/fbs_report_matches_live_set.test.c omx_fbs_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(DSP_CFLAGS) $(LV2_CFLAGS) omx_fbs_lv2.c test/fbs_report_matches_live_set.test.c \
		$(DSP_LIBS) -lm -o $@

test: $(B)/fbs_report_matches_live_set.test
	./$(B)/fbs_report_matches_live_set.test

clean:
	rm -rf $(B)
