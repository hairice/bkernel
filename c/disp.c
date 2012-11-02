/* disp.c : dispatcher
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
extern pcb_t *stop_q;
extern pcb_t proc_table[MAX_PROC];

pcb_t *ready_q;


/*
* dispatch
*
* @desc:	executes the dispatcher
*/
void dispatch() 
{
	unsigned int request;
	pcb_t *p=NULL;
    	va_list ap;

	/* create arg(s) */
	unsigned int stack=0;
	void (*funcptr)(void);

	/* puts arg(s) */
	char* str=NULL;

	/* sleep arg(s) */
	unsigned int sleep_ms=0;

	/* ipc arg(s) */
	unsigned int pid;
	unsigned int *pid_ptr;	/* used for from_id for sysrecv() 		*/
	pcb_t *proc=NULL;
	void *buffer;
	int buffer_len;
	ipc_t *comm;		/* used to temporarily hold the ipc args 	*/
	int *mem;


	/* start dispatcher */
	for(;;) 
	{
		p = next();

		/* execute the idle proc only when there are no other proc in ready_q */
		if(p->pid == IDLE_PROC_PID && count() > 0) 
		{
			ready(p);
			continue;
		}
		
		p->state = RUNNING_STATE;
		request = contextswitch(p);

		// service syscall/interrupt requests
		switch(request) {
 			case TIMER_INT:
				/* signal sleep device is there's at least 1 sleeping proc */
				if(sleeper() > 0 && tick())
					ready(wake());

				p->state = READY_STATE;				
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

				p->state = READY_STATE;				
				ready(p);
				break;

			case YIELD:
				p->state = READY_STATE;				
				ready(p);
				break;

			case STOP:
				/* release all tasks blocked by current proc */
				release(&(p->blocked_senders));
				release(&(p->blocked_receivers));

				/* free allocated memory and put process on stop queue */
				p->state = STOP_STATE;
				stop(p);
				kfree(p->mem);
				break;
			
			case GETPID:
				/* sets the proc pid as the proc return code for next ctsw */
				p->rc = p->pid;
				p->state = READY_STATE;				
				ready(p);								
				break;

			case PUTS:
				/* synchronous kernel print handler*/
				ap = (va_list)p->args;
				str = va_arg(ap, char*);
				kprintf("[kernel]: %s", str);
				p->state = READY_STATE;				
				ready(p);				
				break;

			case SLEEP:
				ap = (va_list)p->args;
				sleep_ms = va_arg(ap, unsigned int);
				p->delta_slice = sleep_to_slice(sleep_ms);

				/* proc requested no sleep or syssleep is blocked for the time requested*/
				if(!p->delta_slice || !sleep(p))
				{
					p->state = READY_STATE;				
					ready(p);
				}
				else
					p->state = SLEEP_STATE;

				break;
		
			case SEND:
				ap = (va_list)p->args;
				pid = va_arg(ap, unsigned int);

				/* when proc sends to itself, add current proc to ready_q */
				if(p->pid == pid)
				{
					proc->state = READY_STATE;	
					ready(p);
					break;
				}

				buffer = va_arg(ap, void*);
				buffer_len = va_arg(ap, int);

			
				/* hold the ipc() args in the generic ptr in pcb */
				mem = kmalloc(sizeof(ipc_t));

				comm = (ipc_t *) ((int)mem); 
				comm->buffer = buffer;
				comm->buffer_len = buffer_len;
				p->ptr = comm;

				/* search for ipc_receiver in block_q */
				proc = unblock(p->blocked_receivers, pid);
				if(proc)
				{
					/* set return value as the number of bytes sent */
					p->rc = send(p, proc);
					proc->rc = p->rc;	

					/* free allocated mem for ipc args for both sender and receiver */
					kfree(mem);
					mem = (int *) proc->ptr;
					kfree(mem);

					/* set proc as ready and put back on ready_q */			
					p->state = READY_STATE;
					proc->state = READY_STATE;	
					ready(p);
					ready(proc);
				}
				else
				{
					/* receiver not found, snd_proc is now blocked */
					proc = get_proc(pid);
					if(proc) 
					{
						/* deadlock detection for ipc blocked send/receive queues */
						/* when a deadlock is detected, only the current proc is put back on the ready_q */
						if(deadlock(p->blocked_senders, proc))
						{
							kfree(mem);
							p->state = READY_STATE;
							ready(p);
						}
						else
						{
							block(&(proc->blocked_senders), p);
							p->state = BLOCK_ON_SEND_STATE;
						}
					}
					else
					{
						/* receiver does not exist, current proc is returned to ready_q */
						kfree(mem);
						p->state = READY_STATE;
						ready(p);
					}
				}
				
				break;
				
			case RECV:
				ap = (va_list)p->args;
				pid_ptr = va_arg(ap, unsigned int*);

				/* when proc sends to itself, add current proc to ready_q */
				if(p->pid == *pid_ptr)
				{
					proc->state = READY_STATE;	
					ready(p);
					break;
				}

				buffer = va_arg(ap, void*);
				buffer_len = va_arg(ap, int);



				/* hold the ipc() args in the generic ptr in pcb */
				mem = kmalloc(sizeof(ipc_t));
				comm = (ipc_t *) ((int)mem); 
				comm->buffer = buffer;
				comm->buffer_len = buffer_len;
				p->ptr = comm;

				/* search for ipc_receiver in block_q */
				proc = unblock(p->blocked_senders, *pid_ptr);
				if(proc)
				{
					/* when the receiver wants to receive from pid 0, update to the actual sender pid */
					if(!(*pid_ptr)) *pid_ptr = proc->pid;

					/* set return value as the number of bytes sent */
					p->rc = recv(proc, p);
					proc->rc = p->rc;					

					/* free allocated mem for ipc args for both sender and receiver */
					kfree(mem);
					mem = (int *) proc->ptr;
					kfree(mem);

					/* set proc as ready and put back on ready_q */		
					p->state = READY_STATE;
					proc->state = READY_STATE;
					ready(p);
					ready(proc);
				}
				else
				{
					/* sender not found, snd_proc is now blocked */
					proc = get_proc(*pid_ptr);
					if(proc) 
					{
						/* deadlock detection for ipc blocked send/receive queues */
						/* when a deadlock is detected, only the current proc is put back on the ready_q */
						if(deadlock(p->blocked_receivers, proc))
						{
							kfree(mem);
							p->state = READY_STATE;
							ready(p);
						}
						else
						{
							block(&(proc->blocked_receivers), p);
							p->state = BLOCK_ON_RECV_STATE;
						}
					}
					else
					{
						/* sender does not exist, current proc is returned to ready_q */
						kfree(mem);
						p->state = READY_STATE;
						ready(p);
					}
				}

				break;
		}
	}
}

/*
* get_proc
*
* @desc: 	get proc with pid
*
* @param:	pid	proc pid
*
* @output:	p	proc with input pid
*/
pcb_t* get_proc(int pid)
{
	int i;

	for(i=0 ; i < MAX_PROC ; i++)
	{
		if(proc_table[i].pid == pid)
			return &(proc_table[i]);
	}

	return NULL;
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
*
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
	p->pid=INVALID_PID;
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
* block
*
* @desc: 	add pcb block to the end of block queue
*
* @note:	a proc can be blocked by the following events,
*		1. ipc_send();
*		2. ipc_recv();
*/
void block(pcb_t **q, pcb_t *p)
{
	pcb_t *tmp = *q;

	p->next = NULL;
	if(!(*q)) 
	{
		*q = p;
		return;
	}

	while(tmp) 
	{
		if(!(tmp->next)) break;
		tmp = tmp->next;
	}
	tmp->next = p;
}

/*
* unblock
*
* @desc: 	get the proc pcb from the block_q
*
* @param:	pid		pid of the proc pcb to retrieve from the block_q
*
* @output:	p		proc pcb for the matching input pid
*		role		ipc counterpart desired role (sender/receiver)
*
* @note:	ipc_senders calls this function to find the matching ipc_receiver, and vice versa
*/
pcb_t* unblock(pcb_t *q, unsigned int pid)
{
	pcb_t *tmp=NULL, *p=NULL;

	/* a proc is unblocked if the following conditions are met
	*  1. the proc's pid is the desired endpoint pid
	*  2. the proc's desired endpoint pid is current pid being serviced by the dispatcher
	*  3. the proc has the correct role (sender/receiver)
	*
	*  a proc can also be blocked if the following conditions are met
	*  1. the desired endpoint pid is 0
	*  2. the role is SENDER
	*  3. the proc's role is SENDER
	*/	
	if(!q) return NULL;
	if(q->pid == pid)
	{
		p = q;
		q = q->next;
		return p;
	}

	/* find matching ipc proc in body of block_q, 
	*  at this point, the algorithm assumes the block_q has at least 2 proc
	*/
	tmp = q;
	while(tmp && tmp->next) 
	{			
		/* stated conditions have been met, the proc is released from block_q */
		if(tmp->next->pid == pid)
		{
			p = tmp->next;
			tmp->next = tmp->next->next;
			return p;
		}

		tmp = tmp->next;
	}

	return NULL;
}

/*
* deadlock
*
* @desc: 	checks for deadlock scenario in ipc communication
*
* @param:	q		queue for checking if certain proc exists
*		p		targetted proc for deadlock checking
*
* @output:	Bool		proc pcb for the matching input pid
*
* @note:	for deadlock to occur, proc1 would be on the same blocked queue of proc2, as proc2 is of proc1
*/
Bool deadlock(pcb_t *q, pcb_t *p)
{
	while(q)
	{
		if(q->pid == p->pid) return TRUE;
		q=q->next;
	}

	return FALSE;
}

/*
* release
*
* @desc: 	place procs in a queue back into the ready_q
*
* @param:	q		queues of proc to be released back into ready_q
*/
void release(pcb_t **q)
{
	pcb_t *tmp1 = *q, *tmp2;

	while(tmp1)
	{
		tmp2 = tmp1->next;
		ready(tmp1);
		tmp1 = tmp2;
	}

	*q = NULL;
}

/*
* puts_ready_q
*
* @desc: 	output all ready queue proc pid to console
*/
void puts_ready_q()
{

	pcb_t *tmp = ready_q;

	while(tmp) 
	{
		kprintf("%d ", tmp->pid);
		tmp=tmp->next;
	}
	kprintf("\n");
}
