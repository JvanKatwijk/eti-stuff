#
/*
 *    Copyright (C) 2016 .. 2025
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

#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	<thread>
#include	<atomic>
class	dll_driver;
//
typedef	struct rtlsdr_dev rtlsdr_dev_t;
//	This class is a simple wrapper around the
//	rtlsdr library that is read is as dll
//	It does not do any processing
class	rtlsdrHandler: public deviceHandler {
public:
			rtlsdrHandler	(int32_t, int16_t,
	                                 int16_t, bool, int16_t);
			~rtlsdrHandler	();
	bool		restartReader	(int32_t);
	void		stopReader	();
	int32_t		getSamples	(std::complex<float> *, int32_t);
	int32_t		Samples		();
	void		resetBuffer	();
	int16_t		bitDepth	();
//
//	These need to be visible for the separate usb handling thread
	RingBuffer<uint8_t>	_I_Buffer;
	struct rtlsdr_dev	*device;
private:
	int32_t		lastFrequency;
	int16_t		theGain;
	float		effectiveGain;
	bool		autogain;
	int16_t		ppmOffset;
	uint16_t	deviceIndex;
	int32_t		inputRate;
	int32_t		deviceCount;
	std::thread	workerHandle;
	int16_t		gainsCount;
	std::atomic<bool>	running;
	int		frequency;
};

