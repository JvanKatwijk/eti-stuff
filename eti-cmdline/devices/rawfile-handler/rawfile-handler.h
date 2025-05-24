#
/*
 *    Copyright (C) 2013 .. 2017
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

#pragma once

#include	<string>
#include	<atomic>
#include	<thread>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"callback-types.h"
/*
 */
class	rawfileHandler: public deviceHandler {
public:
			rawfileHandler	(std::string, bool, inputstopped_t);
	       		~rawfileHandler	();
	int32_t		getSamples	(std::complex<float> *, int32_t);
	int32_t		Samples		();
	bool		restartReader	(int32_t);
	void		stopReader	();
private:
	RingBuffer<uint8_t>		_I_Buffer;
	std::string	fileName;
	bool		continue_on_eof;
	inputstopped_t	inputStopped;
	void		runRead		();
	std::thread	threadHandle;
	std::atomic<bool> run;
	int32_t		readBuffer	(uint8_t *, int32_t);
	int32_t		bufferSize;
	FILE		*filePointer;
	bool		ThreadFinished;
	int64_t		currPos;
	bool		eof;
};


