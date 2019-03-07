/*
 *
 *    Copyright (C) 2009 .. 2017
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
 *    along with DAB-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"fft.h"
#include	<cstring>
/*
 */

	fftHandler::fftHandler (int32_t fft_size) {
int16_t i;
	this	-> fftSize = fft_size;

	vector	= (std::complex<float> *)
	             FFTW_MALLOC (sizeof (std::complex<float>) * fft_size);
	for (i = 0; i < fft_size; i ++)
	   vector [i] = std::complex<float> (0, 0);
	plan	= FFTW_PLAN_DFT_1D (fft_size,
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            reinterpret_cast <fftwf_complex *>(vector),
	                            FFTW_FORWARD, FFTW_ESTIMATE);
}

	fftHandler::~fftHandler (void) {
	   FFTW_DESTROY_PLAN (plan);
	   FFTW_FREE (vector);
}

std::complex<float>*
	fftHandler::getVector (void) {
	return vector;
}

void	fftHandler::do_FFT () {
	FFTW_EXECUTE (plan);
}
//      Note that we do not scale in case of backwards fft,
//      not needed for our applications
void    fftHandler::do_iFFT (void) {
int     i;
        for (i = 0; i < fftSize; i ++)
           vector [i] = conj (vector [i]);
        fftwf_execute (plan);
        for (i = 0; i < fftSize; i ++)
           vector [i] = conj (vector [i]);
}

