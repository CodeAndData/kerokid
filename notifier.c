/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains notifier stuff
 */

#include <linux/keyboard.h>
#include "kerokid.h"
#include "addressAnalysis.h"
#include <linux/netdevice.h>
#include <net/netevent.h>
#include <linux/dca.h>
#include <linux/usb.h>

#define MAX_NOTIFIER_NAME_LENGTH 20

struct notifier_functions{
	char name[MAX_NOTIFIER_NAME_LENGTH];
	int(*registerFunction)(struct notifier_block*);
	int(*unregisterFunction)(struct notifier_block*);
};

int numberOfInterestingNotifiers;
struct notifier_functions *interestingNotifiers;

struct notifier_functions init_a_notifier(char* name,int(*registerFunction)(struct notifier_block*),int(*unregisterFunction)(struct notifier_block*) ){
		struct notifier_functions n = {
				.registerFunction = registerFunction,
				.unregisterFunction = unregisterFunction
		};
		memcpy(n.name,name,MAX_NOTIFIER_NAME_LENGTH);
		return n;
};


void init_notifiers(void){
	numberOfInterestingNotifiers = 4;	// increase this number if you add a notifier
	interestingNotifiers = vmalloc(numberOfInterestingNotifiers * sizeof(struct notifier_functions));

// ----- add new notifiers here -----
	//keyboard
	interestingNotifiers[0]= init_a_notifier("keyboard",&register_keyboard_notifier,&unregister_keyboard_notifier);
	interestingNotifiers[1]= init_a_notifier("module",&register_module_notifier,&unregister_module_notifier);
	interestingNotifiers[2]= init_a_notifier("netdevice",&register_netdevice_notifier,&unregister_netdevice_notifier);
	interestingNotifiers[3]= init_a_notifier("netevent",&register_netevent_notifier,&unregister_netevent_notifier);

	//currently not working due to incompatible function signature
	//interestingNotifiers[4]= init_a_notifier("dca",&dca_register_notify,&dca_unregister_notify);
	//interestingNotifiers[5]= init_a_notifier("usb",&usb_register_notify,&usb_unregister_notify);
}

int blank(struct notifier_block *nblock, unsigned long code, void *param)
{
	return NOTIFY_OK;
}

struct notifier_block nb = {
	.notifier_call = blank,
	.priority = INT_MAX
};

void notifier_init(struct notifier_functions n){
	n.registerFunction(&nb);
}

void notifier_clean(struct notifier_functions n){
	n.unregisterFunction(&nb);
}

void check_blocks(struct notifier_block *nblock){
	struct notifier_block *current_block = nblock;
	while (current_block != NULL){
#if DEBUG
		printk(KERN_INFO"KEROKID: --> current block in chain: %lx \n",(unsigned long)current_block);
#endif
		analyze_address((psize*)current_block->notifier_call);
		current_block = current_block->next;
	}
}

void check_notifier_subscriptions(void){
	int i;
	init_notifiers();
	for (i = 0; i < numberOfInterestingNotifiers; i++){
		printk(KERN_INFO"KEROKID: -> Check %s notifier chain... \n",interestingNotifiers[i].name);
		notifier_init(interestingNotifiers[i]);
		check_blocks(nb.next);
		notifier_clean(interestingNotifiers[i]);
	}
	return;
}
