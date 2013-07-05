# Makefile for PC Xeros to compile on a PC running Linux.
#
# Copyright (c) 2013 Jack Wu <jack.wu@live.ca>
#
# This file is part of bkernel.
#
# bkernel is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# bkernel is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar. If not, see <http://www.gnu.org/licenses/>.

MAKE = make

xeros:
	cd compile; $(MAKE) 
	cd boot; $(MAKE)

beros: xeros
	nice bochs

clean:
	cd compile; $(MAKE) clean
	cd boot; $(MAKE) clean
	rm -f bochsout.txt

libxc:
	rm -f lib/libxc.a
	cd lib/libxc; $(MAKE) clean; $(MAKE)
	mv lib/libxc/libxc.a lib


rebuild: libxc clean xeros


