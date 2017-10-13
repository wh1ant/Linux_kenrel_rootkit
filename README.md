# Simple Linux kernel base rootkit

A long time ago, i tested rootkit in 32bit (Ubuntu) Linux kernel 2.6.35 version.


### Rootkit skill.

1. Kernel level keylogger
	If you press any key, it will logging to file.

2. Keyboard injection
	If you press 'google.co.kr' string, it will change to 'facebook.com'

3. Module hiding.
	You can't see this rootkit module, with lsmod command.
	
4. File copy don't allow, (file mov don't allow)
	Denial, if you try file copy.

4. Root backdoor.
	You can privilege escalation to root user.
