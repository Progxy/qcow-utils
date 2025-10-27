FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
DEFINITIONS = -D_DEBUG

qcow_test: qcow_test.c qcow_parser.h xcomp.h utils.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@

