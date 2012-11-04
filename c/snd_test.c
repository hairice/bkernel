/* test.c : Test Driver for Xeros
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>


/* test driver */
extern void proc1(void);
extern void proc2(void);
extern void proc3(void);

/*
* test_root
*
* @desc:	executes the testroot process
* @note:	this process executes the process management test cases
*/	
void snd_root(void)
{
	int child_pid[3], n=2000, byte,i,pid;
	char buffer[10];

	kprintf("----------------------------------------------------------------------------\n");
	kprintf("proc\t\tstate\t\t\t\tsize\t\t\tdest\n");
	kprintf("----------------------------------------------------------------------------\n");
	pid = sysgetpid();

#ifdef SEND_POSITIVE_TEST
	child_pid[0] = syscreate(&proc1, PROC_STACK);
	child_pid[1] = syscreate(&proc2, PROC_STACK);
	child_pid[2] = syscreate(&proc3, PROC_STACK);

	syssleep(1000);
	sprintf(buffer, "%d", n);
	for(i=0 ; i<3 ; i++)
		byte = syssend(child_pid[i], buffer, strlen(buffer));	


	syssleep(1000);	
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), child_pid[1]);
	byte = syssend(child_pid[1], buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, child_pid[1]);

	syssleep(1000);	
	n *= 1000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), child_pid[2]);
	byte = syssend(child_pid[2], buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, child_pid[2]);
#else 
#ifdef SEND_NEGATIVE_TEST
	syssleep(1000);	
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), pid);
	byte = syssend(pid, buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, pid);
#endif
#endif

	/* eventual loop for send test root */
	for(;;);
}

/*
* test_proc1
*
* @desc:	executes the testproc process
*/
void proc1(void)
{
#ifdef SEND_POSITIVE_TEST
	unsigned int byte=4,n=0,pid;
	unsigned int *ptr = &n;
	unsigned char buffer[4];	

	pid = sysgetpid();
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
#endif

	for(;;);
}

/*
* test_proc2
*
* @desc:	executes the testproc process
*/
void proc2(void)
{
#ifdef SEND_POSITIVE_TEST
	unsigned int byte=4,n=0,pid;
	unsigned int *ptr = &n;
	unsigned char buffer[4];	

	pid = sysgetpid();
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
#endif

	for(;;);
}

/*
* test_proc3
*
* @desc:	executes the testproc process
*/
void proc3(void)
{
#ifdef SEND_POSITIVE_TEST
	unsigned int byte=4,n=0,pid;
	unsigned int *ptr = &n;
	unsigned char buffer[4];	

	pid = sysgetpid();
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
#endif

	for(;;);
}



