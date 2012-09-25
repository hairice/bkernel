/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */



void root()
{
	kprintf("hi from root, make producer!\n");
	syscreate(&producer, 2000);
	kprintf("hi from root, make consumer!\n");
	syscreate(&consumer, 2000);


	for(;;) 
	{
		kprintf("hi from root, work done!\n");
		sysyield();
	}
}

void producer ()
{
	int i=0;
	kprintf("hi from producer!\n");

	for(i; i<12; i++)
	{
		kprintf("Happy \n");
		sysyield();
	}

//	sysstop();
}


void consumer ()
{
	int i=0;
	kprintf("hi from consumer!\n");

	for(i; i<15; i++) 
	{
		kprintf("New Year\n");
		sysyield();
	}

//	sysstop();
}
