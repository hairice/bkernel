/* msg.c : messaging system (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */


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
void send(pcb_t* p, unsigned int pid, void *buffer, int buffer_len)
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

	/* hold the ipc() args in the generic ptr in pcb */
        mem = kmalloc(sizeof(ipc_t));
        comm = (ipc_t *) ((int)mem); 
        comm->pid_ptr = &pid;
        comm->buffer = buffer;
        comm->buffer_len = buffer_len;
        p->ptr = comm;

	/* search for ipc_receiver in block_q */
        proc = unblock(&(p->blocked_receivers), pid);
        if(proc)
        {
		dst_comm = proc->ptr;
		snd_buffer = (char *) comm->buffer;
		rcv_buffer = (char *) dst_comm->buffer;

        	/* set return value as the number of bytes sent */
		sprintf(rcv_buffer, "%s", snd_buffer);

		if(comm->buffer_len >= dst_comm->buffer_len)
			p->rc = dst_comm->buffer_len;
		else
			p->rc = comm->buffer_len;
                proc->rc = p->rc;

                /* free allocated mem for ipc args for both sender and receiver */
                //kfree(mem);
                //mem = (int *) proc->ptr;
                //kfree(mem);

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
                        	/* set return value as the number of bytes sent */
				dst_comm = proc->ptr;
				snd_buffer = (char *) comm->buffer;
				rcv_buffer = (char *) dst_comm->buffer;
				sprintf(rcv_buffer, "%s", snd_buffer);
			
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

                                /* free allocated mem for ipc args for both sender and receiver */
                                //kfree(mem);
                                //mem = (int *) proc->ptr;
                                //kfree(mem);
 			}
                        /* deadlock detection for ipc blocked send/receive queues */
                        /* when a deadlock is detected, only the current proc is put back on the ready_q */
                        else if(deadlock(p->blocked_senders, proc))
                        {
                        	kfree(mem);
                                p->rc = ERR_IPC;
                                p->state = READY_STATE;
                                ready(p);
			}
                        else
                        {
                        	/* deadlock detected, current proc is put back to the ready_q */
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

                /* free allocated mem for ipc args for both sender and receiver */
                //kfree(mem);
                //mem = (int *) proc->ptr;
                //kfree(mem);
                                
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
                        	kfree(mem);
                                p->rc = ERR_IPC;
                                p->state = READY_STATE;
                                ready(p);
                        }
                        else
                        {
                        	/* deadlock detected, current proc is put back to the ready_q */
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
