/*
 * The KErnel ROotKIt Detector (KEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#define VMAP_AREA_LIST ((struct list_head*) 0xc146c704)  // must be set manually -> system.map

void check_memory(void);
void memory_proc(char buf[], unsigned int buf_size);

#endif /* MEMORY_H_ */
