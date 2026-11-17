# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Pau Aliagas
#
# The one reader of the dependency floors for every Makefile of this tree (the top one and each plugin's): the floor of a
# build dependency is its BuildRequires line in packaging/omx-plugins.spec, which is generated from packaging/deps.txt.
# $(call spec_floor,<rpm name>) is the floor, empty when the spec has no such line. No ceiling exists to read.
SPEC_FLOORS_SPEC := $(dir $(lastword $(MAKEFILE_LIST)))../packaging/omx-plugins.spec
spec_floor = $(shell sed -n 's/^BuildRequires: $(1) >= \([0-9.]*\)$$/\1/p' $(SPEC_FLOORS_SPEC))
