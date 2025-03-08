FLAGS = -std=gnu11 -Wall -Wextra -pedantic -ggdb
DEFINITIONS = -D_DEBUG

qcow2img_test: qcow2img_test.c qcow2img.h zlib.h zstd.h bitstream.h utils.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@

inflate_test: inflate_test.c zlib.h zstd.h bitstream.h utils.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@

deflate_test: deflate_test.c zlib.h bitstream.h utils.h
	gcc $(FLAGS) $(DEFINITIONS) $< -o $@

zstd_tester: zstd_tester.c zstd.h bitstream.h xxhash64.h utils.h
	gcc $(FLAGS) $< -o $@
