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
	int i=0;

	// Assignment 1: 	Cycle 12 iterations 
	//			print "Happy ", sysyield()	
	for(i; i<12; i++)
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
	int i=0;

	// Assignment 1: 	Cycle 15 iterations 
	//			print "New Year", sysyield()	
	for(i; i<15; i++) 
	{
		kprintf("New Year\n");
		sysyield();
	}

	sysstop();
}
