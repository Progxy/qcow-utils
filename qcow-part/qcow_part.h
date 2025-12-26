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

typedef enum PartitionScheme { MBR = 0, GPT = 1, UNK = 2 } PartitionScheme;

typedef struct {
    u64 start_lba;          // Absolute LBA on disk
	u64 num_sectors;        // Number of Sectors
    u64 size;               // Partition Size in bytes
    PartitionScheme scheme; // Partition Scheme

    /* Optional Metadata */
    union {
		struct {
			bool bootable;
			u8 mbr_type;
		} mbr_info;
		
		struct {
			u8 type_guid[16];
			u8 part_guid[16];
			char part_name[72];
		} gpt_info;
	};
} partition_t;

static void print_gpt_part_info(const partition_t partition) {
	(void) partition;
	return;
}

static void print_mbr_part_info(const partition_t partition) {
	printf("\n -- MBR Partition Info --\n");
	printf("  -> Bootable: %s\n", partition.mbr_info.bootable ? "YES" : "NO");
	printf("  -> Partition Type: 0x%X\n", partition.mbr_info.mbr_type);
	printf("  -> LBA First Absolute Sector: 0x%llX\n", partition.start_lba);
	printf("  -> Num of Sectors: %llu (Size in Bytes %llu)\n", partition.num_sectors, partition.size);
	printf(" ------------------------- \n\n");
	return;
}

#define MBR_PARTITION_ENTRIES_COUNT 4
#define MBR_EMPTY_PARTITION 0x00
#define MBR_GPT_PROTECTIVE  0xEE
#define FIRST_MBR_PARTITION_OFFSET 0x1BE
static void parse_mbr_entries(const u8* first_sector, partition_t* partitions, unsigned int* partitions_cnt) {
	if (first_sector == NULL || partitions == NULL || partitions_cnt == NULL) return;
	
	for (unsigned int i = 0; i < MBR_PARTITION_ENTRIES_COUNT; ++i) {
		partition_t* partition = partitions + *partitions_cnt;
		const u8* part_entry = first_sector + FIRST_MBR_PARTITION_OFFSET + i * 16;
		
		// Check if the partition entry is empty 
		const u8 part_type = part_entry[4];
		if (part_type == MBR_EMPTY_PARTITION) continue;

		partition -> scheme = MBR;
		partition -> mbr_info.bootable = QCOW_TO_BOOL(part_entry[0] & 0x80);
		partition -> mbr_info.mbr_type = part_type;
		partition -> start_lba = *(u32*) (part_entry + 8);
		partition -> num_sectors = *(u32*) (part_entry + 12);
		partition -> size = partition -> num_sectors * 512;
		
		(*partitions_cnt)++;
	}
	
	return;
}

static partition_t* parse_part_scheme(const u8* first_sector, const PartitionScheme partition_scheme, unsigned int* partitions_cnt) {
	if (first_sector == NULL || partitions_cnt == NULL) return NULL;
	
	*partitions_cnt = 0;
	partition_t* partitions = NULL;

	if (partition_scheme == MBR) {
		QCOW_SAFE_CALLOC(partitions, MBR_PARTITION_ENTRIES_COUNT, sizeof(partition_t), NULL);
		parse_mbr_entries(first_sector, partitions, partitions_cnt);
		QCOW_SAFE_REALLOC(partitions, *partitions_cnt * sizeof(partition_t), NULL);
		return partitions;
	}

	TODO("Parse GPT scheme");

	return partitions;
}

static void parse_partitions(const u8* first_sector) {
	if (first_sector == NULL) return;

	// Check that at LBA 0, there is either a MBR or a Protective MBR
	if (((u16*) first_sector)[255] != 0xAA55) return;
	
	PartitionScheme part_scheme = MBR;
	const u8 part_type = first_sector[FIRST_MBR_PARTITION_OFFSET + 4];
	if (part_type == MBR_GPT_PROTECTIVE) part_scheme = GPT;

	unsigned int partitions_cnt = 0;
	partition_t* partitions = parse_part_scheme(first_sector, part_scheme, &partitions_cnt);
	if (partitions == NULL) return;

	for (unsigned int i = 0; i < partitions_cnt; ++i) {
		if (part_scheme) print_mbr_part_info(partitions[i]);
		else print_gpt_part_info(partitions[i]);
	}

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
	
	parse_partitions(data);

	printf("Data, in address range (0x%llX - 0x%llX):\n", offset, offset + size);
	for (u64 i = 0; i < size; i += 16) {
		printf("[0x%03llX:%03llX]: ", offset + i, offset + i + 16);
		for (u8 j = 0; j < 16; ++j) printf("%02X ", data[i + j]);
		printf("\n");
	}

	deinit_qcow(&qcow_ctx);

	return;
}

#endif //_QCOW_PART_H_

