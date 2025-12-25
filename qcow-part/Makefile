FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
DEFINITIONS = -D_DEBUG

qcow_part_test: qcow_part_test.c qcow_part.h utils.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@
