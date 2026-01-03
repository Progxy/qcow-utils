/*
 * Copyright (C) 2025 TheProgxy <theprogxy@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#define _QCOW_PRINTING_UTILS_
#define _QCOW_UTILS_IMPLEMENTATION_
#define _QCOW_SPECIAL_TYPE_SUPPORT_
#include "./qcow_parser.h"

int contains_substr(const char* str, const char* substr) {
	for (unsigned int i = 0; str[i] != '\0'; ++i) {
		if (str[i] == substr[0]) {
			unsigned int temp = i++;
			unsigned int match_cnt = 1;
			for (unsigned int j = 1; substr[j] != '\0' && str[i] != '\0'; ++i, ++j) {
				if (str[i] == substr[j]) match_cnt++;
				else break;	
			}
			if (str_len(substr) == match_cnt) return 1;
			i = temp;
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		WARNING_LOG("Usage: %s <path to qcow image>\n", argv[0]);
		return -1;
	} else if (!contains_substr(argv[1], ".qcow")) {
		WARNING_LOG("ERROR: Only qcow files are accepted.\n");
		return -1;
	}

	const char* path_qcow = argv[1];
	
	qcow_ctx_t qcow_ctx = {0};
	if (init_qcow(&qcow_ctx, path_qcow) < 0) {
		WARNING_LOG("Failed to parse the qcow image.\n");
		return -1;
	}
	
	u64 size = 375000;
	u64 offset = 0x7000;
	u8* original_data = qcow_calloc(size, sizeof(u8));
	if (original_data == NULL) {
		WARNING_LOG("Failed to allocate original data buffer.\n");
		return 1;
	}

	DEBUG_LOG("Reading original data...\n");
	int ret = qread(original_data, size, sizeof(u8), offset, qcow_ctx);
	if (ret < 0) {
		QCOW_SAFE_FREE(original_data);
		WARNING_LOG("Failed to read %llu bytes at LBA 0x%llX, ret: %d - '%s'\n", size, offset, ret, qcow_errors_str[-ret]);
		deinit_qcow(&qcow_ctx);
		return -1;
	}
	
	u8* data = qcow_calloc(size, sizeof(u8));
	if (data == NULL) {
		QCOW_SAFE_FREE(original_data);
		WARNING_LOG("Failed to allocate original data buffer.\n");
		return 1;
	}

	data[7] = 0x07;
	data[10] = 0x10;
	data[17] = 0x17;
	data[370000] = 0x07;
	data[370010] = 0x10;
	data[370017] = 0x17;

	DEBUG_LOG("Writing new data...\n");
	ret = qwrite(data, size, sizeof(u8), offset, qcow_ctx);
	if (ret < 0) {
		QCOW_SAFE_FREE(original_data);
		QCOW_SAFE_FREE(data);
		WARNING_LOG("Failed to write %llu bytes at address 0x%llX, err: '%s'\n", size, offset, qcow_errors_str[-ret]);
		deinit_qcow(&qcow_ctx);
		return -QCOW_IO_ERROR;
	}

	DEBUG_LOG("Reading back new data...\n");
	ret = qread(data, size, sizeof(u8), offset, qcow_ctx);
	if (ret < 0) {
		QCOW_SAFE_FREE(original_data);
		QCOW_SAFE_FREE(data);
		WARNING_LOG("Failed to read %llu bytes at LBA 0x%llX, ret: %d - '%s'\n", size, offset, ret, qcow_errors_str[-ret]);
		deinit_qcow(&qcow_ctx);
		return -1;
	}
	
	DEBUG_LOG("Data, in address range (0x%llX - 0x%llX):\n", offset, offset + 32);
	for (u8 i = 0; i < 32; ++i) printf("[0x%llX]: 0x%X\n", offset + i, data[i]);

	const u64 n_off = 370000;
	DEBUG_LOG("Data, in address range (0x%llX - 0x%llX):\n", offset + n_off, offset + n_off + 32);
	for (u8 i = 0; i < 32; ++i) printf("[0x%llX]: 0x%X\n", offset + i + n_off, data[i + n_off]);

	DEBUG_LOG("Writing back original data...\n");
	ret = qwrite(original_data, size, sizeof(u8), offset, qcow_ctx);
	if (ret < 0) {
		QCOW_SAFE_FREE(original_data);
		QCOW_SAFE_FREE(data);
		WARNING_LOG("Failed to write %llu bytes at address 0x%llX, err: '%s'\n", size, offset, qcow_errors_str[-ret]);
		deinit_qcow(&qcow_ctx);
		return -QCOW_IO_ERROR;
	}
	
	QCOW_SAFE_FREE(original_data);
	QCOW_SAFE_FREE(data);

	deinit_qcow(&qcow_ctx);

	return 0;
}

