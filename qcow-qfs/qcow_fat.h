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

// TODO: Implement FAT12/FAT16 support?

#ifndef _QCOW_FAT_H_
#define _QCOW_FAT_H_

typedef enum { FAT12 = 0, FAT16 = 1, FAT32 = 2, NOT_FAT = 3 } FatType;
const char* fat_type_str[] = { "FAT12", "FAT16", "FAT32", "NOT_FAT" };

typedef enum { 
	CLNSHUTBITMASK          = 0x08000000,
	HRDERRBITMASK           = 0x04000000,
	MAX_FAT_NAME            = 256,
	FAT_SIGNATURE           = 0xAA55,
	FAT_ATTR_READ_ONLY      = 0x01,
	FAT_ATTR_HIDDEN         = 0x02,
	FAT_ATTR_SYSTEM         = 0x04,
	FAT_ATTR_VOLUME_ID      = 0x08,
	FAT_ATTR_DIRECTORY      = 0x10,
	FAT_ATTR_ARCHIVE        = 0x20,
	FAT_ATTR_LFN            = 0x0F,
	FAT_DIR_DELETED         = 0xE5,
	FAT_EMPTY_DIR           = 0x00,
	FAT_RSV_CLUSTERS        = 0x0FFFFFF6,
	FAT_END_OF_CHAIN        = 0x0FFFFFF8,
	FAT_BAD_CLUSTER         = 0x0FFFFFF7,
	FAT_FREE_CLUSTER        = 0x00,
	FAT_LFN_LAST_ENTRY_MASK = 0x40
} QCowFatConstants;

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

STATIC_ASSERT(sizeof(bpb_sector_t)    == 512, "BPB size mismatch");
STATIC_ASSERT(sizeof(fat_dir_entry_t) == 32,  "FAT dir entry size mismatch");
STATIC_ASSERT(sizeof(fat_lfn_entry_t) == 32,  "FAT LFN entry size mismatch");

typedef struct {
    u32 first_cluster;        // First data cluster
    u32 size;                 // Cached file size
    u8  attr;                 // FAT attributes
} fat_node_t;

typedef struct {
	FatType fat_type;
	bpb_sector_t bpb_sector;
	u32** fat_tables;
	u8 fat_tables_cnt;
	u64 fat_table_size;
	u64 max_valid_cluster;
	u64 lru_cluster_idx;
	u8* lru_cluster;
	u64 cluster_size;
	u64 start_lba;
} qfs_fat_t;

static inline u64 first_sector_of_cluster_n(const qfs_fat_t* qfs_fat, u64 n) {
	u64 first_data_sector = (qfs_fat -> bpb_sector.rsv_sectors_cnt * qfs_fat -> bpb_sector.bytes_per_sector + qfs_fat -> bpb_sector.num_fat * qfs_fat -> fat_table_size) / SECTOR_SIZE;
	return ((n - 2) * qfs_fat -> cluster_size / SECTOR_SIZE) + first_data_sector;
}

static FatType determine_fat_type(qfs_fat_t* qfs_fat) {
	const bpb_sector_t* bpb_sector = &(qfs_fat -> bpb_sector);
	if ((bpb_sector -> bytes_per_sector == 0) || (bpb_sector -> sectors_per_cluster == 0)) return NOT_FAT;

	u64 root_dir_sectors = (bpb_sector -> root_ent_cnt * 32 + bpb_sector -> bytes_per_sector) / bpb_sector -> bytes_per_sector;
	u64 fat_size = (bpb_sector -> fat_size16 != 0) ? bpb_sector -> fat_size16 : bpb_sector -> fat_size;
	u64 tot_sec  = (bpb_sector -> tot_sec16  != 0) ? bpb_sector -> tot_sec16  : bpb_sector -> total_sectors;
	u64 data_sec = tot_sec - (bpb_sector -> rsv_sectors_cnt + (bpb_sector -> num_fat * fat_size) + root_dir_sectors); 
	qfs_fat -> max_valid_cluster = data_sec / bpb_sector -> sectors_per_cluster; 	
	
	if (qfs_fat -> max_valid_cluster < 4085)       return FAT12;
	else if (qfs_fat -> max_valid_cluster < 65525) return FAT16;

	return FAT32;
}

// TODO: Note that this is not optimal as utf16 could actually have 32-bit
//       codepoints and not only 16-bit ones
static inline char utf16le_to_ascii(u16 cp) {
	if (cp >= 0x20 && cp <= 0x7E) return cp;
	else if (cp == 0x00 || cp == 0xFFFF) return '\0';
	return '?';
}

static u8 lfn_checksum(const u8* fcb_name) {
	u8 checksum = 0;
	for (int i = 0; i < 11; ++i) {
		// NOTE: The operation is an unsigned char rotate right
		checksum = ((checksum & 0x01) * 0x80) + (checksum >> 1) + fcb_name[i];
	}
	return checksum;
} 

static int update_lfn_entry_name(const fat_dir_entry_t* entry, char* name, u64* name_size, u8* lfn_checksum) {
	static u8 order = 0xFF;
	static u8 checksum = 0x00;
	const fat_lfn_entry_t* lfn_entry = (fat_lfn_entry_t*) entry;
	
	if ((lfn_entry -> order & ~FAT_LFN_LAST_ENTRY_MASK) >= order) {
		WARNING_LOG("Invalid LFN order, must be less than the previous lfn order.\n");
		return -QCOW_CORRUPTED_LFN_ENTRY;
	} 
	
	if ((lfn_entry -> order & ~FAT_LFN_LAST_ENTRY_MASK) == 1) order = 0xFF;
	else if (lfn_entry -> order & FAT_LFN_LAST_ENTRY_MASK)    checksum = lfn_entry -> checksum;

	if (lfn_entry -> type != 0x00 || lfn_entry -> zero != 0x0000 || checksum != lfn_entry -> checksum) {
		WARNING_LOG("Type and zero field must be zero, and checksum must be equal for each LFN entry.\n");
		return -QCOW_CORRUPTED_LFN_ENTRY;
	}

	*lfn_checksum = checksum;
	order = lfn_entry -> order & FAT_LFN_LAST_ENTRY_MASK;
	
	for (unsigned int i = 0; i < QCOW_ARR_SIZE(lfn_entry -> name1); ++i) {
		if (utf16le_to_ascii((lfn_entry -> name1)[i]) == '\0') break;
		name[*name_size++] = utf16le_to_ascii((lfn_entry -> name1)[i]);
	}

	for (unsigned int i = 0; i < QCOW_ARR_SIZE(lfn_entry -> name2); ++i) {
		if (utf16le_to_ascii((lfn_entry -> name2)[i]) == '\0') break;
		name[*name_size++] = utf16le_to_ascii((lfn_entry -> name2)[i]);
	}
	
	for (unsigned int i = 0; i < QCOW_ARR_SIZE(lfn_entry -> name3); ++i) {
		if (utf16le_to_ascii((lfn_entry -> name3)[i]) == '\0') break;
		name[*name_size++] = utf16le_to_ascii((lfn_entry -> name3)[i]);
	}
	
	return QCOW_NO_ERROR;
}

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

#define FAT_CHECK(cond, warn_msg, err) \
if ((cond)) {                          \
	WARNING_LOG(warn_msg);             \
	return -(err);                     \
}

static int is_valid_fat_qfs(qfs_fat_t* qfs_fat) {
	const bpb_sector_t* bpb_sector = &(qfs_fat -> bpb_sector);
	FAT_CHECK(!is_valid_jmp_boot(bpb_sector -> jmp_boot), "Invalid jump boot field.\n", QCOW_INVALID_BPB_SECTOR);
	
	const u16 bps_values[] = { 512, 1024, 2048, 4096 };
   	FAT_CHECK(!is_contained_in(bpb_sector -> bytes_per_sector, QCOW_ARR_SIZE(bps_values), bps_values), "Invalid bytes per sector field.\n", QCOW_INVALID_BPB_SECTOR);

	const u16 spc_values[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
   	FAT_CHECK(!is_contained_in(bpb_sector -> sectors_per_cluster, QCOW_ARR_SIZE(spc_values), spc_values), "Invalid bytes per sector field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> rsv_sectors_cnt == 0, "Invalid reserved sectors count field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> num_fat == 0, "Invalid num fat field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> root_ent_cnt != 0, "As we do not support other than FAT32, this field should be zero.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> tot_sec16 != 0, "As we do not support other than FAT32, this field should be zero.\n", QCOW_INVALID_BPB_SECTOR);

	const u16 media_values[] = { 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };
   	FAT_CHECK(!is_contained_in(bpb_sector -> media, QCOW_ARR_SIZE(media_values), media_values), "Invalid media field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> fat_size16 != 0, "As we do not support other than FAT32, this field should be zero.\n", QCOW_INVALID_BPB_SECTOR);
	
	// FAT32 Specific Checks
	FAT_CHECK(bpb_sector -> total_sectors == 0, "Invalid total sectors field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> fat_size == 0, "Invalid fat size field.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(
		bpb_sector -> ext_flags.active_fat_idx != 0 && bpb_sector -> ext_flags.is_mirrored, 
		"Active FAT index should only be set and used if mirroring is disabled.\n", 
		QCOW_INVALID_BPB_SECTOR
	);
	FAT_CHECK(bpb_sector -> maj_fs_rev || bpb_sector -> min_fs_rev, "Version must be 0x00.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> root_cluster < 2, "The root cluster must be 2 or the first usable cluster.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(
		bpb_sector -> backup_bpb_sector != 0 && bpb_sector -> backup_bpb_sector != 6, 
		"Backup BPB Sector must be either 0 (not present) or 6 (present).\n",
		QCOW_INVALID_BPB_SECTOR
	);
	FAT_CHECK(bpb_sector -> drive_num != 0x00 && bpb_sector -> drive_num != 0x80, "Drive Number must be either 0x00 or 0x80.\n", QCOW_INVALID_BPB_SECTOR);
	FAT_CHECK(bpb_sector -> signature != FAT_SIGNATURE, "Invalid FAT Signature.\n", QCOW_INVALID_BPB_SECTOR);

	return QCOW_NO_ERROR;
}

static int parse_bpb_sector(qfs_fat_t* qfs_fat) {
	mem_set(&(qfs_fat -> bpb_sector), 0, sizeof(bpb_sector_t));
	if (get_sector_at(qfs_fat -> start_lba, &(qfs_fat -> bpb_sector))) {
		WARNING_LOG("Failed to retrieve the BPB sector from given qcow file.\n");
		return -QCOW_IO_ERROR;
	}
	
	int err = is_valid_fat_qfs(qfs_fat);
	if (err < 0 && qfs_fat -> bpb_sector.backup_bpb_sector == 6) {
		const u64 backup_bpb_offset = qfs_fat -> bpb_sector.backup_bpb_sector * (qfs_fat -> bpb_sector.bytes_per_sector / SECTOR_SIZE);
		if (get_sector_at(qfs_fat -> start_lba + backup_bpb_offset, &(qfs_fat -> bpb_sector))) {
			WARNING_LOG("Failed to retrieve backup BPB sector from given qcow file.\n");
			return -QCOW_IO_ERROR;
		}

		int err = is_valid_fat_qfs(qfs_fat);
		if (err < 0) {
			WARNING_LOG("Failed to parse FAT partition.\n");
			return err;
		}
	} 
	
	qfs_fat -> fat_type = determine_fat_type(qfs_fat);
	DEBUG_LOG("FAT Type: %s\n", fat_type_str[qfs_fat -> fat_type]);
	if (qfs_fat -> fat_type != FAT32) {
		WARNING_LOG("Currently we do not support FAT12/FAT16, but only FAT32.\n");
		return -QCOW_UNSUPPORTED_FAT_FORMAT;
	}

	return QCOW_NO_ERROR;
}

static int parse_fat_tables(qfs_fat_t* qfs_fat) {
	qfs_fat -> fat_tables_cnt = qfs_fat -> bpb_sector.num_fat;
	qfs_fat -> fat_table_size = qfs_fat -> bpb_sector.bytes_per_sector * qfs_fat -> bpb_sector.fat_size;
	qfs_fat -> fat_tables = qcow_calloc(qfs_fat -> fat_tables_cnt, sizeof(u32*));
	if (qfs_fat -> fat_tables == NULL) {
		WARNING_LOG("Failed to allocate buffer for fat tables.\n");
		return -QCOW_IO_ERROR;
	}

	for (u32 j = 0; j < qfs_fat -> fat_tables_cnt; ++j) {
		(qfs_fat -> fat_tables)[j] = qcow_calloc(qfs_fat -> fat_table_size, sizeof(u8));
		if ((qfs_fat -> fat_tables)[j] == NULL) {
			for (u8 z = 0; z < j; ++z) QCOW_SAFE_FREE((qfs_fat -> fat_tables)[z]);
			WARNING_LOG("Failed to allocate fat table buffer.\n");
			return -QCOW_IO_ERROR;
		}

		const u64 fat_region_ofqfs_fatet = qfs_fat -> bpb_sector.rsv_sectors_cnt * (qfs_fat -> bpb_sector.bytes_per_sector / SECTOR_SIZE) + (j * qfs_fat -> fat_table_size / SECTOR_SIZE);
		if (get_n_sector_at(qfs_fat -> start_lba + fat_region_ofqfs_fatet, qfs_fat -> fat_table_size / SECTOR_SIZE, qfs_fat -> fat_tables[j])) {
			for (u8 z = 0; z <= j; ++z) QCOW_SAFE_FREE((qfs_fat -> fat_tables)[z]);
			WARNING_LOG("Failed to retrieve first sector from given qcow file.\n");
			return -QCOW_IO_ERROR;
		}

		u32 entry_1 = (qfs_fat -> fat_tables)[j][1];
		// TODO: Should we care about those?
		if ((entry_1 & CLNSHUTBITMASK) == 0) WARNING_LOG("Dirty Volume.\n");
		if ((entry_1 & HRDERRBITMASK) == 0)  WARNING_LOG("I/O Disk contains faults from previous unsuccessfull unmount operation.\n");
	}

	return QCOW_NO_ERROR;
}

static int parse_fat_fs(qfs_fat_t* qfs_fat) {
	int err = 0;
	if ((err = parse_bpb_sector(qfs_fat)) < 0) {
		WARNING_LOG("Failed to parse the BPB Sector.\n");
		return err;
	}

	if ((err = parse_fat_tables(qfs_fat)) < 0) {
		WARNING_LOG("Failed to parse the FAT tables.\n");
		return err;
	}

	qfs_fat -> cluster_size = qfs_fat -> bpb_sector.sectors_per_cluster * qfs_fat -> bpb_sector.bytes_per_sector;
	qfs_fat -> lru_cluster = qcow_calloc(qfs_fat -> cluster_size, sizeof(u8));
	if (qfs_fat -> lru_cluster == NULL) {
		for (u8 i = 0; i < qfs_fat -> fat_tables_cnt; ++i) QCOW_SAFE_FREE((qfs_fat -> fat_tables)[i]);
		QCOW_SAFE_FREE(qfs_fat -> fat_tables);
		WARNING_LOG("Failed to allocate cluster.\n");
		return -QCOW_IO_ERROR;
	}

	return QCOW_NO_ERROR;
}

static int fetch_next_cluster(qfs_fat_t* qfs_fat, u32* cluster_n, const bool update_cluster_ref) {
	if (update_cluster_ref) {
		*cluster_n = (qfs_fat -> fat_tables)[0][*cluster_n] & 0x0FFFFFFF;
		if (*cluster_n >= FAT_END_OF_CHAIN) {
			WARNING_LOG("Reached end of chain of the FAT table.\n");
			return -QCOW_FILE_NOT_FOUND;
		}

		if ((*cluster_n == FAT_BAD_CLUSTER) || (*cluster_n == FAT_FREE_CLUSTER) || ((*cluster_n > qfs_fat -> max_valid_cluster) && (*cluster_n <= FAT_RSV_CLUSTERS))) {
			WARNING_LOG("Corrupted directory.\n");
			return -QCOW_CORRUPTED_DIRECTORY;
		}
	}

	if (*cluster_n != qfs_fat -> lru_cluster_idx) {
		const u64 cluster_offset = first_sector_of_cluster_n(qfs_fat, *cluster_n);
		if (get_n_sector_at(qfs_fat -> start_lba + cluster_offset, qfs_fat -> cluster_size / SECTOR_SIZE, qfs_fat -> lru_cluster)) {
			WARNING_LOG("Failed to retrieve cluster %u from qcow file.\n", *cluster_n);
			return -QCOW_IO_ERROR;
		}

		qfs_fat -> lru_cluster_idx = *cluster_n;
	}

	return QCOW_NO_ERROR;
}

static int iterate_fat_dir(fat_dir_entry_t* fat_entry, char name[MAX_FAT_NAME], u32* entry_idx, const u64 fat_dir_entries_per_cluster, qfs_fat_t* qfs_fat, const bool fetch_dirent, const char* s_name) {
	int err = 0;
	u64 name_size = 0;
	u8 lfn_entry_checksum = 0;
	
	/* for (; (*entry_idx % fat_dir_entries_per_cluster) < fat_dir_entries_per_cluster; ++(*entry_idx)) { */
	for (u64 i = (*entry_idx % fat_dir_entries_per_cluster) * sizeof(fat_dir_entry_t); i < qfs_fat -> cluster_size; ++(*entry_idx), i += sizeof(fat_dir_entry_t)) {
		*fat_entry = ((fat_dir_entry_t*) qfs_fat -> lru_cluster)[*entry_idx % fat_dir_entries_per_cluster];
		
		if ((fat_entry -> name)[0] == FAT_EMPTY_DIR) {
			DEBUG_LOG("Found first empty dir, and therefore the end of this directory's content.\n");
			return -QCOW_END_OF_DIRECTORY;
		} else if ((fat_entry -> name)[0] == FAT_DIR_DELETED) {
			DEBUG_LOG("This directory/file has been deleted.\n");
			continue;
		} else if (fat_entry -> attr & FAT_ATTR_LFN) {
			DEBUG_LOG("entry has LFN attr.\n");
			if ((err = update_lfn_entry_name(fat_entry, name, &name_size, &lfn_entry_checksum)) < 0) {
				WARNING_LOG("Failed to read the lfn entry.\n");
				return err;
			}
			continue;
		} else if (fat_entry -> attr & FAT_ATTR_VOLUME_ID) {
			DEBUG_LOG("Disk partition must be corrupted as volume label is not accessible from here.\n");
			return -QCOW_CORRUPTED_DIRECTORY;
		} else if (name_size == 0) {
			name_size = str_len((const char*) fat_entry -> name);
			mem_cpy(name, fat_entry -> name, name_size);
		
			if (!(fat_entry -> attr & FAT_ATTR_DIRECTORY) && !is_whitespace((fat_entry -> ext)[0])) {
				name[name_size++] = '.';
				const u64 entry_ext_len = str_len((const char*) fat_entry -> ext);
				mem_cpy(name + name_size, fat_entry -> ext, entry_ext_len);
				name_size += entry_ext_len;
			}
		} else {
			u8 checksum = lfn_checksum(fat_entry -> name);
			if (checksum != lfn_entry_checksum) {
				WARNING_LOG("Checksum mismatch.\n");
				return -QCOW_INVALID_LFN_CHECKSUM;
			}
		}
		
		trim_str(name);
		name_size = str_len(name) + 1;
		
		if (fetch_dirent || (mem_n_cmp(name, s_name, MIN(name_size, str_len(s_name))) == 0)) return QCOW_NO_ERROR;
		
		mem_set(name, 0, MAX_FAT_NAME);
		name_size = 0;
	}
	
	return -QCOW_END_OF_CLUSTER;
}

static int fat_lookup(qfs_fat_t* qfs_fat, u32 cluster_n, fat_dir_entry_t* fat_entry, char name[MAX_FAT_NAME], const char* s_name) {
	int err = 0;
	if ((err = fetch_next_cluster(qfs_fat, &cluster_n, FALSE)) < 0) {
		WARNING_LOG("Failed to fetch the next cluster.\n");
		return err;
	}
	
	const u64 fat_dir_entries_per_cluster = qfs_fat -> cluster_size / sizeof(fat_dir_entry_t);
	do {
		u32 entry_idx = 0;
		err = iterate_fat_dir(fat_entry, name, &entry_idx, fat_dir_entries_per_cluster, qfs_fat, FALSE, s_name);
		if (-err == QCOW_END_OF_DIRECTORY) return -QCOW_FILE_NOT_FOUND;	
		else if (-err != QCOW_END_OF_CLUSTER && err < 0) {
			WARNING_LOG("Failed to iterate directory!\n");
			return err;
		}

		if ((err = fetch_next_cluster(qfs_fat, &cluster_n, TRUE)) < 0) {
			WARNING_LOG("Failed to fetch the next cluster.\n");
			return err;
		}
	} while (TRUE);

	return -QCOW_FILE_NOT_FOUND;
}

static int fat_readdir(qfs_fat_t* qfs_fat, u32* cluster_n, fat_dir_entry_t* fat_entry, u32* entry_idx, char name[MAX_FAT_NAME]) {
	int err = 0;
	if ((err = fetch_next_cluster(qfs_fat, cluster_n, FALSE)) < 0) {
		WARNING_LOG("Failed to fetch the next cluster.\n");
		return err;
	}
	
	const u64 fat_dir_entries_per_cluster = qfs_fat -> cluster_size / sizeof(fat_dir_entry_t);
	do {
		err = iterate_fat_dir(fat_entry, name, entry_idx, fat_dir_entries_per_cluster, qfs_fat, TRUE, NULL);
		
		if (-err == QCOW_END_OF_DIRECTORY) return -QCOW_END_OF_DIRECTORY;
		else if (-err == QCOW_END_OF_CLUSTER) {
			if ((err = fetch_next_cluster(qfs_fat, cluster_n, TRUE)) < 0) {
				WARNING_LOG("Failed to fetch the next cluster.\n");
				return err;
			}
			continue;
		} else if (err < 0) {
			WARNING_LOG("Failed to iterate directory.\n");
			return err;
		}

		(*entry_idx)++;
			
		return QCOW_NO_ERROR;
	} while (TRUE);

	return QCOW_NO_ERROR;
}

#endif //_QCOW_FAT_H_

