#ifndef KEROKID_NOTIFIER_H_
#define KEROKID_NOTIFIER_H_

#define MAX_NOTIFIER_NAME_LENGTH 20

struct notifier_functions{
	char name[MAX_NOTIFIER_NAME_LENGTH];
	int(*registerFunction)(struct notifier_block*);
	int(*unregisterFunction)(struct notifier_block*);
};

void check_notifier_subscriptions(void);
void init_notifier_check(void);

#endif /* KEROKID_NOTIFIER_H_ */
