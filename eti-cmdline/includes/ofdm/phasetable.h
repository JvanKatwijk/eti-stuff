#
/*
 *    Copyright (C) 2016 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti-cmdline
 *
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

#pragma once

#include	<stdio.h>
#include	<stdint.h>
#include	"dab-constants.h"

struct phasetableElement {
	int32_t	kmin, kmax;
	int32_t i;
	int32_t n;
};


class phaseTable {
public:
		phaseTable 	(int16_t);
		~phaseTable	(void);
	DSPFLOAT	get_Phi	(int32_t);
private:

	struct phasetableElement	*currentTable;
	int16_t		Mode;
	int32_t		h_table (int32_t i, int32_t j);
};


