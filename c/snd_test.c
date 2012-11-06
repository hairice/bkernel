/* snd_test.c : ipc_send test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

static Bool result[2];

/* test driver */
extern void sndtest_proc1(void);
extern void sndtest_proc2(void);
extern void sndtest_proc3(void);

/*
* test_root
*
* @desc:	executes the testroot process
* @note:	this process executes the process management test cases
*/	
void sndtest_root(void)
{
	int child_pid[3], n=2000, byte,i,pid;
	char buffer[10];

	kprintf("----------------------------------------------------------------------------\n");
	kprintf("proc\t\tstate\t\t\t\tsize\t\t\tdest\n");
	kprintf("----------------------------------------------------------------------------\n");
	pid = sysgetpid();


#ifdef SEND_POSITIVE_TEST

	child_pid[0] = syscreate(&sndtest_proc1, PROC_STACK);
	child_pid[1] = syscreate(&sndtest_proc2, PROC_STACK);
	child_pid[2] = syscreate(&sndtest_proc3, PROC_STACK);


	syssleep(1000);

	/* initial ipc_send to pass root pid to all child processes */
	sprintf(buffer, "%d", n);
	for(i=0 ; i<3 ; i++)
		byte = syssend(child_pid[i], buffer, strlen(buffer));	


	/*  
	* @test: 	ipc_send_pos_1
	*
	* @desc:	the ipc_sender is able to unblock a blocked receiver from the middle of its blocked_receivers queue
	*
	* @outcome:	4 bytes of data is transferred from the sender to the receiver
	*/
	syssleep(1000);	
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), child_pid[1]);
	byte = syssend(child_pid[1], buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, child_pid[1]);

	if(byte == 4)
		result[0] = TRUE;
	else
		result[0] = FALSE;	


	/*  
	* @test: 	ipc_send_pos_2
	*
	* @desc:	the ipc_sender is able to unblock a blocked receiver when the sender is transmissing less data than the receiver can handle
	*
	* @outcome:	3 bytes of data is transferred from the sender to the receiver
	*/
	syssleep(1000);	
	n /= 10;	/* send only 3 bytes of data */
	sprintf(buffer, "%d", n);
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), child_pid[2]);
	byte = syssend(child_pid[2], buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, child_pid[2]);

	if(byte == 3)
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
					kprintf("ipc_send_pos_1\tpass\t\t4 bytes have been sent\n");
				else
					kprintf("ipc_send_pos_1\tfail\t\tdid not send 4 bytes\n");
				break;

			case 1:
				if(result[1] == TRUE)
					kprintf("ipc_send_pos_2\tpass\t\t3 bytes have been sent\n");
				else
					kprintf("ipc_send_pos_2\tfail\t\tdid not sent 3 bytes\n");
				break;		
		}
	}


#elif defined SEND_NEGATIVE_TEST

	syssleep(1000);	

	/*  
	* @test: 	ipc_send_neg_1
	*
	* @desc:	the ipc_sender is unablet to send an ipc message to itself
	*
	* @outcome:	-2 is returned from the syssend() system call
	*/
	kprintf("[p%d]\t\t[blocked_send]\t\t\t[%d bytes]\t\t[p%d]\n", pid, strlen(buffer), pid);
	byte = syssend(pid, buffer, strlen(buffer));	
	kprintf("[p%d]\t\t[unblocked_send]\t\t[%d bytes]\t\t[p%d]\n", pid, byte, pid);

	if(byte == -2)
		result[0] = TRUE;	
	else
		result[0] = FALSE;	


	/* output test results */
	syssleep(1000);

	kprintf("\n\ntest\t\tresult\t\tcomment\n");
	kprintf("-----------------------------------------------------------\n");

	if(result[0] == TRUE)	
		kprintf("ipc_send_neg_1\tpass\t\t-2 was returned\n");
	else
		kprintf("ipc_send_neg_1\tpass\t\t-2 was not returned\n");

#endif

	/* eventual loop for send test root */
	for(;;);
}

/*
* test_proc1
*
* @desc:	executes the testproc process
*/
void sndtest_proc1(void)
{
#ifdef SEND_POSITIVE_TEST

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
* test_proc2
*
* @desc:	executes the testproc process
*/
void sndtest_proc2(void)
{
#ifdef SEND_POSITIVE_TEST

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
* test_proc3
*
* @desc:	executes the testproc process
*/
void sndtest_proc3(void)
{
#ifdef SEND_POSITIVE_TEST

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
