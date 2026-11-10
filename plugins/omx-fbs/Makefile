#!/usr/bin/make -f
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com>
#
# omx-fbs as an LV2 effect (spec 7.7, 7.13, 7.16): the feedback corrector's face, hand-written C over omx-dsp's live
# shell (omx_fbs_lv2.h) because the generated faces (lib/omx_lv2_face.c) have no worker. Its descriptor, panel
# and TTLs are written by tools/live-faces.mjs (`make -C ../.. live-faces` holds them fresh and equal to the C enum).
#
#   make            build/omx-fbs.lv2/ (omx-fbs.so, the TTLs)
#   make test       the report/live-set oracle
#   make install    DESTDIR/PREFIX/LIBDIR: $(LIBDIR)/lv2/omx-fbs.lv2/
#
# Build deps: omx-dsp (pkg-config omxdsp, >= OMXDSP_MIN, the floor packaging/omx-plugins.spec declares), lv2.

NAME       := omx-fbs
URI        := urn:openmixer:fbs
OMXDSP_MIN ?= 0.32.0
ROOT       := $(abspath ../..)
GEN        := generated
B          := build
BUNDLE     := $(B)/$(NAME).lv2

PKG_CONFIG ?= pkg-config
ifeq ($(shell $(PKG_CONFIG) --atleast-version=$(OMXDSP_MIN) omxdsp && echo y),)
$(error omx-dsp >= $(OMXDSP_MIN) is required (packaging/omx-plugins.spec); `$(PKG_CONFIG) omxdsp` finds $(or $(shell $(PKG_CONFIG) --modversion omxdsp 2>/dev/null),none): install omx-dsp-devel or libomxdsp-dev from the channel (BUILDING.md))
endif

CC         ?= cc
CFLAGS     ?= -O2 -g
WARN       := -std=gnu11 -Wall -Wextra -Werror
DSP_CFLAGS := $(shell $(PKG_CONFIG) --cflags omxdsp)
DSP_LIBS   := $(shell $(PKG_CONFIG) --libs omxdsp)
LV2_CFLAGS := $(shell $(PKG_CONFIG) --cflags lv2 2>/dev/null)
PLUGIN     := -fPIC -shared -Wl,-z,defs -fvisibility=hidden

PREFIX  ?= /usr
LIBDIR  ?= $(PREFIX)/lib
TTL_SRCS := $(wildcard $(GEN)/$(NAME).lv2/*.ttl)

all: $(BUNDLE)/$(NAME).so $(BUNDLE)/manifest.ttl

$(BUNDLE)/$(NAME).so: omx_fbs_lv2.c | $(B)
	@mkdir -p $(BUNDLE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARN) $(PLUGIN) $(DSP_CFLAGS) $(LV2_CFLAGS) -o $@ $< $(LDFLAGS) $(DSP_LIBS) -lm

# The bundle's text is generated and committed (tools/live-faces.mjs); the build copies it beside the binary.
$(BUNDLE)/manifest.ttl: $(TTL_SRCS) | $(B)
	@mkdir -p $(BUNDLE)
	cp $(TTL_SRCS) $(BUNDLE)/

$(B):
	@mkdir -p $@

$(B)/fbs_report_matches_live_set.test: test/fbs_report_matches_live_set.test.c omx_fbs_lv2.c | $(B)
	$(CC) $(CFLAGS) $(WARN) $(DSP_CFLAGS) $(LV2_CFLAGS) omx_fbs_lv2.c test/fbs_report_matches_live_set.test.c \
		$(DSP_LIBS) -lm -o $@

# The oracle: the report ports equal the live set, adopted through the real work_response() callback.
test: all $(B)/fbs_report_matches_live_set.test
	./$(B)/fbs_report_matches_live_set.test

install: all
	mkdir -p $(DESTDIR)$(LIBDIR)/lv2
	cp -r $(BUNDLE) $(DESTDIR)$(LIBDIR)/lv2/

clean:
	rm -rf $(B)

.PHONY: all test install clean
