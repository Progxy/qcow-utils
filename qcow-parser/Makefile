FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
EXTRA_FLAGS = -fsanitize=undefined -fsanitize=address -O2
# FLAGS += $(EXTRA_FLAGS)
DEFINITIONS = -D_DEBUG

qcow_test: qcow_test.c qcow_parser.h xcomp.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@

