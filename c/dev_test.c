/* dev_test.c : device kbd test cases
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <xeroskernel.h>


/*
* devtest_root
*
* @desc:	executes the testroot process
*
* @note:	this process executes the device test cases, 
*		SEND_POSITIVE_TEST or SEND_NEGATIVE_TEST needs to be uncommented in order to execute the tests
*		
*		this test includes 2 SEND_POSITIVE_TEST tests and 2 SEND_NEGATIVE_TEST tests
*/	


unsigned char str1[40];
unsigned char str2[4];
void devtest_root(void)
{
	int fd,rc;
	unsigned int buffer[3];
	unsigned int byte=3;

	
	strncpy(buffer, "", byte);
	/*  
	* @test: 	dev_test_1
	*
	* @desc:	An invalid device major number is passed into sysopen
	*
	* @outcome:	-1 is returned from sysopen
	*/
	fd = sysopen(10);

	kprintf("sysopen: %d\n", fd);
	if(fd == -1)
	{		
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_test_1\tpass\t\t-1 is returned\n\n");
	}


	/*  
	* @test: 	dev_test_2
	*
	* @desc:	An invalid file descriptor is passed into syswrite
	*
	* @outcome:	-1 is returned from syswrite
	*/
	rc = syswrite(10, buffer, byte);

	kprintf("syswrite: %d\n", rc);
	if(fd == -1)
	{		
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_test_2\tpass\t\t-1 is returned\n\n");
	}


	/*  
	* @test: 	dev_test_3
	*
	* @desc:	An invalid command in passed into the sysioctl
	*
	* @outcome:	-1 is returned from sysioctl
	*/
	fd = sysopen(1);

	if(fd != -1)
		rc = sysioctl(fd, 1000, 10);

	kprintf("sysioctl: %d\n", rc);
	if(rc == -1)
	{		
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_test_3\tpass\t\t-1 is returned\n\n");
	}

	/*  
	* @test: 	dev_test_4
	*
	* @desc:	A sysread operation is made on kbd, where an user buffer with length smaller than the internal kbd buffer is passed in
	*
	* @outcome:	After three characters are typed, three characters are put in user buffer and 3 is returned from sysread
	*/
	if(fd != -1)
		rc = sysread(fd, buffer, byte);

	kprintf("sysread: %d %s\n", rc, buffer);
	if(strlen(buffer) == byte && rc == byte)
	{		
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_test_4\tpass\t\t3 is returned\n\n");
	}


	/*  
	* @test: 	dev_test_5
	*
	* @desc:	After setting EOF as 'a', a sysread operation is made on kbd, and 'a' is immediately pressed
	*
	* @outcome:	0 is returned from sysread after eof is pressed
	*/
	strcpy(buffer, "");
	sysioctl(fd, SET_EOF, 97);

	if(fd != -1)
		rc = sysread(fd, buffer, byte);
	
	kprintf("sysread: %d %s\n", rc, buffer);
	if(rc == 0)
	{		
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_test_5\tpass\t\t0 is returned, eof detected\n");
	}
}
