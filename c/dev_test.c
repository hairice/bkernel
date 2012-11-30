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


#ifdef DEV_OPEN_TEST
	/*  
	* @test: 	dev_open_test
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
		kprintf("dev_open_test\tpass\t\t-1 is returned\n\n");
	}
	else
	{
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_open_test\tfail\t\t-1 is not returned\n\n");
	}
#endif

#ifdef DEV_WRITE_TEST
	/*  
	* @test: 	dev_write_test
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
		kprintf("dev_write_test\tpass\t\t-1 is returned\n\n");
	}
	else
	{
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_write_test\tfail\t\t-1 is not returned\n\n");
	}
#endif

#ifdef DEV_IOCTL_TEST
	/*  
	* @test: 	dev_ioctl_test
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
		kprintf("dev_ioctl_test\tpass\t\t-1 is returned\n\n");
	}
	else
	{
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_ioctl_test\tfail\t\t-1 is not returned\n\n");
	}
	
	sysclose(fd);
#endif

#ifdef DEV_READ_TEST
	/*  
	* @test: 	dev_read_test
	*
	* @desc:	A sysread operation is made on kbd, where an user buffer with length smaller than the internal kbd buffer is passed in
	*
	* @outcome:	After three characters are typed, three characters are put in user buffer and 3 is returned from sysread
	*/
	fd = sysopen(1);

	if(fd != -1)
		rc = sysread(fd, buffer, byte);

	kprintf("sysread: %d %s\n", rc, buffer);
	if(strlen(buffer) == byte && rc == byte)
	{		
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_read_test\tpass\t\t3 is returned\n\n");
	}
	else
	{
		kprintf("test\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_read_test\tfail\t\tbuffer does not contain 3 characters\n\n");
	}

	sysclose(fd);
#endif

#ifdef DEV_EOF_TEST
	/*  
	* @test: 	dev_eof_test
	*
	* @desc:	After setting EOF as 'a', a sysread operation is made on kbd, and 'a' is immediately pressed
	*
	* @outcome:	0 is returned from sysread after eof is pressed
	*/
	fd = sysopen(1);

	strcpy(buffer, "");
	sysioctl(fd, SET_EOF, 97);

	if(fd != -1)
		rc = sysread(fd, buffer, byte);
	
	kprintf("sysread: %d %s\n", rc, buffer);
	if(rc == 0)
	{		
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_eof_test\tpass\t\t0 is returned, eof detected\n");
	}
	else
	{
		kprintf("\ntest\t\tresult\t\tcomment\n");
		kprintf("-----------------------------------------------------------\n");
		kprintf("dev_eof_test\tfail\t\tincorrect eof returne value\n");
	}

	sysclose(fd);
#endif
}
