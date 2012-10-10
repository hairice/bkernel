/* create.c : create a process
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here. */
#define STACK_PAD 0x10		/* amount of space the context_frame is shifted from the end */
				/* of the memory allocation */

extern pcb_t *stop_q;
static int pid = 0;		/* pid will increment by 1 everytime a new proc is added */
				/* to the ready queue */

/*
* create
*
* @desc:	create process and put process on the ready queue
*
* @param:	func		process function
*		stack		process stack size
*
* @output:	FALSE		unable to create a new process
*		TRUE		created new process and pushed to ready queue
*/
int create(void (*func)(void), int stack) 
{
	int *mem;
	context_frame_t *frame;
	pcb_t *p = NULL;

	/* remove head of stop queue */
	if(!stop_q) return FALSE;
	p=stop_q;
	stop_q=stop_q->next;	

	/* alloc memory for process stack */
	mem = kmalloc(stack); 
	if(!mem) return FALSE;

	p->mem = (unsigned int*)mem;

	/* set process context frame STACK_PAD away from the end of the allocated memory */
	frame = (context_frame_t *) ((int)mem+stack-sizeof(context_frame_t)-(int)STACK_PAD);	

	/* set process context frame and pcb */
	frame->iret_cs = getCS();
	frame->iret_eip = (unsigned int) func;
    	frame->esp = (unsigned int)(frame);
	frame->ebp = frame->esp;
	frame->eflags = 0;
	p->esp = frame->esp;
	p->pid=pid;
	pid++;

	/* add proc to ready queue */
	ready(p);	
	return TRUE;
}
