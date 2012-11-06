/* disp.c : dispatcher
* 
* name:         Jack Wu
* student id:   17254079
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
* @desc:        executes the dispatcher
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
        unsigned int *pid_ptr;  /* used for from_id for sysrecv()               */
        pcb_t *proc=NULL;
        void *buffer;
        int buffer_len;
        ipc_t *comm;            /* used to temporarily hold the ipc args        */
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
                                        kprintf("%s\n", str);
        
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
* @note:        the stop queue count is (MAX_PROC-cnt)
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
