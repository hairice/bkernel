/* User Space
 *
 * This file is the user-space code.
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

#include <xeroskernel.h>

unsigned char console[80];

/*
* idleproc
*
* @desc:	executes the idle process
*/
void idleproc ()
{
	for(;;);
}


/*
* root
*
* @desc:	executes the root process
*/
void root()
{	
	sprintf(console, "Welcome to bkernel!");
	sysputs(console);


	sprintf(console, "Goodbye!");
	sysputs(console);
}

