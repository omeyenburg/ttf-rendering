.PHONY: build compile

CC = bear -- clang
CFLAGS = -Wall -g -Wno-implicit-function-declaration
LDFLAGS = -I./include/ $(shell pkg-config --cflags --libs sdl2 gl)
SRCS = $(shell find src -name '*.c')
TARGET = build/main

compile:
	@mkdir -p build
	@$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

build: compile
	@echo 'Successfully compiled SDL project! Run it using "build/main".'

run: compile
	@echo 'Successfully compiled SDL project! Executing "build/main"...'
	@./build/main
