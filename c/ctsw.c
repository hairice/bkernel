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


void _ISREntryPoint();
static unsigned int k_stack;
static unsigned int ESP;
static unsigned int rc;
long args;

int contextswitch( pcb_t *p ) 
{
	ESP = p->esp;	

	//kprintf("ESP: %d\n", ESP);  	
	kprintf("hi from contextswitch!\n");
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
 
	p->esp = ESP;
	p->args = args;
	kprintf("back from process!: %d\n", rc);
	return rc;
}


void contextinit() 
{
	set_evec(KERNEL_INT, _ISREntryPoint);
}

