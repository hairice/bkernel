/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
extern pcb_t *stop_q;

pcb_t *ready_q;

/*
* dispatch
*
* @desc:	Executes the dispatcher
*/
void dispatch() 
{
	int request,stack=0;
	pcb_t *p=NULL;
    	va_list ap;
	void (*funcptr)(void);

	// Start dispatcher
	for(;;) 
	{
		p = next();

		if(!p) break; 	// End of process management test mode

		request = contextswitch(p);

		switch(request) {
			case CREATE:	
				// Retrieve args passed from syscall()
				ap = (va_list)p->args;
				funcptr = va_arg(ap, void*);
				stack = va_arg(ap, int);

				// Create new process and put process on ready queue
				if(create(funcptr, stack));
					ready(p);
				break;

			case YIELD:
				ready(p);
				break;

			case STOP:
				// Free allocated memory and put process on stop queue
				stop(p);
				kfree(p->mem);
				break;
		}
	}
}

/*
* next
*
* @desc: 	Pop the head of ready queue
*
* @output:	p	Current head of the ready queue
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
* @desc:	Push pcb block to the end of ready queue
*/
void ready(pcb_t *p) 
{
	pcb_t *tmp = ready_q;

	p->next=NULL;
	if(!tmp) 
	{
		ready_q = p;
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
* @desc:	Count the number of pcb in the ready queue
* @note:	The stop queue count is (MAX_PROC-cnt)
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
* @desc: 	Add pcb block to the end of stop queue
*/
void stop (pcb_t *p)
{
	pcb_t *tmp = stop_q;

	p->next=NULL;
	if(!tmp) 
	{
		stop_q = p;
		return;
	}

	while(tmp) 
	{
		if(!tmp->next) break;
		tmp = tmp->next;
	}
	tmp->next = p;
}
