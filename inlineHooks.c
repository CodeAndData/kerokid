/*
 * The INvisible KErnel ROotKIt Detector (INKEROKID)
 *
 *      (c) 2014 Fraunhofer FKIE
 *
 * This file contains inline hook stuff
 */

#include "kerokid.h"
#include "inlineHooks.h"
#include "addressAnalysis.h"
#include "proc_file.h"


struct inlineHook init_hook(unsigned int codeSize, unsigned int addressOffset, char* hookCode);

#define MAX_HOOK_CODE_SIZE 16

struct inlineHook{
	unsigned int codeSize;
	unsigned int addressOffset;
	unsigned char hookCode[MAX_HOOK_CODE_SIZE];
};

struct inlineHook *hooks;
int numberOfKnownInlineHooks;


/* ----------------- X86 HOOKs ----------------- */
#if defined(_CONFIG_X86_)

void init_hooks(void)
{
	numberOfKnownInlineHooks = 1;	// increase this number if you add a hook
	hooks = vmalloc(numberOfKnownInlineHooks * sizeof(struct inlineHook));

	// suterusu hook: push $addr; ret
	hooks[0] =  init_hook(6, 1, "\x68\x00\x00\x00\x00\xc3");
}

#endif


/* ----------------- X86_64 HOOKs ----------------- */
#if defined(_CONFIG_X86_64_)

void init_hooks(void)
{
	numberOfKnownInlineHooks = 1;	// increase this number if you add a hook
	hooks = vmalloc(numberOfKnownInlineHooks * sizeof(struct inlineHook));

	// suterusu hook: mov rax, $addr; jmp rax
	hooks[0] = init_hook(12, 2, "\x48\xb8\x00\x00\x00\x00\x00\x00\x00\x00\xff\xe0");
}

#endif

/* -------------- functionality ----------------- */

struct inlineHook init_hook(unsigned int codeSize, unsigned int addressOffset, char *hookCode)
{
	struct inlineHook hook;
	hook.codeSize = codeSize;
	hook.addressOffset = addressOffset;
	memcpy(hook.hookCode, hookCode, codeSize);
	return hook;
}

psize *check_hook(psize *addr, struct inlineHook hook)
{
	int i;
	unsigned char *r_code;
	r_code = vmalloc(hook.codeSize);
	memcpy(r_code, addr, hook.codeSize);
	/* check code before address; */
	for (i=0; i < hook.addressOffset; i++ ) {
		if (r_code[i] != hook.hookCode[i]) {
			vfree(r_code);
			return NULL;
		}
	}
	/* check code after address */
	for (i=hook.addressOffset+sizeof(psize); i < hook.codeSize; i++) {
		if (r_code[i] != hook.hookCode[i]) {
			vfree(r_code);
			return NULL;
		}
	}
	/* return hook destination */
	return (psize *)(*((psize *)&r_code[hook.addressOffset]));;
}


int check_for_hooks(psize *addr)
{
	psize *hookAddress;
	int i;
	for (i=0; i < numberOfKnownInlineHooks; i++ ) {
		hookAddress = check_hook(addr, hooks[i]);
		if (hookAddress != NULL) {
			cat_proc_message(formats("found hook #%d leading to address %p\n", i, hookAddress));
			analyze_address(hookAddress);
			finds.inlineHooks++;
		}
	}
	return 0;
}

/* ---- main function */
void check_inline_hooks(psize **Table)
{
	int i;
	init_hooks();
	cat_proc_message("inline hooks:\n");
	for (i = 0; i < NUMBER_OF_SYSCALLS; i++) {
		check_for_hooks(Table[i]);
	}
	if (!finds.inlineHooks)
		cat_proc_message("nothing found\n");
	return;
}
