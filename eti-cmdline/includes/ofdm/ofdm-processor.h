#
/*
 *    Copyright (C) 2016, 2017
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
/*
 */
#include	"dab-constants.h"
#include	<thread>
#include	<atomic>
#include	<stdint.h>
#include	"callback-types.h"
#include	"phasereference.h"
#include	"phasetable.h"
#include	"freq-interleaver.h"
#include	"dab-params.h"
#include	"ringbuffer.h"
#ifdef	HAVE_DUMPING
#include	"sndfile.h"
#endif

//      Note:
//      It was found that enlarging the buffersize to e.g. 8192
//      cannot be handled properly by the underlying system.
#define DUMPSIZE                4096

class	fftHandler;
class	deviceHandler;
class	etiGenerator;


#define SEARCH_RANGE            (2 * 36)
#define CORRELATION_LENGTH      24

class ofdmProcessor {
public:
		ofdmProcessor  	(deviceHandler *,
#ifdef	HAVE_DUMPING
	                         SNDFILE       *,
#endif
	                         uint8_t,
	                         callbacks	*,
	                         etiGenerator	*,
	                         int16_t,
	                         int16_t,
	                         void		*);
	virtual ~ofdmProcessor	(void);
	void	reset			(void);
	void	stop			(void);
	void	setOffset		(int32_t);
	void	start			(void);
	void	syncReached		(void);
	bool	isRunning		(void);
private:
	deviceHandler	*inputDevice;
#ifdef	HAVE_DUMPING
	SNDFILE		*dumpFile;
	int16_t		dumpIndex;
	int16_t		dumpScale;
	int16_t		dumpBuffer [DUMPSIZE];
#endif
	void		*userData;
	etiGenerator	*my_etiGenerator;
	syncsignal_t	set_syncSignal;
	snrsignal_t	show_snr;
	int16_t		*ibits;
	DSPCOMPLEX	*referenceFase;
	std::thread	threadHandle;
	int32_t		syncBufferIndex;
	std::atomic<bool>	running;
	dabParams	params;
	interLeaver	myMapper;
	phaseReference	phaseSynchronizer;
	int32_t		T_null;
	int32_t		T_u;
	int32_t		T_s;
	int32_t		T_g;
	int32_t		T_F;
	int32_t		nrBlocks;
	int32_t		carriers;
	int32_t		carrierDiff;
	float		sLevel;
	DSPCOMPLEX	*dataBuffer;
	int32_t		FreqOffset;
	DSPCOMPLEX	*oscillatorTable;
	int32_t		localPhase;
	int16_t		fineCorrector;
	int32_t		coarseCorrector;

	bool		f2Correction;
	DSPCOMPLEX	*ofdmBuffer;
	uint32_t	ofdmBufferIndex;
	uint32_t	ofdmSymbolCount;

	int32_t		sampleCnt;
	int32_t		inputSize;
	int32_t		inputPointer;
	DSPCOMPLEX	getSample	(int32_t);
	void		getSamples	(DSPCOMPLEX *, int16_t, int32_t);
virtual	void		run		(void);
	int32_t		bufferContent;
	bool		isReset;
	void		processBlock_0	(DSPCOMPLEX *);
	void		processBlock	(DSPCOMPLEX*, int16_t*);
	int16_t		get_snr		(DSPCOMPLEX *);
	int16_t		threshold_1;
	int16_t		threshold_2;
	int16_t		snrCount;
	int16_t		snr;

	fftHandler	*fft_handler;
	std::complex<float>	*fft_buffer;
};


