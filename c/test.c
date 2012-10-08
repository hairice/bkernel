/* test.c : Test Driver for Xeros
 */

#include <xeroskernel.h>

int exit_proc=0;
int result[4];


/*
* testroot
*
* @desc:	Executes the testroot process
* @note:	This process executes the process management test cases
*/	
void testroot(void)
{
	int cnt = MAX_PROC-count()-1;

	// Test Case 3: Able to allocate the max number of processes and push to ready queue 
	//		while able to context switch between the root and child processes
	//		given that this process is the only process creation point in the code
	while(cnt > 0)
	{
		syscreate(&testproc, PROC_STACK);
		cnt=MAX_PROC-count()-1;
	}

	if(count() == MAX_PROC-1)
	{
		result[2]=1;
		kprintf("TC3 [Pass]: Pushed 31 pcb blocks onto ready queue\n");
	}
	else
	{
		result[2]=0;
		kprintf("TC3 [Fail]: Pushed less than 31 pcb block onto ready queue\n");
	}

	// Test Case 4: Able to reclaim the process blocks and push onto the stop queue,
	//		while able to context switch between the root and child processes,
	//		given that this process is the only process creation point in the code
	exit_proc=1;
	cnt = MAX_PROC-count()-1;
	while(cnt < MAX_PROC-1)
	{
		sysyield();
		cnt=MAX_PROC-count()-1;
	}

	if(cnt == MAX_PROC-1)	
	{
		result[3]=1;
		kprintf("TC4 [Pass]: Restored 31 pcb blocks onto stop queue\n");
	}
	else
	{
		result[3]=1;
		kprintf("TC4 [Fail]: Restored less than 31 pcb block onto stop queue\n");
	}

	sysstop();
}

/*
* testproc
*
* @desc:	Executes the testproc process
*/
void testproc(void)
{
	for(;;)
	{
		if(exit_proc) break;
		sysyield();
	}

	sysstop();
}

/*
* testdriver
*
* @desc:	Executes the test root process
*/
void testdriver(void) 
{
	int *blka, *blkb, *blkc, *blkd;
	int total_mem=0;

	
#ifdef	MEM_TEST
	total_mem = kmemtotalsize();
	// Test Case 1: 
	// Attempt to allocate memory that is bigger than any available space
	blka = kmalloc(kmemhdsize()/2);
	blkb = kmalloc(kmemhdsize()/2);
	blkc = kmalloc(kmemhdsize()/2);
	blkd = kmalloc(kmemhdsize()*100);

	if(!blkd) 
	{
		result[0]=1;
		kprintf("TC1 [Pass]: Able to degrade gracefully when attempting to allocate more memory than available\n");
	}
	else
	{
		result[0]=0;
		kprintf("TC1 [Fail]: Allocated memory in space that is larger than any available space in memory\n");
	}

	// Test Case 2: 
	// Free all allocated memory and does not free any memory that is NULL
	kfree(blkc);
	kfree(blka);
	kfree(blkb);
	kfree(blkd);

	// Compare total free memory space
	if(kmemtotalsize() == total_mem) 
	{
		result[1]=1;
		kprintf("TC2 [Pass]: Able to degrade gracefully when attempting to allocate more memory than available\n");
	}
	else
	{
		result[1]=0;
		kprintf("TC2 [Fail]: Allocated memory in space that is larger than any available space in memory\n");
	}
	total_mem = kmemtotalsize();
#endif

#ifdef	PROC_TEST
	create(&testroot, PROC_STACK);
	contextinit();
	dispatch();
#endif	

	testresult();
}


/*
* testresult
*
* @desc:	Print out test result summary
*/
void testresult(void)
{
	int i;

	kprintf("\n----------------------------------------------------\n");
	kprintf("XEROS Test Summary \n");
	kprintf("----------------------------------------------------\n");
	for(i=0; i<4 ; i++) 
	{
		kprintf("Test Case [%d]\t\t", i+1);
		
		if(i<2)
			kprintf("MEM_TEST\t\t");
		else
			kprintf("PROC_TEST\t\t");

		if(result[i])
			kprintf("Pass\n");
		else
			kprintf("Fail\n");
	}
}
