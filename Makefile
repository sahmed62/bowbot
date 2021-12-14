# Copyright 2021 Safayet N Ahmed

.PHONY: all clean

ROOTDIR := $(dir $(lastword $(MAKEFILE_LIST)))
ROOTDIR := $(shell cd $(ROOTDIR) && pwd )
INCDIR := $(ROOTDIR)/include
SRCDIR := $(ROOTDIR)/src
BINDIR := $(ROOTDIR)/bin

CC     :=gcc
CFLAGS :=-I$(INCDIR) -Wall -Wextra -Wpedantic

DEPS :=$(wildcard $(INCDIR)/*.h $(SRCDIR)/*.c)

COMMON_SRC := $(SRCDIR)/bow.c

BINS := $(BINDIR)/bow-bin-to-txt \
        $(BINDIR)/bow-txt-to-bin

all: $(BINS)

clean:
	rm -r $(BINDIR)

$(BINDIR)/bow-bin-to-txt: $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $(SRCDIR)/bow-bin-to-txt.c $(COMMON_SRC)

$(BINDIR)/bow-txt-to-bin: $(DEPS)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $(SRCDIR)/bow-txt-to-bin.c $(COMMON_SRC)

