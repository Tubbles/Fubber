flags = -g --std=c18 -lm

.PHONY=all
all:
	gcc $(flags) main.c -o fubber

.PHONY=all
pedantic:
	gcc $(flags) -Wall -Wextra -pedantic -Werror main.c -o fubber

.PHONY=format
format:
	clang-format-9 -i --style=file main.c

.PHONY=run
run: all
	./fubber

