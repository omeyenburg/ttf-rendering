.PHONY: build


build:
	@mkdir -p build
	@gcc src/main.c -o build/main $(shell pkg-config --cflags --libs sdl2)
	@echo 'Successfully compiled SDL project! Run it using "build/main".'
run:
	@mkdir -p build
	@gcc src/main.c -o build/main $(shell pkg-config --cflags --libs sdl2)
	@echo 'Successfully compiled SDL project! Executing "build/main"...'
	@./build/main
