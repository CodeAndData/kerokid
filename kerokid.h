/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include <asm/asm-offsets.h>
#include <linux/version.h>

#ifndef KEROKID_H_
#define KEROKID_H_

#if defined(_CONFIG_DEBUG_)
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define NUMBER_OF_SYSCALLS __NR_syscall_max

// -------- define architecture specific stuff --------
#if defined(_CONFIG_X86_)
#define KERNEL_SPACE_START 0xc0000000
#define SPACE_WITH_SCT_START 0xc0000000
#define SPACE_WITH_SCT_END 0xd0000000
typedef unsigned int psize;
#endif

#if defined(_CONFIG_X86_64_)
#define KERNEL_SPACE_START 0xffffffff80000000
#define SPACE_WITH_SCT_START 0xffffffff81000000
#define SPACE_WITH_SCT_END 0xffffffffa2000000
typedef unsigned long psize;
#endif


// -------- common functions -------
void alloc_memory_for(unsigned int quantity,char **something);
void get_list_of_unhidden_modules(void);
void print_module_info(struct module *mod);
void print_unhidden_modules(void);
psize startOf(struct module *mod);
psize endOf(struct module *mod);
void init_common(void);
void clean_common(void);


#endif /* KEROKID_H_ */
