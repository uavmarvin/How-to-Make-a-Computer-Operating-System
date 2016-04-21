#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc,char **argv)
{
	int x = 0;
	char c;
	printf("> ");
	fflush(stdout);
	x = 2;
	while (1)
	{
		c = getchar();
		if (c == 0x08)
		{
			if (x == 2)
				continue;
			else
			{
				printf("%c", c);
				x--;
			}
		}
		else
		{
			printf("%c", c);
			x++;
		}
		if ('\n' == c){printf("> "); x = 2;}
		fflush(stdout);
	}
	//Shell sh;
	
	/* Load config files, if any. */
	
	/* Run command loop. */
	//sh.loop();
	
	/* Perform any shutdown/cleanup. */
	return 0;
}
