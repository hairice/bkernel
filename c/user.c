/* user.c : User processes
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

/* Your code goes here */


/*
* idleproc
*
* @desc:	executes the idle process
*/
void idleproc ()
{
	for(;;);
}

/*
* root
*
* @desc:	executes the root process
*/
void root()
{
	syscreate(&consumer, PROC_STACK);
	syscreate(&producer, PROC_STACK);

	/* eventual loop for root */	
	for(;;);
}

/*
* producer
*
* @desc:	executes the producer process
*/
void producer ()
{
	unsigned char *snd_buffer = "2000";	

	syssend(2, snd_buffer, 4);

	for(;;);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void consumer ()
{
	int i;
	unsigned char rcv_buffer[4];	

	sysrecv(3, rcv_buffer, 4);
	i = atoi(rcv_buffer);
	kprintf("i: %d\n", i);

	for(;;);
}



