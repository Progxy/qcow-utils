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
#include "./qcow_fs.h"

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

	fs_t fs = {0};
	if ((err = fs_mount(fat_partition, &fs)) < 0) {
		WARNING_LOG("Failed to mount partition, because: %s\n", qcow_errors_str[-err]);
		return 1;
	}
	
	fs_file_t file = {0};
	if ((err = fs_open(&fs, "EFI/BOOT", &file)) < 0) {
		WARNING_LOG("Failed to open file, because: %s\n", qcow_errors_str[-err]);
		return 1;
	}

	print_file_info(&file);
		
	fs_unmount(&fs);
	
	deinit_qcow_part();

	return 0;
}

