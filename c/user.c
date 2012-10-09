/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */

/*
* root
*
* @desc:	Executes the root process
*/
void root()
{
	syscreate(&producer, PROC_STACK);
	syscreate(&consumer, PROC_STACK);

	// Eventual loop after producer and consumer exits
	for(;;) 
		sysyield();
}

/*
* producer
*
* @desc:	Executes the producer process
*/
void producer ()
{
	int i;

	// Assignment 1: 	Cycle 12 iterations 
	//			print "Happy ", sysyield()	
	for(i=0; i<12; i++)
	{
		kprintf("Happy ");
		sysyield();
	}

	sysstop();
}

/*
* consumer
*
* @desc:	Executes the consumer process
*/
void consumer ()
{
	int i;

	// Assignment 1: 	Cycle 15 iterations 
	//			print "New Year", sysyield()	
	for(i=0; i<15; i++) 
	{
		kprintf("New Year\n");
		sysyield();
	}

	sysstop();
}
