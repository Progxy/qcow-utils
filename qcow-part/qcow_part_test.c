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
#include "qcow_part.h"

int main(void) {
	if (init_default_qcow("../Arch-Linux-x86_64-basic.qcow2")) {
		WARNING_LOG("Failed to init qcow_part.\n");
		return 1;
	}

	partition_t* partitions = NULL;
	unsigned int partitions_cnt = 0;
	if (parse_partitions(&partitions, &partitions_cnt)) {
		WARNING_LOG("Failed to parse the partitions.\n");
		deinit_default_qcow();
		return 1;
	}
	
	for (unsigned int i = 0; i < partitions_cnt; ++i) print_part_info(partitions[i]);
	QCOW_SAFE_FREE(partitions);

	deinit_default_qcow();

	return 0;
}

