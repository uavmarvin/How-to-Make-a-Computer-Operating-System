#include "Shell.h"

int main(int argc,char **argv)
{
	Shell sh;
	
	/* Load config files, if any. */
	
	/* Run command loop. */
	sh.loop();
	
	/* Perform any shutdown/cleanup. */
	return 0;
}
