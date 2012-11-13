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
	return signal(p, sig_no);
}

/*
* signal
*
* @desc:	
*/
int signal(int pid, int sig_no)
{

	return 1;
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
