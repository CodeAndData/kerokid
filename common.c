/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains common functions
 */

#include <linux/proc_fs.h>

#include "kerokid.h"
#include "proc_file.h"
#include "syscallTable.h"


#define MODULE_PROC_NAME "modules"

struct module **unhiddenModules;
unsigned int numberOfUnhiddenModules;
char (*module_proc_message)[MAX_PROC_MESSAGE_LEN];
struct proc_dir_entry *module_proc_folder;


void alloc_memory_for(unsigned int quantity, char **something)
{
	if (quantity)
		*something = vmalloc(quantity * sizeof(char*));
	if (something == NULL)
		printk(KERN_ALERT"KEROKID: ERROR: Could not allocate memory!\n");
}

psize startOf(struct module *mod)
{
	return (psize)mod->module_core;
}

psize endOf(struct module *mod)
{
	return (psize)mod->module_core + mod->core_size;
}

/* ------------ module printing functions -------------------- */
void print_module_info(struct module *mod)
{
	printk(KERN_INFO"KEROKID: Name:           %s\n", mod->name);
	printk(KERN_INFO"KEROKID: Start-Address:  %lx\n", (unsigned long)startOf(mod));
	printk(KERN_INFO"KEROKID: End-Address:    %lx\n", (unsigned long)endOf(mod));
	printk(KERN_INFO"KEROKID: Object-Address: %lx\n", (unsigned long)mod);
	printk(KERN_INFO"KEROKID: Size:           %u Bytes\n", mod->core_size);
	if (mod->state == MODULE_STATE_LIVE)
		printk(KERN_INFO"KEROKID: Module-State:   LIVE\n");
	else
		printk(KERN_INFO"KEROKID: Module-State:   %d\n", (int)mod->state);
}

char *get_module_info(struct module *mod)
{
	static char result[300];
	sprintf(result, "Name: %s\nStart-Address: %lx\nEnd-Address: %lx\n"
			"Object-Address: %lx\nSize: %u Bytes\n", mod->name, (unsigned long)startOf(mod),
			(unsigned long)endOf(mod), (unsigned long)mod, mod->core_size);
	return result;
}

void print_unhidden_modules(void)
{
	int i;
	printk(KERN_INFO"KEROKID: --------> unhidden Modules (%d) <---------   \n", numberOfUnhiddenModules);
	for (i=0; i< numberOfUnhiddenModules; i++) {
		print_module_info(unhiddenModules[i]);
	}
}

/*----  module specific stuff */
unsigned int get_number_of_known_modules(void)
{
	struct module *m;
	unsigned int counter = 0;
	list_for_each_entry(m, &THIS_MODULE->list, list) {
    	counter++;
  	}
	return counter;
}

void store_known_modules_to_list(void)
{
  	struct module *m;
	unsigned int i = 0;
	if (unhiddenModules != NULL) {
		list_for_each_entry(m, &THIS_MODULE->list, list) {
			if ((char)m->name[0] != '\x01') { /* ignore own module because informations are not valid */
				unhiddenModules[i] = m;
				i++;
			}
	  	}
	}
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
void init_module_proc(void)
{
	int i;
	char *module_list = vmalloc(MAX_MODULE_NAME_LEN * numberOfUnhiddenModules * sizeof(char));
	if (module_proc_folder != NULL)
		remove_proc_subtree(MODULE_PROC_NAME, get_proc_parent());
	module_proc_folder = create_proc_folder(MODULE_PROC_NAME, get_proc_parent());
	for (i=0; i< numberOfUnhiddenModules; i++) {
		create_proc_file_with_data(unhiddenModules[i]->name, module_proc_folder, module_proc_message[i]);
		concatenate_if_not_too_long(module_list, formats("%d: %s\n", i+1, unhiddenModules[i]->name), MAX_MODULE_NAME_LEN * numberOfUnhiddenModules);
	}
	create_proc_file_with_data("module_list", module_proc_folder, module_list);
}

void write_proc_module_info(void)
{
	int i;
	module_proc_message = vmalloc(numberOfUnhiddenModules * sizeof(*module_proc_message));
	if (module_proc_message == NULL) {
		printk(KERN_ALERT"KEROKID: ERROR: Could not allocate memory!\n");
		return;
	}
	for (i=0; i< numberOfUnhiddenModules; i++) {
		strcpy(module_proc_message[i], get_module_info(unhiddenModules[i]));
	}
	init_module_proc();
}
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0) */

void get_list_of_unhidden_modules(void)
{
	numberOfUnhiddenModules = get_number_of_known_modules() -1; /* information of own model not valid (-1) */
	alloc_memory_for(numberOfUnhiddenModules, (char **)&unhiddenModules);
	store_known_modules_to_list();
#if DEBUG
	print_unhidden_modules();
#endif
}

void init_common(void)
{
	get_list_of_unhidden_modules();
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	write_proc_module_info();
#endif
}

void clean_common(void)
{
	vfree(unhiddenModules);
	vfree(module_proc_message);
}

void refresh_module_info(void)
{
	clean_common();
	init_common();
}
