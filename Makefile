#
#	Make the LISP Internet Grouper (lig)
#
#	David Meyer
#	dmm@1-4-5.net
#	Wed Apr  8 13:36:24 2009
#
#	This program is free software; you can redistribute it
#	and/or modify it under the terms of the GNU General
#	Public License as published by the Free Software
#	Foundation; either version 2 of the License, or (at your
#	option) any later version. 
#
#	This program is distributed in the hope that it will be
#	useful,  but WITHOUT ANY WARRANTY; without even the
#	implied warranty of MERCHANTABILITY or FITNESS FOR A
#	PARTICULAR PURPOSE.  See the GNU General Public License
#	for more details. 
#
#	You should have received a copy of the GNU General Public
#	License along with this program; if not, write to the
#	Free Software Foundation, Inc., 59 Temple Place - Suite
#	330, Boston, MA  02111-1307, USA. 
#
#	$Header: /home/dmm/lisp/lig/RCS/Makefile,v 1.9 2009/09/10 23:22:23 dmm Exp $
#


SRC      = lig.c send_map_request.c lib.c cksum.c print.c get_my_ip_addr.c
INC	 = lig.h lig-external.h
OBJ	 = $(SRC:%.c=%.o)
EXE      = lig

RCS      = RCS
MISC     = Makefile README 

CC	 = gcc
CFLAGS   = -g -DDEBUG=3
LDLIBS   = 
LDFLAGS  = 

${EXE}: ${OBJ} ${INC} Makefile
	$(CC) -o $@ ${OBJ} $(LDLIBS) $(LDFLAGS)

clean:
	/bin/rm -f ${OBJ} ${EXE} core a.out Make.log Make.err *~

