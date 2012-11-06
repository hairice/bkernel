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
	int child_pid[4], n=2000, byte=5,pid,tmp;
	unsigned int *ptr;
	char buffer[5], console[75];

	pid=sysgetpid();

	child_pid[0]=syscreate(&proc1, PROC_STACK);
	sprintf(console, "[p%d]\t\t[created]\t\t[p%d]\0", pid, child_pid[0]);
	sysputs(console);
	tmp = child_pid[0];
	
	child_pid[1]=syscreate(&proc2, PROC_STACK);
	sprintf(console, "[p%d]\t\t[created]\t\t[p%d]\0", pid, child_pid[1]);
	sysputs(console);

	child_pid[2]=syscreate(&proc3, PROC_STACK);
	sprintf(console, "[p%d]\t\t[created]\t\t[p%d]\0", pid, child_pid[2]);
	sysputs(console);

	child_pid[3]=syscreate(&proc4, PROC_STACK);
	sprintf(console, "[p%d]\t\t[created]\t\t[p%d]\0", pid, child_pid[3]);
	sysputs(console);


	sprintf(console, "\n--------------------------------------------\n\0");
	sysputs(console);

	syssleep(4000);


	/* send the third created process a message to sleep for 10 seconds */
	n=10000;
	sprintf(buffer, "%d", n);
	syssend(child_pid[2], buffer, strlen(buffer));

	/* send the fourth created process a message to sleep for 7 seconds */
	n=7000;
	sprintf(buffer, "%d", n);
	syssend(child_pid[1], buffer, strlen(buffer));

	/* send the third created process a message to sleep for 20 seconds */
	n=20000;
	sprintf(buffer, "%d", n);
	syssend(tmp, buffer, strlen(buffer));

	/* send the third created process a message to sleep for 27 seconds */
	n=27000;
	sprintf(buffer, "%d", n);
	syssend(child_pid[3], buffer, strlen(buffer));

	/* attempt to receive a message from the fourth created process */
	ptr = &(child_pid[3]);
	memset (buffer,'0',byte);
	byte = sysrecv(ptr, buffer, byte);
	sprintf(console, "[p%d]\t\t[received_msg]\t\t[ret_status %d]\0", pid, byte);
	sysputs(console);
	
	
	/* attempt to send a message to the third created process */
	n=1000;
	sprintf(buffer, "%d", n);
	byte = syssend(child_pid[2], buffer, strlen(buffer));
	sprintf(console, "[p%d]\t\t[sent_msg]\t\t[ret_status %d]\0", pid, byte);
	sysputs(console);


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
	unsigned int byte=5,dst=0,n, pid;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];
	char *console[50];	

	pid=sysgetpid();

	syssleep(5000);
	sprintf(console, "[p%d]\t\t[wake]\0", pid);
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "[p%d]\t\t[received_msg]\t\t[sleep %d ms]\0", pid, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "[p%d]\t\t[exiting]\t\t[sleep_ended]\0", pid);
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc2 ()
{
	unsigned int byte=4,dst=0,n,pid;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	

	pid=sysgetpid();


	syssleep(5000);
	sprintf(console, "[p%d]\t\t[wake]\0", pid);
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "[p%d]\t\t[received_msg]\t\t[sleep %d ms]\0", pid, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "[p%d]\t\t[exiting]\t\t[sleep_ended]\0", pid);
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc3 ()
{
	unsigned int byte=5,dst=0,n,pid;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	


	pid=sysgetpid();

	syssleep(5000);
	sprintf(console, "[p%d]\t\t[wake]\0", pid);
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "[p%d]\t\t[received_msg]\t\t[sleep %d ms]\0", pid, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "[p%d]\t\t[exiting]\t\t[sleep_ended]\0", pid);
	sysputs(console);
}

/*
* consumer
*
* @desc:	executes the consumer process
*/
void proc4 ()
{
	unsigned int byte=5,dst=0,n,pid;
	unsigned int *ptr = &dst;
	unsigned char buffer[5];	
	char *console[50];	

	pid=sysgetpid();

	syssleep(5000);
	sprintf(console, "[p%d]\t\t[wake]\0", pid);
	sysputs(console);

	byte = sysrecv(ptr, buffer, byte);
	n=atoi(buffer);
	sprintf(console, "[p%d]\t\t[received_msg]\t\t[sleep %d ms]\0", pid, n);
	sysputs(console);

	syssleep(n);
	sprintf(console, "[p%d]\t\t[exiting]\t\t[sleep_ended]\0", pid);
	sysputs(console);
}

