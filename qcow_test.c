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
#include <stdint.h>
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
		printf("Usage: %s <path to qcow image>\n", argv[0]);
		return -1;
	} else if (!contains_substr(argv[1], ".qcow")) {
		printf("ERROR: Only qcow files are accepted.\n");
		return -1;
	}

	const char* path_qcow = argv[1];
	
	QCowMetadata qcow_metadata = {0};
	if (parse_qcow(&qcow_metadata, path_qcow) < 0) {
		printf("Failed to parse the qcow image.\n");
		return -1;
	}
	
	uint64_t offset = 0x7000;
	unsigned char data[32] = {0};
	data[7] = 0x07;
	data[10] = 0x10;
	int ret = qwrite(data, sizeof(unsigned char), 32, offset, qcow_metadata);
	if (ret < 0) {
		WARNING_LOG("Failed to write %u bytes at address 0x%lX, err: '%s'\n", 32, offset, qcow_errors_str[-ret]);
		deallocate_qcow_metadata(&qcow_metadata);
		return -QCOW_IO_ERROR;
	}

	ret = qread(data, sizeof(unsigned char), 32, offset, qcow_metadata);
	if (ret < 0) {
		WARNING_LOG("Failed to read %u bytes at LBA 0x%lX, ret: %d - '%s'\n", 32, offset, ret, qcow_errors_str[-ret]);
		deallocate_qcow_metadata(&qcow_metadata);
		return -1;
	}
	
	DEBUG_LOG("Data, in address range (0x%lX - 0x%lX):\n", offset, offset + 32);
	for (unsigned char i = 0; i < 32; ++i) printf("[0x%lX]: 0x%X\n", offset + i, data[i]);

	deallocate_qcow_metadata(&qcow_metadata);

	return 0;
}
