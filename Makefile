.PHONY: test

test: test.c
	@gcc -o $@  $^ -lm
	@./$@
