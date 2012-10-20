/* sleep.c : sleep device (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */
pcb_t *sleep_q;
static unsigned int slice_elapsed = 0;

/*
* sleep_to_slice
*
* @desc:	
*/
unsigned int sleep_to_slice (unsigned int ms)
{
	unsigned int slice = (ms / 10) + (ms % 10 ? 1 : 0);
	return slice;
}

/*
* sleep
*
* @desc:	
*/
void sleep (pcb_t *p)
{
	pcb_t *tmp = sleep_q;

	if(!tmp) 
	{
		sleep_q = p;
		sleep_q->next = NULL;
		return;
	}

	/* add to head */
	if(p->delta_slice < sleep_q->delta_slice)
	{
		sleep_q->delta_slice -= p->delta_slice;
		p->next = sleep_q;
		sleep_q = p;
		return;
	}

	while(tmp) 
	{
		if(tmp->delta_slice < p->delta_slice)
		{
			p->delta_slice -= tmp->delta_slice;

			/* add to body */
			if(tmp->next && tmp->next->delta_slice > p->delta_slice)
			{
				tmp->next->delta_slice -= p->delta_slice;
				p->next = tmp->next;
				tmp->next = p;
				return;
			}
			/* add to tail */
			else if(!tmp->next)
			{
				p->next = NULL;
				tmp->next = p;
				return;				
			}
		}

		tmp = tmp->next;
	}
}

/*
* wake
*
* @desc:	
*/
pcb_t* wake ()
{
    	pcb_t *p = sleep_q;
    	if(p) sleep_q = p->next;

	return p;
}

/*
* tick
*
* @desc:	
*/
int tick() 
{
	slice_elapsed++;
	if(slice_elapsed == sleep_q->delta_slice) 
	{
		slice_elapsed = 0;	
		return 1;
	}
	else return 0;
}

/*
* sleeper
*
* @desc:	
*/
int sleeper ()
{
	pcb_t *tmp = sleep_q;
	int cnt = 0;
	while(tmp)
	{
		cnt++;
		tmp = tmp->next;
	}
	return cnt;
}
