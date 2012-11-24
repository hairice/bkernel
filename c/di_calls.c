/* di_calls.c
* 
* name:         Jack Wu
* student id:   17254079
*/

#include <xeroskernel.h>


/* Your code goes here */
extern devsw_t dev_table[DEV_SZ];


/*
* di_open
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int di_open(pcb_t *p, int device_no)
{
	int i;

	if(device_no < 0 || device_no > DEV_SZ)
		return -1;

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
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int di_close(pcb_t *p, int fd)
{
	int i,dvmajor;

	if(fd < 0 || fd > FD_SZ)
		return -1;

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
* @desc:	
*
* @param:	
*	
*		
*
* @output:	
*/
int di_write(pcb_t *p, int fd, void *buf, int buflen)
{
	return -1;
}

/*
* di_read
*
* @desc:	
*
* @param:	
*		
*		
*
* @output:	
*/
int di_read(pcb_t *p, int fd, void *buf, int buflen)
{
	int dvmajor;

	if(fd < 0 || fd > FD_SZ)
		return -1;

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
* @param:	
*		
*		
*
* @output:	
*/
int di_ioctl(pcb_t *p, int fd, unsigned long command, ...)
{


	return 0;
}
