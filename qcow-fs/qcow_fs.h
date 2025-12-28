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

#ifndef _QCOW_FS_H_
#define _QCOW_FS_H_

#define _QCOW_PRINTING_UTILS_
#define _QCOW_UTILS_IMPLEMENTATION_
#define _QCOW_SPECIAL_TYPE_SUPPORT_
#include "../common/utils.h"
#include "../qcow-parser/qcow_parser.h"
#include "../qcow-part/qcow_part.h"

typedef enum { FAT12 = 0, FAT16 = 1, FAT32 = 2, NOT_FAT = 3 } FatType;
const char* fat_type_str[] = { "FAT12", "FAT16", "FAT32", "NOT_FAT" };

typedef struct PACKED_STRUCT {
	u8  jmp_boot[3];
	u64 oem_name;
	u16 bytes_per_sector;
	u8  sectors_per_cluster;
	u16 rsv_sectors_cnt;
	u8  num_fat;
	union {
		struct {
			u16 root_ent_cnt;
			u16 tot_sec16;
		};
		u32 _rsv1; // MBZ
	};
	u8  media;
	union {
		u16 fat_size16;
		u16 _rsv2; // MBZ
	};
	u16 sec_per_track; 
	u16 num_of_heads; 
	u32 hidden_sectors_cnt; 
	u32 total_sectors; 
	// FAT32 Specific Fields
	u32 fat_size; 
	struct {
		u8 active_fat_cnt: 3;
		u8 rsv: 3; // MBZ  
		u8 is_mirrored: 1;
		u8 rsv_; // MBZ
	} ext_flags;
	u8 min_fs_rev;
   	u8 maj_fs_rev;
	u32 root_cluster;
	u16 fs_info;
	u16 backup_bpb_sector;
	u8 _rsv3[12]; // MBZ
	u8 drive_num;
	u8 _rsv4; // MBZ
	u8 boot_signature;
	u32 vol_id;
	u8 vol_label[11];
	u64 fs_type; // 'FAT32   '
	u8 _rsv5[420];
	u16 signature; // 0xAA55
} bpb_sector_t;

#include <inttypes.h>
#include <ctype.h>

static void print_ascii(const char* label, const u8* buf, u64 len) {
    printf("%-22s: \"", label);
    for (u64 i = 0; i < len; i++) {
        putchar(isprint(buf[i]) ? buf[i] : '.');
    }
    printf("\"\n");
	return;
}

static void print_hex_bytes(const char* label, const u8* buf, u64 len) {
    printf("%-22s: ", label);
    for (u64 i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
	return;
}

static void bpb_pretty_print(const bpb_sector_t* bpb) {
    printf("=== FAT32 BPB Sector ===\n");

    print_hex_bytes("jmp_boot", bpb -> jmp_boot, 3);
    print_ascii("oem_name", (const u8*) &(bpb -> oem_name), 8);

    printf("%-22s: %" PRIu16 "\n", "bytes_per_sector", bpb -> bytes_per_sector);
    printf("%-22s: %" PRIu8  "\n", "sectors_per_cluster", bpb -> sectors_per_cluster);
    printf("%-22s: %" PRIu16 "\n", "rsv_sectors_cnt", bpb -> rsv_sectors_cnt);
    printf("%-22s: %" PRIu8  "\n", "num_fat", bpb -> num_fat);

    printf("%-22s: 0x%08" PRIX32 "\n", "_rsv1 (MBZ)", bpb -> _rsv1);
    printf("%-22s: 0x%02" PRIX8  "\n", "media", bpb -> media);
    printf("%-22s: 0x%04" PRIX16 "\n", "_rsv2 (MBZ)", bpb -> _rsv2);

    printf("%-22s: %" PRIu16 "\n", "sec_per_track", bpb -> sec_per_track);
    printf("%-22s: %" PRIu16 "\n", "num_of_heads", bpb -> num_of_heads);
    printf("%-22s: %" PRIu32 "\n", "hidden_sectors_cnt", bpb -> hidden_sectors_cnt);
    printf("%-22s: %" PRIu32 "\n", "total_sectors", bpb -> total_sectors);
    printf("%-22s: %" PRIu32 "\n", "fat_size", bpb -> fat_size);

    printf("ext_flags:\n");
    printf("  %-20s: %" PRIu8 "\n", "active_fat_cnt", bpb -> ext_flags.active_fat_cnt);
    printf("  %-20s: %" PRIu8 "\n", "is_mirrored", bpb -> ext_flags.is_mirrored);
    printf("  %-20s: 0x%02" PRIX8 "\n", "rsv (MBZ)", bpb -> ext_flags.rsv);
    printf("  %-20s: 0x%02" PRIX8 "\n", "rsv_ (MBZ)", bpb -> ext_flags.rsv_);

    printf("%-22s: %" PRIu8  "\n", "min_fs_rev", bpb -> min_fs_rev);
    printf("%-22s: %" PRIu8  "\n", "maj_fs_rev", bpb -> maj_fs_rev);
    printf("%-22s: %" PRIu32 "\n", "root_cluster", bpb -> root_cluster);
    printf("%-22s: %" PRIu16 "\n", "fs_info", bpb -> fs_info);
    printf("%-22s: %" PRIu16 "\n", "backup_bpb_sector", bpb -> backup_bpb_sector);

    print_hex_bytes("_rsv3 (MBZ)", bpb -> _rsv3, sizeof(bpb -> _rsv3));

    printf("%-22s: 0x%02" PRIX8 "\n", "drive_num", bpb -> drive_num);
    printf("%-22s: 0x%02" PRIX8 "\n", "_rsv4 (MBZ)", bpb -> _rsv4);
    printf("%-22s: 0x%02" PRIX8 "\n", "boot_signature", bpb -> boot_signature);
    printf("%-22s: 0x%08" PRIX32 "\n", "vol_id", bpb -> vol_id);

    print_ascii("vol_label", bpb -> vol_label, sizeof(bpb -> vol_label));
    print_ascii("fs_type", (const u8*) &(bpb -> fs_type), 8);

    printf("%-22s: [%" PRIu64 " bytes]\n", "_rsv5 (MBZ)", (long unsigned int) sizeof(bpb -> _rsv5));
    printf("%-22s: 0x%04" PRIX16 "\n", "signature", bpb -> signature);

    printf("========================\n");
	return;
}

static inline u64 first_sector_of_cluster_n(const bpb_sector_t* bpb_sector, const u64 fat_table_size, u64 n) {
	u64 root_dir_sectors = ((bpb_sector -> root_ent_cnt * 32) + (bpb_sector -> bytes_per_sector - 1));
	u64 first_data_sector = (bpb_sector -> rsv_sectors_cnt * bpb_sector -> bytes_per_sector + fat_table_size + root_dir_sectors) / SECTOR_SIZE;
	return ((n - 2) * bpb_sector -> sectors_per_cluster * (bpb_sector -> bytes_per_sector / SECTOR_SIZE)) + first_data_sector;
}

static FatType determine_fat_type(const bpb_sector_t* bpb_sector) {
	if ((bpb_sector -> bytes_per_sector == 0) || (bpb_sector -> sectors_per_cluster == 0)) return NOT_FAT;

	u64 root_dir_sectors = ((bpb_sector -> root_ent_cnt * 32) + (bpb_sector -> bytes_per_sector - 1)) / bpb_sector -> bytes_per_sector;
	u64 fat_size = (bpb_sector -> fat_size16 != 0) ? bpb_sector -> fat_size16 : bpb_sector -> fat_size;
	u64 tot_sec  = (bpb_sector -> tot_sec16  != 0) ? bpb_sector -> tot_sec16  : bpb_sector -> total_sectors;
	u64 data_sec = tot_sec - (bpb_sector -> rsv_sectors_cnt + (bpb_sector -> num_fat * fat_size) + root_dir_sectors); 
	u64 clusters_cnt = data_sec / bpb_sector -> sectors_per_cluster; 	
	
	if (clusters_cnt < 4085)       return FAT12;
	else if (clusters_cnt < 65525) return FAT16;

	return FAT32;
}

static void parse_fat(const partition_t partition) {
	bpb_sector_t bpb_sector = {0};
	if (get_sector_at(partition.start_lba, &bpb_sector)) {
		WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
		return;
	}

	bpb_pretty_print(&bpb_sector);

	FatType fat_type = determine_fat_type(&bpb_sector);
	printf("fat_type: %s\n", fat_type_str[fat_type]);
	
	if (fat_type == NOT_FAT) {
		bpb_sector_t backup_bpb_sector = {0};
		const u64 backup_bpb_offset = bpb_sector.backup_bpb_sector * (bpb_sector.bytes_per_sector / SECTOR_SIZE);
		if (get_sector_at(partition.start_lba + backup_bpb_offset, &backup_bpb_sector)) {
			WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
			return;
		}
		
		bpb_pretty_print(&backup_bpb_sector);

		fat_type = determine_fat_type(&backup_bpb_sector);
		printf("fat_type: %s\n", fat_type_str[fat_type]);
	}

	for (u32 j = 0; j < bpb_sector.num_fat; ++j) {
		u64 fat_table_size = bpb_sector.bytes_per_sector * bpb_sector.fat_size;
		u8* fat_table = qcow_calloc(fat_table_size, sizeof(u8));
		if (fat_table == NULL) {
			WARNING_LOG("Failed to allocate fat table buffer.\n");
			return;
		}

		const u64 fat_region_offset = bpb_sector.rsv_sectors_cnt * (bpb_sector.bytes_per_sector / SECTOR_SIZE) + (j * fat_table_size / SECTOR_SIZE);
		if (get_n_sector_at(partition.start_lba + fat_region_offset, fat_table_size / SECTOR_SIZE, fat_table)) {
			QCOW_SAFE_FREE(fat_table);
			WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
			return;
		}

#define CLNSHUTBITMASK 0x08000000
#define HRDERRBITMASK  0x04000000 

		u32 entry_1 = ((u32*) fat_table)[1];
		if ((entry_1 & CLNSHUTBITMASK) == 0) WARNING_LOG("Dirty Volume.\n");
		if ((entry_1 & HRDERRBITMASK) == 0)  WARNING_LOG("I/O Disk contains faults from previous unsuccessfull unmount operation.\n");

		printf(" -- FAT Table %u -- \n", j);
		for (u64 i = 0; i < fat_table_size / 4; ++i) {
			u32 fat_entry = ((u32*) fat_table)[i];
			printf("Entry %llu: 0x%X\n", i, fat_entry);
			if (fat_entry == 0) break;
		}
		printf(" ----------------- \n");

		QCOW_SAFE_FREE(fat_table);
	}

	return;
}

#endif //_QCOW_FS_H_

