CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -NDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTFLAGS)
PREFIX?=/usr/local

SRCS=$(wildcat src/**/*.c src/*.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))

TESTS_SRC=$(wildcat tests/*_tests.c)
TESTS_BIN=$(patsubst %.c,%,$(TEST_SRC))


clean:
	rm -rf build/*
	rm -rf bin/*
	rm -rf $(OBJS)
	rm -rf $(TESTS_BIN)
