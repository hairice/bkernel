/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */


/*
* root
*
* @desc:	Executes the root thread
*/
void root()
{
	kprintf("hi from root, make producer!\n");
	syscreate(&producer, 4096);
	kprintf("hi from root, make consumer!\n");
	syscreate(&consumer, 4096);

	for(;;) 
	{
		kprintf("hi from root, work done!\n");
		sysyield();
	}
}

/*
* producer
*
* @desc:	Executes the producer thread
*/
void producer ()
{
	int i=0;

	for(i; i<1; i++)
	{
		kprintf("Happy\n");
		sysyield();
	}

	sysstop();
}

/*
* consumer
*
* @desc:	Executes the consumer thread
*/
void consumer ()
{
	int i=0;

	for(i; i<1; i++) 
	{
		kprintf("New Year!\n");
		sysyield();
	}

	sysstop();
}
