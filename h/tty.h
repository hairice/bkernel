/* tty.h
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

#define	IBLEN		256		/* input buffer size		*/
#define	OBLEN		256		/* output buffer size		*/


/* Device name definitions */

#define	CONSOLE     0			/* type tty      */
#define	SERIAL0     1			/* type serial   */
#define	SERIAL1     2			/* type serial   */
#define	KBMON       3			/* type kbmon    */


/* control() functions */

#define TTC_CPID	0x0001		/* set controlling PID		*/
#define	TTC_SYNC	0x0002		/* do synchronous output	*/
#define	TTC_GIF		0x0003		/* get input flags		*/
#define	TTC_GOF		0x0004		/* get output flags		*/
#define	TTC_NEXTC	0x0005		/* peek at next character	*/
#define	TTC_SUSER	0x0006		/* set uid for a tty		*/
#define	TTC_CUSER	0x0007		/* clear uid for a tty		*/

#define	NOCHAR		256

