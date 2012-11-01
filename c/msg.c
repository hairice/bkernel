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
	ipc_t* snd = (ipc_t *) snd_p->ptr;
	ipc_t* rcv = (ipc_t *) rcv_p->ptr;
	char *snd_buffer = (char *) snd->buffer;
	char *rcv_buffer = (char *) rcv->buffer;

	for(i=0 ; i<rcv->buffer_len ; i++)
	{
		if(i > (snd->buffer_len-1)) break;
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
	ipc_t* snd = (ipc_t *) snd_p->ptr;
	ipc_t* rcv = (ipc_t *) rcv_p->ptr;
	char *snd_buffer = (char *) snd->buffer;
	char *rcv_buffer = (char *) rcv->buffer;

	for(i=0 ; i<rcv->buffer_len ; i++)
	{
		if(i > (snd->buffer_len-1)) break;
		rcv_buffer[i] = snd_buffer[i];
	}

	return i;
}

