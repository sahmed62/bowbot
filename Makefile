# Copyright 2021 Safayet N Ahmed

.PHONY: all clean

ROOTDIR := $(dir $(lastword $(MAKEFILE_LIST)))
ROOTDIR := $(shell cd $(ROOTDIR) && pwd )
INCDIR := $(ROOTDIR)/include
SRCDIR := $(ROOTDIR)/src
BINDIR := $(ROOTDIR)/bin

CC     :=gcc
CFLAGS :=-I$(INCDIR) -Wall -Wextra -Wpedantic -g

COMMON_SRC := $(SRCDIR)/bow.c

DEPS :=$(wildcard $(INCDIR)/*.h $(COMMON_SRC))

BINS := $(BINDIR)/bow-bin-to-txt            \
        $(BINDIR)/bow-txt-to-bin            \
        $(BINDIR)/bow-bin-count-histogram   \
        $(BINDIR)/bow-bin-marginal-histogram

all: $(BINS)

clean:
	rm -r $(BINDIR)

$(BINDIR)/bow-bin-to-txt: $(SRCDIR)/bow-bin-to-txt.c $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(COMMON_SRC)

$(BINDIR)/bow-txt-to-bin: $(SRCDIR)/bow-txt-to-bin.c $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(COMMON_SRC)

$(BINDIR)/bow-bin-count-histogram: $(SRCDIR)/bow-bin-count-histogram.c $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(COMMON_SRC)

$(BINDIR)/bow-bin-marginal-histogram: $(SRCDIR)/bow-bin-marginal-histogram.c $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $< $(COMMON_SRC)
