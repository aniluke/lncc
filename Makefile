SRCTOP := $(shell pwd)
CFG = $(shell pwd)

-include $(CFG)/Make.defs


SUBDIRS = src/main

PHONY_TARGETS = lncc

-include $(CFG)/Make.rules

