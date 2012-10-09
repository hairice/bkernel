/* ctsw.c : context switcher
 */

#include <xeroskernel.h>
#include <i386.h>

/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/


void _ISREntryPoint(void);
static unsigned int k_stack;
static unsigned int ESP;
static unsigned int rc;
static unsigned int args;

/*
* contextswitch
*
* @desc:	Entrant/exit point between kernel and application
*
* @param:	p	Process pcb block to context switch into
*
* @output:	rc	System call request id
*/
int contextswitch( pcb_t *p ) 
{
	// Save process esp
	ESP = p->esp;	

	// Context switch between process and kernel
	// Retrieve syscall() arguments by register from 'eax' and 'edx'
	__asm __volatile( " \
     		pushf  \n\
     		pusha  \n\
     		movl  %%esp, k_stack     \n\
     		movl  ESP, %%esp  \n\
     		popa  \n\
		iret \n\
	_ISREntryPoint:  \n\
    		pusha   \n\
    		movl  %%esp, ESP \n\
    		movl  k_stack, %%esp \n\
		movl %%eax, rc \n\
		movl %%edx, args \n\
    		popa \n\
    		popf \n\
        	"
  		:
  		: 
  		: "%eax"
  		);
 
	// Save process esp and passed args
	p->esp = ESP;
	p->args = args;

	return rc;
}

/*
* contextinit
*
* @desc:	Set kernel interrupt event entry in IDT
*/
void contextinit() 
{
	set_evec(KERNEL_INT, _ISREntryPoint);
}

