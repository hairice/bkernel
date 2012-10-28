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

	for(;;);
}

/*
* producer
*
* @desc:	executes the producer process
*/
void producer ()
{
	unsigned char *buffer = "This is a producer message";	
	kprintf("%s\n", buffer);

	syssend(8, buffer, 22);
	kprintf("producer back\n");

	for(;;);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void consumer ()
{
	unsigned char *buffer = "This is a consumer message";
	kprintf("%s\n", buffer);
	
	syssend(7, buffer, 22);
	kprintf("consumer back\n");

	for(;;);
}


