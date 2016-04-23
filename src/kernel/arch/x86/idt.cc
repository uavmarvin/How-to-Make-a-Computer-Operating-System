#include <idt.h>

#include <os.h>

#if defined (__cplusplus)
extern "C" {
#endif

regs_t* stack_ptr = 0;
interrupt_handler_t interrupt_handlers[IDTSIZE]; /* Interruptions functions tables */
idtdesc 	kidt[IDTSIZE]; 		/* IDT table */
idtr 		kidtr; 				/* IDTR registry */

void init_idt_desc(u8 num, u16 select, u32 offset, u16 type)
{
	kidt[num].offset0_15 = (offset & 0xffff);
	kidt[num].select = select;
	kidt[num].type = type;
	kidt[num].offset16_31 = (offset & 0xffff0000) >> 16;
	return;
}

void register_interrupt_handler(u8 n, interrupt_handler_t h)
{
	interrupt_handlers[n] = h;
}

void isr_handler(regs_t *regs)
{
	if (interrupt_handlers[regs->which_int]) {
		interrupt_handlers[regs->which_int](regs);
	} else {
		io.print("[Kernel] Unhandled interrupt: %d\n", regs->which_int);
	}
}

/*
 * Init IDT after kernel is loaded
 */
void init_idt(void)
{
	/* Init irq */
	memset((char*)interrupt_handlers, 0, sizeof(interrupt_handler_t) * IDTSIZE);
	memset((char*)kidt, 0, sizeof(idtdesc) * IDTSIZE);
	
	/* Vectors  0 -> 31 are for exceptions */
	init_idt_desc( 0, 0x08, (u32)isr0,  INTGATE);
	init_idt_desc( 1, 0x08, (u32)isr1,  INTGATE);
	init_idt_desc( 2, 0x08, (u32)isr2,  INTGATE);
	init_idt_desc( 3, 0x08, (u32)isr3,  INTGATE);
	init_idt_desc( 4, 0x08, (u32)isr4,  INTGATE);
	init_idt_desc( 5, 0x08, (u32)isr5,  INTGATE);
	init_idt_desc( 6, 0x08, (u32)isr6,  INTGATE);
	init_idt_desc( 7, 0x08, (u32)isr7,  INTGATE);
	init_idt_desc( 8, 0x08, (u32)isr8,  INTGATE);
	init_idt_desc( 9, 0x08, (u32)isr9,  INTGATE);
	init_idt_desc(10, 0x08, (u32)isr10, INTGATE);
	init_idt_desc(11, 0x08, (u32)isr11, INTGATE);
	init_idt_desc(12, 0x08, (u32)isr12, INTGATE);
	init_idt_desc(13, 0x08, (u32)isr13, INTGATE); /* #GP */
	init_idt_desc(14, 0x08, (u32)isr14, INTGATE); /* #PF */
	init_idt_desc(15, 0x08, (u32)isr15, INTGATE);
	init_idt_desc(16, 0x08, (u32)isr16, INTGATE);
	init_idt_desc(17, 0x08, (u32)isr17, INTGATE);
	init_idt_desc(18, 0x08, (u32)isr18, INTGATE);
	init_idt_desc(19, 0x08, (u32)isr19, INTGATE);
	init_idt_desc(20, 0x08, (u32)isr20, INTGATE);
	init_idt_desc(21, 0x08, (u32)isr21, INTGATE);
	init_idt_desc(22, 0x08, (u32)isr22, INTGATE);
	init_idt_desc(23, 0x08, (u32)isr23, INTGATE);
	init_idt_desc(24, 0x08, (u32)isr24, INTGATE);
	init_idt_desc(25, 0x08, (u32)isr25, INTGATE);
	init_idt_desc(26, 0x08, (u32)isr26, INTGATE);
	init_idt_desc(27, 0x08, (u32)isr27, INTGATE);
	init_idt_desc(28, 0x08, (u32)isr28, INTGATE);
	init_idt_desc(29, 0x08, (u32)isr29, INTGATE);
	init_idt_desc(30, 0x08, (u32)isr30, INTGATE);
	init_idt_desc(31, 0x08, (u32)isr31, INTGATE);
	
	init_idt_desc(32, 0x08, (u32)isr32, INTGATE); /* timer */
	init_idt_desc(33, 0x08, (u32)isr33, INTGATE); /* keyboard */
	
	init_idt_desc(48, 0x08, (u32)isr48, TRAPGATE); /* syscalls */
	init_idt_desc(80, 0x08, (u32)isr80, TRAPGATE); /* syscalls */
	init_idt_desc(128, 0x08, (u32)isr128, TRAPGATE); /* syscalls */
	
	kidtr.limite = IDTSIZE * 8;
	kidtr.base = IDTBASE;
	
	/* Copy the IDT to the memory */
	memcpy((char *) kidtr.base, (char *) kidt, kidtr.limite);

	/* Load the IDTR registry */
	asm("lidtl (kidtr)");
}

void init_pic(void)
{
	/* Initialization of ICW1 */
	io.outb(0x20, 0x11);
	io.outb(0xA0, 0x11);

	/* Initialization of ICW2 */
	io.outb(0x21, 0x20);	/* start vector = 32 */
	io.outb(0xA1, 0x70);	/* start vector = 96 */

	/* Initialization of ICW3 */
	io.outb(0x21, 0x04);
	io.outb(0xA1, 0x02);

	/* Initialization of ICW4 */
	io.outb(0x21, 0x01);
	io.outb(0xA1, 0x01);

	/* mask interrupts */
	io.outb(0x21, 0x0);
	io.outb(0xA1, 0x0);
}

#if defined (__cplusplus)
}
#endif
