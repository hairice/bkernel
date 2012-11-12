/* signal.c : 	process signal handling (assignment 3)
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */



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
