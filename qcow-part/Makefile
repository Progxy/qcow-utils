FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
EXTRA_FLAGS = -fsanitize=undefined -fsanitize=address -O2
# FLAGS += $(EXTRA_FLAGS)
DEFINITIONS = -D_DEBUG

qcow_part_test: qcow_part_test.c qcow_part.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@
