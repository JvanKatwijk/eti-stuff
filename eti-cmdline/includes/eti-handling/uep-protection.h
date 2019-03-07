#
/*
 *    Copyright (C) 2016 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline program
 *    eti-cmdline is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-cmdline is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__UEP_PROTECTION__
#define	__UEP_PROTECTION__

#include	<stdio.h>
#include	<stdint.h>
#include	"protection.h"

        class uep_protection: public protection {
public:
                uep_protection	(int16_t, int16_t);
                ~uep_protection (void);
bool            deconvolve      (int16_t *, int32_t, uint8_t *);
};

#endif

