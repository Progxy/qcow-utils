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

typedef struct PACKED_STRUCT {
	u8  jmp_boot[3];
	u64 oem_name;
	u16 bytes_per_sector;
	u8  sectors_per_cluster;
	u16 rsv_sectors_cnt;
	u8  num_fat;
	u32 _rsv1; // MBZ
	u8  media;
	u16 _rsv2; // MBZ
	u16 sec_per_track; 
	u16 num_of_heads; 
	u32 hidden_sectors_cnt; 
	u32 total_sectors; 
	u32 sector_per_fat; 
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
	u16 boot_record_sector;
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

    print_hex_bytes("jmp_boot", bpb->jmp_boot, 3);
    print_ascii("oem_name", (const u8 *)&bpb->oem_name, 8);

    printf("%-22s: %" PRIu16 "\n", "bytes_per_sector", bpb->bytes_per_sector);
    printf("%-22s: %" PRIu8  "\n", "sectors_per_cluster", bpb->sectors_per_cluster);
    printf("%-22s: %" PRIu16 "\n", "rsv_sectors_cnt", bpb->rsv_sectors_cnt);
    printf("%-22s: %" PRIu8  "\n", "num_fat", bpb->num_fat);

    printf("%-22s: 0x%08" PRIX32 "\n", "_rsv1 (MBZ)", bpb->_rsv1);
    printf("%-22s: 0x%02" PRIX8  "\n", "media", bpb->media);
    printf("%-22s: 0x%04" PRIX16 "\n", "_rsv2 (MBZ)", bpb->_rsv2);

    printf("%-22s: %" PRIu16 "\n", "sec_per_track", bpb->sec_per_track);
    printf("%-22s: %" PRIu16 "\n", "num_of_heads", bpb->num_of_heads);
    printf("%-22s: %" PRIu32 "\n", "hidden_sectors_cnt", bpb->hidden_sectors_cnt);
    printf("%-22s: %" PRIu32 "\n", "total_sectors", bpb->total_sectors);
    printf("%-22s: %" PRIu32 "\n", "sector_per_fat", bpb->sector_per_fat);

    printf("ext_flags:\n");
    printf("  %-20s: %" PRIu8 "\n", "active_fat_cnt", bpb->ext_flags.active_fat_cnt);
    printf("  %-20s: %" PRIu8 "\n", "is_mirrored", bpb->ext_flags.is_mirrored);
    printf("  %-20s: 0x%02" PRIX8 "\n", "rsv (MBZ)", bpb->ext_flags.rsv);
    printf("  %-20s: 0x%02" PRIX8 "\n", "rsv_ (MBZ)", bpb->ext_flags.rsv_);

    printf("%-22s: %" PRIu8  "\n", "min_fs_rev", bpb->min_fs_rev);
    printf("%-22s: %" PRIu8  "\n", "maj_fs_rev", bpb->maj_fs_rev);
    printf("%-22s: %" PRIu32 "\n", "root_cluster", bpb->root_cluster);
    printf("%-22s: %" PRIu16 "\n", "fs_info", bpb->fs_info);
    printf("%-22s: %" PRIu16 "\n", "boot_record_sector", bpb->boot_record_sector);

    print_hex_bytes("_rsv3 (MBZ)", bpb->_rsv3, sizeof(bpb->_rsv3));

    printf("%-22s: 0x%02" PRIX8 "\n", "drive_num", bpb->drive_num);
    printf("%-22s: 0x%02" PRIX8 "\n", "_rsv4 (MBZ)", bpb->_rsv4);
    printf("%-22s: 0x%02" PRIX8 "\n", "boot_signature", bpb->boot_signature);
    printf("%-22s: 0x%08" PRIX32 "\n", "vol_id", bpb->vol_id);

    print_ascii("vol_label", bpb->vol_label, sizeof(bpb->vol_label));
    print_ascii("fs_type", (const u8 *)&bpb->fs_type, 8);

    printf("%-22s: [%" PRIu64 " bytes]\n", "_rsv5 (MBZ)", (u64)sizeof(bpb->_rsv5));
    printf("%-22s: 0x%04" PRIX16 "\n", "signature", bpb->signature);

    printf("========================\n");
	return;
}

static void parse_fat32(const partition_t partition) {
	u8 bpb_sector[SECTOR_SIZE] = {0};
	DEBUG_LOG("Start LBA: %llu\n", partition.start_lba);
	if (get_sector_at(partition.start_lba, bpb_sector)) {
		WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
		return;
	}
    
	DEBUG_LOG("Data:\n");
    for (u64 i = 0; i < SECTOR_SIZE; i += 16) {
		printf("[0x%03llX:%03llX]: ", i, i + 16);
		for (u8 j = 0; j < 16; ++j) printf("%02X ", bpb_sector[i + j]);
		printf("\n");	
	}

	bpb_sector_t bpb_sec = {0};
	mem_cpy(&bpb_sec, bpb_sector, sizeof(bpb_sector_t));
	
	bpb_pretty_print(&bpb_sec);

	return;
}

#endif //_QCOW_FS_H_

