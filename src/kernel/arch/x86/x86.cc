#include <os.h>
#include <x86.h>
#include <keyboard.h>


extern "C" {

regs_t cpu_cpuid(int code)
{
	regs_t r;
	asm volatile("cpuid":"=a"(r.eax),"=b"(r.ebx),
                 "=c"(r.ecx),"=d"(r.edx):"0"(code));
	return r;
}


u32 cpu_vendor_name(char *name)
{
		regs_t r = cpu_cpuid(0x00);
		
		char line1[5];
		line1[0] = ((char *) &r.ebx)[0];
		line1[1] = ((char *) &r.ebx)[1];
		line1[2] = ((char *) &r.ebx)[2];
		line1[3] = ((char *) &r.ebx)[3];
		line1[4] = '\0';

		char line2[5];
		line2[0] = ((char *) &r.ecx)[0];
		line2[1] = ((char *) &r.ecx)[1];
		line2[2] = ((char *) &r.ecx)[2];
		line2[3] = ((char *) &r.ecx)[3];
		line2[4] = '\0';
		
		char line3[5];
		line3[0] = ((char *) &r.edx)[0];
		line3[1] = ((char *) &r.edx)[1];
		line3[2] = ((char *) &r.edx)[2];
		line3[3] = ((char *) &r.edx)[3];
		line3[4] = '\0';
							
		strcpy(name, line1);
		strcat(name, line3);
		strcat(name, line2);
		return 15;
}


void schedule(regs_t*);

idtdesc 	kidt[IDTSIZE]; 		/* IDT table */
interrupt_handler_t interrupt_handlers[IDTSIZE]; 		/* Interruptions functions tables */
gdtdesc 	kgdt[GDTSIZE];		/* GDT */
tss 		default_tss;
gdtr 		kgdtr;				/* GDTR */
idtr 		kidtr; 				/* IDTR registry */
regs_t*		stack_ptr = 0;

/*
 * 'init_desc' initialize a segment descriptor in gdt or ldt.
 * 'desc' is a pointer to the address
 */
void init_gdt_desc(u32 base, u32 limite, u8 acces, u8 other,struct gdtdesc *desc)
{
	desc->lim0_15 = (limite & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->acces = acces;
	desc->lim16_19 = (limite & 0xf0000) >> 16;
	desc->other = (other & 0xf);
	desc->base24_31 = (base & 0xff000000) >> 24;
	return;
}


/*
 * This function initialize the GDT after the kernel is loaded.
 */
void init_gdt(void)
{

	default_tss.debug_flag = 0x00;
	default_tss.io_map = 0x00;
	default_tss.esp0 = 0x1FFF0;
	default_tss.ss0 = 0x18;

	/* initialize gdt segments */
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]);	/* code */
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]);	/* data */
	init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]);		/* stack */

	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]);	/* ucode */
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]);	/* udata */
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]);		/* ustack */

	init_gdt_desc((u32) & default_tss, 0x67, 0xE9, 0x00, &kgdt[7]);	/* descripteur de tss */

	/* initialize the gdtr structure */
	kgdtr.limite = GDTSIZE * 8;
	kgdtr.base = GDTBASE;

	/* copy the gdtr to its memory area */
	memcpy((char *) kgdtr.base, (char *) kgdt, kgdtr.limite);

	/* load the gdtr registry */
	asm("lgdtl (kgdtr)");

	/* initiliaz the segments */
	asm("   movw $0x10, %ax	\n \
            movw %ax, %ds	\n \
            movw %ax, %es	\n \
            movw %ax, %fs	\n \
            movw %ax, %gs	\n \
            ljmp $0x08, $next	\n \
            next:		\n");
			
}


void init_idt_desc(u8 num, u16 select, u32 offset, u16 type)
{
	kidt[num].offset0_15 = (offset & 0xffff);
	kidt[num].select = select;
	kidt[num].type = type;
	kidt[num].offset16_31 = (offset & 0xffff0000) >> 16;
	return;
}

void isr_syscalls_int(regs_t* regs_ptr)
{
	 arch.setParam(regs_ptr->ebx, regs_ptr->ecx, regs_ptr->edx, regs_ptr->edi, regs_ptr->esi);
	 
	 asm("cli");
	 stack_ptr = regs_ptr;
	 syscall.call(regs_ptr->eax);
	 asm("sti");
}



void isr_keyboard_int(regs_t* regs_ptr)
{
	u8 i;
	static int lshift_enable;
	static int rshift_enable;
	static int alt_enable;
	static int ctrl_enable;
	do {
		i = io.inb(0x64);
	} while ((i & 0x01) == 0);
	

	i = io.inb(0x60);
	i--;

	if (i < 0x80) {		/* touche enfoncee */
		switch (i) {
		case 0x29:
			lshift_enable = 1;
			break;
		case 0x35:
			rshift_enable = 1;
			break;
		case 0x1C:
			ctrl_enable = 1;
			break;
		case 0x37:
			alt_enable = 1;
			break;
		default:
		
				if(kbdmap == kbdmap_fr && alt_enable == 1)
				{
					io.putctty(kbdmap[i * 4 + 2]);
					if (&io != io.current_io)
					io.current_io->putctty(kbdmap[i * 4 + 2]);
		 
				}
				else if(lshift_enable == 1 || rshift_enable == 1)
				{
		 
					io.putctty(kbdmap[i * 4 + 1]);
					if (&io != io.current_io)
						io.current_io->putctty(kbdmap[i * 4 + 1]);
		 
				}
				else
				{
					io.putctty(kbdmap[i * 4]);
					if (&io != io.current_io)
						io.current_io->putctty(kbdmap[i * 4]);
		 
				}
               break;

			//io.print("sancode: %x \n",i * 4 + (lshift_enable || rshift_enable));
			/*io.putctty(kbdmap[i * 4 + (lshift_enable || rshift_enable)]);	//replacé depuis la 10.4.6
			if (&io != io.current_io)
				io.current_io->putctty(kbdmap[i * 4 + (lshift_enable || rshift_enable)]);*/
			break;
		}
	} else {		/* touche relachee */
		i -= 0x80;
		switch (i) {
		case 0x29:
			lshift_enable = 0;
			break;
		case 0x35:
			rshift_enable = 0;
			break;
		case 0x1C:
			ctrl_enable = 0;
			break;
		case 0x37:
			alt_enable = 0;
			break;
		}
	}
	
	io.outb(0x20,0x20);
	io.outb(0xA0,0x20); 
}

void isr_schedule_int(regs_t* regs_ptr)
{
	static int tic = 0;
	static int sec = 0;
	tic++;
	if (tic % 100 == 0) {
		sec++;
		tic = 0;
	}
	schedule(regs_ptr);
	io.outb(0x20,0x20);
	io.outb(0xA0,0x20);
}

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

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
void isr32();
void isr33();
void isr48();
void isr80();
void isr128();

void register_interrupt_handler(u8 n, interrupt_handler_t h)
{
	interrupt_handlers[n] = h;
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
	
	register_interrupt_handler(13, isr_GP_int);
	register_interrupt_handler(14, isr_PF_int);
	register_interrupt_handler(32, isr_schedule_int);
	register_interrupt_handler(33, isr_keyboard_int);
	//register_interrupt_handler(48, isr_syscalls_int);
	register_interrupt_handler(128, isr_syscalls_int);
	
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

void isr_handler(regs_t *regs)
{
	if (interrupt_handlers[regs->which_int]) {
		interrupt_handlers[regs->which_int](regs);
	} else {
		io.print("[Kernel] Unhandled interrupt: %d\n", regs->which_int);
	}
}

#define DEBUG_REG(a) io.print("  %s : %x",#a,p->regs.a)

void schedule(regs_t* regs_ptr){
	Process* pcurrent=arch.pcurrent;
	Process*plist=arch.plist;
	if (pcurrent==0)
		return;

	if (pcurrent->getPNext() == 0 && plist==pcurrent)	//si le proc est seul
		return;

	process_st* current=pcurrent->getPInfo();
	process_st *p;
	int i, newpid;

	/* stack_ptr stores in the pointer to the backup registers */
	stack_ptr = regs_ptr;
	//asm("mov (%%eip), %%eax; mov %%eax, %0": "=m"(current->regs.eip):);
	
	//io.print("stack_ptr : %x \n",stack_ptr);
	/* save the records of the current process */
	current->regs.eflags = regs_ptr->eflags;
	current->regs.cs = regs_ptr->cs;
	current->regs.eip = regs_ptr->eip;
	current->regs.eax = regs_ptr->eax;
	current->regs.ecx = regs_ptr->ecx;
	current->regs.edx = regs_ptr->edx;
	current->regs.ebx = regs_ptr->ebx;
	current->regs.ebp = regs_ptr->ebp;
	current->regs.esi = regs_ptr->esi;
	current->regs.edi = regs_ptr->edi;
	current->regs.ds = regs_ptr->ds;
	current->regs.es = regs_ptr->es;
	current->regs.fs = regs_ptr->fs;
	current->regs.gs = regs_ptr->gs;

	/* 
	 * Sauvegarde le contenu des registres de pile (ss, esp)
	 * au moment de l'interruption. Necessaire car le processeur
	 * empile ou non ces valeurs selon le contexte de l'interruption.
	 */
	if (current->regs.cs != 0x08) {	/* user mode */
		current->regs.esp = regs_ptr->user_esp;
		current->regs.ss = regs_ptr->user_ss;
	} else { /* during a system call */
		current->regs.esp = regs_ptr->esp + 20;	/* regs_ptr->esp: which_int, +4: err_code, +8: eip, +12: cs, +16: eflags, +20: user_esp */
		current->regs.ss = default_tss.ss0;
	}

	/* save the TSS of the old process */
	current->kstack.ss0 = default_tss.ss0;
	current->kstack.esp0 = default_tss.esp0;
	
	//io.print("schedule %s ",pcurrent->getName());
	pcurrent=pcurrent->schedule();
	p = pcurrent->getPInfo();

	//io.print("to %s \n",pcurrent->getName());
	/*DEBUG_REG(eax);
	DEBUG_REG(ebx);
	DEBUG_REG(ecx);
	DEBUG_REG(edx);*/
	/*DEBUG_REG(esp); io.print("\t");
	DEBUG_REG(ebp);	io.print("\n");*/
	//DEBUG_REG(esi);
	//DEBUG_REG(edi);
	//DEBUG_REG(eip);	io.print("\t");
	/*DEBUG_REG(eflags);
	DEBUG_REG(cs);
	DEBUG_REG(ss);
	DEBUG_REG(ds);
	DEBUG_REG(es);
	DEBUG_REG(fs);
	DEBUG_REG(gs);
	DEBUG_REG(cr3);
	io.print("\n");*/
	
	/* Commutation */
	if (p->regs.cs != 0x08)
		switch_to_task(p, USERMODE);
	else
		switch_to_task(p, KERNELMODE);
}

/* 
 * switch_to_task(): Prepare la commutation de tache effectuee par do_switch().
 * Le premier parametre indique le pid du processus a charger.
 * Le mode indique si ce processus etait en mode utilisateur ou en mode kernel
 * quand il a ete precedement interrompu par le scheduler.
 * L'empilement des registres sur la pile change selon le cas.
 */
void switch_to_task(process_st* current, int mode)
{

	u32 kesp, eflags;
	u16 kss, ss, cs;
	int sig;
	
	/* Traite les signaux */

		if ((sig = dequeue_signal(current->signal))) 
			handle_signal(sig);
	
	/* Charge le TSS du nouveau processus */
	default_tss.ss0 = current->kstack.ss0;
	default_tss.esp0 = current->kstack.esp0;

	/* 
	 * Empile les registres ss, esp, eflags, cs et eip necessaires a la
	 * commutation. Ensuite, la fonction do_switch() restaure les
	 * registres, la table de page du nouveau processus courant et commute
	 * avec l'instruction iret.
	 */
	ss = current->regs.ss;
	cs = current->regs.cs;
	eflags = (current->regs.eflags | 0x200) & 0xFFFFBFFF;
	

	
	/* Prepare le changement de pile noyau */
	if (mode == USERMODE) {
		kss = current->kstack.ss0;
		kesp = current->kstack.esp0;
	} else {			/* KERNELMODE */
		kss = current->regs.ss;
		kesp = current->regs.esp;
	}
	
	
	//io.print("switch to %x \n",current->regs.eip);
	
	asm("	mov %0, %%ss; \
		mov %1, %%esp; \
		cmp %[KMODE], %[mode]; \
		je nextt; \
		push %2; \
		push %3; \
		nextt: \
		push %4; \
		push %5; \
		push %6; \
		push %7; \
		ljmp $0x08, $do_switch" 
		:: \
		"m"(kss), \
		"m"(kesp), \
		"m"(ss), \
		"m"(current->regs.esp), \
		"m"(eflags), \
		"m"(cs), \
		"m"(current->regs.eip), \
		"m"(current), \
		[KMODE] "i"(KERNELMODE), \
		[mode] "g"(mode)
	    );
}


int dequeue_signal(int mask) 
{
	int sig;

	if (mask) {
		sig = 1;
		while (!(mask & 1)) {
			mask = mask >> 1;
			sig++;
		}
	}
	else
		sig = 0;

	return sig;
}

int handle_signal(int sig)
{
	Process* pcurrent=arch.pcurrent;
	if (pcurrent==0)
		return 0;

	process_st* current=pcurrent->getPInfo();
	
	u32 *esp;

	//io.print("signal> handle signal : signal %d for process %d\n", sig, pcurrent->getPid());

	if (current->sigfn[sig] == (void*) SIG_IGN) {
		clear_signal(&(current->signal), sig);
	}
	else if (current->sigfn[sig] == (void*) SIG_DFL) {
		switch(sig) {
			case SIGHUP : case SIGINT : case SIGQUIT : 
				asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"(current->regs.cr3));
				pcurrent->exit();
				break;
			case SIGCHLD : 
				break;
			default :
				clear_signal(&(current->signal), sig);
		}
	}
	else {

		esp = (u32*) current->regs.esp - 20;

		asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"(current->regs.cr3));
		
		// Code assembleur qui appelle sys_sigreturn() 
		esp[19] = 0x0030CD00;
		esp[18] = 0x00000EB8;

		// Sauvegarde des registres 
		esp[17] = current->kstack.esp0;
		esp[16] = current->regs.ss;
		esp[15] = current->regs.esp;
		esp[14] = current->regs.eflags;
		esp[13] = current->regs.cs;
		esp[12] = current->regs.eip;
		esp[11] = current->regs.eax;
		esp[10] = current->regs.ecx;
		esp[9] = current->regs.edx;
		esp[8] = current->regs.ebx;
		esp[7] = current->regs.ebp;
		esp[6] = current->regs.esi;
		esp[5] = current->regs.edi;
		esp[4] = current->regs.ds;
		esp[3] = current->regs.es;
		esp[2] = current->regs.fs;
		esp[1] = current->regs.gs;

		// Adresse de retour pour %eip 
		esp[0] = (u32) &esp[18];


		current->regs.esp = (u32) esp;
		current->regs.eip = (u32) current->sigfn[sig];

		// Efface le signal et retablit le handler par defaut *
		current->sigfn[sig] = (void*) SIG_DFL;
		if (sig != SIGCHLD)
			clear_signal(&(current->signal), sig);
	}

	return 0;
}


}
