KEROKID: the KErnel ROotKIt Detector                                      
==============

Quick Start Guide
----------------------
```
make linux-x86_64 			// or linux-x86 for 32bit environments
sudo insmod kerokid.ko
```
Results can be accessed via dmesg (available on all kernel versions) or /proc/kerokid (requires Kernel 3.10 or newer)

The system is rescanned whenever a new kernel module is loaded.

Alternatively, you can initialize a rescan by writing to /proc/kerokid/rescan as root. (requires Kernel 3.10 or newer)
```
echo '1' > /proc/kerokid/rescan
```
This can be used for periodic automated rescans via cron jobs.

The idea of KEROKID
----------------------
Kernel Level Rootkits use different techniques to hide themselves, e.g.:
- remove themselves from the kernel module list
- unregister their /sys/module entry
- overwrite their module structures
- extract their code to the kernel heap (the space allocated with kmalloc or vmalloc)
Nevertheless, these rootkits need space in memory. Therefore KEROKID uses memory forensics to detect rootkits in the kernel's heap or module space.
In addition, rootkits must react to user or system activity and therefore use hooks or subscribe themselves to notifiers.
KEROKID detects such suspicious calls originating from:
- system call hooks
- inline hooks
- notifier subscriptions

Proc output (requires Kernel 3.10 or newer)
----------------------
results and further information can be accessed through /proc/kerokid
- /proc/kerokid/results contains the results for each area
- /proc/kerokid/memory lists the addresses of modules, syscalls and allocated memory
- the folder /proc/kerokid/notifiers contains the results for each notifier chain
- the folder /proc/kerokid/modules contains information on each module

Dmesg output (available on all kernel versions)
----------------------
This is a sample output showing a hidden module that manipulated five syscall table entries.
The addresses are the destinations of the manipulated syscalls.    
```
[  421.296881] KEROKID: Started
[  421.302176] KEROKID: Check for syscall table hooks...
[  421.302176] KEROKID: ALERT: Jump to hidden module: ffffffffa0002010 
[  421.302176] KEROKID: ALERT: Jump to hidden module: ffffffffa0002050 
[  421.302176] KEROKID: ALERT: Jump to hidden module: ffffffffa0002020 
[  421.302176] KEROKID: ALERT: Jump to hidden module: ffffffffa0002040 
[  421.302176] KEROKID: ALERT: Jump to hidden module: ffffffffa0002000 
[  421.302176] KEROKID: Check for inline hooks...
[  421.304729] KEROKID: Check notifier_subscriptions...
[  421.304729] KEROKID: -> Check keyboard notifier chain... 
[  421.308363] KEROKID: -> Check module notifier chain... 
[  421.308363] KEROKID: -> Check netdevice notifier chain... 
[  421.308363] KEROKID: -> Check netevent notifier chain... 
```

What we can detect...
----------------------
- syscall table entries directing to module space or kernel heap: determines if module is present at destination or not
- inline hooking of syscall functions: determines if module is present at destination or not
- notifier entries to module space or kernel heap: determines if module is present at destination or not
- module structure of hidden modules in memory [in progress]


What we cannot detect at the moment...
----------------------
- notifiers registered with highest priority
- Debugging Hooks as described in: http://phrack.org/issues/65/8.html
- Interrupt Descriptor Table (IDT) manipulation
- Inline hook detection covers only ONE way. Significant improvement required. (see inline hook detection paragraph)

Successfully tested with
----------------------
x86
- Debian-7.7.0-i386 3.2.0-4-486
- Fedora-Desktop-i686-20-1 3.11.10-301.fc20.i686
- Ubuntu 14.04 i686 3.13.0-32-generic

x86_64
- recent vanilla kernels
- Debian 7.7.0 3.2.0-4-amd64, 3.16-0.bpo.2-amd64
- Fedora Desktop 3.11.10.301-fc20.x86_64
- Gentoo Linux 3.17.7-gentoo (genkernel)
- Kali Linux 1.0.9a x86_64 3.14.5-1kali1
- Mageia-4.1 3.12.21-desktop-2.mga4
- OpenSUSE 13.2 3.16.6-2-2-desktop
- Ubuntu 14.10 3.16.0-23-generic
- Ubuntu 14.04 3.13.0-generic
- Ubuntu 12.04.5 LTS x86_64 3.2.0-33-generic, 3.2.64, 3.13.0-32-generic



Implementation details and development related stuff                                  
==============

Debugging
----------------------
Debug features can be enabled by:
```
make linux-x86_64 DEBUG=y
```

Dumping loaded module
----------------------
At the moment you can only dump unhidden modules.
As root use the following command to get a list of loaded modules:
```
cat /proc/kerokid/modules_list
```
Pick the NUMBER of the corresponding Module and type:
```
echo "NUMBER" > /proc/kerokid/dump_module
```
The module binary can now be copied from /proc/kerokid/MODULNAME to wherever you want.

Inline hook detection
----------------------
- checks all functions referenced in the System Call Table by comparing the first bytes of the function with a list of known hooks
- looks for suspicious jumps (especially jumps to modules and dynamically allocated memory)
- hooks are specified in inlineHooks.c (currently only the hooks used by Suterusu)
- the hook definitions consist of the (hexadecimal) code, it's size and the (address) offset
- works with 32- and 64-bit architecture hooks
