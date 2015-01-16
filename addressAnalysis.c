/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains stuff to analyze addresses
 */

#include <linux/module.h>
#include <linux/kernel.h>

#include "kerokid.h"
#include "addressAnalysis.h"
#include "proc_file.h"


#define MAX_MODULE_INFO_LEN 150

extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;
extern char *current_notifier;
char unhiddenmoduleinfo[MAX_MODULE_INFO_LEN];


int call_to_address_in_space(psize begin, psize end, psize call){
	if (begin <= call && call <= end)
		return 1;
	else
		return 0;
}

int address_is_part_of_moduleSpace(psize *addr){
	return call_to_address_in_space(MODULE_MAPPING_SPACE_START,MODULE_MAPPING_SPACE_END,(psize)addr);
}

int address_is_part_of_vmalloc(psize *addr){
	return call_to_address_in_space(VMALLOC_START,VMALLOC_START,(psize)addr);
}

int address_is_part_of_module(psize *addr){
	int i;
	for (i=0; i < numberOfUnhiddenModules; i++){
		if (call_to_address_in_space(startOf(unhiddenModules[i]),endOf(unhiddenModules[i]),(psize)addr)){
			return i+1;
		}
	}
	return 0;
}

char *get_unhidden_module_info(void)
{
	return unhiddenmoduleinfo;
}

int analyze_address(psize *addr)
{
	if (address_is_part_of_moduleSpace(addr)) {
		const int part_of_module = address_is_part_of_module(addr);
		if (!part_of_module) {
			printk(KERN_ALERT"KEROKID: ALERT: Jump to hidden module: %lx \n", (unsigned long)addr);
			cat_proc_message(formats("found jump to hidden module in %s notifier: %lx \n", current_notifier, (unsigned long)addr));
		} else {
			printk(KERN_ALERT"KEROKID: WARNING: Jump to module: %lx \n", (unsigned long)addr);
			print_module_info(unhiddenModules[part_of_module-1]);
			cat_proc_message(formats("found jump to module in %s notifier:\n%s\n", current_notifier, get_module_info(unhiddenModules[part_of_module-1])));
			snprintf(unhiddenmoduleinfo, strlen(get_module_info(unhiddenModules[part_of_module-1])) + 1,
								"%s\n", get_module_info(unhiddenModules[part_of_module-1]));
		}
		return 1;
	}
#if defined(_CONFIG_X86_64_)
	if (address_is_part_of_vmalloc(addr)) {
		printk(KERN_ALERT"KEROKID: ALERT: Jump to Code in kmalloc: %lx \n", (unsigned long)addr);
		return 1;
	}
#endif
	return 0;
}
