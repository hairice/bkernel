/* msg.c : messaging system (assignment 2)
 */

#include <xeroskernel.h>

/* Your code goes here */


/*
* send
*
* @desc:		copy sender buffer content into receiver buffer
*
* @param:		snd_p		sender proc
*			rcv_p		receiver proc
*
* @output:		on success, this function will return the number of bytes copied
*
* @note:	
*/
int send(pcb_t* snd_p, pcb_t* rcv_p)
{
	int i;	
	char *snd_buffer = (char *) snd_p->comm.buffer;
	char *rcv_buffer = (char *) rcv_p->comm.buffer;

	for(i=0 ; i<rcv_p->comm.buffer_len ; i++)
	{
		if(i > (snd_p->comm.buffer_len-1)) break;
		rcv_buffer[i] = snd_buffer[i];
	}

	return i;
}

/*
* recv
*
* @desc:		copy sender buffer content into receiver buffer
*
* @param:		snd_p		sender proc
*			rcv_p		receiver proc
*
* @output:		on success, this function will return the number of bytes copied	
*
* @note:	
*/
int recv(pcb_t* snd_p, pcb_t* rcv_p)
{
	int i;	
	char *snd_buffer = (char *) snd_p->comm.buffer;
	char *rcv_buffer = (char *) rcv_p->comm.buffer;

	for(i=0 ; i<rcv_p->comm.buffer_len ; i++)
	{
		if(i > (snd_p->comm.buffer_len-1)) break;
		rcv_buffer[i] = snd_buffer[i];
	}

	return i;
}

