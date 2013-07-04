#include <xeroskernel.h>

static Bool result[2];
extern void rcvtest_proc1(void);
extern void rcvtest_proc2(void);
extern void rcvtest_proc3(void);


/*
* rcvtest_root
*
* @desc:	executes the testroot process
*
* @note:	this process executes the ipc_recv test cases, 
*		RECV_POSITIVE_TEST or REC_NEGATIVE_TEST needs to be uncommented in order to execute the tests
*		
*		this test includes 2 RECV_POSITIVE_TEST tests and 2 RECV_NEGATIVE_TEST tests
*/	
void rcvtest_root(void)
{
#ifdef RECV_POSITIVE_TEST
	unsigned int child_pid[3], n=2000, byte,i,pid,dst;
	unsigned int *ptr=&dst;
	char buffer[10], rcv_buffer[1];

	kprintf("----------------------------------------------------------------------------\n");
	kprintf("proc\t\tstate\t\t\t\tsize\t\t\tdest\n");
	kprintf("----------------------------------------------------------------------------\n");
	pid = sysgetpid();
	sprintf(buffer, "%d", n);

	child_pid[0] = syscreate(&rcvtest_proc1, PROC_STACK);
	child_pid[1] = syscreate(&rcvtest_proc2, PROC_STACK);
	child_pid[2] = syscreate(&rcvtest_proc3, PROC_STACK);

	/* initial ipc_send to pass root pid to all child processes */
	syssleep(1000);
	for(i=0 ; i<3 ; i++)
		byte = syssend(child_pid[i], buffer, strlen(buffer));	

	/*  
	* @test: 	ipc_recv_pos_1
	*
	* @desc:	the ipc_receiver is able to unblock a blocked sender from the middle of its blocked_senders queue
	*
	* @outcome:	4 bytes of data is transferred from the sender to the receiver
	*/
	syssleep(1000);
	byte=4;
	dst=child_pid[1];
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]*\t\t[unblocked_receive]\t\t[%d bytes]*\t\t[p%d]\n", pid, byte, *ptr);

	if(byte == 4)
		result[0] = TRUE;
	else
		result[0] = FALSE;

	/*  
	* @test: 	ipc_recv_pos_2
	*
	* @desc:	the ipc_receiver is able to unblock a blocked sender when the sender is transmissing more data than the receiver can handle
	*
	* @outcome:	1 byte of data is transferred from the sender to the receiver
	*/
	syssleep(1000);
	byte=1;			/* receive only 1 byte of data */
	dst=child_pid[0];
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, rcv_buffer, byte);
	kprintf("[p%d]*\t\t[unblocked_receive]\t\t[%d bytes]*\t\t[p%d]\n", pid, byte, *ptr);

	if(byte == 1)
		result[1] = TRUE;
	else
		result[1] = FALSE;


	/* output test results */
	syssleep(2000);

	kprintf("\n\ntest\t\tresult\t\tcomment\n");
	kprintf("-----------------------------------------------------------\n");
	for(i=0 ; i<2 ; i++)
	{
		switch(i)
		{
			case 0:
				if(result[0] == TRUE)
					kprintf("ipc_recv_pos_1\tpass\t\t4 bytes have been received\n");
				else
					kprintf("ipc_recv_pos_1\tfail\t\tdid not receive 4 bytes\n");
				break;

			case 1:
				if(result[1] == TRUE)
					kprintf("ipc_recv_pos_2\tpass\t\t1 byte have been received\n");
				else
					kprintf("ipc_recv_pos_2\tfail\t\tdid not receive 1 byte\n");
				break;		
		}
	}

#elif defined RECV_NEGATIVE_TEST

	unsigned int child_pid[3], n=2000, byte,i,pid,dst;
	unsigned int *ptr=&dst;
	char buffer[10];

	kprintf("----------------------------------------------------------------------------\n");
	kprintf("proc\t\tstate\t\t\t\tsize\t\t\tdest\n");
	kprintf("----------------------------------------------------------------------------\n");
	pid = sysgetpid();
	sprintf(buffer, "%d", n);

	/* initial ipc_send to pass root pid to all child processes */
	child_pid[0] = syscreate(&rcvtest_proc1, PROC_STACK);
	syssend(child_pid[0], buffer, strlen(buffer));
	
	/*  
	* @test: 	ipc_recv_neg_1
	*
	* @desc:	the ipc_receiver is unable to receive from an invalid process pid sender
	*
	* @outcome:	-1 is returned from the sysrecv() system call
	*/
	syssleep(1000);
	byte=4;
	dst=9;
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]*\t\t[unblocked_receive]\t\t[%d bytes]*\t\t[p%d]\n", pid, byte, *ptr);

	if(byte == -1)
		result[0] = TRUE;
	else
		result[0] = FALSE;	

	/*  
	* @test: 	ipc_recv_neg_2
	*
	* @desc:	the ipc_receiver is unable to receive from a process who is currently blocked waiting to receive from this current process 
	*
	* @outcome:	-3 is returned from the sysrecv() system call
	*/
	syssleep(1000);
	dst=child_pid[0];
	byte=4;
	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]*\t\t[unblocked_receive]\t\t[%d bytes]*\t\t[p%d]\n", pid, byte, *ptr);

	if(byte == -3)
		result[1] = TRUE;
	else
		result[1] = FALSE;


	/* output test results */
	syssleep(1000);

	kprintf("\n\ntest\t\tresult\t\tcomment\n");
	kprintf("-----------------------------------------------------------\n");
	for(i=0 ; i<2 ; i++)
	{
		switch(i)
		{
			case 0:
				if(result[0] == TRUE)
					kprintf("ipc_recv_neg_1\tpass\t\t-1 was returned\n");
				else
					kprintf("ipc_recv_neg_1\tfail\t\t-1 was not returned\n");
				break;

			case 1:
				if(result[1] == TRUE)
					kprintf("ipc_recv_neg_2\tpass\t\t-3 was returned\n");
				else
					kprintf("ipc_recv_neg_2\tfail\t\t-3 was not returned\n");
				break;		
		}
	}

#endif

	/* eventual loop for receive test root */
	for(;;);
}

/*
* rcvtest_proc1
*
* @desc:	executes a child test proc
*/
void rcvtest_proc1(void)
{
#ifdef RECV_POSITIVE_TEST

	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	
	pid = sysgetpid();

	/* initial ipc_recv to get the root's pid */
	byte = sysrecv(ptr, buffer, byte);

	n = 2000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = syssend(*ptr, buffer, strlen(buffer));
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#elif defined RECV_NEGATIVE_TEST

	unsigned int byte=4,dst=0,pid;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	
	pid = sysgetpid();

	/* initial ipc_recv to get the root's pid */
	byte = sysrecv(ptr, buffer, byte);

	kprintf("[p%d]\t\t[blocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = sysrecv(ptr, buffer, byte);
	kprintf("[p%d]\t\t[unblocked_receive]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#endif

	for(;;);
}

/*
* rcvtest_proc2
*
* @desc:	executes a child test proc
*/
void rcvtest_proc2(void)
{

#ifdef RECV_POSITIVE_TEST

	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	

	pid = sysgetpid();

	/* initial ipc_recv to get the root's pid */
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
* rcvtest_proc3
*
* @desc:	executes a child test proc
*/
void rcvtest_proc3(void)
{

#ifdef RECV_POSITIVE_TEST

	unsigned int byte=4,dst=0,pid,n;
	unsigned int *ptr = &dst;
	unsigned char buffer[10];	

	pid = sysgetpid();

	/* initial ipc_recv to get the root's pid */
	byte = sysrecv(ptr, buffer, byte);

	n = 2000;
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);
	byte = syssend(*ptr, buffer, strlen(buffer));
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, *ptr);

#endif


	for(;;);
}
