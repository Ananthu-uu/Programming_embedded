/* auto-generated by gen_syscalls.py, don't edit */

#include <syscalls/kernel.h>

extern int z_vrfy_k_condvar_signal(struct k_condvar * condvar);
uintptr_t z_mrsh_k_condvar_signal(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
		uintptr_t arg3, uintptr_t arg4, uintptr_t arg5, void *ssf)
{
	_current->syscall_frame = ssf;
	(void) arg1;	/* unused */
	(void) arg2;	/* unused */
	(void) arg3;	/* unused */
	(void) arg4;	/* unused */
	(void) arg5;	/* unused */
	union { uintptr_t x; struct k_condvar * val; } parm0;
	parm0.x = arg0;
	int ret = z_vrfy_k_condvar_signal(parm0.val);
	_current->syscall_frame = NULL;
	return (uintptr_t) ret;
}

