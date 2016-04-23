#include <kbd.h>
#include <os.h>
#include <keyboard.h>
#include <idt.h>

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

void init_kbd(void)
{
	register_interrupt_handler(33, isr_keyboard_int);
}
