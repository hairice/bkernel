/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */

extern pcb_t proc_table[MAX_PROC];

int create(void (*func)(), int stack, int i) 
{
	int *mem;
	context_frame_t *frame;

	// Alloc Memory for Process Stack
	mem = kmalloc(stack*2); 
	if(!mem) return -1;

	proc_table[i].mem = mem;

	// Set General Reg 
	frame = (context_frame_t *) ((int)mem+stack);	
	frame->iret_cs = getCS();
	frame->iret_eip = func;

    	frame->esp = (int)(frame);
	frame->ebp = frame->esp;
	proc_table[i].esp = frame->esp;

	// tmp set
	proc_table[i].pid=i;

	// Add Process to Ready Queue
	ready(&proc_table[i]);	
}
