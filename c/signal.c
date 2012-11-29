/* signal.c : 	process signal handling (assignment 3)
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */
extern long freemem;
extern pcb_t proc_table[PROC_SZ];


/* signal arguments for sigtramp to be placed right below a new signal stack in user space */
typedef struct sig_arg sig_arg_t;
struct sig_arg
{
	void (*handler)( void *);		/* pointer to signal handler 						*/
        void *cntx;				
     	void *osp;				/* stack pointer to stack right below the new signal stack 		*/
	int rc;					/* saved return value from the stack right below the new signal stack 	*/
	unsigned int sig_ignore_mask;		/* saved ignore mask from the stack right below the new signal stack 	*/
};


/*
* sigtramp
*
* @desc:	
*
* @param:	handler			signal handler function
*		cntx			proc old stack pointer value from stack below
*		osp			proc old stack pointer value from stack below
*		rc			proc old return value from stack below
*		oim			proc old ignore mask value from stack below
*
* @note:	this is signal function the context switcher first switches into, and then signal handler would be called
*/
void sigtramp(void (*handler)(void *), void *cntx, void *osp, int rc, unsigned int oim)
{
	int *stack = (int *) osp;
	handler(cntx);

	/* old stack pointer, return value, and ignore mask are passed back to the stack below */
	sigreturn(osp, rc, oim);
}

/*
* sighigh
*
* @desc:	retrieves the highest pending and not ignored signal for proc to be delivered on next dispatch
*
* @param:	p		proc to deliver signal to
*/
int sighigh(pcb_t *p)
{
	int sig_no=-1,i;
	unsigned int tmp_mask = p->sig_pend_mask, bit_mask=BIT_ON;

	/* get signal number */
	while(tmp_mask)
	{
		sig_no++;
		tmp_mask >>= 1;
	}

	if(sig_no == -1) return 0;

	/* make mask to toggle off signal bit */
	for(i=0 ; i<sig_no ; i++)
		bit_mask *= 2;

	/* toggle off pend signal in mask */
	p->sig_pend_mask &= ~bit_mask;

	return signal(p->pid, sig_no);
}

/*
* signal
*
* @desc:	deliver signal to proc by placing new signal stack in its space
*
* @param:	p		proc to deliver signal to
*		sig_no		signal to be delivered to proc	
*
* @output:	returns the following values
*		SIG_SUCCESS		signal has successfully been delivered onto proc stack
*/
int signal(int pid, int sig_no)
{	
	int i;
	unsigned int mem,bit_mask=BIT_ON;
	pcb_t *p = NULL;
	sig_arg_t *sig_args = NULL;
	context_frame_t *frame = NULL;

	if(sig_no < 0 || sig_no >= SIG_SZ) return ERR_SIG_NO;
	p = get_proc(pid);
	if(!p) return ERR_SIG_PEND_PROC;

	/* get user proc stack pointer */
	mem = p->esp;

	/* setup sigtramp arguments */
	mem -= sizeof(sig_arg_t);
	sig_args = (sig_arg_t*) mem;
	sig_args->rc = p->rc;
	sig_args->osp = p->esp;
	sig_args->cntx = p->esp;
	sig_args->handler = p->sig_table[sig_no];
	sig_args->sig_ignore_mask = p->sig_ignore_mask;		/* old sig_ignore_mask of the stack below */

	/* toggle off all bits for same or lower level signals */
	bit_mask = BIT_ON;
	for(i=0 ; i<sig_no ; i++)
	{
		bit_mask <<= 1;
		bit_mask |= BIT_ON;	
	}
	p->sig_ignore_mask &= ~bit_mask;

	//kprintf("signal\t\tpend_mask: %d\n", p->sig_pend_mask);
	//kprintf("signal\t\tignore_mask: %d\n", ~(p->sig_ignore_mask));

	/* setup sigtramp frame */
	mem -= sizeof(context_frame_t);
	frame = (context_frame_t*) mem;
	frame->iret_cs = getCS();
	frame->iret_eip = (unsigned int) sigtramp;
    	frame->esp = mem;
	frame->ebp = frame->esp;
	frame->eflags = 0x00003200;

	p->esp = mem;

	//newct->edi = 0;
	//newct->esi = 0;
	//newct->ebx = 0;
	//newct->edx = 0;
	//newct->ecx = 0;
	//newct->eax = 0;
	
	return SIG_SUCCESS;
}

/*
* siginstall
*
* @desc:	install new handler for proc in its sig_table
*
* @param:	p			proc for installing new signal handler
*		sig_no			signal for new handler
*		new_handler		handler function
*		old_handler		pointer for replaced signal handler function to be returned back to proc
*
* @output:	returns the following values
*		SIG_SUCCESS		signal has successfully been installed for proc
*/
int siginstall(pcb_t *p, int sig_no, void (*new_handler)(void *), void (**old_handler)(void *))
{
	unsigned int bit_mask=BIT_ON,i;	

	if(!new_handler) return ERR_SIG_HANDLER;
	if(sig_no < 0 || sig_no >= SIG_SZ) return ERR_SIG_NO;
	if(new_handler > freemem) return ERR_SIG_HANDLER;

	/* save old signal handler */
	*old_handler = p->sig_table[sig_no];

	p->sig_table[sig_no] = new_handler;

	/* make mask to toggle on signal bit */
	for(i=0 ; i<sig_no ; i++)
		bit_mask *= 2;

	p->sig_install_mask |= bit_mask;

	return SIG_SUCCESS;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									

/*
* sigkill
*
* @desc:	pend an installed signal for proc
*
* @param:	pid		pid to proc for pending new signal
*		sig_no		signal to be pended for proc
*
* @output:	returns the following values
*		SIG_SUCCESS		signal has successfully been pended for proc
*/
int sigkill(int pid, int sig_no)
{	
	int i;
	unsigned int bit_mask=BIT_OFF;
	pcb_t* p = NULL;
	pcb_t* ipc = NULL;	
	ipc_t* comm = NULL;

	if(sig_no < 0 || sig_no >= PROC_SZ) return -2;
	p = get_proc(pid);
	if(!p) return -1;


	/* make mask to toggle on signal bit */
	bit_mask=BIT_ON;
	for(i=0 ; i<sig_no ; i++)
		bit_mask *= 2;

	bit_mask &= p->sig_install_mask;

	/* signal not installed, target process will not receive signal */
	if(!bit_mask)
		return;

	/* check if proc is sleeping */
	if(p->state == SLEEP_STATE)
		wake_early(p);

	/* check if proc is waiting for kbd */
	if(p->state == BLOCK_ON_DEV_STATE)
	{
		kbd_dequeue();		
		p->rc = -128;
	}

	/* check if proc is blocked on ipc_recv or ipc_send */
	if(p->state == BLOCK_ON_RECV_STATE || p->state == BLOCK_ON_SEND_STATE) 
	{
		comm = (ipc_t*) p->ptr;
			
		switch(p->state) 
		{
			case BLOCK_ON_RECV_STATE:
				/* check if ipc_recv is receive any */
				if(comm && *(comm->pid_ptr))
				{
					ipc = get_proc(*(comm->pid_ptr));
					p = unblock(&(ipc->blocked_receivers), pid);
				}

				break;
			case BLOCK_ON_SEND_STATE:
				/* check if ipc_recv is receive any */
				if(comm && comm->pid)
				{
					ipc = get_proc(comm->pid);
					p = unblock(&(ipc->blocked_senders), pid);
				}

				break;
		}

		p->state = READY_STATE;
		p->rc = -128;
		ready(p);
	}

	/* check if proc is blocked waiting on a signal */
	if(p->state == BLOCK_ON_SIG_STATE)
	{
		p->state == READY_STATE;

		/* set proc rc as the signal which unblocked it */
		p->rc = sig_no;
		ready(p);
	}

	/* enable proc target_mask */
	p->sig_pend_mask |= bit_mask;

	return SIG_SUCCESS;
}

/*
* sigcease
*
* @desc:	set old ignore mask for proc
*
* @param:	p		proc for settting the old ignore mask
*		oim		old ignore mask
*/
void sigcease(pcb_t *p, unsigned int oim)
{
	int sig_no = -1,i;
	unsigned int bit_mask = ~(p->sig_ignore_mask);

	/* get signal number */
	while(bit_mask)
	{
		sig_no++;
		bit_mask >>= 1;
	}

	if(sig_no == -1) return;	

	/* toggle on all bits for same or lower level signals */
	bit_mask = BIT_ON;
	for(i=0 ; i<sig_no ; i++)
	{
		bit_mask <<= 1;
		bit_mask |= BIT_ON;	
	}

	//kprintf("sigcease\tbit_mask: %d	oim: %d\n", bit_mask, ~oim);

	bit_mask -= ~oim;
	p->sig_ignore_mask |= bit_mask;	
}

/*
* puts_sig_mask
*
* @desc:	output all signal masks for all valid proc
*/
void puts_sig_mask()
{
	int i;

	for(i=0 ; i<PROC_SZ ; i++) 
	{
		if(proc_table[i].sig_install_mask)
			kprintf("pid %d: sig_install %d\n", i, proc_table[i].sig_install_mask);

		if(proc_table[i].sig_pend_mask)
			kprintf("pid %d: sig_pend %d\n", i, proc_table[i].sig_pend_mask);
	}
}


/*
* puts_sig_table
*
* @desc:	output all sig_no handler address of user proc to console
*/
void puts_sig_table(pcb_t *p)
{
	int i;

	kprintf("sig: ");
	for(i=0 ; i<SIG_SZ ; i++)
	{
		if(p->sig_table[i])
			kprintf("%d ", p->sig_table[i]);
	}
	kprintf("\n");
}
