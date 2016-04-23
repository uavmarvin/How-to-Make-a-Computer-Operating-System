#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <os.h>
#include <idt.h>

#if defined (__cplusplus)
extern "C" {
#endif

void switch_to_task(process_st* current, int mode);
void schedule(regs_t* regs_ptr);
void init_scheduler(u32 frequency);

#if defined (__cplusplus)
}
#endif

#endif /* _SCHEDULER_H_ */
