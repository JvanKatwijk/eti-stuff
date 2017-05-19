#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  DAB-library
 *    DAB-library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    FAB-library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"ofdm-processor.h"
#include	"fic-handler.h"
#include	"device-handler.h"
#include	"eti-generator.h"
#include	"fft.h"
//

/**
  *	\brief ofdmProcessor
  *	The ofdmProcessor class is the driver of the processing
  *	of the samplestream.
  *	It takes as parameter (a.o) the handler for the
  *	input device as well as the interpreters for
  *	FIC blocks and for MSC blocks.
  *	Local is a class ofdmDecoder that will - as the name suggests -
  *	map samples to bits and that will pass on the bits
  *	to the interpreters for FIC and MSC
  */

	ofdmProcessor::ofdmProcessor	(deviceHandler	*inputDevice,
	                                 uint8_t	dabMode,
	                                 etiGenerator	*eti,
	                                 int16_t	threshold,
	                                 uint8_t	freqsyncMethod):
	                                    params (dabMode),
	                                    myMapper (&params),
	                                    phaseSynchronizer (&params,
	                                                       threshold) {
int32_t	i;
	this	-> inputDevice		= inputDevice;
	this	-> freqSyncMethod	= freqsyncMethod;
	this	-> T_null		= params. get_T_null ();
	this	-> T_s			= params. get_T_s    ();
	this	-> T_u			= params. get_T_u    ();
	this	-> T_g			= T_s - T_u;
	this	-> T_F			= params. get_T_F ();
	this	-> nrBlocks		= params. get_L ();
	this	-> carriers		= params. get_carriers ();
	this	-> carrierDiff		= params. get_carrierDiff ();
	this	-> ibits		= new int16_t [2 * this -> carriers];
	this	-> referenceFase	= new DSPCOMPLEX [T_u];
	this	-> my_etiGenerator	= eti;
	fft_handler			= new common_fft (T_u);
	fft_buffer			= fft_handler -> getVector ();
//
	ofdmBuffer			= new DSPCOMPLEX [76 * T_s];
	ofdmBufferIndex			= 0;
	ofdmSymbolCount			= 0;
	sampleCnt			= 0;
	fineCorrector		= 0;	
	coarseCorrector		= 0;
	f2Correction		= true;
	oscillatorTable		= new DSPCOMPLEX [INPUT_RATE];

	for (i = 0; i < INPUT_RATE; i ++)
	   oscillatorTable [i] = DSPCOMPLEX (cos (2.0 * M_PI * i / INPUT_RATE),
	                                     sin (2.0 * M_PI * i / INPUT_RATE));

	bufferContent	= 0;
//
//	and for the correlation 
	for (i = 0; i < CORRELATION_LENGTH; i ++)  {
	   refArg [i] = arg (phaseSynchronizer. refTable [(T_u + i) % T_u] *
	              conj (phaseSynchronizer. refTable [(T_u + i + 1) % T_u]));
	}
}

	ofdmProcessor::~ofdmProcessor	(void) {
	stop ();
	delete		ofdmBuffer;
	delete		oscillatorTable;
	delete		fft_handler;
}

void	ofdmProcessor::start	(void) {
	coarseCorrector	= 0;
	fineCorrector	= 0;
	f2Correction	= true;
	syncBufferIndex	= 0;
	sLevel		= 0;
	localPhase	= 0;
	inputDevice	-> resetBuffer ();
	running. store (true);
	threadHandle	= std::thread (&ofdmProcessor::run, this);
}
/**
  */
DSPCOMPLEX ofdmProcessor::getSample (int32_t phase) {
DSPCOMPLEX temp;
	
	if (!running. load ())
	   throw 21;

	if (bufferContent == 0) {
	   bufferContent = inputDevice -> Samples ();
	   while ((bufferContent == 0) && running. load ()) {
	      usleep (1000);
	      bufferContent = inputDevice -> Samples (); 
	   }
	}

	if (!running. load ())	
	   throw 20;
//
//	so here, bufferContent > 0, fetch a sample
	inputDevice -> getSamples (&temp, 1);
	bufferContent --;
//
//	OK, we have a sample!!
//	first: adjust frequency. We need Hz accuracy
	localPhase	-= phase;
	localPhase	= (localPhase + INPUT_RATE) % INPUT_RATE;
	temp		*= oscillatorTable [localPhase];
	sLevel		= 0.00001 * jan_abs (temp) + (1 - 0.00001) * sLevel;
#define	N	5
	sampleCnt	++;
	if (++ sampleCnt > INPUT_RATE / N) {
//	may be we add here something
	   sampleCnt = 0;
	}
	return temp;
}
//

void	ofdmProcessor::getSamples (DSPCOMPLEX *v, int16_t n, int32_t phase) {
int32_t		i;

	if (!running. load ())
	   throw 21;
	if (n > bufferContent) {
	   bufferContent = inputDevice -> Samples ();
	   while ((bufferContent < n) && running. load ()) {
	      usleep (1000);
	      bufferContent = inputDevice -> Samples ();
	   }
	}
	if (!running. load ())	
	   throw 20;
//
//	so here, bufferContent >= n
	n	= inputDevice -> getSamples (v, n);
	bufferContent -= n;

//	OK, we have samples!!
//	first: adjust frequency. We need Hz accuracy
	for (i = 0; i < n; i ++) {
	   localPhase	-= phase;
	   localPhase	= (localPhase + INPUT_RATE) % INPUT_RATE;
	   v [i]	*= oscillatorTable [localPhase];
	   sLevel	= 0.00001 * jan_abs (v [i]) + (1 - 0.00001) * sLevel;
	}

	sampleCnt	+= n;
	if (sampleCnt > INPUT_RATE / N) {
	   sampleCnt = 0;
	}
}

/***
   */
void	ofdmProcessor::run	(void) {
int32_t		startIndex;
int32_t		i;
DSPCOMPLEX	FreqCorr;
int32_t		counter;
float		currentStrength;
int32_t		syncBufferSize	= 32768;
int32_t		syncBufferMask	= syncBufferSize - 1;
float		envBuffer	[syncBufferSize];

	inputDevice	-> restartReader ();
	try {

Initing:
///	first, we need samples to get a reasonable sLevel
	   sLevel	= 0;
	   for (i = 0; i < T_F / 2; i ++) {
	      jan_abs (getSample (0));
	   }
notSynced:
	   syncBufferIndex	= 0;
	   currentStrength	= 0;
///     first, we need samples to get a reasonable sLevel
           sLevel       = 0;
           for (i = 0; i < T_F; i ++) {
              jan_abs (getSample (0));
           }


//	read in T_s samples for a next attempt;
	   syncBufferIndex = 0;
	   currentStrength	= 0;
	   for (i = 0; i < 50; i ++) {
	      DSPCOMPLEX sample			= getSample (0);
	      envBuffer [syncBufferIndex]	= jan_abs (sample);
	      currentStrength 			+= envBuffer [syncBufferIndex];
	      syncBufferIndex ++;
	   }
/**
  *	We now have initial values for currentStrength (i.e. the sum
  *	over the last 50 samples) and sLevel, the long term average.
  */
SyncOnNull:
/**
  *	here we start looking for the null level, i.e. a dip
  */
	   counter	= 0;
	   while (currentStrength / 50  > 0.50 * sLevel) {
	      DSPCOMPLEX sample	=
	                      getSample (coarseCorrector + fineCorrector);
	      envBuffer [syncBufferIndex] = jan_abs (sample);
//	update the levels
	      currentStrength += envBuffer [syncBufferIndex] -
	                         envBuffer [(syncBufferIndex - 50) & syncBufferMask];
	      syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	      counter ++;
	      if (counter > T_F) { // hopeless
	         goto notSynced;
	      }
	   }
/**
  *	It seemed we found a dip that started app 65/100 * 50 samples earlier.
  *	We now start looking for the end of the null period.
  */
	   counter	= 0;
SyncOnEndNull:
	   while (currentStrength / 50 < 0.75 * sLevel) {
	      DSPCOMPLEX sample = getSample (coarseCorrector + fineCorrector);
	      envBuffer [syncBufferIndex] = jan_abs (sample);

//	update the levels
	      currentStrength += envBuffer [syncBufferIndex] -
	                         envBuffer [(syncBufferIndex - 50) & syncBufferMask];
	      syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	      counter	++;
//
	      if (counter > T_null + 50) { // hopeless
	         goto notSynced;
	      }
	   }
/**
  *	The end of the null period is identified, probably about 40
  *	samples earlier.
  */
SyncOnPhase:
/**
  *	We now have to find the exact first sample of the non-null period.
  *	We use a correlation that will find the first sample after the
  *	cyclic prefix.
  *	When in "sync", i.e. pretty sure that we know were we are,
  *	we skip the "dip" identification and come here right away.
  *
  *	now read in Tu samples. The precise number is not really important
  *	as long as we can be sure that the first sample to be identified
  *	is part of the samples read.
  */
	   for (i = 0; i <  T_u; i ++) 
	      ofdmBuffer [i] = getSample (coarseCorrector + fineCorrector);
//
///	and then, call upon the phase synchronizer to verify/compute
///	the real "first" sample
	   startIndex = phaseSynchronizer. findIndex (ofdmBuffer);
	   if (startIndex < 0) { // no sync, try again
//	      fprintf (stderr, "startIndex = %d\n", startIndex);
/**
  *	In case we do not have a correlation value larger than
  *	a given threshold, we start all over again.
  */
	      goto notSynced;
	   }
/**
  *	Once here, we are synchronized, we need to copy the data we
  *	used for synchronization for block 0
  */
	   memmove (ofdmBuffer, &ofdmBuffer [startIndex],
	                  (T_u - startIndex) * sizeof (DSPCOMPLEX));
	   ofdmBufferIndex	= T_u - startIndex;

	   my_etiGenerator	-> newFrame ();
Block_0:
/**
  *	Block 0 is special in that it is used for coarse time synchronization
  *	and its content is used as a reference for decoding the
  *	first datablock.
  *	We read the missing samples in the ofdm buffer
  */
	   getSamples (&ofdmBuffer [ofdmBufferIndex],
	               T_u - ofdmBufferIndex,
	               coarseCorrector + fineCorrector);
	   int correction	=
	           processBlock_0 (ofdmBuffer, f2Correction);
	   if (f2Correction) {
              if (correction != 100) {
                 coarseCorrector        += correction * carrierDiff;
                 if (abs (coarseCorrector) > Khz (35))
                    coarseCorrector = 0;
              }
           }

/**
  *	after block 0, we will just read in the other (params -> L - 1) blocks
  */
Data_blocks:
/**
  *	The first ones are the FIC blocks. We immediately
  *	start with building up an average of the phase difference
  *	between the samples in the cyclic prefix and the
  *	corresponding samples in the datapart.
  */
	   FreqCorr		= DSPCOMPLEX (0, 0);
	   for (ofdmSymbolCount = 2;
	        ofdmSymbolCount <= (uint16_t)nrBlocks;
	        ofdmSymbolCount ++) {
	      getSamples (ofdmBuffer, T_s, coarseCorrector + fineCorrector);
	      for (i = (int)T_u; i < (int)T_s; i ++) 
	         FreqCorr += ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	      processBlock (ofdmBuffer, ibits);
	      my_etiGenerator -> processBlock (ibits, ofdmSymbolCount);
	   }

NewOffset:
///	we integrate the newly found frequency error with the
///	existing frequency error.
	   fineCorrector += 0.1 * arg (FreqCorr) / M_PI *
	                        (carrierDiff / 2);
//
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and skip T_null samples
  */
	   syncBufferIndex	= 0;
	   currentStrength	= 0;
	   getSamples (ofdmBuffer, T_null, coarseCorrector + fineCorrector);
/**
  *	The first sample to be found for the next frame should be T_g
  *	samples ahead
  *	Here we just check the fineCorrector
  */
	   counter	= 0;
//
	   if (fineCorrector > carrierDiff / 2) {
	      coarseCorrector += carrierDiff;
	      fineCorrector -= carrierDiff;
	   }
	   else
	   if (fineCorrector < -carrierDiff / 2) {
	      coarseCorrector -= carrierDiff;
	      fineCorrector += carrierDiff;
	   }
ReadyForNewFrame:
///	and off we go, up to the next frame
	   goto SyncOnPhase;
	}
	catch (int e) {
	   ;
	}
	inputDevice	-> stopReader ();
	running. store (false);
	fprintf (stderr, "ofdmProcessor is shutting down\n");
}

void	ofdmProcessor::processBlock (DSPCOMPLEX *inv, int16_t *ibits) {
int16_t		i;

	memcpy (fft_buffer, &inv [T_s - T_u], T_u * sizeof (DSPCOMPLEX));
	fft_handler -> do_FFT ();
//
//	Note that "mapIn" maps to -carriers / 2 .. carriers / 2
//	we did not set the fft output to low .. high

	for (i = 0; i < carriers; i ++) {
	   int16_t	index	= myMapper.  mapIn (i);
	   if (index < 0) 
	      index += T_u;
	      
	   DSPCOMPLEX	r1 = fft_buffer [index] * conj (referenceFase [index]);
	   referenceFase [index] = fft_buffer [index];
	   DSPFLOAT ab1	= jan_abs (r1);
//	Recall:  with this viterbi decoder
//	we have 127 = max pos, -127 = max neg
	   ibits [i]		= - real (r1) / ab1 * 127.0;
	   ibits [carriers + i] = - imag (r1) / ab1 * 127.0;
	}
}

////////////////////////////////////////////////////////////////////////////

int16_t	ofdmProcessor::processBlock_0 (DSPCOMPLEX *vi, bool flag) {
int16_t	i, j, index_1 = 100;

	memcpy (fft_buffer, vi, T_u * sizeof (DSPCOMPLEX));
	fft_handler	-> do_FFT ();

	memcpy (referenceFase, fft_buffer, T_u * sizeof (DSPCOMPLEX));

	if (!flag)		// no need to synchronize further
	   return 0;
//	as a side effect we "compute" an estimate for the
//	coarse offset

	if (freqSyncMethod == 1) {
//	The phase differences were computed once
	   for (i = 0; i < SEARCH_RANGE + CORRELATION_LENGTH; i ++) {
	      int16_t baseIndex = T_u - SEARCH_RANGE / 2 + i;
	      correlationVector [i] =
	                      arg (fft_buffer [baseIndex % T_u] *
	                      conj (fft_buffer [(baseIndex + 1) % T_u]));
	   }
	   float	MMax	= 0;
	   for (i = 0; i < SEARCH_RANGE; i ++) {
	      float sum	= 0;
	      for (j = 1; j < CORRELATION_LENGTH; j ++) 
	         sum += abs (refArg [j] * correlationVector [i + j]);
	      if (sum > MMax) {
	         MMax = sum;
	         index_1 = i;
	      }
	   }
//
//	Now map the index back to the right carrier
//	   return T_u - SEARCH_RANGE / 2 + index_1 - T_u;
	}
//
//	An alternative way is to look at a special pattern consisting
//	of zeros in the row of args between successive carriers.
	float Mmin	= 1000;
	float OMmin	= 1000;
	index_1		= 100;
	for (i = T_u - SEARCH_RANGE / 2; i < T_u + SEARCH_RANGE / 2; i ++) {
              float a1  =  abs (abs (arg (fft_buffer [(i + 1) % T_u] *
                                conj (fft_buffer [(i + 2) % T_u])) / M_PI) - 1);
              float a2  =  abs (abs (arg (fft_buffer [(i + 2) % T_u] *
                                conj (fft_buffer [(i + 3) % T_u])) / M_PI) - 1);
	      float a3	= abs (arg (fft_buffer [(i + 3) % T_u] *
	      	                    conj (fft_buffer [(i + 4) % T_u])));
	      float a4	= abs (arg (fft_buffer [(i + 4) % T_u] *
	      	                    conj (fft_buffer [(i + 5) % T_u])));
	      float a5	= abs (arg (fft_buffer [(i + 5) % T_u] *
	      	                    conj (fft_buffer [(i + 6) % T_u])));
	      float b1	= abs (abs (arg (fft_buffer [(i + 16 + 1) % T_u] *
	      	                    conj (fft_buffer [(i + 16 + 3) % T_u])) / M_PI) - 1);
	      float b2	= abs (arg (fft_buffer [(i + 16 + 3) % T_u] *
	      	                    conj (fft_buffer [(i + 16 + 4) % T_u])));
	      float b3	= abs (arg (fft_buffer [(i + 16 + 4) % T_u] *
	      	                    conj (fft_buffer [(i + 16 + 5) % T_u])));
	      float b4	= abs (arg (fft_buffer [(i + 16 + 5) % T_u] *
	      	                    conj (fft_buffer [(i + 16 + 6) % T_u])));
	      float sum = a1 + a2 + a3 + a4 + a5 + b1 + b2 + b3 + b4;
	      if (sum < Mmin) {
	        OMmin = Mmin;
	         Mmin = sum;
	         index_1 = i;
	      }
	}
	return index_1 - T_u;
}

//
void    ofdmProcessor:: reset   (void) {
        if (running. load ()) {
           running. store (false);
           threadHandle. join ();
        }
        start ();
}

void    ofdmProcessor::stop     (void) {
        if (running. load ()) {
           running. store (false);
           threadHandle. join ();
        }
}

void	ofdmProcessor::syncReached (void) {
	f2Correction = false;
}

