/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains common functions
 */

#include "kerokid.h"

struct module **unhiddenModules;
unsigned int numberOfUnhiddenModules;


void alloc_memory_for(unsigned int quantity,char **something){
	if (quantity) *something = vmalloc(quantity * sizeof(char*));
	if (something == NULL) printk(KERN_ALERT"KEROKID: ERROR: Could not allocate memory!\n");
}

psize startOf(struct module *mod){
	return (psize)mod->module_core;
}
psize endOf(struct module *mod){
	return (psize)mod->module_core + mod->core_size;
}

// ------------ module printing functions --------------------

void print_module_info(struct module *mod){
	printk(KERN_INFO"KEROKID: Name:           %s\n",mod->name);
	printk(KERN_INFO"KEROKID: Start-Address:  %lx\n",(unsigned long)startOf(mod));
	printk(KERN_INFO"KEROKID: End-Address:    %lx\n",(unsigned long)endOf(mod));
	printk(KERN_INFO"KEROKID: Object-Address: %lx\n",(unsigned long)mod);
	printk(KERN_INFO"KEROKID: Size:           %u Bytes\n",mod->core_size);
	if (mod->state == MODULE_STATE_LIVE){
		printk(KERN_INFO"KEROKID: Module-State:   LIVE\n");
	}
	else{
		printk(KERN_INFO"KEROKID: Module-State:   %d\n",(int)mod->state);
	}

}

void print_unhidden_modules(void){
	int i;
	printk(KERN_INFO"KEROKID: --------> unhidden Modules (%d) <---------   \n",numberOfUnhiddenModules);
	for (i=0; i< numberOfUnhiddenModules; i++){
		print_module_info(unhiddenModules[i]);
	}
}

//----  module specific stuff


unsigned int get_number_of_known_modules(void){
	struct module *m;
	unsigned int counter = 0;
	list_for_each_entry(m,&THIS_MODULE->list,list){
    	counter++;
  	}
	return counter;
}

void store_known_modules_to_list(void){
  	struct module *m;
	unsigned int i = 0;
	if (unhiddenModules != NULL){
		list_for_each_entry(m,&THIS_MODULE->list,list) {
			if (i < numberOfUnhiddenModules) { 							// ignore own module because informations are not valid
				unhiddenModules[i] = m;
			}
			i++;
	  	}
	}
}

void get_list_of_unhidden_modules(void){
	numberOfUnhiddenModules = get_number_of_known_modules() -1; // information of own model not valid (-1)
	alloc_memory_for(numberOfUnhiddenModules,(char**)&unhiddenModules);
	store_known_modules_to_list();
#if DEBUG
	print_unhidden_modules();
#endif
}

void init_common(void){
	get_list_of_unhidden_modules();
}

void clean_common(void){
	vfree(unhiddenModules);
}
