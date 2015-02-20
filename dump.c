/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains stuff for memory dumping
 */

#include <linux/proc_fs.h>
#include <linux/slab.h>

#include "dump.h"
#include "kerokid.h"
#include "proc_file.h"
#include "addressAnalysis.h"


#define MEMORY_DUMP_PROC_NAME "memory_dump"

char current_dump_file_name[MAX_PROC_NAME_LEN];
ssize_t dump_size;
extern struct module **unhiddenModules;
extern unsigned int numberOfUnhiddenModules;
char user_buffer[MAX_PROC_MESSAGE_LEN];
char *memory_buffer = NULL;
char *module_buffer = NULL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static struct proc_dir_entry *dump_proc_folder;


void create_module_dump_proc(int module_number);
void create_memory_dump_proc(unsigned long lower, unsigned long upper);

char *get_module_dump(int module_num)
{
	struct module *mod = unhiddenModules[module_num];
	char *buffer;
	buffer = kmalloc(mod->core_size * sizeof(char), GFP_KERNEL);
	memcpy((void *)buffer, (void *)startOf(mod), mod->core_size);
	return buffer;
}

ssize_t dump_proc_read(struct file *filp, char *buf, size_t count, loff_t *offp)
{
	char *data;
	ssize_t ret = 0, retval = 0;
	size_t len = count;
	data = PDE_DATA(file_inode(filp));

	if (*offp >= dump_size)
	        goto out;
	if (*offp + count > dump_size)
		len = dump_size - *offp;

	ret = copy_to_user(buf, data, len);

	*offp += len - ret;
	retval = len - ret;

out:
	return retval;
}

ssize_t dump_proc_write(struct file *filp, const char *buf, size_t count, loff_t *offp)
{
	unsigned long i, user_input, address = 0;
	memset((void *)user_buffer, 0, MAX_PROC_MESSAGE_LEN); /* clear user input buffer */
	i = copy_from_user(user_buffer, buf, count);
	i = kstrtol(user_buffer, 0, &user_input);

	if (user_input == 0 || user_input > numberOfUnhiddenModules)
		if (address_is_part_of_vmalloc((psize *)user_input) || address_is_part_of_moduleSpace((psize *)user_input)) { /* user wants to dump an address range */
			if (address) {/* user has specified lower end of address space to be dumped */
				create_memory_dump_proc(address, user_input);
				address = 0;
			} else {/* only one address has been specified */
				address = user_input;
				printk(KERN_INFO"KEROKID: Lower address range is set to %lx. Please specify the upper limit.\n", address);
			}
		} else { /* user input is neither module number nor legal address */
			printk(KERN_INFO"KEROKID: Illegal entry found in dump module file: %lx. Please specify the module number (1-%d) or kernel addresses\n", user_input, numberOfUnhiddenModules);
		}
	else /* user wants to dump a module */
		create_module_dump_proc(user_input - 1); /* modules are numbered from 0 to n-1*/
	return count;
}

struct file_operations dump_fops = {
		read: dump_proc_read,
		write: dump_proc_write
};

void create_module_dump_proc(int module_number)
{
	struct proc_dir_entry *dump_module_proc;
	if (*current_dump_file_name) {
		remove_proc_entry(current_dump_file_name, dump_proc_folder);
		kfree(module_buffer);
	}
	printk(KERN_INFO"KEROKID: creating dump for module '%s'\n", unhiddenModules[module_number]->name);
	dump_size = unhiddenModules[module_number]->core_size;
	module_buffer = get_module_dump(module_number);
	sprintf(current_dump_file_name, "%s_dump", unhiddenModules[module_number]->name);
	dump_module_proc = proc_create_data(current_dump_file_name, 0, dump_proc_folder, &dump_fops, module_buffer);
}

void create_memory_dump_proc(unsigned long lower, unsigned long upper)
{
	struct proc_dir_entry *dump_memory_proc = NULL;
	if (dump_memory_proc) {
		remove_proc_entry(MEMORY_DUMP_PROC_NAME, dump_proc_folder);
		kfree(memory_buffer);
	}
	memory_buffer = kmalloc((upper - lower) * sizeof(char), GFP_KERNEL);
	printk(KERN_INFO"KEROKID: creating dump from address range %lx-%lx", lower, upper);
	memcpy((void *)memory_buffer, (void *)lower, upper - lower);
	dump_memory_proc = proc_create_data(MEMORY_DUMP_PROC_NAME, 0, dump_proc_folder, &dump_fops, memory_buffer);
}
#endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)*/

void init_dump_proc(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	struct proc_dir_entry *dump_input_proc;
	dump_proc_folder = create_proc_folder("dump", get_proc_parent());
	dump_input_proc = proc_create("dump_module", 0, dump_proc_folder, &dump_fops);
#endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)*/
}

void dump_cleanup(void)
{
	kfree(memory_buffer);
	kfree(module_buffer);
}
