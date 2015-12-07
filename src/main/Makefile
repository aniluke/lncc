SRCTOP := $(shell pwd)
CFG = $(shell pwd)/../../

-include $(CFG)/Make.defs

SUBDIRS = utils worker

CXXFLAGS += -I. -Iutils/include -Iworker/include

OBJS = main.o ln_config.o data_set.o feature_col.o gradient_boosting_learner.o least_square_loss_function.o logistic_loss_function.o loss_function_factory.o tree_function.o pred_node.o train_node.o boosting_predictor.o

LIBS = -Lutils -lutils -Lworker -lworker

EXTERN_LIBS = -lpthread

TARGETS = lncc

-include $(CFG)/Make.rules

all:
	cp $(TARGETS) ../../

