/* signal.c : 	process signal handling (assignment 3)
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */
extern long freemem;


/*
* sigtramp
*
* @desc:	
*/
void sigtramp(void (*handler)(void *), void *cntx, void *osp)
{
	handler(cntx);
	sigreturn(osp);
}


/*
* siginstall
*
* @desc:	
*/
int siginstall(pcb_t *p, int signal, void (*new_handler)(void *), void (**old_handler)(void *))
{
	int tmp;

	if(signal < 0 || signal >= SIG_SZ) return ERR_SIG_NO;
	if(new_handler > freemem) return ERR_SIG_HANDLER;

	*old_handler = p->sig_table[signal];
	p->sig_table[signal] = new_handler;

	/* save current state of sig_accept_mask */
	tmp = p->sig_accept_mask;

	/* shift and toggle signal mask bit */
	p->sig_accept_mask >>= signal;
	p->sig_accept_mask |= SIG_ON;
	p->sig_accept_mask <<= signal;

	/* return shifted bits */
	p->sig_accept_mask |= tmp;

	return SIG_SUCCESS;
}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																									

/*
* sigkill
*
* @desc:	
*/
int sigkill(int pid, int signal)
{
	unsigned int old_target_mask, new_target_mask = SIG_OFF;
	pcb_t* p = NULL;

	if(signal < 0 || signal >= PROC_SZ) return ERR_SIG_NO;

	p = get_proc(pid);
	if(!p) return ERR_SIG_TARGET_PROC;

	new_target_mask >>= signal;
	new_target_mask |= SIG_ON;
	new_target_mask <<= signal;

	new_target_mask &= p->sig_accept_mask;

	/* enable proc target_mask */
	if(new_target_mask)
		p->sig_target_mask |= new_target_mask;

	return SIG_SUCCESS;
}


/*
* puts_sig_table
*
* @desc:	output all signal handler address of user proc to console
*/
void puts_sig_table(pcb_t *p)
{
	int i;

	kprintf("sig: ");
	for(i=0 ; i<PROC_SZ ; i++)
		kprintf("%d ", p->sig_table[i]);
	kprintf("\n");
}
