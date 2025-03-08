#include <stdio.h>
#include <stdlib.h>

#include "./utils.h"
#include "./zlib.h"

static const char zlib_data[] = "This is a test string, DEFLATE.";

int main(void) {
	int err = 0;
	
	printf("ZLib test string: '%s'\n", zlib_data);

	unsigned int zlib_compressed_data_length = 0;
	unsigned char* zlib_deflate_test = (unsigned char*) calloc(sizeof(zlib_data), sizeof(unsigned char));
	mem_cpy(zlib_deflate_test, zlib_data, sizeof(zlib_data));
	unsigned char* zlib_compressed_data = zlib_deflate((unsigned char*) zlib_deflate_test, sizeof(zlib_data), &zlib_compressed_data_length, &err);
	if (err) {
		printf(COLOR_STR("ZLIB_ERROR::%s: ", RED) "%s", zlib_errors_str[-err], zlib_compressed_data);
		return -1;
	}
	
	printf("ZLIB compressed from %lu -> %u bytes.\n", sizeof(zlib_data), zlib_compressed_data_length);

	unsigned int zlib_decompressed_data_length = 0;
	unsigned char* zlib_decompressed_data = zlib_inflate(zlib_compressed_data, zlib_compressed_data_length, &zlib_decompressed_data_length, &err);
	if (err) {
		printf(COLOR_STR("ZLIB_ERROR::%s: ", RED) "%s", zlib_errors_str[-err], zlib_decompressed_data);
		return -1;
	}

	if (str_n_cmp(zlib_data, (const char*) zlib_decompressed_data, MIN(sizeof(zlib_data), zlib_decompressed_data_length)) != 0) {
		printf(COLOR_STR("ERROR: ", RED) "Failed to decompress/compress the string.\n");
		printf("Original String: '%s'\n", zlib_data);
		printf("String After   : '%.*s'\n", zlib_decompressed_data_length, zlib_decompressed_data);
		free(zlib_decompressed_data);
		return -1;
	}
	
	printf("ZLib test string: '%.*s'\n", zlib_decompressed_data_length, zlib_decompressed_data);
	free(zlib_decompressed_data);
	
	return 0;
}
