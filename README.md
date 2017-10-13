# Simple Linux kernel base rootkit
A long time ago, i tested rootkit in 32bit Linux kernel 2.6.35 version.<br>
<br>
<br>
### Rootkit skill
<br>
1. Kernel level keylogger<br>
&nbsp;If you press any key, it will logging to file.<br>
<br>
2. Keyboard injection<br>
&nbsp;If you press 'google.co.kr' string, it will change to 'facebook.com'.<br>
<br>
3. Module hiding<br>
&nbsp;You can't see this rootkit module, with lsmod command.<br>
<br>
4. File copy don't allow (file mov don't allow)<br>
&nbsp;Denial, if you try file copy.<br>
<br>
4. Root backdoor<br>
&nbsp;You can privilege escalation to root user.<br>
<br>
<br>
### Build
"&#35;make<br>
"&#35;cd app && make<br>
<br>
<br>
### Usage
"&#35;insmod rootkit.ko<br>
"&#35;app/kdae<br>
