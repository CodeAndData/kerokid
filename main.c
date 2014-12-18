/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include "kerokid.h"
#include "addressAnalysis.h"
#include "inlineHooks.h"
#include "notifier.h"
#include "syscallTable.h"

//deactivated memory forensics temporarily until memory problems are fixed
/*#if defined(_CONFIG_X86_)
#include "memory.h"
#endif*/

// -------------- MODULE INIT and CLEANUP -------------
int init_module(void)
{
	printk(KERN_INFO"KEROKID: Started\n");
	init_common();

	printk(KERN_INFO"KEROKID: Check for syscall table hooks...\n");
	init_systemcall_table();
	check_syscall_table();

	printk(KERN_INFO"KEROKID: Check for inline hooks...\n");
	check_inline_hooks(get_systemcall_table());

	printk(KERN_INFO"KEROKID: Check notifier_subscriptions...\n");
	check_notifier_subscriptions();

/*#if defined(_CONFIG_X86_)
	printk(KERN_INFO"KEROKID: Look for indicators of hidden modules in memory...\n");
	check_memory();
#endif*/

	return 0;
}

void cleanup_module(void)
{
	clean_common();
	printk(KERN_INFO"KEROKID: Bye!\n");
}

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Fraunhofer FKIE");
MODULE_DESCRIPTION("The KErnel ROotKIt Detector (KEROKID)");
