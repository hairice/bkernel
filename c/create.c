/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */
#define STACK_PAD 0x10

extern pcb_t *stop_q;
static int pid = 0;

/*
* create
*
* @desc:	Create process and put process on the ready queue
*
* @param:	func		Process function
*		stack		Process stack size
*
* @output:	FALSE		Unable to create a new process
*		TRUE		Created new process and pushed to ready queue
*/
int create(void (*func)(void), int stack) 
{
	int *mem;
	context_frame_t *frame;
	pcb_t *p = NULL;

	// Remove head of stop queue	
	if(!stop_q) return FALSE;
	p=stop_q;
	stop_q=stop_q->next;	

	// Alloc Memory for Process Stack
	mem = kmalloc(stack); 
	if(!mem) return FALSE;

	p->mem = mem;

	// Set process context frame STACK_SIZE away from the end of the allocated memory
	frame = (context_frame_t *) ((int)mem+stack-sizeof(context_frame_t)-(int)STACK_PAD);	

	// Set process context frame and pcb 
	frame->iret_cs = getCS();
	frame->iret_eip = (unsigned int) func;
    	frame->esp = (unsigned int)(frame);
	frame->ebp = frame->esp;
	frame->eflags = 0;
	p->esp = frame->esp;
	p->pid=pid;
	pid++;

	// Add proc to ready queue
	ready(p);	
	return TRUE;
}
