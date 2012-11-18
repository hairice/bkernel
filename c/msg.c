/* msg.c : messaging system (assignment 2)
 */

#include <xeroskernel.h>
#include <i386.h>

/* Your code goes here */
extern long freemem;		/* used to check buffer address location is in user stack space */


/*
* send
*
* @desc:                execute transferring of an ipc message from the perspective of the sender
*
* @param:               p		sender proc
*			pid		receiver proc pid
*			buffer		sender buffer
*			buffer_len	sender buffer length
*
* @note:        	if the receiver process is currently not on the current proc's blocked receiver queue or no receiver_any proc is found,
*			then this proc added to the receiver's blocked_sender queue if the receiver proc pid exists and that no deadlock is detected
*/
void send(pcb_t *p, unsigned int pid, void *buffer, int buffer_len)
{
	ipc_t *comm = NULL, *dst_comm = NULL;
	char *rcv_buffer, *snd_buffer;
	int *mem = NULL;
	pcb_t *proc = NULL;

	/* when proc sends to itself, add current proc to ready_q */
        if(p->pid == pid)
        {
        	p->state = READY_STATE; 
                p->rc = ERR_LOOPBACK;
                ready(p);
                return;
	}

        /* when an empty buffer_len or null buffer is passed, add current proc to ready_q */
        if(!buffer_len || !buffer || !pid)
        {
         	p->state = READY_STATE; 
                p->rc = ERR_IPC;
                ready(p);
                return;
	}

	/* check the buffer address is within the user stack space, 
	* the buffer address should be ensured that it is not in the following regions.
	* 1. below freemem
	* 2. between holestart and holeend
	* 3. above 4mb
	*/
	if( (int)buffer < freemem ||
	( (int)buffer > HOLESTART && (int)buffer < HOLEEND ) ||
	( (int)buffer > (int)0x400000 ))
	{
         	p->state = READY_STATE; 
                p->rc = ERR_IPC;
                ready(p);
                return;
	}

	/* hold the ipc() args in the generic ptr in pcb */
        mem = kmalloc(sizeof(ipc_t));
        comm = (ipc_t *) ((int)mem); 
        comm->pid_ptr = &pid;
        comm->buffer = buffer;
        comm->buffer_len = buffer_len;
	comm->pid = pid;
        p->ptr = comm;

	/* search for ipc_receiver in block_q */
        proc = unblock(&(p->blocked_receivers), pid);
        if(proc)
        {
		/* transfer data from sender buffer to receiver buffer */
		dst_comm = proc->ptr;
		snd_buffer = (char *) comm->buffer;
		rcv_buffer = (char *) dst_comm->buffer;
		sprintf(rcv_buffer, "%s", snd_buffer);

              	/* set return value as the number of bytes sent */
		if(comm->buffer_len >= dst_comm->buffer_len)
			p->rc = dst_comm->buffer_len;
		else
			p->rc = comm->buffer_len;
                proc->rc = p->rc;

                /* set proc as ready and put back on ready_q */                 
                p->state = READY_STATE;
                proc->state = READY_STATE;      
                ready(p);
                ready(proc);
	}
        else
        {
        	proc = get_proc(pid);
                if(proc) 
                {
                	/* check for receive any */
                        dst_comm = (ipc_t*) proc->ptr;
                        if(proc->ptr && *(dst_comm->pid_ptr) == RECEIVE_ANY_PID && proc->state == BLOCK_ON_RECV_STATE)
                        {
				/* transfer data from sender buffer to receiver buffer */
				dst_comm = proc->ptr;
				snd_buffer = (char *) comm->buffer;
				rcv_buffer = (char *) dst_comm->buffer;
				sprintf(rcv_buffer, "%s", snd_buffer);
			
                        	/* set return value as the number of bytes sent */
				if(comm->buffer_len >= dst_comm->buffer_len)
					p->rc = dst_comm->buffer_len;
				else
					p->rc = comm->buffer_len;
                                proc->rc = p->rc;

                                /* update sender pid for the receiver */
                                *(dst_comm->pid_ptr) = p->pid;

                                /* set proc as ready and put back on ready_q */                 
                                p->state = READY_STATE;
                                proc->state = READY_STATE;      
                                ready(p);
                                ready(proc);
 			}
                        /* deadlock detection for ipc blocked send/receive queues */
                        /* when a deadlock is detected, only the current proc is put back on the ready_q */
                        else if(deadlock(p->blocked_senders, proc))
                        {
                        	/* deadlock detected, current proc is put back to the ready_q */
                        	kfree(mem);
                                p->rc = ERR_IPC;
                                p->state = READY_STATE;
                                ready(p);
			}
                        else
                        {
				/* no deadlock detected, add proc to blocked_senders queue */
                        	block(&(proc->blocked_senders), p);
                                p->state = BLOCK_ON_SEND_STATE;
  			}
		}
                else
                {
                	/* receiver does not exist, current proc is returned to ready_q */
                        kfree(mem);
                        p->rc = ERR_PID;
                        p->state = READY_STATE;
                        ready(p);
		}
	}
}

/*
* recv
*
* @desc:                execute transferring of an ipc message from the perspective of the receiver
*
* @param:               p		receiver proc
*			pid		sender proc pid
*			buffer		receiver buffer
*			buffer_len	receiver buffer length
*        
*
* @note:        	if the sender process is currently not on the current proc's blocked sender queue, then this proc
*			added to the sender's blocked_receivers queue if the sender proc pid exists and that no deadlock is detected
*/
void recv(pcb_t *p, unsigned int *pid, void *buffer, int buffer_len)
{
	ipc_t *comm = NULL, *src_comm = NULL;
	char *rcv_buffer, *snd_buffer;
	int *mem = NULL;
	pcb_t *proc = NULL;

	/* when an empty buffer_len or null buffer is passed, add current proc to ready_q */
        if(p->pid == *pid)
        {
        	p->state = READY_STATE; 
                p->rc = ERR_LOOPBACK;
                ready(p);
		return;
        }


	/* when an empty buffer_len is passed, add current proc to ready_q */
        if(!buffer_len || !buffer)
        {
        	p->state = READY_STATE; 
        	p->rc = ERR_IPC;
                ready(p);
		return;
	}

	/* check the buffer address is within the user stack space, 
	* the buffer address should be ensured that it is not in the following regions.
	* 1. below freemem
	* 2. between holestart and holeend
	* 3. above 4mb
	*/
	if( (int)buffer < freemem ||
	( (int)buffer > HOLESTART && (int)buffer < HOLEEND ) ||
	( (int)buffer > (int)0x400000 ))
	{
         	p->state = READY_STATE; 
                p->rc = ERR_IPC;
                ready(p);
                return;
	}
	
	/* hold the ipc() args in the generic ptr in pcb */
        mem = kmalloc(sizeof(ipc_t));
        comm = (ipc_t *) ((int)mem); 
        comm->pid_ptr = pid;
        comm->buffer = buffer;
        comm->buffer_len = buffer_len;
        p->ptr = comm;
                                

        /* search for ipc_receiver in block_q */
        proc = unblock(&(p->blocked_senders), *pid);                
        if(proc)
        {
        	/* when the receiver wants to receive from pid 0, update to the actual sender pid */
                if(!(*pid)) *pid = proc->pid;

		/* transfer data from sender buffer to receiver buffer */
		src_comm = proc->ptr;
		snd_buffer = (char *) src_comm->buffer;
		rcv_buffer = (char *) comm->buffer;
		sprintf(rcv_buffer, "%s", snd_buffer);

                /* set return value as the number of bytes sent */
		if(comm->buffer_len >= src_comm->buffer_len)
			p->rc = src_comm->buffer_len;
		else
			p->rc = comm->buffer_len;
                proc->rc = p->rc;                                    
                              
                /* set proc as ready and put back on ready_q */         
                p->state = READY_STATE;
                proc->state = READY_STATE;
                ready(p);
                ready(proc);
	}
        else
        {
        	/* place receive_any receiver in block state, this process now is no longer attached to any queues */
                if(!(*pid))
                {
                	p->state = BLOCK_ON_RECV_STATE;
			return;
		}

                /* sender not found, snd_proc is now blocked */
                proc = get_proc(*pid);
                if(proc) 
                {
                	/* deadlock detection for ipc blocked send/receive queues */
                        /* when a deadlock is detected, only the current proc is put back on the ready_q */
                        if(deadlock(p->blocked_receivers, proc))
                        {
                        	/* deadlock detected, current proc is put back to the ready_q */
                        	kfree(mem);
                                p->rc = ERR_IPC;
                                p->state = READY_STATE;
                                ready(p);
                        }
                        else
                        {
				/* no deadlock detected, add proc to blocked_receivers queue */
                                block(&(proc->blocked_receivers), p);
                                p->state = BLOCK_ON_RECV_STATE;
                        }
		}
                else
                {
                	/* sender does not exist, current proc is returned to ready_q */
                        kfree(mem);
                        p->rc = ERR_PID;
                        p->state = READY_STATE;
                        ready(p);
                }
	}
}

/*
* block
*
* @desc:        add pcb block to the end of block queue
*
* @param:       p               proc to be added to a blocked queue
*               q               blocked queue to add a new proc
*
* @note:        a proc can be blocked by the following events,
*               1. ipc_send();
*               2. ipc_recv();
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
* @desc:        get the proc pcb from the block_q
*
* @param:       pid             pid of the proc pcb to retrieve from the block_q
*               q               blocked queue to search for a particular proc pid
*
* @output:      p               proc that has been unblocked from queue
*
* @note:        ipc_senders call this function to find the matching ipc_receiver, and vice versa
*/
pcb_t* unblock(pcb_t **q, unsigned int pid)
{
        pcb_t *tmp=*q, *p=NULL;

        /* a proc is unblocked if the following conditions are met
        * 1. the queue proc pid matches to the provided pid
        * 2. the provided pid is 0 (receive_any)
        */      

        if(!(*q)) return NULL;
        if((*q)->pid == pid || !pid)
        {
                p = *q;
                *q = (*q)->next;
                return p;
        }

        /* find matching ipc proc in body of block_q, 
        *  at this point, the algorithm assumes the block_q has at least 2 proc
        */
        while(tmp && tmp->next) 
        {                       
                /* stated conditions have been met, the proc is released from block_q */
                if(tmp->next->pid == pid || !pid)
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
* @desc:        checks for deadlock scenario in ipc communication
*
* @param:       q               queue for checking if certain proc exists
*               p               targetted proc for deadlock checking
*
* @output:      Bool            proc pcb for the matching input pid
*
* @note:        for deadlock to occur, proc1 would be on the same blocked queue of proc2, as proc2 is of proc1
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
* puts_blocked_q
*
* @desc:        output all blocked queue proc pid to console
*/
void puts_blocked_q()
{
        int i;
        pcb_t *tmp;
	    ipc_t *comm;

        for(i=0; i<PROC_SZ; i++)
        {
                if(proc_table[i].pid == INVALID_PID) continue;          
                if(proc_table[i].pid == IDLE_PROC_PID) continue;
                
                /* check proc blocked_senders queue */ 
                tmp = proc_table[i].blocked_senders;
                if(tmp)
                {
                        kprintf("pid %d blocked_sender:\t", proc_table[i].pid);
                        while(tmp)
                        {
				comm = (ipc_t *) tmp->ptr;
                                kprintf("%d(%d) ", tmp->pid, comm->pid);
                                tmp=tmp->next;
                        }
                        kprintf("\n");
                }

                /* check proc blocked_receivers queue */ 
                tmp = proc_table[i].blocked_receivers;
                if(tmp)
                {
                        tmp = proc_table[i].blocked_receivers;
                        kprintf("pid %d blocked_receiver:\t", proc_table[i].pid);
                        while(tmp)
                        {
				comm = (ipc_t *) tmp->ptr;
                                kprintf("%d(%d) ", tmp->pid, *(comm->pid_ptr));
                                tmp=tmp->next;
                        }
                        kprintf("\n");
                }
        }
}

/*
* puts_receive_any
*
* @desc:        output all receive any proc pid with state BLOCK_ON_RECV_STATE to console
*
* @note:        a proc that has been blocked on a receive_any call does not exist on any particular proc's blocked_receivers queue
*/
void puts_receive_any ()
{
        int i;
        ipc_t *comm;

        kprintf("receive_any: ");
        for(i=0; i<PROC_SZ; i++)
        {
                if(proc_table[i].pid == INVALID_PID) continue;          
                if(proc_table[i].pid == IDLE_PROC_PID) continue;
                if(!(proc_table[i].ptr)) continue;
        
                comm = (ipc_t *) proc_table[i].ptr;
                
                /* cycle through the process table and look for proc whose dest_proc pid is 0 and whose state is BLOCK_ON_RECV_STATE */
                if(*(comm->pid_ptr) == RECEIVE_ANY_PID && proc_table[i].state == BLOCK_ON_RECV_STATE)
                        kprintf("%d ", proc_table[i].pid);
        }
        kprintf("\n");
}
