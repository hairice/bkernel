/* initialize.c - initproc 
* 
* name:		Jack Wu
* student id:	17254079
*/

#include <i386.h>
#include <xeroskernel.h>
#include <xeroslib.h>

extern	int	entry( void );  /* start of kernel image, use &start    */
extern	int	end( void );    /* end of kernel image, use &end        */
extern  long	freemem; 	/* start of free memory (set in i386.c) */
extern char	*maxaddr;	/* max memory address (set in i386.c)	*/

/************************************************************************/
/***				NOTE:				      ***/
/***								      ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially DISABLED.  The     ***/
/***   interrupt table has been initialized with a default handler    ***/
/***								      ***/
/***								      ***/
/************************************************************************/

/*------------------------------------------------------------------------
 *  The init process, this is where it all begins...
 *------------------------------------------------------------------------
 */
extern pcb_t proc_table[MAX_PROC];

/*
* initproc
*
* @desc:	initializes xeros by starting the memory management unit, dispatcher, and context switcher
*/
void initproc( void )				/* The beginning */
{
	int i;
	kprintf( "\n\nCPSC 415, 2012W1 \n32 Bit Xeros 1.1\nLocated at: %x to %x\n", &entry, &end ); 
        /* Your code goes here */
	kmeminit();
	contextinit();

	/* fill the stop queue */
	for(i=0 ; i<MAX_PROC ; i++) 
		stop(&proc_table[i]);

	create(&idleproc, PROC_STACK);

#ifdef SEND_TEST	
	create(&sndtest_root, PROC_STACK);
#elif defined RECV_TEST
	create(&rcvtest_root, PROC_STACK);
#elif defined TIME_TEST
	create(&timetest_root, PROC_STACK);
#else 
	create(&root, PROC_STACK);
#endif
	dispatch();

        /* This code should never be reached after you are done */
	for(;;); /* loop forever */
}
