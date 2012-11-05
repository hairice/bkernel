/* create.c : create a process
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here. */
#define STACK_PAD 0x10		/* amount of space the context_frame is shifted from the end 	*/
				/* of the memory allocation 					*/

extern pcb_t *stop_q;
extern pcb_t proc_table[MAX_PROC];

static int min_pid = MIN_PID;		/* lower bound of used pid region 						*/
static int max_pid = MIN_PID;		/* upper bound of used pid region 						*/
static int next_pid = MIN_PID+1;	/* reference to the next pid, the next pid may/may not be an available pid 	*/

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
	int *mem,pid;
	context_frame_t *frame;
	pcb_t *p = NULL;

	/* add new proc if provided func is not null or stack_size is at least MIN_STACK */
	if(!func || stack < MIN_STACK) return SYSERR;

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
		/* find unused pid */
		pid = find_pid();

		if(pid == SYSERR)
			return SYSERR;
		else				
			p->pid = pid;
	}
	
	/* add proc to ready queue */
	p->state = READY_STATE;	
	p->blocked_senders=NULL;			
	p->blocked_receivers=NULL;
	p->ptr=NULL;

	ready(p);	
	return p->pid;
}

/*
* find_pid
*
* @desc:	create process and put process on the ready queue
*
* @output:	pid		unused pid in the range of (MIN_PID+1, MAX_PID-1)
*
* @note:	this algorithm divides the pid number space into three regions
*
*		|___lower_unused_region___|___middle_used_region___|___upper_unused_region___|
*
*					  ^			   ^
*				       min_pid			max_pid
*
*		three references are updated when a pid is created (syscreated()) or removed (sysstop()). Hence the min and max bounds 
*		are constantly changing depending on the process management
*			1. min_pid
*			2. max_pid
*			3. next_pid
*
*		the regions "lower_unused" & "upper_unused" will generate pids in constant time, 
*		when the next_pid reference is between min_pid and max_pid, the algorithm will cycle incrementally to find the smallest avaiable pid 
*		within the range (min_pid, max_pid)
*/
unsigned int find_pid ()
{
	unsigned int pid=next_pid,i,tmp;
	Bool pid_found = FALSE;

	/* skip over max_pid, this forces 'pid' to be possibly in the upper free region, or be wrapped around to the lower free region */
	if(pid == max_pid)
		pid = max_pid + 1;
	
	/* wrap pid value around to the lower bound */
	if(pid == MAX_PID)
	{
		pid = MIN_PID + 1;

		/* set pid to MIN_PID + 1, assumes the min_pid is at MIN_PID */
		if(pid == min_pid)
			pid = min_pid + 1;	

		/* set pid to MIN_PID + 2, assumes the max_pid is at MIN_PID+1 */
		if(pid == max_pid)	
			pid = max_pid +1;
	}
	/* get unused pid in the upper free region */
	if(pid > max_pid && pid < MAX_PID)
	{
		next_pid = pid+1;
		max_pid = pid;
		return pid;
	}
	/* get unused pid in the lower free region */
	else if(pid < min_pid && pid > MIN_PID)
	{
		next_pid = pid+1;
		min_pid = pid;		
	
		if(next_pid+1 == max_pid)
			next_pid = max_pid;

		return pid;
	}

	/* search for the smallest value in the range (min_pid, max_pid) that is unused */
	for(pid ; pid<max_pid ; pid++)
	{
		for(i=0 ; i<MAX_PROC ; i++)
		{
			if(proc_table[i].pid == pid)
				break;

			if(i == MAX_PROC-1) 
				pid_found = TRUE;
		}

		if(pid_found)
		{
			next_pid = pid+1;
			break;
		}
	}

	/* at this point, (next_pid-1) is the return value */
	/* since unused pid was found within the range (min_pid, max_pid), move next_pid to the next available spot that is < max_pid */
	if(pid_found)
	{
		for(pid=next_pid ; pid<max_pid ; pid++)	
		{
			for(i=0 ; i<MAX_PROC ; i++)
			{
				if(proc_table[i].pid == pid) continue;

				/* found an unused pid in the range [next_pid+1, max_pid) */
				if(i == MAX_PROC-1) 
				{
					tmp = next_pid-1;
					next_pid = pid;
					return tmp;
				}
			}	
		}
		
		/* no more available pid in range [next_pid+1, max_pid), set next_pid to max_pid */
		tmp = next_pid-1;
		next_pid = pid;
		return tmp;
	}

	/* no more avaiable pid, this return statement should not be reached given the size of the proc_table and max_pid */
	return SYSERR;
}

/*
* set_max_pid
*
* @desc:	set the upper bound in pid value for the proc_table
*/
void set_max_pid()
{
	int i=0;

	max_pid = proc_table[0].pid;

	/* skip over idle_proc_pid */
	while(max_pid == IDLE_PROC_PID)
	{
		i++;
		max_pid = proc_table[i].pid;
	}
	
	for(i ; i<MAX_PROC ; i++)
	{
		if(proc_table[i].pid > max_pid)
			max_pid = proc_table[i].pid;
	}
}

/*
* set_min_pid
*
* @desc:	set the lower bound in pid value for the proc_table
*/
void set_min_pid()
{
	int i;

	min_pid = proc_table[0].pid;
	for(i=1 ; i<MAX_PROC ; i++)
	{
		if(proc_table[i].pid == IDLE_PROC_PID) continue;
		if(proc_table[i].pid == INVALID_PID) continue;
		if(proc_table[i].pid < min_pid)
			min_pid = proc_table[i].pid;
	}

	if(min_pid == INVALID_PID)
		min_pid = MIN_PID;	
}
