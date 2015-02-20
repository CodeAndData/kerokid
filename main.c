/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/vmalloc.h>

#include "kerokid.h"
#include "addressAnalysis.h"
#include "inlineHooks.h"
#include "notifier.h"
#include "syscallTable.h"
#include "memory.h"
#include "scheduling.h"
#include "proc_file.h"
#include "dump.h"

/* -------- storage for global variables -------------- */
struct findings_counter finds;

/* -------------- MODULE INIT and CLEANUP ------------- */
void scan(void)
{
	printk(KERN_INFO"KEROKID: Check for syscall table hooks...\n");
	check_syscall_table();

	printk(KERN_INFO"KEROKID: Check for inline hooks...\n");
	check_inline_hooks(get_systemcall_table());

	printk(KERN_INFO"KEROKID: Check notifier_subscriptions...\n");
	check_notifier_subscriptions();

#if defined(_ENABLE_MEMORY_)
	printk(KERN_INFO"KEROKID: Look for indicators of hidden modules in memory...\n");
	check_memory();
#endif
}

int init_module(void)
{
	printk(KERN_INFO"KEROKID: Started\n");
	init_systemcall_table();
	init_proc_file();
	init_common();
	init_dump_proc();
	init_notifier_check();
	init_inline_hook_check();
	init_scheduling();

	scan();
	return 0;
}

void cleanup_module(void)
{
	clean_common();
	clean_scheduling();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	proc_cleanup();
	dump_cleanup();
#endif
	printk(KERN_INFO"KEROKID: Bye!\n");
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Fraunhofer FKIE");
MODULE_DESCRIPTION("The KErnel ROotKIt Detector (KEROKID)");
