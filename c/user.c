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
	int byte = syssend(3, snd_buffer, 3);
	kprintf("sent: %d bytes\n", byte);

	for(;;);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void consumer ()
{
	int i,byte;
	unsigned char rcv_buffer[4];	

	byte = sysrecv(2, rcv_buffer, 4);
	kprintf("recv: %d bytes\n", byte);

	i = atoi(rcv_buffer);
	kprintf("i: %d\n", i);

	for(;;);
}



