#ifndef _QCOW_EXT4_H_
#define _QCOW_EXT4_H_

typedef enum {
	EXT4_MAX_NAME = 256
} QCowExt4Constants;

typedef struct {
	u64 size;
	u64 start_lba;
} qfs_ext4_t;

static int parse_ext4_fs(qfs_ext4_t* qfs_ext4) {
	u8 superblock[SECTOR_SIZE * 2] = {0};
	if (get_n_sector_at(qfs_ext4 -> start_lba, 2, superblock)) {
		return -QCOW_IO_ERROR;
	}

	for (unsigned int i = 0; i < SECTOR_SIZE * 2; ++i) {
		printf("%02X ", superblock[i]);
		if (i % 16 == 15) printf("\n");
	}

	TODO("Implement me!");
	return -QCOW_TODO;
}

static int ext4_lookup(qfs_ext4_t* qfs_ext4, char name[EXT4_MAX_NAME], const char* s_name) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

static int ext4_readdir(qfs_ext4_t* qfs_ext4, u32* entry_idx, char name[EXT4_MAX_NAME]) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

static int ext4_read(qfs_ext4_t* qfs_ext4, u64* offset, void* buf, u64 size) {
	TODO("Implement me!");
	return -QCOW_TODO;
}

#endif //_QCOW_EXT4_H_

