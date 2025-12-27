#include <stdio.h>
#include <stdlib.h>
#include "qcow_part.h"

int main(void) {
	if (init_qcow_part("../Arch-Linux-x86_64-basic.qcow2")) {
		WARNING_LOG("Failed to init qcow_part.\n");
		return 1;
	}

	if (parse_partitions()) {
		WARNING_LOG("Failed to parse the partitions.\n");
		deinit_qcow_part();
		return 1;
	}
	
	deinit_qcow_part();
	
	return 0;
}
