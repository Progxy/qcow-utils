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

#include <stdio.h>
#include <stdlib.h>
#include "./qcow_qfs.h"

int main(void) {
	if (init_qcow_part("../Arch-Linux-x86_64-basic.qcow2")) {
		WARNING_LOG("Failed to init qcow_part.\n");
		return 1;
	}

	int err = 0;
	partition_t* partitions = NULL;
	unsigned int partitions_cnt = 0;
	if ((err = parse_partitions(&partitions, &partitions_cnt)) < 0) {
		WARNING_LOG("Failed to parse the partitions, because: %s.\n", qcow_errors_str[-err]);
		deinit_qcow_part();
		return 1;
	}
	
	const partition_t fat_partition = partitions[1];
	for (unsigned int i = 0; i < partitions_cnt; ++i) print_part_info(partitions[i]);
	QCOW_SAFE_FREE(partitions);

	qfs_t qfs = {0};
	if ((err = qfs_mount(fat_partition, &qfs)) < 0) {
		WARNING_LOG("Failed to mount partition, because: %s\n", qcow_errors_str[-err]);
		return 1;
	}
	
	/* qfs_file_t file = {0}; */
	qfs_dir_t dir = {0};
	if ((err = qfs_opendir(&qfs, "EFI/BOOT", &dir)) < 0) {
		WARNING_LOG("Failed to open dir, because: %s\n", qcow_errors_str[-err]);
		return 1;
	}

	print_dir_info(&dir);
	
	do {
		qfs_dirent_t dirent = {0};
		err = qfs_readdir(&qfs, &dir, &dirent);
		if (err < 0 && -err != QCOW_END_OF_DIRECTORY) {
			WARNING_LOG("Failed to read dir, because: %s\n", qcow_errors_str[-err]);
			return 1;
		} else if (-err == QCOW_END_OF_DIRECTORY) {
			DEBUG_LOG("Reached end of directory.\n");
		} else {
			print_dirent_info(&dirent);
		}
	} while (-err != QCOW_END_OF_DIRECTORY);

	qfs_stat_t stat = {0};
	if ((err = qfs_stat(&qfs, "EFI/BOOT", &stat)) < 0) {
		WARNING_LOG("Failed to qfs_stat, because: %s\n", qcow_errors_str[-err]);
		return 1;
	}
	
	print_stat_info(&stat);

	qfs_unmount(&qfs);
	
	deinit_qcow_part();

	return 0;
}

