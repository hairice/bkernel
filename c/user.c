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
	int pid[4], n=2000, byte=5,i;
	unsigned int *ptr;
	char buffer[5], console[50];

	pid[0]=syscreate(&proc1, PROC_STACK);
	//kprintf("created proc pid=%d\n", pid[0]);

	pid[1]=syscreate(&proc2, PROC_STACK);
	//kprintf("created proc pid=%d\n", pid[1]);

	pid[2]=syscreate(&proc3, PROC_STACK);
	//kprintf("created proc pid=%d\n", pid[2]);

	pid[3]=syscreate(&proc4, PROC_STACK);
	//kprintf("created proc pid=%d\n", pid[3]);

	syssleep(4000);


	n=10000;
	sprintf(buffer, "%d", n);

	sprintf(console, "send proc pid=%d to sleep for %d ms\n", pid[2], n);
	sysputs(&console);
	syssend(pid[2], buffer, strlen(buffer));

	n=7000;
	sprintf(buffer, "%d", n);
	sprintf(console, "send proc pid=%d to sleep for %d ms\n", pid[1], n);
	sysputs(&console);
	syssend(pid[1], buffer, strlen(buffer));

	n=20000;
	sprintf(buffer, "%d", n);
	sprintf(console, "send proc pid=%d to sleep for %d ms\n", pid[0], n);
	sysputs(&console);
	syssend(pid[0], buffer, strlen(buffer));

	n=27000;
	sprintf(buffer, "%d", n);
	sprintf(console, "send proc pid=%d to sleep for %d ms\n", pid[3], n);
	sysputs(&console);
	syssend(pid[3], buffer, strlen(buffer));

	ptr = &(pid[3]);
	byte = sysrecv(ptr, buffer, byte);
	sprintf(console, "root received msg from pid=%d byte=%d\n", pid[3], byte);
	sysputs(&console);
	
	n=1000;
	sprintf(buffer, "%d", n);
	byte = syssend(pid[2], buffer, strlen(buffer));
	sprintf(console, "send proc pid=%d to sleep for %d ms, ret=%d\n", pid[3], n, byte);
	sysputs(&console);
	/* eventual loop for root */	
	for(;;);
}

/*
* producer
*
* @desc:	executes the producer process
*/
void proc1 ()
{
	unsigned int byte=5,dst=0,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];
	char *console[50];	

	//kprintf("proc1 alive!\n");
	syssleep(5000);
	sprintf(console, "proc1 wake!\n");
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);

	sprintf(console, "proc1 received msg byte=%d, sleep=%d ms\n", byte, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "proc1 wake again! exiting\n");
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc2 ()
{
	unsigned int byte=4,dst=0,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	


	//kprintf("proc2 alive!\n");
	syssleep(5000);
	sprintf(console, "proc2 wake!\n");
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "proc2 received msg byte=%d, sleep=%d ms\n", byte, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "proc2 wake again! exiting\n");
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc3 ()
{
	unsigned int byte=5,dst=0,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	

	//kprintf("proc3 alive!\n");
	syssleep(5000);
	sprintf(console, "proc3 wake!\n");
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "proc3 received msg byte=%d, sleep=%d ms\n", byte, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "proc3 wake again! exiting\n");
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc4 ()
{
	unsigned int byte=5,dst=0,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	

	//kprintf("proc4 alive!\n");
	syssleep(5000);
	sprintf(console, "proc4 wake!\n");
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "proc4 received msg byte=%d, sleep=%d ms\n", byte, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "proc4 wake again! exiting\n");
	sysputs(console);
}

