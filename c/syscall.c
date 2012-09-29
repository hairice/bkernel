/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */


int syscall (int call, ...) {
    	va_list	ap;
	int stack;

	va_start(ap, call);
	__asm __volatile( " \
		movl %0, %%eax \n\
		movl %1, %%edx \n\
		int %2 \n\
     		"
		:
		: "r"(call), "r"(ap), "i" (KERNEL_INT)
	);
}

int syscreate(void (*func)(), int stack)
{
	return (syscall(CREATE, func, stack));
}

void sysyield() 
{
	return (syscall(YIELD));
}


void sysstop()
{
	return (syscall(STOP));
}
