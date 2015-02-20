#include "kerokid.h"

#ifndef KEROKID_ADDRESSANALYSIS_H_
#define KEROKID_ADDRESSANALYSIS_H_

/* -------- define architecture specific stuff -------- */
#if defined(_CONFIG_X86_)
#include <asm/pgtable_32_types.h> //includes VMALLOC_START / END
#define MODULE_MAPPING_SPACE_START VMALLOC_START
#define MODULE_MAPPING_SPACE_END VMALLOC_END
#endif

#if defined(_CONFIG_X86_64_)
#define MODULE_MAPPING_SPACE_START 0xffffffffa0000000
#define MODULE_MAPPING_SPACE_END 0xffffffffff5fffff
#endif

/* ------ exported functions -------- */
int address_is_part_of_vmalloc(psize *addr);
int address_is_part_of_moduleSpace(psize *addr);
char *get_unhidden_module_info(void);
int analyze_address(psize *addr, char *source);

#endif /* KEROKID_ADDRESSANALYSIS_H_ */
