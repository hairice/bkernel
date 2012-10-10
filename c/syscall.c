/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */


/*
* syscall
*
* @desc:	generates an interrupt call to the kernel
*
* @param:	call		system call request id
*		funcptr		process function
*		stack		process stack size
*
* @output:	TRUE
*/
int syscall (int call, ...) {
    	va_list	ap;
	va_start(ap, call);

	/* pass syscall() args by register, 'eax' and 'edx' */
	__asm __volatile( " \
		movl %0, %%eax \n\
		movl %1, %%edx \n\
		int %2 \n\
     		"
		:
		: "r"(call), "r"(ap), "i" (KERNEL_INT)
	);

	return TRUE;
}

/*
* syscreate
*
* @desc:	signals a create process interrupt 
*
* @param:	funcptr		process function
*		stack		process stack size
*
* @output:	TRUE
*/
int syscreate(void (*func)(void), int stack)
{
	return (syscall(CREATE, func, stack));
}

/*
* sysyield
*
* @desc:	signals a yield process interrupt
*/
void sysyield() 
{
	syscall(YIELD);
}

/*
* sysstop
*
* @desc:	signals a stop process interrupt
*/
void sysstop()
{
	syscall(STOP);
}
