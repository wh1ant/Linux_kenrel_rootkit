<h1>Simple Linux kernel base rootkit</h1>
A long time ago, i made rootkit in 32bit Linux kernel 2.6.35 version.<br>
<br>
<br>
<h3>Rootkit skill</h3>
1. Kernel level keylogger<br>
If you press any key, it will logging to file.<br>
<br>
2. Keyboard injection<br>
If you press 'google.co.kr' string, it will change to 'facebook.com'.<br>
<br>
3. Module hiding<br>
You can't see this rootkit module, with lsmod command.<br>
<br>
4. File copy don't allow (file move don't allow)<br>
Denial, if you try file copy.<br>
<br>
4. Root backdoor<br>
You can privilege escalation to root user.<br>
<br>
<br>
<h3>Build</h3>
&#35;make<br>
&#35;cd app && make<br>
<br>
<br>
<h3>Usage</h3>
&#35;mv app/kdae /tmp<br>
&#35;insmod rootkit.ko<br>
