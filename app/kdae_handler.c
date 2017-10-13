#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "kdae_fd.h"

extern int fd_arr[ALL_FD_SIZE];

/* function for memory or resource free */
void termination_handler(int sig)
{
	int i;

	for(i=0;i<ALL_FD_SIZE;i++)
	{
		if(fd_arr[i]>-1)  // file open check
			close(fd_arr[i]);  // file descriptor close
	}

	//unlink(CHAR_DEV);
#ifdef DEBUG_MODE
	unlink(LOCK_FILE);

	if(sig==0)  // programmer call
		fputs("Error occurrence, " FILENAME " terminate\n", stderr);
	else if(sig==11)  // segmentation fault signal
	{
		fputs("Segmentation fault, " FILENAME " terminate\n", stderr);
		abort();  // create core file
	}
	else if(sig==15)  // kill signal
	{
		fputs("normal " FILENAME "terminate\n", stderr);
	}
	else
	{
		fputs("unknown " FILENAME "terminate\n", stderr);
	}
#endif
}


void error_handler(const char * msg)
{
	termination_handler(0);
#ifdef DEBUG_MODE
	fputs(msg, stderr);
#endif
	exit(1);
}
