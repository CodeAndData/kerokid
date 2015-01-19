/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#include "kerokid.h"

#ifndef SYSCALLTABLE_H_
#define SYSCALLTABLE_H_

void init_systemcall_table(void);
psize **get_systemcall_table(void);
void check_syscall_table(void);

#endif /* SYSCALLTABLE_H_ */
