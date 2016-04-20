#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc,char **argv){
	printf("[init] start!\n");
	/*Spawn a child to run the program.*/
	pid_t pid = fork();
	if (pid == 0)
	{
		/* child process */
	//      static char *argv[]={"echo","Foo is my name.",NULL};
		execv("/bin/sh", argv);
		exit(127); /* only if execv fails */
	}
	else
	{
		/* pid!=0; parent process */
		waitpid(pid, 0, 0); /* wait for child to exit */
	}
	return 0;
}
