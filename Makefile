KDIR = /lib/modules/$(shell uname -r)/build
obj-m += inkerokid.o
inkerokid-objs := main.o common.o addressAnalysis.o inlineHooks.o notifier.o memory.o

ifdef DEBUG
	EFLAGS += -D_CONFIG_DEBUG_
endif

default:
	@echo "Supported targets:"
	@echo "linux-x86"
	@echo "linux-x86_64"
	@echo ""
	@echo "Add 'DEBUG=y' to include debugging messages"
	
linux-x86:
	$(MAKE) ARCH=x86 EXTRA_CFLAGS="-D_CONFIG_X86_ ${EFLAGS}" -C $(KDIR)  M=$(PWD) modules


linux-x86_64:
	$(MAKE) ARCH=x86_64 EXTRA_CFLAGS="-D_CONFIG_X86_64_ ${EFLAGS}" -C $(KDIR) M=$(PWD) modules


clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
