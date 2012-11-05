/* ctsw.c : context switcher
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#include <i386.h>

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/


void _timer_entry_point(void);		/* timer isr 				*/
void _syscall_entry_point(void);	/* system call isr 			*/
void _common_entry_point(void);		/* system call and interrupt isr 	*/

static unsigned int esp;		/* user stack pointer location 		*/
static unsigned int k_esp;		/* kernel stack pointer location 	*/
static unsigned int rc;			/* syscall() call request id 		*/
static unsigned int interrupt;		/* interrupt code
					*  0 - system call
					*  1 - timer interrupt
					*/
static unsigned int args;		/* args passed from syscall() 		*/

/*
* contextswitch
*
* @desc:	entrant/exit point between kernel and application
*
* @param:	p	process pcb block to context switch into
*
* @output:	rc	system call request id
*/
int contextswitch( pcb_t *p ) 
{
	k_esp = 0;
	/* save process esp and return code*/
	esp=p->esp;	
	rc=p->rc;


	/*
	* on leaving the kernel:
	* after any request has been serviced, it returns back to a process in the following way,
	* 1. hold return value in eax
	* 2. save kernel stack
	* 3. change stack pointer from kernel stack to user stack
	* 4. put eax value onto kernel stack
	* 5. pop user stack into registers
	*/
	
	/*
	* on entering the kernel:
	* timer interrupts will be handled by the isr _timer_entry_point
	* - for timer interrupts, 1 has been assigned as the return code
	* 
	* system call interrupts will be handled by the isr _syscall_entry_point
	*
	* for hardware/software interrupts, interrupts are disabled on entering the kernel, hence this is a non-reentrant kernel
	*
	*
	* when software/hardware interupts are received, all interrupts will then jump and be handled by _common_entry_point, 
	* where the listed actions will follow
	* 1. save user stack pointer
	* 2. change stack pointer from user stack to kernel stack
	* 3. retrieve request code from user stack
	* 4. retrieve interrupt code from user stack (only applicable for timer interrupts)
	* 5. retrieve data arguments from user stack (only applicable for syscall())
	* 6. pop kernel stack into registers
	*/
	__asm __volatile( " 					\
				pushf 				\n\
				pusha 				\n\
				movl	rc, %%eax    		\n\
				movl    esp, %%edx    		\n\
				movl    %%esp, k_esp    	\n\
				movl    %%edx, %%esp 		\n\
				movl    %%eax, 28(%%esp) 	\n\
				popa 				\n\
				iret 				\n\
	_timer_entry_point:					\n\
				cli				\n\
    				pusha   			\n\
				movl 	$1, %%ecx		\n\
				jmp	_common_entry_point	\n\
	_syscall_entry_point:					\n\
				cli				\n\
    				pusha   			\n\
				movl 	$0, %%ecx		\n\
	_common_entry_point:  					\n\
    				movl 	%%esp, esp 		\n\
    				movl 	k_esp, %%esp 		\n\
				movl 	%%eax, 28(%%esp)	\n\
				movl	%%ecx, 24(%%esp)	\n\
				movl	%%edx, 20(%%esp)	\n\
    				popa 				\n\
    				popf 				\n\
				movl 	%%eax, rc 		\n\
				movl 	%%ecx, interrupt	\n\
				movl 	%%edx, args 		\n\
        			"
  				:
  				: 
  				: "%eax", "%ecx", "%edx"
  	);
 
	/* treat an interrupt similar to a syscall(),
	*  where the value received from %%ecx is returned to dispatch() 
	*/
	if(interrupt) 
	{
		p->rc = rc;
		rc = interrupt;
	} 
	if(args) p->args = args;

	/* save process esp and passed args */
	p->esp = esp;
	return rc;
}

/*
* contextinit
*
* @desc:	set kernel interrupt event entry in IDT
*
* @note:	both software & hardware interrupt entries are set in this function
*/
void contextinit() 
{
	// set idt vector entry point for all syscall()
	set_evec(KERNEL_INT, _syscall_entry_point);

	// set idet vector entry point for timer interrupt
	set_evec(IRQBASE, _timer_entry_point);

	// set timer interrupt quantum
	initPIT(CLOCK_DIVISOR);
}
