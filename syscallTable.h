#include "kerokid.h"

#ifndef KEROKID_SYSCALLTABLE_H_
#define KEROKID_SYSCALLTABLE_H_

void init_systemcall_table(void);
psize **get_systemcall_table(void);
void check_syscall_table(void);

#endif /* KEROKID_SYSCALLTABLE_H_ */
