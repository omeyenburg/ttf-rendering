build:
	@gcc main.c -o main $(shell pkg-config --cflags --libs sdl2)
	@echo 'Successfully compiled SDL project. Run it using "./main".'
run:
	@gcc main.c -o main $(shell pkg-config --cflags --libs sdl2)
	@echo 'Successfully compiled SDL project. Executing "./main"...'
	@./main
