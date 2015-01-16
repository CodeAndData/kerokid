/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include "kerokid.h"

#ifndef ADDRESSANALYSIS_H_
#define ADDRESSANALYSIS_H_

// -------- define architecture specific stuff --------
#if defined(_CONFIG_X86_)
#include <asm/pgtable_32_types.h> //includes VMALLOC_START / END
#define MODULE_MAPPING_SPACE_START VMALLOC_START
#define MODULE_MAPPING_SPACE_END VMALLOC_END
#endif

#if defined(_CONFIG_X86_64_)
#define MODULE_MAPPING_SPACE_START 0xffffffffa0000000
#define MODULE_MAPPING_SPACE_END 0xffffffffff5fffff
#endif

// ------ exported functions --------
char *get_unhidden_module_info(void);
int analyze_address(psize *addr);

#endif /* ADDRESSANALYSIS_H_ */
