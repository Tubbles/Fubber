flags = -g --std=c18 -lm

srcs = src/*.c

.PHONY=all
all:
	gcc $(flags) $(srcs) -o fubber

.PHONY=all
pedantic:
	gcc $(flags) -Wall -Wextra -pedantic -Werror $(srcs) -o fubber

.PHONY=format
format:
	for f in $(srcs) ; do clang-format-9 -i --style=file $$f ; done

.PHONY=run
run: all
	./fubber

