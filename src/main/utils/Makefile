SRCTOP := $(shell pwd)
CFG = $(shell pwd)/../../../

-include $(CFG)/Make.defs

SUBDIRS = 

CXXFLAGS += -Iinclude -I..

OBJS = common_utils.o pthread_lock.o lock_factory.o tinyxml2.o

LIBS = 

EXTERN_LIBS = 

LIBTARGET = libutils.a


-include $(CFG)/Make.rules

