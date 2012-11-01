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

	unsigned int receiver = 2;
	unsigned char *snd_buffer = "2000";	
	int byte;

	syssleep(1500);
	kprintf("producer wake\n");
	byte = syssend(receiver, snd_buffer, 4);	
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
	unsigned int n,byte;
	unsigned int *sender = &n;
	unsigned char rcv_buffer[4];	

	syssleep(1000);
	kprintf("consumer wake\n");
	byte = sysrecv(3, rcv_buffer, 4);
	kprintf("recv: %d bytes\n", byte);

	n = atoi(rcv_buffer);
	kprintf("consumer sleep(%d)\n", n);
	syssleep(n);
	kprintf("consumer wake\n");
	
	for(;;);
}

