#include <stdio.h>
#include <stdlib.h>
#include "qcow_part.h"

int main(void) {
	if (init_qcow_part("../Arch-Linux-x86_64-basic.qcow2")) {
		WARNING_LOG("Failed to init qcow_part.\n");
		return 1;
	}

	partition_t* partitions = NULL;
	unsigned int partitions_cnt = 0;
	if (parse_partitions(&partitions, &partitions_cnt)) {
		WARNING_LOG("Failed to parse the partitions.\n");
		deinit_qcow_part();
		return 1;
	}
	
	for (unsigned int i = 0; i < partitions_cnt; ++i) print_part_info(partitions[i]);
	QCOW_SAFE_FREE(partitions);

	deinit_qcow_part();

	return 0;
}
