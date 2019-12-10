#
/*
 *    Copyright (C) 2013 .. 2017
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
#ifndef	__WAVFILE_HANDLER__
#define	__WAVFILE_HANDLER__

#include        <string>
#include        <atomic>
#include        <thread>
#include        "dab-constants.h"
#include        "device-handler.h"
#include	"sndfile.h"
#include        "ringbuffer.h"
#include	"callback-types.h"

class	wavfileHandler: public deviceHandler {
public:
			wavfileHandler	(std::string, bool, inputstopped_t);
	       		~wavfileHandler	(void);
	int32_t		getSamples	(std::complex<float> *, int32_t);
	int32_t		Samples		(void);
	bool		restartReader	(void);
	void		stopReader	(void);
private:
        bool            continue_on_eof;
	SNDFILE		*filePointer;
	inputstopped_t	inputStopped;
        void            runRead         (void);
        std::thread     threadHandle;
        std::atomic<bool> run;
        int32_t         readBuffer      (std::complex<float> *, int32_t);
        RingBuffer<std::complex<float>>	*_I_Buffer;
        int32_t         bufferSize;
        bool            readerOK;
        bool            readerPausing;
        bool            ThreadFinished;
        int64_t         currPos;
	bool		eof;
};

#endif

