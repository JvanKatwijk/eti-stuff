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
	this	-> fftSize = fft_size;
	vector		= new std::complex<float> [fftSize];
	fftVector_in	= new kiss_fft_cpx [fftSize];
        fftVector_out	= new kiss_fft_cpx [fftSize];
        plan_forward	= kiss_fft_alloc (fftSize, false, 0, 0);
        plan_backwards	= kiss_fft_alloc (fftSize, true, 0, 0);
	memset ((void*)vector, 0, sizeof(std::complex<float>) * fft_size);

}

	fftHandler::~fftHandler () {
	delete [] vector;
	delete [] fftVector_in;
	delete [] fftVector_out;
}

void	fftHandler::do_FFT	() {
	for (int i = 0; i < fftSize; i ++) {
	   fftVector_in [i]. r = real (vector [i]);
	   fftVector_in [i]. i = imag (vector [i]);
	}
	kiss_fft (plan_forward, fftVector_in, fftVector_out);
	for (int i = 0; i < fftSize; i ++)
	   vector [i] = std::complex<float> (fftVector_out [i]. r,
	                                     fftVector_out [i]. i);
}
	
//      Note that we do not scale in case of backwards fft,
//      not needed for our applications
void	fftHandler::do_iFFT	() {
	for (int i = 0; i < fftSize; i ++) {
	   fftVector_in [i]. r = real (vector [i]);
	   fftVector_in [i]. i = imag (vector [i]);
	}
	kiss_fft (plan_backwards, fftVector_in, fftVector_out);
	for (int i = 0; i < fftSize; i ++)
	   vector [i] = std::complex<float> (fftVector_out [i]. r,
	                                     fftVector_out [i]. i);
}

