#!/usr/bin/make -f
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# omx-hrp's LV2 face (spec 7.7, 7.13, 7.16): the first worker-threaded plugin in this tree, so it
# is hand-written C over omx-dsp's live-shell header rather than the generated faces the other
# plugins share (lib/omx_lv2_face.c has no worker support yet).
#
# Build deps: omx-dsp (pkg-config omxdsp, >= OMXDSP_MIN, the floor packaging/omx-plugins.spec declares: the
# live-shell header omx_hrp_lv2.h is in its 0.32), lv2.

OMXDSP_MIN ?= 0.32.0
PKG_CONFIG ?= pkg-config
ifeq ($(shell $(PKG_CONFIG) --atleast-version=$(OMXDSP_MIN) omxdsp && echo y),)
$(error omx-dsp >= $(OMXDSP_MIN) is required (packaging/omx-plugins.spec); `$(PKG_CONFIG) omxdsp` finds $(or $(shell $(PKG_CONFIG) --modversion omxdsp 2>/dev/null),none): install omx-dsp-devel or libomxdsp-dev from the channel (BUILDING.md))
endif

CC     ?= cc
CFLAGS ?= -O0 -g
WARN   := -std=gnu11 -Wall -Wextra -Werror
INC    := $(shell $(PKG_CONFIG) --cflags omxdsp) $(shell $(PKG_CONFIG) --cflags lv2 2>/dev/null)
DSP_LIBS := $(shell $(PKG_CONFIG) --libs omxdsp)

B := build

.PHONY: all test clean
all: $(B)/omx_hrp_lv2.o

$(B)/omx_hrp_lv2.o: omx_hrp_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(INC) -fPIC -c $< -o $@

$(B)/hrp_report_matches_live_set.test: test/hrp_report_matches_live_set.test.c omx_hrp_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(INC) omx_hrp_lv2.c test/hrp_report_matches_live_set.test.c \
		$(DSP_LIBS) -lm -o $@

test: $(B)/hrp_report_matches_live_set.test
	./$(B)/hrp_report_matches_live_set.test

clean:
	rm -rf $(B)
