#ifndef _QCOW_EXT4_H_
#define _QCOW_EXT4_H_

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

#endif //_QCOW_EXT4_H_

