/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */


/*
* syscall
*
* @desc:	Generates an interrupt call to the kernel
*
* @param:	call		System call request id
*		funcptr		Process function
*		stack		Process stack size
*/
int syscall (int call, ...) {
    	va_list	ap;

	va_start(ap, call);
	__asm __volatile( " \
		movl %0, %%eax \n\
		movl %1, %%edx \n\
		int %2 \n\
     		"
		:
		: "r"(call), "r"(ap), "i" (KERNEL_INT)
	);

	return 0;
}

/*
* syscreate
*
* @desc:	Signals a create process interrupt 
*
* @param:	funcptr		Process function
*		stack		Process stack size
*/
int syscreate(void (*func)(), int stack)
{
	return (syscall(CREATE, func, stack));
}

/*
* sysyield
*
* @desc:	Signals a yield process interrupt
*/
void sysyield() 
{
	syscall(YIELD);
}

/*
* sysstop
*
* @desc:	Signals a stop process interrupt
*/
void sysstop()
{
	syscall(STOP);
}
