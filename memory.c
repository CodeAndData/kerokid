/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains stuff to analyze memory
 */

#include "kerokid.h"
#include "addressAnalysis.h"
#include "memory.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/page.h>
#include <linux/vmalloc.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>

DEFINE_SPINLOCK(vmap_area_lock);

extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;

struct vm_struct **vm_area;
unsigned int number_of_vm_areas = 0;


unsigned int get_number_of_vm_areas_without_IOremapping(void){
	struct vmap_area *va;
	struct vm_struct *vm;
	unsigned int counter = 0;
 	list_for_each_entry(va,VMAP_AREA_LIST,list) {
 		if (!(va->flags & VM_MAP)) continue;
      	vm = va->vm;
      	if (!(vm->flags & VM_IOREMAP)){
      		counter++;
      	}
 	}
 	return counter;
}

void store_vm_areas_without_IOremapping_to_list(void){
	struct vmap_area *va;
	struct vm_struct *vm;
	unsigned int i = 0;
 	list_for_each_entry(va,VMAP_AREA_LIST,list){
		if (!(va->flags & VM_MAP)) continue;
		vm = va->vm;
		if (!(vm->flags & VM_IOREMAP)) {
			vm_area[i] = vm;
			i++;
		}
	}
}

void get_VMAs(void)
{
	spin_lock(&vmap_area_lock);
	number_of_vm_areas = get_number_of_vm_areas_without_IOremapping();
 	alloc_memory_for(number_of_vm_areas,(char**)&vm_area);
 	store_vm_areas_without_IOremapping_to_list();
}

void check_memory(void){
	get_VMAs();

	vfree(vm_area);
	return;
}
