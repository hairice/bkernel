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
		stack		Process stack size
*/
int create(void (*func)(), int stack) 
{
	int *mem;
	context_frame_t *frame;
	pcb_t *p = NULL;
	
	if(!stop_q) return -1;
	p=stop_q;
	stop_q=stop_q->next;	

	// Alloc Memory for Process Stack
	mem = kmalloc(stack); 
	if(!mem) return -1;

	p->mem = mem;

	// Set process context frame STACK_SIZE away from the end of the allocated memory
	frame = (context_frame_t *) ((int)mem+stack-sizeof(context_frame_t)-(int)STACK_PAD);	

	// Set process context frame and pcb 
	frame->iret_cs = getCS();
	frame->iret_eip = func;
    	frame->esp = (int)(frame);
	frame->ebp = frame->esp;
	frame->eflags = 0;
	p->esp = frame->esp;
	p->pid=pid;
	pid++;

	// Add proc to ready queue
	ready(p);	
	return 1;
}
