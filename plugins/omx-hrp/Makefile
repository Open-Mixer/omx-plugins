#!/usr/bin/make -f
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# omx-hrp's LV2 face (spec 7.7, 7.13, 7.16): the first worker-threaded plugin in this tree, so it
# is hand-written C over omx-dsp's live-shell header rather than the generated faces the other
# plugins share (lib/omx_lv2_face.c has no worker support yet).
#
# Build deps: omx-dsp's live-shell API (omx_hrp_lv2.h, omx_band_report.h, omx_live_shell.h),
# which is NOT YET in a released omx-dsp (0.31.0 is served; this needs
# FreeMixer/omx-dsp@feat/curve-marks, merged PRs #305 and #306, sha 77b3c6b). Point OMXDSP_INC at
# a clone of that branch's include/ and OMXDSP_LIB at its built build/libomxdsp.a (`make lib` in
# that tree); never vendor the headers into this repo.
#
#   make test   build and run the report/live-set oracle

OMXDSP_INC ?= $(error set OMXDSP_INC to <clone of omx-dsp@feat/curve-marks sha 77b3c6b>/include)
OMXDSP_LIB ?= $(error set OMXDSP_LIB to <that clone>/build/libomxdsp.a)

CC     ?= cc
CFLAGS ?= -O0 -g
WARN   := -std=gnu11 -Wall -Wextra -Werror
INC    := -I$(OMXDSP_INC) $(shell pkg-config --cflags lv2 2>/dev/null)

B := build

.PHONY: all test clean
all: $(B)/omx_hrp_lv2.o

$(B)/omx_hrp_lv2.o: omx_hrp_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(INC) -fPIC -c $< -o $@

$(B)/hrp_report_matches_live_set.test: test/hrp_report_matches_live_set.test.c omx_hrp_lv2.c
	@mkdir -p $(B)
	$(CC) $(CFLAGS) $(WARN) $(INC) omx_hrp_lv2.c test/hrp_report_matches_live_set.test.c \
		$(OMXDSP_LIB) -lm -o $@

test: $(B)/hrp_report_matches_live_set.test
	./$(B)/hrp_report_matches_live_set.test

clean:
	rm -rf $(B)
