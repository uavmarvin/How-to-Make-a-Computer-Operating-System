#include <scheduler.h>

#if defined (__cplusplus)
extern "C" {
#endif

extern regs_t* stack_ptr;

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

#define DEBUG_REG(a) io.print("  %s : %x",#a,p->regs.a)

void schedule(regs_t* regs_ptr)
{
	Process* pcurrent=arch.pcurrent;
	Process* plist=arch.plist;
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

void isr_scheduler_int(regs_t* regs_ptr)
{
	static int tic = 0;
	static int sec = 0;
	tic++;
	if (tic % 100 == 0)
	{
		sec++;
		tic = 0;
	}
	schedule(regs_ptr);
	io.outb(0x20,0x20);
	io.outb(0xA0,0x20);
}

void init_scheduler(u32 frequency)
{
    register_interrupt_handler(32, isr_scheduler_int);

    u32 divisor = 1193180 / frequency;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    io.outb(0x43, 0x36);

    u8 low = (u8)(divisor & 0xFF);
    u8 hign = (u8)((divisor >> 8) & 0xFF);
    
    io.outb(0x40, low);
    io.outb(0x40, hign);
}

#if defined (__cplusplus)
}
#endif
