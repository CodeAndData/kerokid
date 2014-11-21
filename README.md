KEROKID: the KErnel ROotKit Detector                                      
==============

usage:
----------------------
```
make linux-x86 // for i386
make linux-x86_64 // for amd64
make linux-x86 DEBUG=y // or
make linux-x86_64 DEBUG=y // for debugging
sudo insmod kerokid.ko
dmesg // to see results
```
features
----------------------
- detects syscall table entries directing to module space: determines if module is present at destination or not
- detects inline hooking of syscall table entries: determines if module is present at destination or not
- detects notifier entries to module space: detects module structure of hidden modules in memory [in progress]
 
successfully tested with
----------------------
- recent vanilla kernels
- Ubuntu 14.10 x86_64 3.16.0-23-generic
- Ubuntu 14.04 i686 3.13.0-32-generic
- Ubuntu 14.04 x86_64 3.13.0-generic

known issues
----------------------
- notifiers registered with highest priority cannot be found
- Debugging hooks as described in: http://phrack.org/issues/65/8.html
- Interupt Descriptor Table (IDT) manipulation
- unknown inline hook codes
- advanced inline hooks -> somewhere in the function, calculated jump address, jump from subroutine
- memory issues with some kernels. confirmed for:
  - Ubuntu 12.04.5 LTS x86_64 3.2.0-33-generic
  - Kali Linux 1.0.9a x86_64 3.14.5-1kali1
  - Debian 7.7.0 3.2.0-4-amd64
