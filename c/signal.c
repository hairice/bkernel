/* signal.c : 	process signal handling (assignment 3)
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */
extern long freemem;


typedef struct sig_arg sig_arg_t;
struct sig_arg
{
	void (*handler)( void *);
        void *cntx;
     	void *osp;
	unsigned int rc;
	unsigned int sig_ignore_mask;
};


/*
* sigtramp
*
* @desc:	
*/
void sigtramp(void (*handler)(void *), void *cntx, void *osp)
{
	int *stack = (int *) osp;
	handler(cntx);
	sigreturn(osp);
}

/*
* sighigh
*
* @desc:	
*/
int sighigh(pcb_t *p)
{
	int sig_no=-1,i;
	unsigned int tmp_mask = p->sig_target_mask, bit_mask=SIG_ON;

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

	p->sig_target_mask &= ~bit_mask;
	return signal(p->pid, sig_no);
}

/*
* signal
*
* @desc:	
*/
int signal(int pid, int sig_no)
{	
	unsigned int mem;
	pcb_t *p = NULL;
	sig_arg_t *sig_args = NULL;
	context_frame_t *frame = NULL;


	if(sig_no < 0 || sig_no >= SIG_SZ) return ERR_SIG_NO;
	p = get_proc(pid);
	if(!p) return ERR_SIG_TARGET_PROC;

	// TODO: unblock proc


	/* get user proc stack pointer */
	mem = p->esp;

	/* setup sigtramp arguments */
	mem -= sizeof(sig_arg_t);
	sig_args = (sig_arg_t*) mem;
	//sig_args->rc = p->rc;
	sig_args->osp = p->esp;
	sig_args->cntx = p->esp;
	sig_args->handler = p->sig_table[sig_no];
	//sig_args->sig_ignore_mask = p->sig_ignore_mask;

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
* @desc:	
*/
int siginstall(pcb_t *p, int sig_no, void (*new_handler)(void *), void (**old_handler)(void *))
{
	unsigned int bit_mask=SIG_ON,i;

	if(sig_no < 0 || sig_no >= SIG_SZ) return ERR_SIG_NO;
	if(new_handler > freemem) return ERR_SIG_HANDLER;

	*old_handler = p->sig_table[sig_no];
	p->sig_table[sig_no] = new_handler;

	/* make mask to toggle on signal bit */
	for(i=0 ; i<sig_no ; i++)
		bit_mask *= 2;

	p->sig_accept_mask |= bit_mask;
	return SIG_SUCCESS;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									

/*
* sigkill
*
* @desc:	
*/
int sigkill(int pid, int sig_no)
{	
	int i;
	unsigned int bit_mask=SIG_OFF;
	pcb_t* p = NULL;

	if(sig_no < 0 || sig_no >= PROC_SZ) return ERR_SIG_NO;
	p = get_proc(pid);
	if(!p) return ERR_SIG_TARGET_PROC;

	/* check proc state if is in block_state */
	if(p->state == BLOCK_ON_SIG_STATE)
	{
		p->state == READY_STATE;

		/* set proc rc as the signal which unblocked it */
		p->rc = sig_no;
		ready(p);
	}

	/* make mask to toggle on signal bit */
	bit_mask=SIG_ON;
	for(i=0 ; i<sig_no ; i++)
		bit_mask *= 2;

	bit_mask &= p->sig_accept_mask;

	/* enable proc target_mask */
	if(bit_mask)
		p->sig_target_mask |= bit_mask;

	return SIG_SUCCESS;
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
	for(i=0 ; i<PROC_SZ ; i++)
		kprintf("%d ", p->sig_table[i]);
	kprintf("\n");
}
