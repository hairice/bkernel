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
	syscreate(&producer, PROC_STACK);
	syscreate(&consumer, PROC_STACK);

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

	syssend(3, snd_buffer, 4);

	for(;;);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void consumer ()
{
	unsigned char *rcv_buffer[4];	

	sysrecv(2, &rcv_buffer, 4);

	for(;;);
}



