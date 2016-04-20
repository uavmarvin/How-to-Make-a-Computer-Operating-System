#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc,char **argv)
{
	char c;
	printf("> ");
	fflush(stdout);
	while (1)
	{
		c = getchar();
		printf("%c", c);
		if ('\n' == c) printf("> ");
		fflush(stdout);
	}
	//Shell sh;
	
	/* Load config files, if any. */
	
	/* Run command loop. */
	//sh.loop();
	
	/* Perform any shutdown/cleanup. */
	return 0;
}
