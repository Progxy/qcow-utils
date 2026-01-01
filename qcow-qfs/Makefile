FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb 
FLAGS += -Wno-unused-parameter -Wno-unused-function
# FLAGS += -fsanitize=undefined -fsanitize=address -O2
DEFINITIONS = -D_DEBUG

qcow_qfs_test: qcow_qfs_test.c qcow_qfs.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@
