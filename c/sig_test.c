/* sig_test.c : signal test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>

extern void sigtest_proc1(void);
Bool result[5];
Bool priority_test = FALSE;
Bool kill_test = FALSE;


static Bool lock1 = FALSE;
static Bool lock2 = FALSE;
static Bool lock3 = FALSE;
static Bool lock4 = FALSE;


/*
* sigtest_handler1
*
* @desc:	
*/
void sigtest_handler1(void *arg)
{
	kprintf("handler1 start\n");

	for(;;)
	{
		if(lock1)
		{
			result[4] = TRUE;
			priority_test = TRUE;
			break;
		}
	}

	kprintf("handler1 end\n");
}

/*
* sigtest_handler2
*
* @desc:	
*/
void sigtest_handler2(void *arg)
{
	kprintf("handler2 start\n");

	for(;;)
	{
		if(lock2)
		{
			lock1=TRUE;
			result[3] = TRUE;
			break;
		}
	}

	kprintf("handler2 end\n");
}



/*
* sigtest_handler6
*
* @desc:	
*/
void sigtest_handler6(void *arg)
{
	kprintf("handler6 start\n");

	for(;;)
	{
		if(lock3)
		{
			lock2=TRUE;
			result[2] = TRUE;
			break;
		}
	}

	kprintf("handler6 end\n");
}

/*
* sigtest_handler12
*
* @desc:	
*/
void sigtest_handler12(void *arg)
{
	kprintf("handler12 start\n");
	kill_test = TRUE;
	kprintf("handler12 end\n");
}

/*
* sigtest_handler15
*
* @desc:	
*/
void sigtest_handler15(void *arg)
{
	kprintf("handler15 start\n");

	for(;;)
	{
		if(lock4)
		{
			lock3=TRUE;
			result[1] = TRUE;
			break;
		}
	}

	kprintf("handler15 end\n");
}

/*
* sigtest_handler27
*
* @desc:	
*/
void sigtest_handler27(void *arg)
{
	kprintf("handler27 start\n");
	lock4 = TRUE;	
	result[0] = TRUE;
	kprintf("handler27 end\n");	
}

/*
* sigtest_root
*
* @desc:	executes the testroot process
*
* @note:	this process executes the signal test cases, 
*		SEND_POSITIVE_TEST or SEND_NEGATIVE_TEST needs to be uncommented in order to execute the tests
*		
*		this test includes 2 SEND_POSITIVE_TEST tests and 2 SEND_NEGATIVE_TEST tests
*/	
void sigtest_root(void)
{
	unsigned char buffer[10];
	unsigned int byte=10;
	int rc,i;
	unsigned int dst=2,child_pid[3];
	unsigned int *ptr = &dst;
	void *old_handler = NULL;

	/* install handlers for signals 1, 2, 6, 15, and 27 */
	syssighandler(1, sigtest_handler1, &old_handler);
	syssighandler(2, sigtest_handler2, &old_handler);
	syssighandler(6, sigtest_handler6, &old_handler);
	syssighandler(12, sigtest_handler12, &old_handler);
	syssighandler(15, sigtest_handler15, &old_handler);
	syssighandler(27, sigtest_handler27, &old_handler);


	/* send pid to child proc */
	child_pid[1] = syscreate(&sigtest_proc1, PROC_STACK);
	sprintf(buffer, "%d", 0);
	syssend(child_pid[1], buffer, strlen(buffer));


#ifdef SIG_INSTALL_TEST
	/*  
	* @test: 	sig_install_test
	*
	* @desc:	A null handler is attempted for installing into signal 1
	*
	* @outcome:	-1 is returned from syssighandler
	*/
	rc = syssighandler(1, NULL, &old_handler);

	if(rc < 0)
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_1\tpass\t\tnull signal handler detected\n\n");
	}
#endif

#ifdef SIG_KILL_TEST
	/*  
	* @test: 	sig_kill_test
	*
	* @desc:	Root proc receives a syskill from child proc 1
	*
	* @outcome:	Root proc regains control from its signal stack
	*/
	while(!kill_test);

	if(kill_test)
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_2\tpass\t\tsignalled by signal 12\n\n");
	}
	else
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_2\tfail\t\tinvalid regain of control\n\n");
	}
#endif

#ifdef SIG_WAIT_TEST
	/*  
	* @test: 	sig_wait_test
	*
	* @desc:	Root proc is put in BLOCK_ON_SIG_STATE
	*
	* @outcome:	12 is returned from syssigwait
	*/
	rc = syssigwait();
	if(rc == 12)
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_3\tpass\t\tunblocked by signal 12\n\n");
	}
#endif

#ifdef SIG_PRIO_TEST
	/*  
	* @test: 	sig_prio_test
	*
	* @desc:	5 signal stacks in priority order are put on the user stack, and unrolls based on priority
	*
	* @outcome:	Root proc regains control
	*/
	while(!priority_test);

	for(i=0 ; i<5 ; i++)
	{
		if(!result[i])
		{
			kprintf("\ntest\t\tresult\t\tcomment\n");
			kprintf("-----------------------------------------------------------\n");
			kprintf("sig_test_4\tfail\t\tsignal stack unrolled order-of-order\n\n");
		}

		if(i == 4)
		{
			kprintf("\ntest\t\tresult\t\tcomment\n");
			kprintf("-----------------------------------------------------------\n");
			kprintf("sig_test_4\tpass\t\tsignal stack unrolled in priority order\n\n");
		}
	}
#endif

#ifdef SIG_SLEEP_TEST
	/*  
	* @test: 	sig_sleep_test
	*
	* @desc:	root proc is interrupted while in a 5 seconds sleep by child proc 1
	*
	* @outcome:	400 is returned
	*/
	rc = syssleep(5000);
	if(rc == 400)
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_5\tpass\t\tawaken early by 400 ticks\n\n");
	}
	else
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("sig_test_5\tfail\t\tincorrect rc or incorrect awake value\n\n");
	}
#endif

	for(;;);
}

/*
* sigtest_proc1
*
* @desc:	executes a child test proc
*/
void sigtest_proc1(void)
{
	unsigned int buffer[10];
	unsigned int byte=10;
	unsigned int dst=0;
	unsigned int *ptr = &dst;

	/* get root pid */
	byte = sysrecv(ptr, buffer, byte);

#ifdef SIG_KILL_TEST
	syssleep(1000);
	syskill(dst, 12);
#endif


#ifdef SIG_WAIT_TEST
	syssleep(1000);
	syskill(dst, 12);
#endif

#ifdef SIG_PRIO_TEST
	syssleep(1000);	
	syskill(dst, 1);
	syskill(dst, 2);
	syskill(dst, 6);
	syskill(dst, 15);
	syskill(dst, 27);
#endif

#ifdef SIG_SLEEP_TEST
	syssleep(1000);
	syskill(dst, 12);
#endif
}
