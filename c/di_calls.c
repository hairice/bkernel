/* di_calls.c
* 
* name:         Jack Wu
* student id:   17254079
*/

#include <xeroskernel.h>
#include <stdarg.h>

/* Your code goes here */
extern devsw_t dev_table[DEV_SZ];


/*
* di_open
*
* @desc:	opens the specified device at device_no
*
* @param:	p			proc that is making attempting to open a device
*		device_no		device to be opened at device number in dev_table
*		
* @output:	SYSOK			specified device has been opened		
*		SYSERR			failed device parameter checking
*
* @note:	only 1 device can be opened in this operating system, however both ensuring correct device owner and number of opened device are asserted
*/
int di_open(pcb_t *p, int device_no)
{
	int i;

	/* check device_no is within the correct range */
	if(device_no < 0 || device_no > DEV_SZ)
		return -1;

	/* check device is not opened by any other device */
    	if(dev_table[device_no].dvowner)
    		return -1;

	/* only 1 kbd device can be opened, echo/non-echo */
	for(i=0 ; i<DEV_SZ ; i++)
	{
		if(dev_table[i].dvowner) 
			return -1;
	}

	/* scan for a free spot in the file descriptor table */
	for(i = 0 ; i<FD_SZ ; i++)
	{
	       	if(p->fd_table[i].dvmajor == -1)
		{
	       		p->fd_table[i].dvmajor = device_no;            
			dev_table[device_no].dvowner = p->pid;

			(*dev_table[device_no].dvopen)(&(dev_table[device_no]));

			return i;
		}
	}
            
	/* no free space found, device not opened */
	return -1;
}

/*
* di_close
*
* @desc:	closes the specified device stored in proc fd_table
*
* @param:	p			proc that is making attempting to close a device
*		fd			fd_table index for opened device in proc fd_table
*		
* @output:	SYSOK			specified device has been closed		
*		SYSERR			failed device parameter checking
*/
int di_close(pcb_t *p, int fd)
{
	int dvmajor;

	/* check fd is within the correct range */
	if(fd < 0 || fd > FD_SZ)
		return -1;

	/* check for valid fd_table entry */
	if(p->fd_table[fd].dvmajor == -1)
		return -1;

	dvmajor = p->fd_table[fd].dvmajor;

	/* check if the device is owned by the current proc */
	if(dev_table[dvmajor].dvowner != p->pid)
		return -1;

	dev_table[dvmajor].dvowner = 0;
	p->fd_table[fd].dvmajor = -1;

	return (*dev_table[dvmajor].dvclose)(&(dev_table[dvmajor]));
}

/*
* di_write
*
* @desc:	writes a buffer of data to specificed opened device for proc
*
* @param:	p			proc that is attempting to write to a device
*		fd			fd_table index for opened device in proc fd_table
*		buf			user supplied data buffer to be written to device
*		buflen			length of data to be written to device
*
* @output:	SYSERR			failed device parameter checking	
*
* @note: 	the implementation of the kbd device does not support syswrite, hence this call will always return SYSERR
*/
int di_write(pcb_t *p, int fd, void *buf, int buflen)
{
	int dvmajor;

	/* check fd is within the correct range */
	if(fd < 0 || fd > FD_SZ)
		return -1;

	/* check for valid fd_table entry */
	if(p->fd_table[fd].dvmajor == -1)
		return -1;

	if(!buf || buflen == 0)
		return -1;

	dvmajor = p->fd_table[fd].dvmajor;

	/* check if the device is owned by the current proc */
	if(dev_table[dvmajor].dvowner != p->pid)
		return -1;

	return -1;
}

/*
* di_read
*
* @desc:	reads a buffer of data to specificed opened device for proc
*
* @param:	p			proc that is attempting to read a device
*		fd			fd_table index for opened device in proc fd_table
*		buf			user supplied data buffer to copy device buffer
*		buflen			length of data that can be copied into user buffer
*		
* @output:	SYSOK			specified device has been read		
*		SYSERR			failed device parameter checking
*/
int di_read(pcb_t *p, int fd, void *buf, int buflen)
{
	int dvmajor;

	/* check fd is within the correct range */
	if(fd < 0 || fd > FD_SZ)
		return -1;

	/* check for valid fd_table entry */
	if(p->fd_table[fd].dvmajor == -1)
		return -1;

	if(!buf || buflen == 0)
		return -1;

	dvmajor = p->fd_table[fd].dvmajor;

	/* check if the device is owned by the current proc */
	if(dev_table[dvmajor].dvowner != p->pid)
		return -1;
	
	return (*dev_table[dvmajor].dvread)(p, &(dev_table[dvmajor]), buf, buflen);
}

/*
* di_ioctl
*
* @desc:	
*
* @param:	p			proc that is attempting to read a device
*		fd			fd_table index for opened device in proc fd_table
*		command			device specific defined macros for manipulating the device
*		
* @output:	SYSOK			specified device has been manipulated with command		
*		SYSERR			failed device parameter checking	
*
* @note:	for the kbd device implemented, only SET_OEF is supported
*/
int di_ioctl(pcb_t *p, int fd, unsigned long command, ...)
{
	int dvmajor;
	va_list ap;
	va_start(ap, command);
	int eof = va_arg(ap, int);

	/* check fd is within the correct range */
	if(fd < 0 || fd > FD_SZ)
		return -1;

	/* check for valid fd_table entry */
	if(p->fd_table[fd].dvmajor == -1)
		return -1;

	/* check for invalid command that is not SET_OEF */
	if(command != SET_EOF) 
		return -1;

	/* check for invalid eof ascii character */
	if(eof < 0 || eof > 127)
		return -1;

	dvmajor = p->fd_table[fd].dvmajor;

	/* check if the device is owned by the current proc */
	if(dev_table[dvmajor].dvowner != p->pid)
		return -1;
	
	return (*dev_table[dvmajor].dvcntl)(eof);
}
