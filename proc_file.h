#include <linux/version.h>

#ifndef KEROKID_PROC_FILE_H_
#define KEROKID_PROC_FILE_H_

#define procfs_folder_name "kerokid"
#define MAX_PROC_MESSAGE_LEN 2000

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
int init_proc_file(void);
void proc_cleanup(void);

struct proc_dir_entry* create_proc_folder(char *name, struct proc_dir_entry *parent);
struct proc_dir_entry* create_proc_file_with_data(char *name, struct proc_dir_entry *parent, void *data);
struct proc_dir_entry* get_proc_parent(void);
#endif

void clear_proc_message(void);
void cat_proc_message(char *message);
int concatenate_if_not_too_long(char *target, char *destination, int maxLen);
char* formats(const char *format, ...);

#endif /* KEROKID_PROC_FILE_H_ */
