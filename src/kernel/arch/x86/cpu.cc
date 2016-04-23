#include <cpu.h>
#include <os.h>
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
