/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains proc file stuff
 */

#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>

#include "kerokid.h"
#include "proc_file.h"
#include "syscallTable.h"
#include "memory.h"


#define BUFFER_SIZE 30000
#define FORMAT_STR_LEN 500

extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;

char results_msg[MAX_PROC_MESSAGE_LEN];

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static struct proc_dir_entry *proc_parent_folder;


int proc_show_data(struct seq_file *m, void *v)
{
	seq_printf(m, "%s", (char *)m->private);
	return 0;
}

int proc_open_data(struct inode *inode, struct file *file)
{
	char *data;
	data = PDE_DATA(file_inode(file));
	return single_open(file, proc_show_data, (void *)data);
}

static const struct file_operations proc_data_fops = {
		.owner		= THIS_MODULE,
		.open		= proc_open_data,
		.read		= seq_read,
		.llseek		= seq_lseek,
		.release	= single_release,
};

struct proc_dir_entry *create_proc_file_with_data(char *name, struct proc_dir_entry *parent, void *data)
{
	struct proc_dir_entry *proc_file;
	proc_file = proc_create_data(name, 0, parent, &proc_data_fops, data);
	if (proc_file == NULL) {
		remove_proc_entry(name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", name);
		return NULL;
	}
	return proc_file;
}

struct proc_dir_entry *create_proc_folder(char *name, struct proc_dir_entry *parent)
{
	struct proc_dir_entry *proc_folder;
	proc_folder = proc_mkdir(name, parent);
	return proc_folder;
}

char *get_memory_proc_msg(void)
{
	int i;
	static char memory_msg[BUFFER_SIZE];
	psize **syscallTable = get_systemcall_table();
	for (i=0; i < numberOfUnhiddenModules; i++) {
		if (concatenate_if_not_too_long(memory_msg, formats("module %s %lx %lx\n", unhiddenModules[i]->name,
					(unsigned long) startOf(unhiddenModules[i]), (unsigned long) endOf(unhiddenModules[i])), BUFFER_SIZE)) {
			printk(KERN_ALERT "Error: memory proc message too short (for modules)\n");
			break;
		}
	}
	for (i=0; i < NUMBER_OF_SYSCALLS; i++) {
		if (concatenate_if_not_too_long(memory_msg, formats("syscall %d %lx\n", i, (unsigned long) syscallTable[i]), BUFFER_SIZE)) {
			printk(KERN_ALERT "Error: memory proc message too short (for syscalls)\n");
			break;
		}
	}

#if defined(_ENABLE_MEMORY_)
	memory_proc(memory_msg,BUFFER_SIZE);
#endif

	return memory_msg;
}

int init_proc_file(void)
{
	struct proc_dir_entry *results_proc, *memory_proc;
	proc_parent_folder = create_proc_folder(procfs_folder_name, NULL);
	if (!proc_parent_folder) {
	    printk(KERN_INFO "Error creating proc parent folder");
	    return -ENOMEM;
	}
	cat_proc_message("kerokid results:\n");
	results_proc = proc_create_data("results", 0, proc_parent_folder, &proc_data_fops, results_msg);
	memory_proc = proc_create_data("memory", 0, proc_parent_folder, &proc_data_fops, get_memory_proc_msg());
	return 0;
}

void proc_cleanup(void)
{
	remove_proc_subtree(procfs_folder_name, NULL);
}

struct proc_dir_entry *get_proc_parent(void)
{
	return proc_parent_folder;
}
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0) */

void cat_proc_message(char *message)
{
	concatenate_if_not_too_long(results_msg, message, MAX_PROC_MESSAGE_LEN);
}

int concatenate_if_not_too_long(char *dest, char *source, int maxlen)
{
	if (strlen(dest) + strlen(source) < maxlen) {
		strcat(dest, source);
		return 0;
	} else {
		printk(KERN_INFO"KEROKID: error concatenating proc message: buffer too small\n");
		return 1;
	}
}

char *formats(const char* format, ...)
{
    static char fstr[FORMAT_STR_LEN];
    va_list args;
    va_start(args, format);
	vsnprintf(fstr, sizeof(fstr), format, args);
    va_end(args);
    return fstr;
}
