.PHONY: build

CC = gcc
CFLAGS = -Wall -g -Wno-implicit-function-declaration
LDFLAGS = $(shell pkg-config --cflags --libs sdl2 gl glew)
SRCS = $(shell find src -name '*.c')
TARGET = build/main

build:
	@mkdir -p build
	@$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)
	@echo 'Successfully compiled SDL project! Run it using "build/main".'
run:
	@mkdir -p build
	@$(CC) $(SRCS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)
	@echo 'Successfully compiled SDL project! Executing "build/main"...'
	@./build/main
