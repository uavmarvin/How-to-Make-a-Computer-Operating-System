
#include <os.h>
#include <ide.h>

#include <api/dev/ioctl.h>


/*
 *	Cette fonction attend que le disque soit pret avant une operation
 */
int bl_wait(unsigned short base)
{
	for (int i = 0; i < 4; i++) io.inb(base+0x7); /* 400ns delays */
	while ((io.inb(base+0x7) & 0xC0) != 0x40);
	return 0;	
}

/*
 *	Cette fonction permet de ce deplacer sur le disque
 */
int bl_common(int drive, int numblock, int count)
{
	bl_wait(0x1F0);
	io.outb(0x1F0+0x206, 0x02);
	bl_wait(0x1F0);
	
	io.outb(0x1F1, 0x00);	/* NULL byte to port 0x1F1 */
	io.outb(0x1F2, count);	/* Sector count */
	io.outb(0x1F3, (unsigned char) (numblock & 0xFF));	/* Low 8 bits of the block address */
	io.outb(0x1F4, (unsigned char) ((numblock >> 8) & 0xFF));	/* Next 8 bits of the block address */
	io.outb(0x1F5, (unsigned char) ((numblock >> 16) & 0xFF));	/* Next 8 bits of the block address */

	/* Drive indicator, magic bits, and highest 4 bits of the block address */
	io.outb(0x1F6, 0xE0 | (drive << 4) | ((numblock >> 24) & 0x0F));

	return 0;
}

/*
 *	Cette fonction permet de lire un buffer sur le disque
 */
int bl_read(int drive, int numblock, int count, char *buf)
{
	u16 tmpword;
	int idx;
	
	bl_common(drive, numblock, count);
	io.outb(0x1F7, 0x20);
	
	for (int i = 0; i < count; i++)
	{
		/* wait for hard disk buffer ready */
		for (int i = 0; i < 4; i++) io.inb(0x1F7); /* 400ns delays */
		while ((io.inb(0x1F7) & 0x88) != 0x08);
		
		for (int j = 0; j < 256; j ++)
		{
			idx = i * 256 + j;
			tmpword = io.inw(0x1F0);
			buf[idx * 2] = (unsigned char) tmpword;
			buf[idx * 2 + 1] = (unsigned char) (tmpword >> 8);
		}
	}
	return count;
}

/*
 *	Cette fonction permet d'ecrire un buffer sur le disque
 */
int bl_write(int drive, int numblock, int count, char *buf)
{
	u16 tmpword;
	int idx;

	bl_common(drive, numblock, count);
	io.outb(0x1F7, 0x30);
	
	for (int i = 0; i < count; i++)
	{
		/* Wait for the drive to signal that it's ready: */
		for (int i = 0; i < 4; i++) io.inb(0x1F7); /* 400ns delays */
		while ((io.inb(0x1F7) & 0x88) != 0x08);
		
		for (int j = 0; j < 256; j++)
		{
			idx = i * 256 + j;
			tmpword = (buf[idx * 2 + 1] << 8) | buf[idx * 2];
			io.outw(0x1F0, tmpword);
		}
	}
	
	io.outb(0x1F7, 0xE7); /* flush cache */

	return count;
}




File* ide_mknod(char* name,u32 flag,File* dev){
	Ide* disk=new Ide(name);
	disk->setId(flag);
	return disk;
}

module("module.ide",MODULE_DEVICE,Ide,ide_mknod)

Ide::~Ide(){
	
}

Ide::Ide(char* n) : Device(n)
{
	
}

u32	Ide::close(){
	return RETURN_OK;
}

void Ide::scan(){
	
}

u32	Ide::open(u32 flag){
	return RETURN_OK;
}

u32	Ide::read(u32 pos,u8* buffer,u32 sizee){
	int count=(int)sizee;
	
	if (buffer==NULL)
		return -1;
	
	int offset=(int)pos;
	int bl_begin, bl_end, blocks;

	bl_begin = (offset/512);
	bl_end = ((offset + count)/512);
	blocks = bl_end - bl_begin + 1;
	//io.print("%s> read at %d - %d  size=%d begin=%d blocks=%d\n",getName(),offset,offset/512,count,bl_begin,blocks);
	char*bl_buffer = (char *) kmalloc(blocks * 512);
	bl_read(id, bl_begin, blocks,bl_buffer);
	memcpy((char*)buffer, (char *) ((int)bl_buffer + ((int)offset % (int)(512))), count);
	kfree(bl_buffer);
	return count;
}

u32	Ide::write(u32 pos,u8* buffer,u32 sizee){
	return NOT_DEFINED;
}

u32	Ide::ioctl(u32 idd,u8* buffer){
	u32 ret=0;
	switch (idd){
		case DEV_GET_TYPE:
			ret=DEV_TYPE_DISK;
			break;
			
		case DEV_GET_STATE:
			ret=DEV_STATE_OK;
			break;
			
		case DEV_GET_FORMAT:
			ret=DEV_FORMAT_BLOCK;
			break;
			
		default:
			ret=NOT_DEFINED;
			break;
	}	
	return ret;
}



u32	Ide::remove(){
	delete this;
	return RETURN_OK;
}

void Ide::setId(u32 flag){
	id=flag;
}
