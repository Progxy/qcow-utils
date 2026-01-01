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

#ifndef _QCOW_QFS_H_
#define _QCOW_QFS_H_

#define _QCOW_PRINTING_UTILS_
#define _QCOW_UTILS_IMPLEMENTATION_
#define _QCOW_SPECIAL_TYPE_SUPPORT_
#include "../common/utils.h"
#include "../qcow-parser/qcow_parser.h"
#include "../qcow-part/qcow_part.h"
#include "./qcow_fat.h"

typedef enum { MAX_QFS_NAME = 256 } QFSConstants;
typedef enum { QFS_DIRECTORY = 0x01, QFS_FILE = 0x02 } QFSNodeType;
typedef enum { QFS_IRW = 0x04 } QFSMode;
typedef enum { QFS_SEEK_SET = 0, QFS_SEEK_CUR, QFS_SEEK_END } QFSWhence;
typedef enum { QFS_FAT = 0 } QFSType;

typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hours;
	u8 mins;
	u8 secs;
} qfs_time_t;

typedef struct qfs_node_t {
	char name[MAX_QFS_NAME];
	QFSNodeType node_type;
	union {
		fat_node_t fat_node;
	};
} qfs_node_t;

typedef struct {
	qfs_node_t node;
	u64 ofqfset;
	u32 flags;
	union {
		u32 cur_cluster; // This is specific to fat_file_t
	};
} qfs_file_t;

typedef struct {
	qfs_node_t node;
	long long int entries_cnt;
	u32 entry_idx;
	union {
		u32 cur_cluster; // This is specific to fat_file_t
	};
} qfs_dir_t;

typedef struct {
	QFSNodeType node_type;
	char name[MAX_QFS_NAME];
} qfs_dirent_t;

typedef struct {
	u32 size;                    // File size in bytes
    u32 mode;                    // Type + permissions
    qfs_time_t acc_time;          // Access time
    qfs_time_t mod_time;          // Modification time
    qfs_time_t create_time;       // Creation time
	char name[MAX_QFS_NAME];
} qfs_stat_t;

#define RECENT_NODES_CNT 16
typedef struct {
	QFSType qfs_type;
	qfs_node_t root;
	// TODO: Maybe should also add the qfs_node_t cwd
	
	union {
		qfs_fat_t qfs_fat;
	};
} qfs_ctx;

typedef qfs_ctx qfs_t;

// TODO: For when start supporting multiple QFS, the common and a union of ctxs should
// be put here
/* typedef struct { */
/* 	u64 start_lba; */
/* 	/1* QFSType qfs_type; *1/ */
/* 	qfs_ctx* qfs_ctx; */
/* } qfs_t; */

// Utilities Functions
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

static void print_qfs_time(qfs_time_t time) {
	printf(
		"%02u:%02u:%02u %02u/%02u/%04u",
		time.hours,
		time.mins,
		time.secs,
		time.day,
		time.month,
		time.year
	);
	return;
}

static void print_file_info(qfs_file_t* file) {
	printf(" -- File Info -- \n");
	printf("size: %u\n", file -> node.fat_node.size);
	printf("ofqfset: %llu\n", file -> ofqfset);
	printf("flags: %X\n", file -> flags);
	printf("current cluster: %u\n", file -> cur_cluster);
	printf("name: '%s'\n", file -> node.name);
	
	printf("attributes: ");
	print_attr(file -> node.fat_node.attr);
	printf("\n");
	
	printf("----------------\n");
	return;
}

static void print_dir_info(qfs_dir_t* dir) {
	printf(" -- Dir Info -- \n");
	printf("current cluster: %u\n", dir -> cur_cluster);
	printf("entries_cnt: %lld\n", dir -> entries_cnt);
	printf("entries_idx: %u\n", dir -> entry_idx);
	printf("name: '%s'\n", dir -> node.name);
	
	printf("attributes: ");
	print_attr(dir -> node.fat_node.attr);
	printf("\n");
	
	printf("----------------\n");
	return;
}

static void print_dirent_info(qfs_dirent_t* dirent) {
	printf(" -- Dir Entry Info -- \n");
	printf("node_type: '%s'\n", dirent -> node_type == QFS_DIRECTORY ? "DIRECTORY" : "FILE");
	printf("name: '%s'\n", dirent -> name);
	printf("----------------\n");
	return;
}

static void print_stat_info(qfs_stat_t* stat) {
	printf(" -- Stat Info -- \n");
	printf("size: %u\n", stat -> size);
	printf("mode: %X\n", stat -> mode);
	printf("name: '%s'\n", stat -> name);
	
	printf("access time: ");
	print_qfs_time(stat -> acc_time);
	printf("\n");
	
	printf("modification time: ");
	print_qfs_time(stat -> mod_time);
	printf("\n");
	
	printf("creation time: ");
	print_qfs_time(stat -> create_time);
	printf("\n");
	
	printf("----------------\n");
	return;
}

static qfs_time_t fat_time_to_qfs_time(const u16 date, const u16 time) {
	qfs_time_t qfs_time = {0};
	qfs_time.year  = ((date >> 9) & 0x7F) + 1980;
	qfs_time.month = clamp((date >> 5) & 0x0F, 1, 12);
	qfs_time.day   = clamp(date & 0x1F, 1, 31);
	qfs_time.hours = clamp((time >> 11) & 0x1F, 0, 23);
	qfs_time.mins  = clamp((time >> 5) & 0x3F, 0, 59);
	qfs_time.secs  = clamp((time & 0x1F) * 2, 0, 58);
	return qfs_time;
}

// API (Stil in Progress)
int qfs_mount(const partition_t partition, qfs_t* qfs);
void qfs_unmount(qfs_t* qfs);
int qfs_open(qfs_t* qfs, const char* path, qfs_file_t* file, u32 flags);
int qfs_close(qfs_t* qfs, qfs_file_t* file);
int qfs_opendir(qfs_t* qfs, const char* path, qfs_dir_t* dir);
int qfs_stat(qfs_t* qfs, const char* path, qfs_stat_t* stat);
int qfs_lookup(qfs_t* qfs, const qfs_node_t* dir, const char* name, qfs_node_t* node, qfs_stat_t* stat);
int qfs_read(qfs_file_t* file, void* buf, const int size);
int qfs_write(qfs_file_t* file, const void* buf, const int size);
int qfs_seek(qfs_file_t* file, const u64 off, const int whence);
inline u64 qfs_tell(qfs_file_t* file);
int qfs_readdir(qfs_t* qfs, qfs_dir_t* dir, qfs_dirent_t* ent);

// Exposed API Functions
int qfs_mount(const partition_t partition, qfs_t* qfs) {
	qfs -> qfs_fat.start_lba = partition.start_lba;

	int err = parse_fat_fs(&(qfs -> qfs_fat));
	if (err == 0) {
		qfs -> qfs_type = QFS_FAT;
		qfs -> root.fat_node.size = 0;
		qfs -> root.fat_node.first_cluster = qfs -> qfs_fat.bpb_sector.root_cluster;
		qfs -> root.fat_node.attr = FAT_ATTR_VOLUME_ID | FAT_ATTR_DIRECTORY | FAT_ATTR_READ_ONLY;
		qfs -> root.node_type = QFS_DIRECTORY;
		(qfs -> root.name)[0]  = '/';

		DEBUG_LOG("Partition successfully mounted.\n");
		return QCOW_NO_ERROR;
	} else if (-err == QCOW_IO_ERROR) {
		WARNING_LOG("Failed to parse the FAT partition.\n");
		return err;
	} 
	
	TODO("try parsing other qfs.");
	return -QCOW_TODO;
}

void qfs_unmount(qfs_t* qfs) {
	if (qfs -> qfs_type == QFS_FAT) {
		for (u8 i = 0; i < qfs -> qfs_fat.fat_tables_cnt; ++i) QCOW_SAFE_FREE((qfs -> qfs_fat.fat_tables)[i]);
		QCOW_SAFE_FREE(qfs -> qfs_fat.fat_tables);
		QCOW_SAFE_FREE(qfs -> qfs_fat.lru_cluster);
	}
	DEBUG_LOG("Partition successfully unmounted.\n");
	return; 
}

int qfs_lookup(qfs_t* qfs, const qfs_node_t* dir, const char* s_name, qfs_node_t* node, qfs_stat_t* stat) {
	if (!(dir -> node_type & QFS_DIRECTORY)) {
		WARNING_LOG("The given node object is not a directory.\n");
		return -QCOW_NOT_A_DIRECTORY;
	}

	int err = 0;
	u64 cluster_n = dir -> fat_node.first_cluster;
	fat_dir_entry_t fat_entry = {0};
	char* name = (node != NULL) ? node -> name : stat -> name;
	if ((err = fat_lookup(&(qfs -> qfs_fat), cluster_n, &fat_entry, name, s_name)) < 0) {
		WARNING_LOG("Failed to perform fat lookup.\n");
		return err;
	}

	if (node != NULL) {
		node -> fat_node.size = fat_entry.size;
		node -> fat_node.attr = fat_entry.attr;
		node -> fat_node.first_cluster = ((u32) fat_entry.cluster_high << 16) | fat_entry.cluster_low;
		node -> node_type = (fat_entry.attr & FAT_ATTR_DIRECTORY) ? QFS_DIRECTORY : QFS_FILE;
	} else if (stat != NULL) {
		stat -> size = fat_entry.size;
		stat -> mode = (fat_entry.attr & FAT_ATTR_DIRECTORY) ? QFS_DIRECTORY : QFS_FILE;
		stat -> mode |= (fat_entry.attr & FAT_ATTR_READ_ONLY) ? QFS_IRW : 0x00;
		stat -> create_time = fat_time_to_qfs_time(fat_entry.cdate, fat_entry.ctime);
		stat -> acc_time    = fat_time_to_qfs_time(fat_entry.adate, 0);
		stat -> mod_time    = fat_time_to_qfs_time(fat_entry.mdate, fat_entry.mtime);
	}
			
	return -QCOW_FILE_NOT_FOUND;
}

int qfs_open(qfs_t* qfs, const char* path, qfs_file_t* file, u32 flags) {
	u64 path_len = str_len(path);
	char* sub_path = qcow_calloc(path_len, sizeof(char));
	if (sub_path == NULL) {
		WARNING_LOG("Failed to allocate buffer for sub_path.\n");
		return -QCOW_IO_ERROR;
	}

	// TODO: Should be changed with its correct representation which is the cwd?
	qfs_node_t node = qfs -> root;

	int err = 0;
	for (unsigned int prev_tok = 0; prev_tok < path_len;) {
		int tok = str_tok(path + prev_tok, "/");
		if (tok == -1) tok = path_len;
		else tok += prev_tok;
		mem_cpy(sub_path, path + prev_tok, tok - prev_tok);
		DEBUG_LOG("looking up sub_path: '%s' from path: '%s'\n", sub_path, path);
		if ((err = qfs_lookup(qfs, &node, sub_path, &node, NULL)) < 0) {
			QCOW_SAFE_FREE(sub_path);
			return err;
		}
		prev_tok = tok + 1;
		mem_set(sub_path, 0, path_len);
	}
	
	QCOW_SAFE_FREE(sub_path);
	
	if (node.node_type & QFS_DIRECTORY) {
		WARNING_LOG("The given path resolves into a directory.\n");
		return -QCOW_NOT_A_FILE;
	}

	file -> node = node;
	file -> ofqfset = 0;
	file -> cur_cluster = node.fat_node.first_cluster;
	file -> flags = flags;

	return QCOW_NO_ERROR;
}

int qfs_opendir(qfs_t* qfs, const char* path, qfs_dir_t* dir) {
	u64 path_len = str_len(path);
	char* sub_path = qcow_calloc(path_len, sizeof(char));
	if (sub_path == NULL) {
		WARNING_LOG("Failed to allocate buffer for sub_path.\n");
		return -QCOW_IO_ERROR;
	}

	// TODO: Should be changed with its correct representation which is the cwd?
	qfs_node_t node = qfs -> root;

	int err = 0;
	for (unsigned int prev_tok = 0; prev_tok < path_len;) {
		int tok = str_tok(path + prev_tok, "/");
		if (tok == -1) tok = path_len;
		else tok += prev_tok;
		mem_cpy(sub_path, path + prev_tok, tok - prev_tok);
		DEBUG_LOG("looking up sub_path: '%s' from path: '%s'\n", sub_path, path);
		if ((err = qfs_lookup(qfs, &node, sub_path, &node, NULL)) < 0) {
			QCOW_SAFE_FREE(sub_path);
			return err;
		}
		prev_tok = tok + 1;
		mem_set(sub_path, 0, path_len);
	}
	
	QCOW_SAFE_FREE(sub_path);
	
	if (!(node.node_type & QFS_DIRECTORY)) {
		WARNING_LOG("The given path does not resolve into a directory.\n");
		return -QCOW_NOT_A_DIRECTORY;
	}

	dir -> node = node;
	dir -> cur_cluster = node.fat_node.first_cluster;
	dir -> entries_cnt = -1;
	dir -> entry_idx = 0;	

	return QCOW_NO_ERROR;
}

int qfs_stat(qfs_t* qfs, const char* path, qfs_stat_t* stat) {
	u64 path_len = str_len(path);
	char* sub_path = qcow_calloc(path_len, sizeof(char));
	if (sub_path == NULL) {
		WARNING_LOG("Failed to allocate buffer for sub_path.\n");
		return -QCOW_IO_ERROR;
	}

	// TODO: Should be changed with its correct representation which is the cwd?
	qfs_node_t node = qfs -> root;

	int err = 0;
	for (unsigned int prev_tok = 0; prev_tok < path_len;) {
		int tok = str_tok(path + prev_tok, "/");
		if (tok == -1) tok = path_len;
		else tok += prev_tok;
		mem_cpy(sub_path, path + prev_tok, tok - prev_tok);
		DEBUG_LOG("looking up sub_path: '%s' from path: '%s'\n", sub_path, path);
		if ((u64) tok != path_len) {
			if ((err = qfs_lookup(qfs, &node, sub_path, &node, NULL)) < 0) {
				QCOW_SAFE_FREE(sub_path);
				return err;
			} 
		} else {
			if ((err = qfs_lookup(qfs, &node, sub_path, NULL, stat)) < 0) {
				QCOW_SAFE_FREE(sub_path);
				return err;
			}
		}
		prev_tok = tok + 1;
		mem_set(sub_path, 0, path_len);
	}
	
	QCOW_SAFE_FREE(sub_path);
	
	return QCOW_NO_ERROR;
}

int qfs_read(qfs_file_t* file, void* buf, const int size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int qfs_write(qfs_file_t* file, const void* buf, const int size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

int qfs_seek(qfs_file_t* file, const u64 off, const int whence) {
	if (whence == QFS_SEEK_SET) {
		if (off > file -> node.fat_node.size) return -QCOW_INVALID_OFFSET;
		file -> ofqfset = off;
		return QCOW_NO_ERROR;
	}

	if (whence == QFS_SEEK_CUR) {
		if (off + file -> ofqfset > file -> node.fat_node.size) return -QCOW_INVALID_OFFSET;
		file -> ofqfset += off;		
		return QCOW_NO_ERROR;
	}

	if (whence != QFS_SEEK_END) {
		WARNING_LOG("The only possible whence values are QFS_SEEK_SET, QFS_SEEK_CUR or QFS_SEEK_END.\n");
		return -QCOW_INVALID_WHENCE;
	}

	file -> ofqfset = file -> node.fat_node.size;

	return QCOW_NO_ERROR;
}

inline u64 qfs_tell(qfs_file_t* file) {
	return file -> ofqfset;
}

int qfs_readdir(qfs_t* qfs, qfs_dir_t* dir, qfs_dirent_t* ent) {
	if ((dir -> entries_cnt > 0) && (dir -> entries_cnt == dir -> entry_idx)) return -QCOW_END_OF_DIRECTORY;
	
	fat_dir_entry_t fat_entry = {0};
	int err = fat_readdir(&(qfs -> qfs_fat), &(dir -> cur_cluster), &fat_entry, &(dir -> entry_idx), ent -> name);
	if (-err == QCOW_END_OF_DIRECTORY) {
		dir -> entries_cnt = dir -> entry_idx;
		return QCOW_NO_ERROR;
	} else if (err < 0) {
		return err;
	}
	
	ent -> node_type = (fat_entry.attr & FAT_ATTR_DIRECTORY) ? QFS_DIRECTORY : QFS_FILE;
	
	return QCOW_NO_ERROR;
}

#endif //_QCOW_QFS_H_

