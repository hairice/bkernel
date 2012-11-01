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
static int pid = 1;		/* pid will increment by 1 everytime a new proc is added */
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
*
* @note:	pid starts at 1, pid 0 is reserved for the idle proc
*/
int create(void (*func)(void), int stack) 
{
	int *mem;
	context_frame_t *frame;
	pcb_t *p = NULL;

	/* remove head of stop queue */
	if(!stop_q) return -1;
	p=stop_q;
	stop_q=stop_q->next;	

	/* alloc memory for process stack */
	mem = kmalloc(stack); 
	if(!mem) return -1;

	p->mem = (unsigned int*)mem;

	/* set process context frame STACK_PAD away from the end of the allocated memory */
	frame = (context_frame_t *) ((int)mem+stack-sizeof(context_frame_t)-(int)STACK_PAD);	

	/* set process context frame and pcb */
	frame->iret_cs = getCS();
	frame->iret_eip = (unsigned int) func;
    	frame->esp = (unsigned int)(frame);
	frame->ebp = frame->esp;
	frame->eflags = 0x00003200;
	frame->iret_func = &sysstop;

	p->esp = frame->esp;

	if(func == &idleproc) p->pid = IDLE_PROC_PID;
	else 
	{
		p->pid=pid;
		pid++;
	}
	
	/* add proc to ready queue */
	p->state = READY_STATE;	
	p->blocked_senders=NULL;			
	p->blocked_receivers=NULL;
	p->ptr=NULL;

	ready(p);	
	return p->pid;
}
