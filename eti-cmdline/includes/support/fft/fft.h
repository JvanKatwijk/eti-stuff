
/*
 *    Copyright (C) 2016 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti-cmdline program
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

#ifndef __COMMON_FFT__
#define	__COMMON_FFT__

//
//	Simple wrapper around fftwf
#include	"dab-constants.h"
//
#include	"kiss_fft.h"
/*
 *	a simple wrapper
 */

class	fftHandler {
public:
	fftHandler	(int32_t);
	~fftHandler	();

	inline std::complex<float>* getVector(void) { return vector; }
	void	do_FFT	();
	// doesn't normalise
	void	do_iFFT ();

private:
	int32_t		fftSize;
	std::complex<float>	*vector;
	kiss_fft_cfg    plan_forward, plan_backwards;
        kiss_fft_cpx    *fftVector_in;
        kiss_fft_cpx    *fftVector_out;
};

#endif

