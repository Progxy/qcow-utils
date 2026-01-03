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
#include "../common/utils.h"
#include "../qcow-parser/qcow_parser.h"
#include "./crc_32.h"

typedef enum PartitionScheme { MBR = 0, GPT = 1, UNK = 2 } PartitionScheme;
const char* partition_scheme_str[] = { "MBR", "GPT", "UNK" };

typedef enum {
#define GPT_HEADER_SIGNATURE      0x5452415020494645
	GPT_REVISION_1_0            = 0x00010000,
	GPT_MIN_HEADER_SIZE         = 92,
	MBR_SIGNATURE               = 0xAA55,
	MBR_PARTITION_ENTRIES_COUNT = 4,
	MBR_EMPTY_PARTITION         = 0x00,
	MBR_GPT_PROTECTIVE          = 0xEE,
	FIRST_MBR_PARTITION_OFFSET  = 0x1BE,
	SECTOR_SIZE = 512,
} PartitionConstants;

typedef u8 guid_t[16];
typedef char part_name_t[72];

typedef struct PACKED_STRUCT {
	guid_t partition_type;
	guid_t unique_partition;
	u64 starting_lba;
	u64 ending_lba;
	u64 attributes;
	part_name_t name;
} gpt_entry_t;

typedef struct PACKED_STRUCT {
	u64 gpt_signature;
	u32 revision;
	u32 header_size;
	u32 header_crc32;
	u32 rsv_1;
	u64 my_lba;
	u64 alternate_lba;
	u64 first_usable_lba;
	u64 last_usable_lba;
	guid_t disk_guid;
	u64 partition_entry_lba;
	u32 num_of_partition_entries;
	u32 size_of_partition_entries;
	u32 partition_entry_array_crc32;
} gpt_header_t;

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
			guid_t type;
			guid_t unique;
			part_name_t name;
		} gpt_info;
	};
} partition_t;

// Utility Functions
static inline void print_guid(const guid_t g) {
    printf(
		"%02X%02X%02X%02X-"
		"%02X%02X-"
		"%02X%02X-"
		"%02X%02X-"
		"%02X%02X%02X%02X%02X%02X",
		g[3], g[2], g[1], g[0],                   /* Data1 */
		g[5], g[4],                               /* Data2 */
		g[7], g[6],                               /* Data3 */
		g[8], g[9],                               /* Data4 (first 2 bytes) */
		g[10], g[11], g[12], g[13], g[14], g[15]
	);
}

static inline u64 align(const u64 val, const u64 alignment) {
	if (val % alignment == 0) return val;
	return val + (val % alignment);
}

static inline void print_part_name(const part_name_t name) {
	for (unsigned int i = 0; i < sizeof(part_name_t); i += 2) {
		if (name[i] == 0 || name[i] == '\n') break;
		printf("%c", name[i]);
	}
	return;
}

#define get_sector_at(n, data) get_n_sector_at(n, 1, data)
static int get_n_sector_at(const unsigned int at, const unsigned int cnt, void* data) {
	if (data == NULL) return -QCOW_INVALID_PARAMETERS;

	int err = 0;
	u64 offset = at * SECTOR_SIZE;
	u64 size = cnt * SECTOR_SIZE;
	if ((err = qread(data, sizeof(u8), size, offset, default_qcow_ctx)) < 0) {
		WARNING_LOG("Failed to read %llu bytes at LBA 0x%llX, ret: %d - '%s'\n", size, offset, err, qcow_errors_str[-err]);
		return err;
	}

	if (err > 0) DEBUG_LOG("Read %d bytes out of requested %llu.\n", err, size);

	return QCOW_NO_ERROR;
}

static void print_gpt_array_entry(const gpt_entry_t entry) {
	printf("\n");
	printf("     -> Partition Type GUID:   ");
	print_guid(entry.partition_type);
	printf("\n");
	printf("     -> Unique Partition GUID: ");
	print_guid(entry.unique_partition);
	printf("\n");
	printf("     -> Starting LBA:          %llu\n", entry.starting_lba);
	printf("     -> Ending LBA:            %llu\n", entry.ending_lba);
	printf("     -> Attributes:            0x%016llX\n", entry.attributes);
	printf("     -> Partition Name:        '");
	print_part_name(entry.name);
	printf("'\n");
	return;
}

static void print_gpt_header_info(const gpt_header_t gpt_header) {
	printf("\n -- GPT Header Info --\n");
	printf("  -> GPT Signature:                '%.*s'\n", (int) sizeof(u64), (const char*) &(gpt_header.gpt_signature));	
	printf("  -> Revision:                     0x%08X\n", gpt_header.revision);
	printf("  -> Header Size:                  %u bytes\n", gpt_header.header_size);
	printf("  -> Header CRC_32:                0x%08X\n", gpt_header.header_crc32);
	printf("  -> My_LBA:                       %llu\n", gpt_header.my_lba);
	printf("  -> Alternate LBA:                %llu\n", gpt_header.alternate_lba);
	printf("  -> First Usable LBA:             %llu\n", gpt_header.first_usable_lba);
	printf("  -> Last Usable LBA:              %llu\n", gpt_header.last_usable_lba);
	printf("  -> Disk GUID:                    ");
	print_guid(gpt_header.disk_guid);
	printf("\n");
	printf("  -> Partition Entry LBA:          %llu\n", gpt_header.partition_entry_lba);
	printf("  -> Number of Partition Entries:  %u\n", gpt_header.num_of_partition_entries);
	printf("  -> Size of Partition Entries:    %u\n", gpt_header.size_of_partition_entries);
	printf("  -> Partition Entry Array CRC_32: 0x%08X\n", gpt_header.partition_entry_array_crc32);
	printf(" ------------------------- \n\n");
	return;
}

static void print_part_info(const partition_t partition) {
	printf("\n -- %s Partition Info --\n", partition_scheme_str[partition.scheme]);
	
	if (partition.scheme == UNK) {
		printf(" ------------------------- \n\n");
		return;
	}
	
	printf("  -> Start LBA:             %llu\n", partition.start_lba);
	printf("  -> Num of Sectors:        %llu\n", partition.num_sectors);
	printf("  -> Size in Bytes:         %llu\n", partition.size);
	
	if (partition.scheme == MBR) {
		printf("  -> Bootable:              %s\n", partition.mbr_info.bootable ? "YES" : "NO");
		printf("  -> Partition Type:        0x%X\n", partition.mbr_info.mbr_type);
	} else {
		printf("  -> Partition Type GUID:   ");
		print_guid(partition.gpt_info.type);
		printf("\n");
		printf("  -> Unique Partition GUID: ");
		print_guid(partition.gpt_info.unique);
		printf("\n");
		printf("  -> Partition Name:        '");
		print_part_name(partition.gpt_info.name);
		printf("'\n");
	}

	printf(" ------------------------- \n");
	return;
}

// Partition Functions
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

static int parse_gpt_entry(const u32 size_of_partition_entries, u8* entry_ptr, gpt_entry_t* entry) {
	mem_cpy(entry, entry_ptr, sizeof(gpt_entry_t));	
	entry_ptr += sizeof(gpt_entry_t);

	const u32 rem_bytes = sizeof(gpt_entry_t) % size_of_partition_entries;
	if (rem_bytes) {
		for (unsigned int i = rem_bytes; i < size_of_partition_entries; ++i) {
			if (*entry_ptr++) {
				WARNING_LOG("The end of the partition entry must be zero, as it is reserved.\n");
				return -QCOW_INVALID_GPT_ENTRY;
			}
		}
	}

	return QCOW_NO_ERROR;
}

static int parse_gpt_entry_array(const gpt_header_t gpt_header, partition_t* partitions, unsigned int* entries_cnt) {
	*entries_cnt = 0;

	const u64 size = align(gpt_header.num_of_partition_entries * gpt_header.size_of_partition_entries, SECTOR_SIZE);
	u8* entries_array_ptr = qcow_calloc(size, sizeof(u8));
	if (entries_array_ptr == NULL) {
		WARNING_LOG("Failed to allocate partition entry array buffer.\n");
		return -QCOW_IO_ERROR;
	}

	if (get_n_sector_at(gpt_header.partition_entry_lba, size / SECTOR_SIZE, entries_array_ptr) < 0) {
		QCOW_SAFE_FREE(entries_array_ptr);
		WARNING_LOG("Failed to retrieve sector %llu from given qcow file.\n", gpt_header.partition_entry_lba);
		return -QCOW_IO_ERROR;
	}
	
	u32 check_crc = crc32(entries_array_ptr, gpt_header.num_of_partition_entries * gpt_header.size_of_partition_entries);
	if (check_crc != gpt_header.partition_entry_array_crc32) {
		QCOW_SAFE_FREE(entries_array_ptr);
		WARNING_LOG("Invalid CRC32 Checksum, expected: 0x%X, but calculated: 0x%X\n", gpt_header.partition_entry_array_crc32, check_crc);
		return -QCOW_INVALID_CRC_CHECKSUM;
	}

	printf("\n -- GPT Partition Entry Array Info --\n");

	const guid_t zero = {0};
	for (unsigned int j = 0; j < gpt_header.num_of_partition_entries; ++j) {
		gpt_entry_t entry = {0};
		if (parse_gpt_entry(gpt_header.size_of_partition_entries, entries_array_ptr + j * gpt_header.size_of_partition_entries, &entry)) {
			QCOW_SAFE_FREE(entries_array_ptr);
			WARNING_LOG("Failed to parse the entry.\n");
			return -QCOW_INVALID_GPT_ENTRY;
		}
		
		if (mem_n_cmp(entry.partition_type, zero, sizeof(guid_t))) {
			printf("  -> Partition Entry %03u: ", j);
			print_gpt_array_entry(entry);
			
			partition_t* partition = partitions + *entries_cnt;
			partition -> start_lba = entry.starting_lba;
			partition -> num_sectors = entry.ending_lba - entry.starting_lba;
			partition -> size = partition -> num_sectors * SECTOR_SIZE;
			partition -> scheme = GPT;
			mem_cpy(partition -> gpt_info.type, entry.partition_type, sizeof(guid_t));
			mem_cpy(partition -> gpt_info.unique, entry.unique_partition, sizeof(guid_t));
			mem_cpy(partition -> gpt_info.name, entry.name, sizeof(part_name_t));
			
			(*entries_cnt)++;
		} 
	}
	
	printf(" ------------------------- \n\n");
	
	QCOW_SAFE_FREE(entries_array_ptr);
	
	return QCOW_NO_ERROR;
}

static int parse_gpt_header(u8* gpt_header_ptr, gpt_header_t* gpt_header) {
	mem_cpy(gpt_header, gpt_header_ptr, sizeof(gpt_header_t));
	gpt_header_ptr += sizeof(gpt_header_t);

	if (gpt_header -> gpt_signature != GPT_HEADER_SIGNATURE) {
		WARNING_LOG("Invalid GPT Header Signature: 0x%llX\n", gpt_header -> gpt_signature);
		return -QCOW_GPT_INVALID_SIGNATURE;
	}
	
	if (gpt_header -> revision != GPT_REVISION_1_0) {
		WARNING_LOG("Invalid GPT Header Revision, expected 1.0, found: 0x%08X\n", gpt_header -> revision);
		return -QCOW_GPT_INVALID_REVISION;
	}
	
	if ((gpt_header -> header_size < GPT_MIN_HEADER_SIZE) || (gpt_header -> header_size > SECTOR_SIZE)) {
		WARNING_LOG("Invalid GPT Header Revision, expected 1.0, found: 0x%08X\n", gpt_header -> revision);
		return -QCOW_GPT_INVALID_HEADER_SIZE;
	}
	
	gpt_header_t crc_gpt = {0};
	mem_cpy(&crc_gpt, gpt_header, sizeof(gpt_header_t));
	crc_gpt.header_crc32 = 0;

	u32 check_crc = crc32((const u8*) &crc_gpt, gpt_header -> header_size);
	if (check_crc != gpt_header -> header_crc32) {
		WARNING_LOG("Invalid CRC32 Checksum, expected: 0x%X, but calculated: 0x%X\n", gpt_header -> header_crc32, check_crc);
		return -QCOW_INVALID_CRC_CHECKSUM;
	}

	if (gpt_header -> rsv_1 != 0) {
		WARNING_LOG("Reserved Field must be zero: 0x%X\n", gpt_header -> rsv_1);
		return -QCOW_GPT_INVALID_RSV;
	}
	
	for (unsigned int i = sizeof(gpt_header_t); i < SECTOR_SIZE; ++i) {
		if (*gpt_header_ptr++) {
			WARNING_LOG("The end of the LBA must be zero, as it is reserved.\n");
			return -QCOW_GPT_INVALID_RSV;
		}
	}

	return QCOW_NO_ERROR;
}

static int parse_gpt_entries(const u8* gpt_sector, partition_t** partitions, unsigned int* partitions_cnt, const bool store_partitions) {
	int err = 0;
	gpt_header_t gpt_header = {0};
	if ((err = parse_gpt_header((u8*) gpt_sector, &gpt_header))) {
		WARNING_LOG("Failed to parse GPT Header - '%s'.\n", qcow_errors_str[-err]);
		return err;
	}
	
	print_gpt_header_info(gpt_header);
	
	if (store_partitions) {
		QCOW_SAFE_CALLOC(*partitions, gpt_header.num_of_partition_entries, sizeof(partition_t), -QCOW_IO_ERROR);
		
		if ((err = parse_gpt_entry_array(gpt_header, *partitions, partitions_cnt))) {
			QCOW_SAFE_FREE(*partitions);
			*partitions_cnt = 0;
			WARNING_LOG("Failed to parse GPT Entry Array - '%s'.\n", qcow_errors_str[-err]);
			return err;
		}
		
		QCOW_SAFE_REALLOC(*partitions, *partitions_cnt * sizeof(partition_t), -QCOW_IO_ERROR);
	}
	
	return QCOW_NO_ERROR;
}

static int parse_part_scheme(const u8* first_sector, const PartitionScheme partition_scheme, partition_t** partitions, unsigned int* partitions_cnt) {
	if (first_sector == NULL || partitions_cnt == NULL) return -QCOW_INVALID_PARAMETERS;
	
	*partitions_cnt = 0;
	if (partition_scheme == MBR) {
		QCOW_SAFE_CALLOC(*partitions, MBR_PARTITION_ENTRIES_COUNT, sizeof(partition_t), -QCOW_IO_ERROR);
		parse_mbr_entries(first_sector, *partitions, partitions_cnt);
		QCOW_SAFE_REALLOC(*partitions, *partitions_cnt * sizeof(partition_t), -QCOW_IO_ERROR);
		return QCOW_NO_ERROR;
	}

	// Retrieve LBA1
	u8 second_sector[SECTOR_SIZE] = {0};
	if (get_sector_at(1, second_sector) < 0) {
		WARNING_LOG("Failed to retrieve second sector from given qcow file.\n");
		return -QCOW_IO_ERROR;
	}

	int err = parse_gpt_entries(second_sector, partitions, partitions_cnt, TRUE);
	if ((-err >= QCOW_GPT_INVALID_SIGNATURE) && (-err <= QCOW_GPT_INVALID_RSV)) {
		DEBUG_LOG("Failed to parse the Primary GPT Header.\n");
	} else if (err < 0) return err;
	
	// Retrieve last LBA
	u8 last_sector[SECTOR_SIZE] = {0};
	if (get_sector_at((default_qcow_ctx.size / SECTOR_SIZE) - 1, last_sector) < 0) {
		WARNING_LOG("Failed to retrieve last sector from given qcow file.\n");
		return -QCOW_IO_ERROR;
	}
	
	err = parse_gpt_entries(last_sector, partitions, partitions_cnt, *partitions != NULL);
	if ((-err >= QCOW_GPT_INVALID_SIGNATURE) && (-err <= QCOW_GPT_INVALID_RSV)) {
		if (*partitions == NULL) {
			WARNING_LOG("Failed to parse both the Primary and Backup GPT Headers.\n");
			return err;
		}

		DEBUG_LOG("Failed to parse the Backup GPT Header.\n");
	} else if (err < 0) {
		if (*partitions == NULL) return err;
	}

	return QCOW_NO_ERROR;
}

static int parse_partitions(partition_t** partitions, unsigned int* partitions_cnt) {
	int err = 0;
	
	// Retrieve LBA0
	u8 first_sector[SECTOR_SIZE] = {0};
	if ((err = get_sector_at(0, first_sector)) < 0) {
		WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
		return err;
	}

	// Check that at LBA0, there is either a MBR or a Protective MBR partition
	if (((u16*) first_sector)[255] != MBR_SIGNATURE) return -QCOW_INVALID_DISK;
	
	PartitionScheme part_scheme = MBR;
	const u8 part_type = first_sector[FIRST_MBR_PARTITION_OFFSET + 4];
	if (part_type == MBR_GPT_PROTECTIVE) part_scheme = GPT;

	*partitions_cnt = 0;
	if ((err = parse_part_scheme(first_sector, part_scheme, partitions, partitions_cnt)) < 0) {
		WARNING_LOG("Failed to parse the partition scheme.\n");
		return err;
	}

	return QCOW_NO_ERROR;
}

#endif //_QCOW_PART_H_

