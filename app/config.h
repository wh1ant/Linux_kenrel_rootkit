#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEBUG_MODE

#define FILENAME	"kdae"

#define LOCK_FILE			"/var/lock/subsys/"FILENAME
#define PROC_FILE			"/proc/wh1ant"
#define KEY_LOG_FILE	"/tmp/keylog.ant"

#define CHAR_DEV				"/dev/wh1ant_poll"
#define CHAR_DEV_MAJOR	240
#define CHAR_DEV_MINOR	0

#endif
