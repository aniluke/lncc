SRCTOP := $(shell pwd)
CFG = $(shell pwd)/../../../

-include $(CFG)/Make.defs

SUBDIRS = 

CXXFLAGS = -Iinclude -I../ -I../utils/include

OBJS = worker.o local_task.o

LIBS = 

EXTERN_LIBS = 

LIBTARGET = libworker.a

-include $(CFG)/Make.rules

