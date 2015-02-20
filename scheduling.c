/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include <linux/delay.h>
#include <linux/proc_fs.h>

#include "kerokid.h"
#include "notifier.h"
#include "proc_file.h"
#include "main.h"


char msg[MAX_PROC_MESSAGE_LEN];


void init_rescan(void)
{
	clear_proc_message();
	refresh_module_info();

	mdelay(100);
	scan();
}

/* ------- rescan on module notification -----*/
int rescan_notifier_function(struct notifier_block *nblock, unsigned long code, void *param)
{
	printk(KERN_INFO"KEROKID: initiating rescan (module load)\n");
	init_rescan();
	return NOTIFY_OK;
}

struct notifier_block nb_rescan = {
	.notifier_call = rescan_notifier_function,
	.priority = INT_MAX - 1
};

/* ------ rescan on write to proc file -----*/
ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	unsigned long i;
	i = copy_from_user(msg, buf, count);
	printk(KERN_INFO"KEROKID: initiating rescan (manual)\n");
	init_rescan();
	return count;
}

struct file_operations proc_writing_fops = {
	write: write_proc
};

/* ------ init and cleanup ------ */
void init_scheduling(void)
{
	struct proc_dir_entry *reading_proc;
	reading_proc = proc_create("rescan", 0, get_proc_parent(), &proc_writing_fops);
	register_module_notifier(&nb_rescan);
}

void clean_scheduling(void)
{
	unregister_module_notifier(&nb_rescan);
}
