/* auto-generated by gen_syscalls.py, don't edit */

#include <syscalls/ipm.h>

extern int z_vrfy_ipm_send(const struct device * ipmdev, int wait, uint32_t id, const void * data, int size);
uintptr_t z_mrsh_ipm_send(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
		uintptr_t arg3, uintptr_t arg4, uintptr_t arg5, void *ssf)
{
	_current->syscall_frame = ssf;
	(void) arg5;	/* unused */
	union { uintptr_t x; const struct device * val; } parm0;
	parm0.x = arg0;
	union { uintptr_t x; int val; } parm1;
	parm1.x = arg1;
	union { uintptr_t x; uint32_t val; } parm2;
	parm2.x = arg2;
	union { uintptr_t x; const void * val; } parm3;
	parm3.x = arg3;
	union { uintptr_t x; int val; } parm4;
	parm4.x = arg4;
	int ret = z_vrfy_ipm_send(parm0.val, parm1.val, parm2.val, parm3.val, parm4.val);
	_current->syscall_frame = NULL;
	return (uintptr_t) ret;
}

