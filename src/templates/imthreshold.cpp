//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//	http://www.gnu.org/copyleft/gpl.html

// This algorithm was taken from the C++ Augmented Reality Toolkit sourcecodes
// http://www.dandiggins.co.uk/arlib-1.html
// and adopted for the FreeImage library
//
// Copyright (C) 2007-2008:
// monday2000  monday2000@yandex.ru

#include "imthreshold.h"

////////////////////////////////////////////////////////////////////////////////

IMTpixel IMTset(BYTE c0, BYTE c1, BYTE c2)
{
	IMTpixel im;
	
	im.c[0] = (BYTE)c0;
	im.c[1] = (BYTE)c1;
	im.c[2] = (BYTE)c2;
	im.s = (WORD)c0 + (WORD)c1 + (WORD)c2;
		
	return im;
}

////////////////////////////////////////////////////////////////////////////////
