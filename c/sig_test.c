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

	/*  
	* @test: 	sig_handler_neg_1
	*
	* @desc:	
	*
	* @outcome:	
	*/
	rc = syssighandler(1, NULL, &old_handler);

	if(rc < 0)
		kprintf("pass1\n");


	/*  
	* @test: 	sig_kill_pos_1
	*
	* @desc:	
	*
	* @outcome:	
	*/
	while(!kill_test);

	kprintf("pass2\n");


	/*  
	* @test: 	sig_wait_pos_1
	*
	* @desc:	
	*
	* @outcome:	
	*/
	rc = syssigwait();
	if(rc == 12)
		kprintf("pass3\n");


	/*  
	* @test: 	sig_prio_pos_1
	*
	* @desc:	
	*
	* @outcome:	
	*/

	while(!priority_test);

	for(i=0 ; i<5 ; i++)
	{
		if(!result[i])
		{
			kprintf("fail4: %d\n", i);
			break;
		}

		if(i == 4)
			kprintf("pass4\n");
	}

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

	syssleep(1000);

	syskill(dst, 12);

	syssleep(1000);

	syskill(dst, 12);

	syssleep(1000);
	
	syskill(dst, 1);
	syskill(dst, 2);
	syskill(dst, 6);
	syskill(dst, 15);
	syskill(dst, 27);
}
