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
	* @test: 	dev_neg_1
	*
	* @desc:	An invalid device major number is passed into sysopen
	*
	* @outcome:	
	*/
	fd = sysopen(10);
	if(fd == -1)
		kprintf("pass1\n");


	/*  
	* @test: 	dev_neg_2
	*
	* @desc:	An invalid file descriptor is passed into syswrite
	*
	* @outcome:	
	*/
	rc = syswrite(10, buffer, byte);
	if(fd == -1)
		kprintf("pass2\n");


	/*  
	* @test: 	dev_neg_3
	*
	* @desc:	An invalid command in passed into the sysioctl
	*
	* @outcome:	
	*/
	fd = sysopen(1);

	if(fd != -1)
		rc = sysioctl(fd, 1000, 10);

	if(rc == -1)
		kprintf("pass3\n");

	/*  
	* @test: 	dev_pos_1
	*
	* @desc:	A sysread operation is made on kbd, where an user buffer with length smaller than the internal kbd buffer is passed in
	*
	* @outcome:	
	*/
	if(fd != -1)
		sysread(fd, buffer, byte);

	if(strlen(buffer) == byte)
		kprintf("pass4\n");


	/*  
	* @test: 	dev_pos_2
	*
	* @desc:	After setting EOF as 'a', a sysread operation is made on kbd, and 'a' is immediately pressed
	*
	* @outcome:	
	*/
	strcpy(buffer, "");
	sysioctl(fd, SET_EOF, 97);

	if(fd != -1)
		rc = sysread(fd, buffer, byte);
	
	if(rc == 0)
		kprintf("pass5\n");
}
