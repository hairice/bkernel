/* disp.c : dispatcher
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
extern pcb_t *stop_q;
pcb_t *ready_q;

/*
* dispatch
*
* @desc:	executes the dispatcher
*/
void dispatch() 
{
	int request,stack=0,sleep_ms=0;
	pcb_t *p=NULL;
    	va_list ap;
	void (*funcptr)(void);
	char* str=NULL;

	/* start dispatcher */
	for(;;) 
	{
		p = next();

		/* execute the idle proc only when there are no other proc in ready_q */
		if(p->pid == 0 && count() > 0) 
		{
			ready(p);
			continue;
		}
		request = contextswitch(p);

		switch(request) {
 			case TIMER_INT:
				/* signal sleep device is there's at least 1 sleeping proc */
				if(sleeper() > 0 && tick())
					ready(wake());

				ready(p);			
				end_of_intr();
				break;

			case CREATE:	
				/* retrieve args passed from syscall() */
				ap = (va_list)p->args;
				funcptr = va_arg(ap, void*);
				stack = va_arg(ap, int);

				/* create new process */
				create(funcptr, stack);

				ready(p);
				break;

			case YIELD:
				ready(p);
				break;

			case STOP:
				/* free allocated memory and put process on stop queue */
				stop(p);
				kfree(p->mem);
				break;
			
			case GETPID:
				/* sets the proc pid as the proc return code for next ctsw */
				p->rc = p->pid;
				ready(p);								
				break;

			case PUTS:
				/* synchronous kernel print handler*/
				ap = (va_list)p->args;
				str = va_arg(ap, char*);
				kprintf("[kernel]: %s", str);
				ready(p);				
				break;

			case SLEEP:
				ap = (va_list)p->args;
				sleep_ms = va_arg(ap, unsigned int);
				p->delta_slice = sleep_to_slice(sleep_ms);

				/* proc requested no sleep or syssleep is blocked for the time requested*/
				if(!p->delta_slice || !sleep(p))
					ready(p);
		}
	}
}

/*
* next
*
* @desc: 	pop the head of ready queue
*
* @output:	p	current head of the ready queue
*/
pcb_t* next ()
{
    	pcb_t *p = ready_q;
    	if(p) ready_q = p->next;
    	return p;
}

/*
* ready
*
* @desc:	push pcb block to the end of ready queue
*/
void ready(pcb_t *p) 
{
	pcb_t *tmp = ready_q;

	p->next=NULL;
	if(!tmp) 
	{
		ready_q = p;
		ready_q->next = NULL;
		return;
	}

	while(tmp) 
	{
		if(!tmp->next) break;
		tmp = tmp->next;
	}
	tmp->next = p;
}

/*
* count
*
* @desc:	count the number of pcb in the ready queue
* @note:	the stop queue count is (MAX_PROC-cnt)
*/
int count (void)
{
	int cnt=0;
	pcb_t *tmp = ready_q;

	while(tmp) 
	{
		cnt++;
		tmp=tmp->next;
	}

	return cnt;
}

/*
* stop
*
* @desc: 	add pcb block to the end of stop queue
*/
void stop (pcb_t *p)
{
	pcb_t *tmp = stop_q;

	p->next=NULL;
	if(!tmp) 
	{
		stop_q = p;
		stop_q->next = NULL;
		return;
	}

	while(tmp) 
	{
		if(!tmp->next) break;
		tmp = tmp->next;
	}
	tmp->next = p;
}

/*
* print_ready
*
* @desc: 	output all ready queue proc pid to console
*/
void print_ready ()
{
	pcb_t *tmp = ready_q;

	while(tmp) 
	{
		kprintf("%d ", tmp->pid);
		tmp=tmp->next;
	}
	kprintf("\n");
}
