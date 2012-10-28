/* syscall.c : syscalls
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
static unsigned int rc;

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
		movl %0, %%eax 	\n\
		movl %1, %%edx 	\n\
		int %2		\n\
		movl %%eax, rc 	\n\
     		"
		:
		: "r"(call), "r"(ap), "i" (KERNEL_INT)
	);
	
	return rc;
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
	return syscall(CREATE, func, stack);
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

/*
* sysgetpid
*
* @desc:	returns the current process pid
* 
* @output:	
*/
unsigned int sysgetpid(void)
{
	return syscall(GETPID);
}

/*
* sysputs
*
* @desc:	signals a synchronous kernel console output message
*
* @param:	str		string message to display on console
*/
void sysputs( char *str )
{
	syscall(PUTS, str);
}

/*
* syssleep
*
* @desc:	signals the current process to sleep for input milliseconds
*
* @param:	milliseconds		time in milliseconds for a process to sleep
* 
* @output:
*/
unsigned int syssleep( unsigned int milliseconds )
{
	return syscall(SLEEP, milliseconds);
}

/*
* sysputs
*
* @desc:	signals a synchronous kernel console output message
*
* @param:	str		string message to display on console
*/
extern int syssend(unsigned int dest_pid, void *buffer, int buffer_len)
{
	return syscall(SEND, dest_pid, buffer, buffer_len);
}

/*
* sysputs
*
* @desc:	signals a synchronous kernel console output message
*
* @param:	str		string message to display on console
*/
extern int sysrecv(unsigned int *from_pid, void *buffer, int buffer_len)
{
	return syscall(RECV, from_pid, buffer, buffer_len);
}
