/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains stuff to analyze memory
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/page.h>
#include <linux/vmalloc.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
#include <linux/kallsyms.h>

#include "kerokid.h"
#include "addressAnalysis.h"
#include "memory.h"
#include "proc_file.h"


DEFINE_SPINLOCK(vmap_area_lock);

extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;

struct vm_struct **vm_area;
unsigned int number_of_vm_areas = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
struct vmap_area {
        unsigned long va_start;
        unsigned long va_end;
        unsigned long flags;
        struct rb_node rb_node;         /* address sorted rbtree */
        struct list_head list;          /* address sorted list */
        struct list_head purge_list;    /* "lazy purge" list */
        struct vm_struct *vm;
        struct rcu_head rcu_head;
};
#endif


unsigned int get_number_of_vm_areas_without_IOremapping(void)
{
	struct vmap_area *va;
	struct vm_struct *vm;
	unsigned int counter = 0;
 	list_for_each_entry(va, VMAP_AREA_LIST, list) {
 		if (!(va->flags & VM_MAP))
 			continue;
      	vm = va->vm;
      	if (!(vm->flags & VM_IOREMAP))
      		counter++;
 	}
 	return counter;
}

void store_vm_areas_without_IOremapping_to_list(void)
{
	struct vmap_area *va;
	struct vm_struct *vm;
	unsigned int i = 0;
 	list_for_each_entry(va, VMAP_AREA_LIST, list) {
		if (!(va->flags & VM_MAP))
			continue;
		vm = va->vm;
		if (!(vm->flags & VM_IOREMAP)) {
			vm_area[i] = vm;
			i++;
		}
	}
}

void get_VMAs(void)
{
	if (number_of_vm_areas == 0) {
		spin_lock(&vmap_area_lock);
		number_of_vm_areas = get_number_of_vm_areas_without_IOremapping();
		alloc_memory_for(number_of_vm_areas, (char **)&vm_area);
		store_vm_areas_without_IOremapping_to_list();
#if DEBUG
 	printk(KERN_INFO"KEROKID: --> found vmap_area_list at %p", (void *)VMAP_AREA_LIST);
	printk(KERN_INFO"KEROKID: --> stored %d vm areas to list\n", number_of_vm_areas);
#endif
	}
}

void memory_proc(char buf[], unsigned int buf_size)
{
	unsigned int i;
	if (VMAP_AREA_LIST != 0)
		get_VMAs();
	for (i=0; i < number_of_vm_areas; i++) {
		if (concatenate_if_not_too_long(buf, formats("vm_struct %u %lu %lx\n", i, vm_area[i]->size, (unsigned long)vm_area[i]->addr), buf_size)) {
			printk(KERN_ALERT "KEROKID: Error: memory proc message too short (for memory)\n");
			break;
		}
	}
}

void check_memory(void)
{
	if (VMAP_AREA_LIST == 0) {
		printk(KERN_ALERT "KEROKID: Error: vmap_area_list not found in kallsyms. Aborting memory check...\n");
		return;
	}
	get_VMAs();

	vfree(vm_area);
	return;
}
