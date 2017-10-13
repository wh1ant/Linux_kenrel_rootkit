#ifndef __CHAR_DEV_H__
#define __CHAR_DEV_H__

#define CHAR_DEV				"wh1ant_poll"
#define CHAR_DEV_MAJOR	240

int poll_open(struct inode * inode, struct file * filp);
int poll_release(struct inode * inode, struct file * filp);
ssize_t poll_write(struct file * filp, const char * buf, size_t count, loff_t * f_pos);
ssize_t poll_read(struct file * filp, char * buf, size_t count, loff_t * f_pos);
ssize_t poll_poll(struct file * filp, struct poll_table_struct * wait);
void char_dev_init(void);
void char_dev_term(void);

#endif
