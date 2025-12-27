FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
EXTRA_FLAGS = -fsanitize=undefined -fsanitize=address -O2
# FLAGS += $(EXTRA_FLAGS)
DEFINITIONS = -D_DEBUG

qcow_fs_test: qcow_fs_test.c qcow_fs.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@
