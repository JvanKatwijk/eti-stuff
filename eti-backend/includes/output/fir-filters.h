#
/*
 *    Copyright (C) 2016 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-backed
 *
 *    eti-backend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-backend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-backend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __FIR_LOWPASSFILTER__
#define __FIR_LOWPASSFILTER__

#include	"dab-constants.h"

class	LowPassFIR {
public:
			LowPassFIR (int16_t,	// order
	                            int32_t, 	// cutoff frequency
	                            int32_t	// samplerate
	                           );
			~LowPassFIR (void);
	DSPCOMPLEX	Pass		(DSPCOMPLEX);
	DSPFLOAT	Pass		(DSPFLOAT);
private:
	int16_t		filterSize;
	DSPCOMPLEX	*filterKernel;
	DSPCOMPLEX	*Buffer;
	int16_t		ip;
};

#endif

