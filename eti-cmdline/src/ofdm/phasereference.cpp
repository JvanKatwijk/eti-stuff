
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-cmdline
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
#include	"phasereference.h" 
#include	"string.h"
/**
  *	\class phaseReference
  *	Implements the correlation that is used to identify
  *	the "first" element (following the cyclic prefix) of
  *	the first non-null block of a frame
  *	The class inherits from the phaseTable.
  */
	phaseReference::phaseReference (dabParams	*p,
	                                int16_t		diff_length):
	                                     phaseTable (p -> get_dabMode ()) {
int32_t	i;
float	Phi_k;

	this	-> T_u		= p -> get_T_u ();
	this	-> diff_length	= diff_length;
	refTable		= new std::complex<float> [T_u];	//
	fft_processor		= new fftHandler 	(T_u);
	fft_buffer		= fft_processor		-> getVector ();
	phasedifferences	= new std::complex<float>	[diff_length];
	fft_counter		= 0;

	for (i = 0; i < T_u; i ++)
	   refTable [i] = std::complex<float> (0, 0);

	for (i = 1; i <= p -> get_carriers () / 2; i ++) {
	   Phi_k =  get_Phi (i);
	   refTable [i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	   Phi_k = get_Phi (-i);
	   refTable [T_u - i] = std::complex<float> (cos (Phi_k), sin (Phi_k));
	}
//
//      prepare a table for the coarse frequency synchronization
        for (i = 1; i <= diff_length; i ++)
           phasedifferences [i - 1] = refTable [(T_u + i) % T_u] *
                                      conj (refTable [(T_u + i + 1) % T_u]);

}

	phaseReference::~phaseReference (void) {
	delete		fft_processor;
	delete []	refTable;
	delete []	phasedifferences;
}

/**
  *	\brief findIndex
  *	the vector v contains "Tu" samples that are believed to
  *	belong to the first non-null block of a DAB frame.
  *	We correlate the data in this verctor with the predefined
  *	data, and if the maximum exceeds a threshold value,
  *	we believe that that indicates the first sample we were
  *	looking for.
  */
int32_t	phaseReference::findIndex (std::complex<float> *v, int16_t threshold) {
int32_t	i;
int32_t	maxIndex	= -1;
float	sum		= 0;
float	Max		= 1.0;

	memcpy (fft_buffer, v, T_u * sizeof (std::complex<float>));
	fft_processor -> do_FFT ();
//
//	back into the frequency domain, now correlate
	for (i = 0; i < T_u; i ++) 
	   fft_buffer [i] = fft_buffer [i] * conj (refTable [i]);
//	and, again, back into the time domain
	fft_processor	-> do_iFFT ();
/**
  *	We compute the average signal value and the max
  */
	for (i = 0; i < T_u; i ++)
	   sum	+= abs (fft_buffer [i]);
	Max	= -10000;
	for (i = 0; i < T_u; i ++)
	   if (abs (fft_buffer [i]) > Max) {
	      maxIndex = i;
	      Max = abs (fft_buffer [i]);
	   }
/**
  *	that gives us a basis for defining the threshold
  */
	if (Max < threshold * sum / T_u) {
	   return  - abs (Max * T_u / sum) - 1;
	}
	else
	   return maxIndex;	
}
//
#define	SEARCH_RANGE	(2 * 35)
int16_t	phaseReference::estimateOffset (std::complex<float> *v) {
int16_t	i, j, index = 100;

	memcpy (fft_buffer, v, T_u * sizeof (std::complex<float>));
	fft_processor	-> do_FFT ();

//	We investigate a sequence of phasedifferences that should
//	are known around carrier 0. In previous versions we looked
//	at the "weight" of the positive and negative carriers in the
//	fft, but that did not work too well.
//	Note that due to phases being in a modulo system,
//	plain correlation does not work well, so we just compute
//	the difference.
	int Mmin	= 1000;
	for (i = T_u - SEARCH_RANGE / 2; i < T_u + SEARCH_RANGE / 2; i ++) {
	   float diff = 0;
	   for (j = 0; j < DIFF_LENGTH; j ++) {
	      int16_t ind1 = (i + j + 1) % T_u;
	      int16_t ind2 = (i + j + 2) % T_u;
	      std::complex<float>  pd =
	                  fft_buffer [ind1] * conj (fft_buffer [ind2]);
	      diff += abs (arg (pd * conj (phasedifferences [j])));
	   }
	   if (diff < Mmin) {
	      Mmin = diff;
	      index = i;
	   }
	}
	return index - T_u;
}

