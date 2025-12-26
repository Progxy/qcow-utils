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

#ifndef _QCOW_PART_H_
#define _QCOW_PART_H_

#define _QCOW_PRINTING_UTILS_
#define _QCOW_UTILS_IMPLEMENTATION_
#define _QCOW_SPECIAL_TYPE_SUPPORT_
#include "../qcow-parser/qcow_parser.h"

typedef enum PartitionType { MBR = 0, GPT = 1, UNK = 2 } PartitionType;
static const char* partition_type_str[] = { "MBR", "GPT", "UNKNOWN" };

static PartitionType get_part_type(u8* first_sector) {
	if (first_sector == NULL) {
		WARNING_LOG("Null pointer passed for first sector.\n");
		return UNK;
	}

	if (((u16*) first_sector)[255] == 0xAA55) return MBR;
	
	// TODO: missing gpt check
	
	return UNK;
}

static void print_part_entry(const u8* part_entry) {
	if (part_entry == NULL) return;
	
	printf("\t\tstatus: 0x%02X\n", part_entry[0]);
	
	printf("\t\tCHS address (first absolute sector): 0x");
	for (int i = 2; i >= 0; --i) printf("%02X", part_entry[1 + i]);
	printf("\n");

	printf("\t\tpartition type: 0x%X\n", part_entry[4]);
	
	printf("\t\tCHS address (last absolute sector): 0x");
	for (int i = 2; i >= 0; --i) printf("%02X", part_entry[5 + i]);
	printf("\n");
	
	printf("\t\tLBA first absolute sector: 0x%X\n", *(u32*) (part_entry + 8));

	const u32 num_sectors = *(u32*) (part_entry + 12);
	const u64 size_in_bytes = num_sectors * 512;
	printf("\t\tNum of Sectors: %u (Size in Bytes %llu)\n", num_sectors, size_in_bytes);

	return;
}

static void get_mbr_info(u8* first_sector) {
	if (first_sector == NULL) return;

	printf("\n -- MBR Info --\n");

	for (u8 i = 0; i < 4; ++i) {
		const u8* part_entry = first_sector + 0x1BE + i * 16;
		printf(" -> Partition Entry %u:\n", i);
		print_part_entry(part_entry);
	}

	printf(" -> 32-bit disk signature: 0x%04X\n", *(u32*) (first_sector + 0x1B8));
	printf(" -> copy-protection: 0x%04X\n", *(u32*) (first_sector + 0x1BC));

	printf(" -------------- \n");

	return;
}

void get_first_sector(const char* path_qcow) {
	QCowCtx qcow_ctx = {0};
	if (init_qcow(&qcow_ctx, path_qcow) < 0) {
		WARNING_LOG("Failed to parse the qcow image.\n");
		return;
	}
	
	u64 offset = 0x00;
	u64 size = 512;

	u8 data[512] = {0};
	int ret = qread(data, sizeof(u8), size, offset, qcow_ctx);
	if (ret < 0) {
		WARNING_LOG("Failed to read %llu bytes at LBA 0x%llX, ret: %d - '%s'\n", size, offset, ret, qcow_errors_str[-ret]);
		deinit_qcow(&qcow_ctx);
		return;
	}
	
	PartitionType part_type = get_part_type(data);
	DEBUG_LOG("Partition Type: '%s'\n", partition_type_str[part_type]);

	DEBUG_LOG("Data, in address range (0x%llX - 0x%llX):\n", offset, offset + size);
	for (u64 i = 0; i < size; i += 16) {
		printf("[0x%03llX:%03llX]: ", offset + i, offset + i + 16);
		for (u8 j = 0; j < 16; ++j) printf("%02X ", data[i + j]);
		printf("\n");
	}

	if (part_type == MBR) get_mbr_info(data);

	deinit_qcow(&qcow_ctx);

	return;
}

#endif //_QCOW_PART_H_

