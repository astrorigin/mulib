#!/usr/bin/make -f
# Plib Makefile for Linux

AR = ar
CC = gcc
CP = cp -rf
MKDIR = mkdir -p
RM = rm -rf

INCDIR = .

CFLAGS += -ansi -Wall -Wextra -Werror -pthread -fPIC -I$(INCDIR)
LDFLAGS+=

# Compile options, (un)comment at will

# One of these two variant must be chosen
OPT = -g # debug mode
#OPT = -DNDEBUG -O3 # release mode

# Without memory pool layer
#OPT += -DP_NO_MEMPOOL

# Global trace switch
#OPT += -DP_TRACE_MODE

# Enable trace in these modules
#OPT += -DP_TRACE_BTREE
#OPT += -DP_TRACE_DICT
#OPT += -DP_TRACE_MEMLIST
#OPT += -DP_TRACE_MEMPOOL
#OPT += -DP_TRACE_STRING
#OPT += -DP_TRACE_VECTOR

# Other options
#OPT += -DP_STRING_QUANTA=32

# Packaging options
VERSION = test

# Meat

INC = p_btree.h \
      p_chrono.h \
      p_csv.h \
      p_dict.h \
      p_dllist.h \
      p_fbuf.h \
      p_h.h \
      p_llabs.h \
      p_memlist.h \
      p_mempool.h \
      p_mutex.h \
      p_sllist.h \
      p_string.h \
      p_vector.h

SRC = p_btree.c \
      p_btree_test.c \
      p_chrono.c \
      p_csv.c \
      p_csv_test.c \
      p_dict.c \
      p_dict_test.c \
      p_fbuf.c \
      p_llabs.c \
      p_memlist.c \
      p_mempool.c \
      p_mempool_test_01.c \
      p_mempool_test_02.c \
      p_mempool_test.c \
      p_sllist.c \
      p_string.c \
      p_string_test.c \
      p_vector.c \
      p_vector_test.c

AUX = CMakeLists.txt \
      COPYING.TXT \
      COPYING.LESSER.TXT \
      Makefile \
      README.TXT

OBJ = p_btree.o \
      p_chrono.o \
      p_csv.o \
      p_dict.o \
      p_fbuf.o \
      p_llabs.o \
      p_memlist.o \
      p_mempool.o \
      p_sllist.o \
      p_string.o \
      p_vector.o

.PHONY: all clean clean-obj clean-test install test uninstall

.DEFAULT_GOAL = all

all: libplib.a libplib.so

# Headers, for reference

p_btree.h: p_h.h
p_chrono.h: p_h.h
p_csv.h: p_h.h
p_dict.h: p_btree.h p_h.h p_mempool.h p_string.h
p_dllist.h: p_h.h
p_fbuf.h: p_h.h
p_h.h:
p_llabs.h:
p_memlist.h: p_h.h p_mutex.h
p_mempool.h: p_btree.h p_h.h p_memlist.h
p_mutex.h: p_h.h
p_sllist.h: p_h.h
p_string.h: p_h.h p_vector.h
p_vector.h: p_h.h

# Objects

p_btree.o: p_btree.c p_btree.h p_h.h p_llabs.h p_memlist.h p_mempool.h p_mutex.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_chrono.o: p_chrono.c p_chrono.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_csv.o: p_csv.c p_csv.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_dict.o: p_dict.c p_btree.h p_dict.h p_h.h p_mempool.h p_string.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_fbuf.o: p_fbuf.c p_fbuf.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_llabs.o: p_llabs.c p_llabs.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_memlist.o: p_memlist.c p_h.h p_memlist.h p_mutex.h p_sllist.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_mempool.o: p_mempool.c p_btree.h p_h.h p_memlist.h p_mempool.h p_sllist.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_sllist.o: p_sllist.c p_h.h p_sllist.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_string.o: p_string.c p_h.h p_mempool.h p_string.h p_vector.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_vector.o: p_vector.c p_btree.h p_h.h p_memlist.h p_mempool.h p_vector.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

# Libraries

libplib.a: $(OBJ)
	$(AR) -cr $@ $(OBJ)

libplib.so: $(OBJ)
	$(CC) -shared -o $@ $(OBJ)

# Tests

p_btree_test.o: p_btree_test.c p_btree.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_btree_test: p_btree_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_csv_test.o: p_csv_test.c p_csv.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_csv_test: p_csv_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_dict_test.o: p_dict_test.c p_btree.h p_dict.h p_h.h p_mempool.h p_string.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_dict_test: p_dict_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_mempool_test.o: p_mempool_test.c p_btree.h p_h.h p_mempool.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_mempool_test: p_mempool_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_mempool_test_01.o: p_mempool_test_01.c p_btree.h p_h.h p_mempool.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_mempool_test_01: p_mempool_test_01.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_mempool_test_02.o: p_mempool_test_02.c p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_mempool_test_02: p_mempool_test_02.o
	$(CC) $(LDFLAGS) -o $@ $<

p_string_test.o: p_string_test.c p_h.h p_string.h p_vector.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_string_test: p_string_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

p_vector_test.o: p_vector_test.c p_vector.h p_h.h
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

p_vector_test: p_vector_test.o libplib.a
	$(CC) $(LDFLAGS) -o $@ $< libplib.a

test: p_btree_test \
      p_mempool_test \
      p_mempool_test_01 \
      p_mempool_test_02 \
      p_string_test \
      p_vector_test

clean-test:
	$(RM) *_test *_test_01 *_test_02

# Packaging

dist:
	$(MKDIR) $@

dist/plib-$(VERSION):
	$(MKDIR) $@

dist/plib-$(VERSION).tar.bz2: $(AUX) $(INC) $(SRC)
	$(MKDIR) dist/plib-$(VERSION); \
    $(CP) $(AUX) $(INC) $(SRC) dist/plib-$(VERSION); \
    tar -C dist -cjf $@ plib-$(VERSION); \
    $(RM) dist/plib-$(VERSION)

# Stuff

clean-obj:
	$(RM) *.o

clean: clean-obj clean-test
	$(RM) *.a *.so

install: libplib.so
	$(MKDIR) /usr/local/include/plib; \
    $(CP) $(INC) /usr/local/include/plib; \
    $(CP) libplib.so /usr/local/lib;

uninstall:
	$(RM) /usr/local/include/plib; \
    $(RM) /usr/local/lib/libplib.so;


# vi: set fenc=utf-8 ff=unix ft=make et sw=4 ts=4 sts=4 :
