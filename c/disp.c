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
	int i,request,stack=0;
	pcb_t *p=NULL;
    	va_list ap;
	void (*funcptr)(void);

	// Start dispatcher
	for(i=0;i<10;i++) 
	{
		p = next();
		request = contextswitch(p);

		switch(request) {
			case CREATE:	
				// Retrieve args passed from system call
				ap = (va_list)p->args;
				funcptr = (va_arg(ap, int));
				stack = va_arg(ap, int);

				// Create new process and put process on ready queue
				create(funcptr, stack);
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
*
*
*/
pcb_t* next ()
{
    	pcb_t *p = ready_q;
    	if(p) ready_q = p->next;
    	return p;
}

/*
*
*
*/
void ready(pcb_t *p) 
{
	pcb_t *tmp;

	p->next=NULL;
	tmp = ready_q;
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
*
*
*/
void stop (pcb_t *p)
{
	pcb_t *tmp;

	p->next=NULL;
	tmp = stop_q;
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
