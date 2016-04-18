#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <os.h>

#define MAX_ENV_COUNT 256


int main(int argc, char *argv []);

int errno;

char** environ={
	"PATH","/bin/",
	NULL,NULL	
};

int __environ_allocated;


void _start(int argc, char** argv) {
	stdout=fdopen(0,"r+");
	stdin=fdopen(1,"r+");
	stderr=fdopen(2,"r+");
	int error;
    environ = 0;
    __environ_allocated = 0;

    /* Call the main function of the application */

    error = main( argc, argv);

    /* Exit the process */

    exit( error );
}
