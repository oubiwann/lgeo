ROOT_DIR = $(shell pwd)
REPO = $(shell git config --get remote.origin.url)
LFE_BIN = _build/default/lib/lfe/bin
LFE = $(LFE_BIN)/lfe

all: build

clean-all: clean clean-docs
	rebar3 lfe clean

include resources/make/code.mk
include resources/make/docs.mk

.PHONY: all clean-all
