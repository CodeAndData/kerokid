/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains stuff to analyze addresses
 */

#include "kerokid.h"
#include "addressAnalysis.h"
#include <linux/module.h>
#include <linux/kernel.h>

extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;

int call_to_address_in_space(psize begin, psize end, psize call){
	if (begin <= call && call <= end){
		return 1;
	}
	else return 0;
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
			printk(KERN_ALERT"KEROKID: WARNING: Jump to module: %lx \n",(unsigned long)addr);
			print_module_info(unhiddenModules[i]);
			return 1;
		}
	}
	return 0;
}

// ---- main function ----

void analyze_address(psize *addr){
	if (address_is_part_of_moduleSpace(addr)){
		if (!address_is_part_of_module(addr)){
			printk(KERN_ALERT"KEROKID: ALERT: Jump to hidden module: %lx \n",(unsigned long)addr);
		}
	}
#if defined(_CONFIG_X86_64_)
	if (address_is_part_of_vmalloc(addr)){
		printk(KERN_ALERT"KEROKID: ALERT: Jump to Code in kmalloc: %lx \n",(unsigned long)addr);
	}
#endif
}
