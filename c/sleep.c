/* sleep.c : sleep device (assignment 2)
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */
pcb_t *sleep_q;
static unsigned int slice_elapsed = 0;

/*
* sleep_to_slice
*
* @desc:	converts time in millisecond to number of time slices
*
* @param:	ms		time in milliseconds
*
* @output:	slice		millisecond in milliseconds
*
* @note:	the quantum of a time slice is CLOCK_DIVISOR / 10
*/
unsigned int sleep_to_slice (unsigned int ms)
{
	unsigned int slice = (ms / 10) + (ms % 10 ? 1 : 0);
	return slice;
}

/*
* sleep
*
* @desc:	puts proc pcb in the sleep delta list
*
* @param:	p		proc pcb to place in the sleep queue
*
* @output:	cnt		returns the number of time slices a process will sleep
*
* @note:	the delta list is dependent on the pcb->delta_slice value
*/
unsigned int sleep (pcb_t *p)
{
	pcb_t *tmp = sleep_q;
	unsigned int cnt=0;

	if(!tmp) 
	{
		sleep_q = p;
		sleep_q->next = NULL;
		return sleep_q->delta_slice;
	}

	/* add proc to head */
	if(p->delta_slice < sleep_q->delta_slice)
	{
		cnt += p->delta_slice;
		sleep_q->delta_slice -= p->delta_slice;
		cnt += sleep_q->delta_slice;
		p->next = sleep_q;
		sleep_q = p;
		return cnt;
	}

	while(tmp) 
	{	
		cnt += tmp->delta_slice;
	
		if(tmp->delta_slice <= p->delta_slice)
		{
			p->delta_slice -= tmp->delta_slice;
		
			/* add proc to body */
			if(tmp->next && tmp->next->delta_slice > p->delta_slice)
			{
				tmp->next->delta_slice -= p->delta_slice;
				p->next = tmp->next;
				tmp->next = p;
				return cnt;
			}
			/* add proc to tail */
			else if(!tmp->next)
			{
				p->next = NULL;
				tmp->next = p;
				return cnt;				
			}
		}

		tmp = tmp->next;
	}

	/* extraneous case, the proc does not get added to sleep queue */
	return 0;
}

/*
* wake
*
* @desc:	returns the head of the sleep queue
* 
* @output:	p	head of sleep queue
*
* @note:	the sleep queue is a delta list, hence removing any element from the list is constant time,
*		hence it does not need to iterate through the list to decrement each element sleep value
*/
void wake ()
{
	/* put sleep_q head back in ready_q */
    	pcb_t *p = sleep_q;
   	sleep_q = sleep_q->next;
	ready(p);

	/* put back any proc whose delta_slice is 0 and trails the head */
	while(sleep_q)
	{
		if(!(sleep_q->delta_slice))	
		{
			p = sleep_q;
			sleep_q = sleep_q->next;
			ready(p);
		}
		else
			break;
	}
}

/*
* tick
*
* @desc:	increment the global tick value, if the global tick value equates to the head queue element sleep value,
*		return true, 
*		otherwise return false
*
* @output	bool		return true or false to indicate the whether to wake up the head of sleep queue
*/
unsigned int tick() 
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
* @desc:	returns the number of elements in the sleep queue
*
* @output:	cnt		number of elements in sleep queue
*/
unsigned int sleeper()
{
	pcb_t *tmp = sleep_q;
	unsigned int cnt = 0;
	while(tmp)
	{
		cnt++;
		tmp = tmp->next;
	}
	return cnt;
}

/*
* puts_sleep_q
*
* @desc:	output all sleep queue proc pid with its respective delta_slice to console
*/
void puts_sleep_q()
{
	pcb_t *tmp = sleep_q;

	while(tmp)
	{
		kprintf("%d(%d) ", tmp->pid, tmp->delta_slice);
		tmp = tmp->next;
	}
	kprintf("\n");
}
