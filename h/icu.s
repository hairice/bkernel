/* icu.S
 *
 * This is the signal processing between user-space processes, it supports 
 * priority signal stacking.
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

	.set	ICU1,0x20
	.set	ICU2,0xA0
	.set	OCW1_2,ICU1
	.set	OCW2_2,ICU2
	.set	IMR1,ICU1 + 1
	.set	IMR2,ICU2 + 1
	.set	EOI,0x20

