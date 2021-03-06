flags = -g --std=c18 -lm

modules = $(wildcard modules/*)
incs = src/*.h
srcs = src/*.c

flags += $(patsubst %,-I%,$(modules))
# srcs += $(patsubst %,%/*.c,$(modules))
srcs += modules/str/str.c


.PHONY=all
all:
	gcc $(flags) -O2 $(srcs) -o fubber

.PHONY=debug
debug:
	gcc $(flags) $(srcs) -o fubber

.PHONY=pedantic
pedantic:
	gcc $(flags) -Wall -Wextra -pedantic -Werror $(srcs) -o fubber

.PHONY=format
format:
	for f in $(incs) $(srcs) ; do clang-format-9 -i --style=file $$f ; done

.PHONY=run
run: all
	./fubber
