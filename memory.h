/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include <linux/kallsyms.h>

#ifndef KEROKID_MEMORY_H_
#define KEROKID_MEMORY_H_

#define VMAP_AREA_LIST ((struct list_head *)kallsyms_lookup_name("vmap_area_list"))

void check_memory(void);
void memory_proc(char buf[], unsigned int buf_size);

#endif /* KEROKID_MEMORY_H_ */
