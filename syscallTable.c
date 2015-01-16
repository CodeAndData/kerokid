/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include "syscallTable.h"
#include "kerokid.h"
#include "addressAnalysis.h"
#include "proc_file.h"

// -------- global variables --------------
psize **syscallTable;

// ------------- setup functions ----------------------
void init_systemcall_table(void){
	psize **sctable;
	psize i = SPACE_WITH_SCT_START;
	while (i < SPACE_WITH_SCT_END) {
		sctable = (psize **) i;
		if (sctable[__NR_close] == (psize *) sys_close) {
			syscallTable = &sctable[0];
			break;
		}
		i += sizeof(void *);
	}
	if (syscallTable == NULL) printk(KERN_ALERT"KEROKID: ERROR: Could not detect SystemCall table!\n");
}

// ----------------------------------------------------------------
psize** get_systemcall_table(void){
	return syscallTable;
}

void check_syscall_table(void){
	int i;
	cat_proc_message("syscall table:\n");
	for (i=0; i < NUMBER_OF_SYSCALLS; i++){
		if (analyze_address(syscallTable[i]))
			finds.syscalls++;
	}
	if (!finds.syscalls)
		cat_proc_message("nothing found\n");
	return;
}

