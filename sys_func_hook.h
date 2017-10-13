#ifndef __SYS_FUNC_HOOK_H__
#define __SYS_FUNC_HOOK_H__

#define FILENAME_ARRAY		4

#define FILE_NAME1				"secret.docx"  // protect file name 1
#define FILE_NAME2				"/secret.docx"  // protect file name 2
#define DUMMY_FILE_NAME		"/usr/local/src/fake.docx"


void sys_func_hook_init(void);
void sys_func_hook_term(void);

#endif
