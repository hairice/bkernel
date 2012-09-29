/* disp.c : dispatcher
 */

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */


pcb_t *ready_q;

static int pid = 1;

void dispatch() 
{
	pcb_t *p;
	int request;
    	int stack;
    	va_list ap;
	void (*funcptr)(void);
	int i;


	for(i=0;i<6;i++) 
	{
		p = next();
		request = contextswitch(p);

		switch(request) {
			case CREATE:	
				ap = (va_list)p->args;
				funcptr = (va_arg(ap, int));
				stack = va_arg(ap, int);
				create(funcptr, stack, pid);
				pid++;
				ready(p);
				break;

			case YIELD:
				ready(p);
				break;

			case STOP:
				kfree(p->mem);
				break;
		}
	}
}

pcb_t* next ()
{
    	pcb_t *p = ready_q;
    	if(p) ready_q = p->next;
    	return p;
}

void ready(pcb_t *p) 
{
	pcb_t *tmp;
	int sz;

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
