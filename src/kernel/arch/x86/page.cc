#include <page.h>

#include <idt.h>
#include <scheduler.h>

void isr_GP_int(regs_t* regs_ptr)
{
	u32 faulting_addr;
	 asm("mov %%cr2, %%eax	\n \
		mov %%eax, %0"
		: "=m"(faulting_addr));
	io.print("\n General protection fault !\n");
	io.print("No autorized memory acces on : %p (eip:%p,code:%p)\n", faulting_addr, regs_ptr->eip,  regs_ptr->cs);
	
	if (arch.pcurrent!=NULL){
		io.print("The processus %s have to be killed !\n\n",(arch.pcurrent)->getName());
		(arch.pcurrent)->exit();
		schedule(regs_ptr);
	}
	else{
		io.print("The kernel have to be killed !\n\n");
		asm("hlt");
	}
}

void isr_PF_int(regs_t* regs_ptr)
{
	u32 faulting_addr;
	struct page *pg;
	u32 stack;
 	asm("mov %%cr2, %%eax	\n \
		mov %%eax, %0"
		: "=m"(faulting_addr));
	asm("mov %%ebp, %0": "=m"(stack):);
	
	//io.print("#PF : %x \n",faulting_addr);
	
	//for (;;);
	if (arch.pcurrent==NULL)
		return;
			
	process_st* current=arch.pcurrent->getPInfo();

	if (faulting_addr >= USER_OFFSET && faulting_addr <= USER_STACK) {
		pg = (struct page *) kmalloc(sizeof(struct page));
		pg->p_addr = get_page_frame();
		pg->v_addr = (char *) (faulting_addr & 0xFFFFF000);
		list_add(&pg->list, &current->pglist);
		pd_add_page(pg->v_addr, pg->p_addr, PG_USER, current->pd);
	}
	else {
		io.print("\n");
		io.print("No autorized memory acces on : %p (eip:%p,code:%p)\n", faulting_addr, regs_ptr->eip,  regs_ptr->cs);
		io.print("heap=%x, heap_limit=%x, stack=%x\n",kern_heap,KERN_HEAP_LIM,stack);
		
		if (arch.pcurrent!=NULL){
			io.print("The processus %s have to be killed !\n\n",(arch.pcurrent)->getName());
			(arch.pcurrent)->exit();
			schedule(regs_ptr);
		}
		else{
			io.print("The kernel have to be killed !\n\n");
			asm("hlt");
		}
	}
		
}

void init_page(void)
{
	register_interrupt_handler(13, isr_GP_int);
	register_interrupt_handler(14, isr_PF_int);
}
