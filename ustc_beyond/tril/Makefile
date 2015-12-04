#
# (C) Copyleft 2011 2012 2013
# Late Lee from http://www.latelee.org
# 
# A simple Makefile for *ONE* project(c or/and cc file) in *ONE*  directory
#
# note: 
# you can put head file(s) in 'include' directory, so it looks 
# a little neat.
#
# usage: $ make
#        $ make debug=y
###############################################################################

# !!!=== cross compile...
CROSS_COMPILE = 

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar

ARFLAGS = cr
RM = -rm -rf
MAKE = make

CFLAGS = -Wall
#debug = y

ifeq ($(debug), y)
CFLAGS += -g
else
CFLAGS += -O2 -s
endif

# !!!===
DEFS = 

CFLAGS += $(DEFS)

LDFLAGS = $(LIBS)

# !!!===
INCDIRS = ./

# !!!===
CFLAGS += -I$(INCDIRS)

# !!!===
LDFLAGS += 

# !!!===
# source file(s), including c file(s) cc file(s)
# you can also use $(wildcard *.c), etc.
SRC_C   := $(wildcard *.c)
SRC_CPP := $(wildcard *.cc)

# object file(s)
OBJ_C   := $(patsubst %.c,%.o,$(SRC_C))
OBJ_CPP := $(patsubst %.cc,%.o,$(SRC_CPP))

# !!!===
# executable file
target = hello

###############################################################################

all: $(target)

$(target): $(OBJ_C) $(OBJ_CPP)
	@echo "Generating executable file..." $(notdir $(target))
	@$(CXX) $(CFLAGS) $^ -o $(target) $(LDFLAGS)

# make all .c or .cc
%.o: %.c
	@echo "Compiling: " $(addsuffix .c, $(basename $(notdir $@)))
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	@echo "Compiling: " $(addsuffix .cc, $(basename $(notdir $@)))
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@$(RM) $(target)
	@$(RM) *.o *.back *~

.PHONY: all clean
