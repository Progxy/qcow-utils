FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb 
FLAGS += -Wno-unused-parameter -Wno-unused-function
# FLAGS += -fsanitize=undefined -fsanitize=address -O2
DEFINITIONS = -D_DEBUG

qcow_fs_test: qcow_fs_test.c qcow_fs.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@
