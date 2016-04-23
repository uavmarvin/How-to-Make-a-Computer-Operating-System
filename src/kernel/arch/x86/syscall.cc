#include <syscalls.h>
#include <idt.h>
#include <architecture.h>

#if defined (__cplusplus)
extern "C" {
#endif

void isr_syscall_int(regs_t* regs_ptr)
{

	 arch.setParam(regs_ptr->ebx, regs_ptr->ecx, regs_ptr->edx, regs_ptr->edi, regs_ptr->esi);
	 
	 asm("cli");
	 stack_ptr = regs_ptr;
	 syscall.call(regs_ptr->eax);
	 asm("sti");

}

void init_syscall(void)
{
	register_interrupt_handler(128, isr_syscall_int);
}

#if defined (__cplusplus)
}
#endif
