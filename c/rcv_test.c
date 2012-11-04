/* snd_test.c : ipc_send test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>


/* test driver */
extern void rcvtest_proc1(void);
extern void rcvtest_proc2(void);
extern void rcvtest_proc3(void);

/*
* test_root
*
* @desc:	executes the testroot process
* @note:	this process executes the process management test cases
*/	
void rcvtest_root(void)
{
	unsigned int child_pid[3], n=2000, byte,i,pid,dst;
	unsigned int *ptr=&dst;
	char buffer[10];

	kprintf("----------------------------------------------------------------------------\n");
	kprintf("proc\t\tstate\t\t\t\tsize\t\t\tdest\n");
	kprintf("----------------------------------------------------------------------------\n");
	pid = sysgetpid();
	sprintf(buffer, "%d", n);

#ifdef RECV_POSITIVE_TEST
	child_pid[0] = syscreate(&rcvtest_proc1, PROC_STACK);
	child_pid[1] = syscreate(&rcvtest_proc2, PROC_STACK);
	child_pid[2] = syscreate(&rcvtest_proc3, PROC_STACK);

	syssleep(1000);
	for(i=0 ; i<3 ; i++)
		byte = syssend(child_pid[i], buffer, strlen(buffer));	

	syssleep(1000);
	byte=4;
	dst=child_pid[1];
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);


	syssleep(1000);
	byte=1;
	dst=child_pid[0];
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);


#elif defined RECV_NEGATIVE_TEST
	child_pid[0] = syscreate(&rcvtest_proc1, PROC_STACK);
	syssend(child_pid[0], buffer, strlen(buffer));
	
	syssleep(1000);
	byte=4;
	dst=9;
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);


	syssleep(1000);
	dst=child_pid[0];
	byte=4;
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#endif

	/* eventual loop for send test root */
	for(;;);
}

/*
* test_proc1
*
* @desc:	executes the testproc process
*/
void rcvtest_proc1(void)
{
	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	
	pid = sysgetpid();

	byte = sysrecv(ptr, buffer, byte);

#ifdef RECV_POSITIVE_TEST
	n = 2000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = syssend(*ptr, buffer, strlen(buffer));
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#elif defined RECV_NEGATIVE_TEST
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
void rcvtest_proc2(void)
{
#ifdef RECV_POSITIVE_TEST
	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	

	pid = sysgetpid();
	byte = sysrecv(ptr, buffer, byte);

	n = 2000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = syssend(*ptr, buffer, strlen(buffer));
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#endif

	for(;;);
}

/*
* test_proc3
*
* @desc:	executes the testproc process
*/
void rcvtest_proc3(void)
{
#ifdef RECV_POSITIVE_TEST
	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	

	pid = sysgetpid();
	byte = sysrecv(ptr, buffer, byte);

	n = 2000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = syssend(*ptr, buffer, strlen(buffer));
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
#endif

	for(;;);
}



