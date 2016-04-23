#ifndef _PAGE_H_
#define _PAGE_H_

#define	KERN_PDIR			0x00001000
#define	KERN_STACK			0x0009FFF0
#define	KERN_BASE			0x00100000
#define KERN_PG_HEAP		0x00800000
#define KERN_PG_HEAP_LIM	0x10000000
#define KERN_HEAP			0x10000000
#define KERN_HEAP_LIM		0x40000000

#define	USER_OFFSET 		0x40000000
#define	USER_STACK 			0xE0000000
	
#define KERN_PG_1			0x400000
#define KERN_PG_1_LIM 		0x800000

#define	VADDR_PD_OFFSET(addr)	((addr) & 0xFFC00000) >> 22
#define	VADDR_PT_OFFSET(addr)	((addr) & 0x003FF000) >> 12
#define	VADDR_PG_OFFSET(addr)	(addr) & 0x00000FFF
#define PAGE(addr)		(addr) >> 12

#define	PAGING_FLAG 		0x80000000	/* CR0 - bit 31 */
#define PSE_FLAG			0x00000010	/* CR4 - bit 4  */

#define PG_PRESENT			0x00000001	/* page directory / table */
#define PG_WRITE			0x00000002
#define PG_USER				0x00000004
#define PG_4MB				0x00000080

#define	PAGESIZE 			4096
#define	RAM_MAXSIZE			0x100000000
#define	RAM_MAXPAGE			0x100000

void init_page(void);

#endif /* _PAGE_H_ */
