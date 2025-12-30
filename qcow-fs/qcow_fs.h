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

#include <stddef.h>

typedef enum { FAT12 = 0, FAT16 = 1, FAT32 = 2, NOT_FAT = 3 } FatType;
const char* fat_type_str[] = { "FAT12", "FAT16", "FAT32", "NOT_FAT" };

typedef enum { 
	CLNSHUTBITMASK     = 0x08000000,
	HRDERRBITMASK      = 0x04000000,
	FAT_SIGNATURE      = 0xAA55,
	MAX_FAT_NAME       = 256,
	FAT_ATTR_READ_ONLY = 0x01,
	FAT_ATTR_HIDDEN    = 0x02,
	FAT_ATTR_SYSTEM    = 0x04,
	FAT_ATTR_VOLUME_ID = 0x08,
	FAT_ATTR_DIRECTORY = 0x10,
	FAT_ATTR_ARCHIVE   = 0x20,
	FAT_ATTR_LFN       = 0x0F,
} QCowFsConstants;

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
		u8 active_fat_idx: 3;
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

typedef struct PACKED_STRUCT {
    u8  name[8];          // 0x00: Filename (space-padded)
    u8  ext[3];           // 0x08: Extension (space-padded)
    u8  attr;             // 0x0B: Attributes
    u8  nt_reserved;      // 0x0C: Reserved for NT
    u8  ctime_tenths;     // 0x0D: Creation time (tenths of sec)
    u16 ctime;            // 0x0E: Creation time
    u16 cdate;            // 0x10: Creation date
    u16 adate;            // 0x12: Last access date
    u16 cluster_high;     // 0x14: High 16 bits of first cluster (FAT32)
    u16 mtime;            // 0x16: Last modification time
    u16 mdate;            // 0x18: Last modification date
    u16 cluster_low;      // 0x1A: Low 16 bits of first cluster
    u32 size;             // 0x1C: File size in bytes
} fat_dir_entry_t;

typedef struct PACKED_STRUCT {
    u8  order;            // 0x00: Sequence number (bit 6 = last)
    u16 name1[5];         // 0x01: UTF-16 chars (1–5)
    u8  attr;             // 0x0B: Always 0x0F
    u8  type;             // 0x0C: Always 0
    u8  checksum;         // 0x0D: Checksum of short name
    u16 name2[6];         // 0x0E: UTF-16 chars (6–11)
    u16 zero;             // 0x1A: Always 0
    u16 name3[2];         // 0x1C: UTF-16 chars (12–13)
} fat_lfn_entry_t;

STATIC_ASSERT(sizeof(bpb_sector_t) == 512,   "BPB size mismatch");
STATIC_ASSERT(sizeof(fat_dir_entry_t) == 32, "FAT dir entry size mismatch");
STATIC_ASSERT(sizeof(fat_lfn_entry_t) == 32, "FAT LFN entry size mismatch");

#define RECENT_NODES_CNT 16
typedef struct {
	u64 start_lba;
	FatType fat_type;
	bpb_sector_t bpb_sector;
	u32** fat_tables;
	u8 fat_tables_cnt;
	u64 fat_table_size;

	/* fs_node_t recent_nodes[RECENT_NODES_CNT]; */
	/* u8 recent_nodes_idx; */

} fs_ctx;

// For when start supporting multiple FS, the common and a union of ctxs should
// be put here
/* typedef struct { */
/* 	u64 start_lba; */
/* 	/1* FSType fs_type; *1/ */
/* 	fs_ctx* fs_ctx; */
/* } fs_t; */

typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hours;
	u8 mins;
	u8 secs;
} fs_time_t;

typedef fs_ctx fs_t;
typedef fs_ctx fs_dir_t;
typedef fs_ctx fs_dirent;
typedef fs_ctx fs_stat_t;

typedef struct fs_node_t {
	u64 size;
	u32 cluster;
	u8  attributes;
	char name[MAX_FAT_NAME];
	fs_time_t creation_time;
	fs_time_t access_time;
	fs_time_t modification_time;
	struct fs_node_t* next_node;
	struct fs_node_t* prev_node;
} fs_node_t;

typedef struct {
	u64 size;
	u64 offset;
	u8  attributes;
	fs_node_t node;
	char name[MAX_FAT_NAME];
} fs_file_t;

// API (Stil in Progress)
int fs_mount(const partition_t partition, fs_t* fs);
void fs_unmount(fs_t* fs);
int fs_open(fs_t* fs, const char* path, fs_file_t* file);
int fs_close(fs_t* fs, fs_file_t* file);
int fs_opendir(fs_t* fs, const char* path, fs_dir_t* dir);
int fs_stat(fs_t* fs, const char* path, fs_stat_t* st);
int fs_lookup(fs_t* fs, const fs_node_t* dir, const char* name, fs_node_t* node);
int fs_read(fs_file_t* file, void* buf, const int size);
int fs_write(fs_file_t* file, const void* buf, const int size);
int fs_seek(fs_file_t* file, const u64 off, const int whence);
int fs_readdir(fs_dir_t* dir, fs_dirent* ent);

#include <inttypes.h>
#include <ctype.h>

static void print_ascii(const char* label, const u8* buf, u64 len) {
    printf("%-22s: \"", label);
    for (u64 i = 0; i < len; i++) putchar(isprint(buf[i]) ? buf[i] : '.');
    printf("\"\n");
	return;
}

static void print_hex_bytes(const char* label, const u8* buf, u64 len) {
    printf("%-22s: ", label);
    for (u64 i = 0; i < len; i++) printf("%02X ", buf[i]);
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
    printf("  %-20s: %" PRIu8 "\n", "active_fat_idx", bpb -> ext_flags.active_fat_idx);
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

static void print_attr(const u8 attr) {
    printf("ATTR: [");
	if (attr & FAT_ATTR_READ_ONLY) printf(" R");
	if (attr & FAT_ATTR_HIDDEN)    printf(" H");
	if (attr & FAT_ATTR_SYSTEM)    printf(" S");
	if (attr & FAT_ATTR_VOLUME_ID) printf(" V");
	if (attr & FAT_ATTR_DIRECTORY) printf(" D");
	if (attr & FAT_ATTR_ARCHIVE)   printf(" A");
    printf(" ] - 0x%X", attr);
	return;
}

static void fat_print_dir_entry(const fat_dir_entry_t* e) {
    char name[9] = {0};
    char ext[4] = {0};

    /* Copy and trim name */
    for (int i = 0; i < 8; i++) name[i] = (e -> name[i] == ' ') ? '\0' : e -> name[i];
    name[8] = '\0';

    /* Copy and trim extension */
    for (int i = 0; i < 3; i++) ext[i] = (e -> ext[i] == ' ') ? '\0' : e -> ext[i];
    ext[3] = '\0';

    /* Combine cluster */
    u32 first_cluster = (((u32) e -> cluster_high) << 16) | e -> cluster_low;

    printf("-------------\n");
    printf("FAT DIR ENTRY\n");
    printf("-------------\n");

    if (ext[0]) printf("Name        : '%s.%s'\n", name, ext);
    else        printf("Name        : '%s'\n", name);

    print_attr(e -> attr);
    printf("\n");

    printf("FirstCluster: %u (0x%08X)\n", first_cluster, first_cluster);
    printf("Size        : %u bytes\n", e -> size);

    printf("Create Time : %04u-%02u-%02u %02u:%02u:%02u.%u\n",
        ((e -> cdate >> 9) & 0x7F) + 1980,
        (e -> cdate >> 5) & 0x0F,
        e -> cdate & 0x1F,
        (e -> ctime >> 11) & 0x1F,
        (e -> ctime >> 5) & 0x3F,
        (e -> ctime & 0x1F) * 2,
        e -> ctime_tenths * 10
    );

    printf("Modify Time : %04u-%02u-%02u %02u:%02u:%02u\n",
        ((e -> mdate >> 9) & 0x7F) + 1980,
        (e -> mdate >> 5) & 0x0F,
        e -> mdate & 0x1F,
        (e -> mtime >> 11) & 0x1F,
        (e -> mtime >> 5) & 0x3F,
        (e -> mtime & 0x1F) * 2
    );

    printf("Last Access Time : %04u-%02u-%02u\n",
        ((e -> adate >> 9) & 0x7F) + 1980,
        (e -> adate >> 5) & 0x0F,
        e -> adate & 0x1F
    );

    printf("\n");

	return;
}

static inline u64 first_sector_of_cluster_n(const fs_t* fs, u64 n) {
	u64 first_data_sector = (fs -> bpb_sector.rsv_sectors_cnt * fs -> bpb_sector.bytes_per_sector + fs -> bpb_sector.num_fat * fs -> fat_table_size) / SECTOR_SIZE;
	return ((n - 2) * fs -> bpb_sector.sectors_per_cluster * (fs -> bpb_sector.bytes_per_sector / SECTOR_SIZE)) + first_data_sector;
}

static FatType determine_fat_type(const bpb_sector_t* bpb_sector) {
	if ((bpb_sector -> bytes_per_sector == 0) || (bpb_sector -> sectors_per_cluster == 0)) return NOT_FAT;

	u64 root_dir_sectors = (bpb_sector -> root_ent_cnt * 32 + bpb_sector -> bytes_per_sector) / bpb_sector -> bytes_per_sector;
	u64 fat_size = (bpb_sector -> fat_size16 != 0) ? bpb_sector -> fat_size16 : bpb_sector -> fat_size;
	u64 tot_sec  = (bpb_sector -> tot_sec16  != 0) ? bpb_sector -> tot_sec16  : bpb_sector -> total_sectors;
	u64 data_sec = tot_sec - (bpb_sector -> rsv_sectors_cnt + (bpb_sector -> num_fat * fat_size) + root_dir_sectors); 
	u64 clusters_cnt = data_sec / bpb_sector -> sectors_per_cluster; 	
	
	if (clusters_cnt < 4085)       return FAT12;
	else if (clusters_cnt < 65525) return FAT16;

	return FAT32;
}

/* static void fat_walker(const u64 start_lba, const bpb_sector_t* bpb_sector, const u64 fat_table_size, const u64 cluster_idx) { */
/* 	const u64 cluster_size = bpb_sector -> bytes_per_sector * bpb_sector -> sectors_per_cluster; */
/* 	const u64 cluster_offset = first_sector_of_cluster_n(bpb_sector, fat_table_size, cluster_idx); */
	
/* 	// TODO: First should check if the current cluster_idx is available/valid within the File Allocation Table */
/* 	DEBUG_LOG("Walking cluster %llu - offset %llu\n", cluster_idx, cluster_offset); */
/* 	u8 cluster[SECTOR_SIZE * 8] = {0}; */
/* 	if (get_n_sector_at(start_lba + cluster_offset, cluster_size / SECTOR_SIZE, cluster)) { */
/* 		WARNING_LOG("Failed to retrieve cluster %llu from qcow file.\n", cluster_idx); */
/* 		return; */
/* 	} */
	
/* 	for (u64 i = 0, j = 0; i < cluster_size; ++j, i += sizeof(fat_dir_entry_t)) { */
/* 		const fat_dir_entry_t entry = ((fat_dir_entry_t*) cluster)[j]; */
/* 		if ((entry.name)[0] == 0) break; */

/* 		if (entry.attr & FAT_ATTR_LFN) DEBUG_LOG("entry has LFN attr.\n"); */
/* 		else fat_print_dir_entry(&entry); */
		
/* 		if ((entry.attr & FAT_ATTR_DIRECTORY) && ((entry.name)[0] != '.')) { */
/* 			DEBUG_LOG("Entering sub directory.\n"); */
/* 			// TODO: First should check if the cluster is available/valid within the File Allocation Table */
/* 			const u32 entry_cluster = ((u32) entry.cluster_high << 16) | entry.cluster_low; */
/* 			fat_walker(start_lba, bpb_sector, fat_table_size, entry_cluster); */
/* 		} else if (entry.attr & FAT_ATTR_ARCHIVE) { */
/* 			const u64 used_clusters = align(entry.size, bpb_sector -> bytes_per_sector) / bpb_sector -> bytes_per_sector; */
/* 			DEBUG_LOG("entry has ARCHIVE attr, and spans for %llu clusters.\n", used_clusters); */
/* 		} */
/* 	} */

/* 	return; */
/* } */

static bool is_contained_in(const u16 val, const u16 values_cnt, const u16* values) {
	for (u64 i = 0; i < values_cnt; ++i) {
		if (val == values[i]) return TRUE;
	}
	return FALSE;
}

static bool is_valid_jmp_boot(const u8* jmp_boot) {
	if (jmp_boot == NULL) return FALSE;
	if (jmp_boot[0] == 0xEB && jmp_boot[2] == 0x90) return TRUE;
	if (jmp_boot[0] == 0xE9) return TRUE;
	return FALSE;
}

static int is_valid_fat_fs(const bpb_sector_t* bpb_sector) {
	if (!is_valid_jmp_boot(bpb_sector -> jmp_boot)) {
		WARNING_LOG("Invalid jump boot field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	} 
	
	const u16 bps_values[] = { 512, 1024, 2048, 4096 };
   	if (!is_contained_in(bpb_sector -> bytes_per_sector, QCOW_ARR_SIZE(bps_values), bps_values)) {	
		WARNING_LOG("Invalid bytes per sector field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	const u16 spc_values[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
   	if (!is_contained_in(bpb_sector -> sectors_per_cluster, QCOW_ARR_SIZE(spc_values), spc_values)) {	
		WARNING_LOG("Invalid bytes per sector field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}
	
	if (bpb_sector -> rsv_sectors_cnt == 0) {
		WARNING_LOG("Invalid reserved sectors count field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> num_fat == 0) {
		WARNING_LOG("Invalid num fat field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> root_ent_cnt != 0) {
		WARNING_LOG("As we do not support other than FAT32, this field should be zero.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> tot_sec16 != 0) {
		WARNING_LOG("As we do not support other than FAT32, this field should be zero.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	const u16 media_values[] = { 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };
   	if (!is_contained_in(bpb_sector -> media, QCOW_ARR_SIZE(media_values), media_values)) {	
		WARNING_LOG("Invalid media field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> fat_size16 != 0) {
		WARNING_LOG("As we do not support other than FAT32, this field should be zero.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}
	
	// FAT32 Specific Checks
	if (bpb_sector -> total_sectors == 0) {
		WARNING_LOG("Invalid total sectors field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> fat_size == 0) {
		WARNING_LOG("Invalid fat size field.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> ext_flags.active_fat_idx != 0 && bpb_sector -> ext_flags.is_mirrored) {
		WARNING_LOG("Active FAT index should only be set and used if mirroring is disabled.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> maj_fs_rev || bpb_sector -> min_fs_rev) {
		WARNING_LOG("Version must be 0x00.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> root_cluster < 2) {
		WARNING_LOG("The root cluster must be 2 or the first usable cluster.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> backup_bpb_sector != 0 && bpb_sector -> backup_bpb_sector != 6) {
		WARNING_LOG("Backup BPB Sector must be either 0 (not present) or 6 (present).\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	if (bpb_sector -> drive_num != 0x00 && bpb_sector -> drive_num != 0x80) {
		WARNING_LOG("Drive Number must be either 0x00 or 0x80.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}
	
	if (bpb_sector -> signature != FAT_SIGNATURE) {
		WARNING_LOG("Invalid FAT Signature.\n");
		return -QCOW_INVALID_BPB_SECTOR;
	}

	return QCOW_NO_ERROR;
}

static int parse_fat_fs(fs_ctx* fs) {
	mem_set(&(fs -> bpb_sector), 0, sizeof(bpb_sector_t));
	if (get_sector_at(fs -> start_lba, &(fs -> bpb_sector))) {
		WARNING_LOG("Failed to retrieve the BPB sector from given qcow file.\n");
		return -QCOW_IO_ERROR;
	}
	
	int err = is_valid_fat_fs(&(fs -> bpb_sector));
	if (err < 0 && fs -> bpb_sector.backup_bpb_sector == 6) {
		const u64 backup_bpb_offset = fs -> bpb_sector.backup_bpb_sector * (fs -> bpb_sector.bytes_per_sector / SECTOR_SIZE);
		if (get_sector_at(fs -> start_lba + backup_bpb_offset, &(fs -> bpb_sector))) {
			WARNING_LOG("Failed to retrieve backup BPB sector from given qcow file.\n");
			return -QCOW_IO_ERROR;
		}

		int err = is_valid_fat_fs(&(fs -> bpb_sector));
		if (err < 0) {
			WARNING_LOG("Failed to parse FAT partition.\n");
			return err;
		}
	} 
	
	bpb_pretty_print(&(fs -> bpb_sector));

	fs -> fat_type = determine_fat_type(&(fs -> bpb_sector));
	DEBUG_LOG("FAT Type: %s\n", fat_type_str[fs -> fat_type]);
	if (fs -> fat_type != FAT32) {
		WARNING_LOG("Currently we do not support FAT12/FAT16, but only FAT32.\n");
		return -QCOW_UNSUPPORTED_FAT_FORMAT;
	}

	fs -> fat_tables_cnt = fs -> bpb_sector.num_fat;
	fs -> fat_table_size = fs -> bpb_sector.bytes_per_sector * fs -> bpb_sector.fat_size;
	fs -> fat_tables = qcow_calloc(fs -> fat_tables_cnt, sizeof(u32*));
	if (fs -> fat_tables == NULL) {
		WARNING_LOG("Failed to allocate buffer for fat tables.\n");
		return -QCOW_IO_ERROR;
	}

	for (u32 j = 0; j < fs -> fat_tables_cnt; ++j) {
		(fs -> fat_tables)[j] = qcow_calloc(fs -> fat_table_size, sizeof(u8));
		if ((fs -> fat_tables)[j] == NULL) {
			for (u8 z = 0; z < j; ++z) QCOW_SAFE_FREE((fs -> fat_tables)[z]);
			WARNING_LOG("Failed to allocate fat table buffer.\n");
			return -QCOW_IO_ERROR;
		}

		const u64 fat_region_offset = fs -> bpb_sector.rsv_sectors_cnt * (fs -> bpb_sector.bytes_per_sector / SECTOR_SIZE) + (j * fs -> fat_table_size / SECTOR_SIZE);
		if (get_n_sector_at(fs -> start_lba + fat_region_offset, fs -> fat_table_size / SECTOR_SIZE, fs -> fat_tables[j])) {
			for (u8 z = 0; z <= j; ++z) QCOW_SAFE_FREE((fs -> fat_tables)[z]);
			WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
			return -QCOW_IO_ERROR;
		}

		u32 entry_1 = (fs -> fat_tables)[j][1];
		// TODO: Should we care about those?
		if ((entry_1 & CLNSHUTBITMASK) == 0) WARNING_LOG("Dirty Volume.\n");
		if ((entry_1 & HRDERRBITMASK) == 0)  WARNING_LOG("I/O Disk contains faults from previous unsuccessfull unmount operation.\n");
	}

	fs -> root.size    = 0;
	fs -> root.cluster = fs -> bpb_sector.root_cluster;
	fs -> attributes   = FAT_ATTR_VOLUME_ID;
	(fs -> name)[0]    = '/';
	fs -> prev_node    = NULL;
	fs -> next_node    = NULL;

	return QCOW_NO_ERROR;
}

// Exposed API Functions
int fs_mount(const partition_t partition, fs_t* fs) {
	fs -> start_lba = partition.start_lba;

	int err = parse_fat_fs(fs);
	if (err == 0) {
		DEBUG_LOG("Partition successfully mounted.\n");
		return QCOW_NO_ERROR;
	} else if (-err == QCOW_IO_ERROR) {
		WARNING_LOG("Failed to parse the FAT partition.\n");
		return err;
	} 
	
	TODO("try parsing other fs.");
	return -QCOW_TODO;
}

void fs_unmount(fs_t* fs) {
	for (u8 i = 0; i < fs -> fat_tables_cnt; ++i) QCOW_SAFE_FREE((fs -> fat_tables)[i]);
	QCOW_SAFE_FREE(fs -> fat_tables);
	DEBUG_LOG("Partition successfully unmounted.\n");
	return; 
}

int fs_open(fs_t* fs, const char* path, fs_file_t* file) {
	u64 path_len = str_len(path);
	char* sub_path = qcow_calloc(path_len, sizeof(char));
	if (sub_path == NULL) return -QCOW_IO_ERROR;

	fs_node_t node = fs -> root;

	int err = 0;
	unsigned int prev_tok = 0;
	for (unsigned int i = 0; i < path_len; ++i) {
		int tok = str_tok(path + i, "/");
		if (tok == -1) tok = path_len;
		mem_cpy(sub_path, path + i, tok - prev_tok);
		if ((err = fs_lookup(fs, &node, sub_path, &node)) < 0) {
			QCOW_SAFE_FREE(sub_path);
			return err;
		}
		prev_tok = tok;
	}
	
	QCOW_SAFE_FREE(sub_path);
	
	file -> node = node;
	file -> size = node.size;
	file -> offset = 0;
	file -> attributes = node.attributes;
	mem_cpy(file -> name, node.name, MAX_FAT_NAME);
		
	return QCOW_NO_ERROR;
}

int fs_opendir(fs_t* fs, const char* path, fs_dir_t* out) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_stat(fs_t* fs, const char* path, fs_stat_t* st) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_lookup(fs_t* fs, const fs_node_t* dir, const char* name, fs_node_t* out) {
	const u64 cluster_size = fs -> bpb_sector.bytes_per_sector * bpb_sector -> sectors_per_cluster;
	const u64 cluster_offset = first_sector_of_cluster_n(fs, dir -> cluster);
	
	// TODO: First should check if the current cluster_idx is available/valid within the File Allocation Table
	DEBUG_LOG("Walking cluster %llu - offset %llu\n", dir -> cluster, cluster_offset);
	u8 cluster[SECTOR_SIZE * 8] = {0};
	if (get_n_sector_at(fs -> start_lba + cluster_offset, cluster_size / SECTOR_SIZE, cluster)) {
		WARNING_LOG("Failed to retrieve cluster %llu from qcow file.\n", dir -> cluster);
		return;
	}
	
	for (u64 i = 0, j = 0; i < cluster_size; ++j, i += sizeof(fat_dir_entry_t)) {
		const fat_dir_entry_t entry = ((fat_dir_entry_t*) cluster)[j];
		if ((entry.name)[0] == 0) break;

		if (entry.attr & FAT_ATTR_LFN) DEBUG_LOG("entry has LFN attr.\n");
		else fat_print_dir_entry(&entry);
		
		if ((entry.attr & FAT_ATTR_DIRECTORY) && ((entry.name)[0] != '.')) {
			DEBUG_LOG("Entering sub directory.\n");
			// TODO: First should check if the cluster is available/valid within the File Allocation Table
			const u32 entry_cluster = ((u32) entry.cluster_high << 16) | entry.cluster_low;
			fat_walker(start_lba, bpb_sector, fat_table_size, entry_cluster);
		} else if (entry.attr & FAT_ATTR_ARCHIVE) {
			const u64 used_clusters = align(entry.size, bpb_sector -> bytes_per_sector) / bpb_sector -> bytes_per_sector;
			DEBUG_LOG("entry has ARCHIVE attr, and spans for %llu clusters.\n", used_clusters);
		}
	}

	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_read(fs_file_t* file, void* buf, const int size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_write(fs_file_t* file, const void* buf, const int size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_seek(fs_file_t* file, const u64 off, const int whence) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int fs_readdir(fs_dir_t* dir, fs_dirent* ent) {
	TODO("Implement me!");
	return -QCOW_TODO;
}


#endif //_QCOW_FS_H_

