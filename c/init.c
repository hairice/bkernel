/* Kernel Initializer 
 *
 * This is kernel initializer, where the memory manager, process manager, and
 * real-time clock
 *
 * Copyright (c) 2013 Jack Wu <jack.wu@live.ca>
 *
 * This file is part of bkernel.
 *
 * bkernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bkernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar. If not, see <http://www.gnu.org/licenses/>.
 */

#include <i386.h>
#include <xeroskernel.h>
#include <xeroslib.h>

extern int entry(void); 	/* start of kernel image, use &start    */
extern int end(void); 		/* end of kernel image, use &end        */
extern long freemem; 		/* start of free memory (set in i386.c) */
extern char *maxaddr; 		/* max memory address (set in i386.c)	*/

/*------------------------------------------------------------------------
 *  The init process, this is where it all begins...
 *------------------------------------------------------------------------
 */
 extern pcb_t proc_table[PROC_SZ];

/*
 * initproc
 *
 * @desc:	initializes xeros by starting the memory management unit, 
 * 			dispatcher, and context switcher
 */
 void initproc(void)
 {
 	int i;

 	kmeminit();
 	kbd_init();
 	contextinit();

	/* fill the process stop queue */
 	for (i = 0; i < PROC_SZ; i++)
 		stop(&proc_table[i]);

 	create(&idleproc, PROC_STACK);
 	create(&root, PROC_STACK);
 	dispatch();


 	for (;;)
 		;
 }
