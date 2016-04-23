#ifndef _IDT_H_
#define _IDT_H_

#include <runtime/types.h>

#define IDTSIZE		0xFF	/* nombre max. de descripteurs dans la table */
#define IDTBASE		0x00000000	/* addr. physique ou doit resider la IDT */

#define INTGATE  0x8E00		/* utilise pour gerer les interruptions */
#define TRAPGATE 0xEF00		/* utilise pour faire des appels systemes */

#if defined (__cplusplus)
extern "C" {
#endif

/* Descripteur de segment */
struct idtdesc {
	u16 offset0_15;
	u16 select;
	u16 type;
	u16 offset16_31;
} __attribute__ ((packed));

/* Registre IDTR */
struct idtr {
	u16 limite;
	u32 base;
} __attribute__ ((packed));

typedef struct
{
	u32 gs, fs, es, ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 which_int, err_code;
	u32 eip, cs, eflags, user_esp, user_ss;
} __attribute__((packed)) regs_t;

typedef void (*interrupt_handler_t)(regs_t *);

void register_interrupt_handler(u8 n, interrupt_handler_t h);
void init_idt(void);
void init_pic(void);
void isr_handler(regs_t *regs);

extern regs_t* stack_ptr;

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

#if defined (__cplusplus)
}
#endif

#endif /* _IDT_H_ */
