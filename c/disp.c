/* disp.c : dispatcher
* 
* name:         Jack Wu
* student id:   17254079
*/

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
extern pcb_t *stop_q;
extern pcb_t proc_table[PROC_SZ];

pcb_t *ready_q;


/*
* dispatch
*
* @desc:        executes the kernel dispatcher
*
* @note:	the following request are serviced by this dispatcher
*		1. timer_interrupt
*		2. syscreate()
*		3. sysyield() 
*		4. sysstop()
*		5. sysgetpid()
*		6. sysputs()
*		7. syssleep()
*		8. syssend()
*		9. sysrecv()
*/
void dispatch() 
{
        unsigned int request,pid;
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
        unsigned int *pid_ptr;  /* used for from_id for sysrecv()               */
        void *buffer;
        int buffer_len;

	/* sig arg(s) */
	unsigned int signal;
	void *new_handler;
	void **old_handler;
	void *osp;


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

		
		/* find high priority signal and execute handler */		
		if(p->sig_target_mask)
			p->rc = sighigh(p);

                p->state = RUNNING_STATE;
                request = contextswitch(p);

                /* service syscall/interrupt requests */
                switch(request) {
                        case TIMER_INT:
                                /* signal sleep device is there's at least 1 sleeping proc */
                                if(sleeper() > 0 && tick())
                                        wake();

                                p->state = READY_STATE;                         
                                ready(p);               
        
                                end_of_intr();
                                break;

			case KBD_INT:
				break;

                        case CREATE:    
                                /* retrieve args passed from syscall() */
                                ap = (va_list)p->args;
                                funcptr = va_arg(ap, void*);
                                stack = va_arg(ap, int);

                                /* create new process */
                                /* parameter checking is done inside create() */
                                p->rc = create(funcptr, stack);

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

                                /* update max and min pid values in proc_table */
                                set_max_pid();  
                                set_min_pid();

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
                
                                if(str)
                                        kprintf("%s\n\0", str);
        
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
					p->rc = BLOCKED_SLEEP;		/* erroneous request time for sleep */
                                        p->state = READY_STATE;                         
                                        ready(p);
                                }
                                else
                                        p->state = SLEEP_STATE;

                                break;
                
                        case SEND:
                                ap = (va_list)p->args;
                                pid = va_arg(ap, unsigned int);
                                buffer = va_arg(ap, void*);
                                buffer_len = va_arg(ap, int);

				/* execute ipc_send */
				send(p, pid, buffer, buffer_len);
                                break;
                                
                        case RECV:
                                ap = (va_list)p->args;
                                pid_ptr = va_arg(ap, unsigned int*);
                                buffer = va_arg(ap, void*);
                                buffer_len = va_arg(ap, int);

				/* execute ipc_recv */
				recv(p, pid_ptr, buffer, buffer_len);
                                break;

			/* signal handlers */
			case SIG_HANDLER:
                                ap = (va_list)p->args;
                                signal = va_arg(ap, unsigned int);
                                new_handler = va_arg(ap, void*);
                                old_handler = va_arg(ap, void**);

				/* install new signal */
				p->rc = siginstall(p, signal, new_handler, old_handler);		
                                ready(p);                               
				break;

			case SIG_RETURN:
                                ap = (va_list)p->args;
                                osp = va_arg(ap, void*);

				p->esp = (int *) osp;
                                ready(p);                               	
				break;

			case SIG_KILL:
                                ap = (va_list)p->args;
                                pid = va_arg(ap, unsigned int);
                                signal = va_arg(ap, unsigned int);

				/* enable target bit in proc target_mask */
				p->rc = sigkill(pid, signal);
				ready(p);                               	
				break;

			case SIG_WAIT:
				p->state = BLOCK_ON_SIG_STATE;
				break;	

			case DEV_OPEN:
				break;

			case DEV_CLOSE:
				break;

			case DEV_WRITE:
				break;

			case DEV_READ:
				break;

			case DEV_IOCTL:
				break;
                }
        }
}

/*
* get_proc
*
* @desc:        get proc with pid
*
* @param:       pid     proc pid
*
* @output:      p       proc with input pid
*/
pcb_t* get_proc(int pid)
{
        int i;

        for(i=0 ; i < PROC_SZ ; i++)
        {
                if(proc_table[i].pid == pid)
                        return &(proc_table[i]);
        }

        return NULL;
}

/*
* next
*
* @desc:        pop the head of ready queue
*
* @output:      p       current head of the ready queue
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
* @desc:        push pcb block to the end of ready queue
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
* @desc:        count the number of pcb in the ready queue
*
* @note:        the stop queue count is (PROC_SZ-cnt)
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
* @desc:        add pcb block to the end of stop queue
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
* release
*
* @desc:        place all procs in a queue back into the ready_q
*
* @param:       q		queues of proc to be released back into ready_q
*/
void release(pcb_t **q)
{
        pcb_t *tmp1 = *q, *tmp2;

        while(tmp1)
        {
                tmp2 = tmp1->next;
                tmp1->rc = ERR_IPC;
                ready(tmp1);
                tmp1 = tmp2;
        }

        *q = NULL;
}

/*
* puts_ready_q
*
* @desc:        output all ready queue proc pid to console
*/
void puts_ready_q()
{
        pcb_t *tmp = ready_q;

        kprintf("ready_q: ");
        while(tmp) 
        {
                kprintf("%d ", tmp->pid);
                tmp=tmp->next;
        }
        kprintf("\n");
}
