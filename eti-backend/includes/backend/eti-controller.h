#
/*
 *    Copyright (C) 2016 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-backend
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
 */
#
#ifndef	__ETI_CONTROLLER__
#define	__ETI_CONTROLLER__
/*
 *
 */
#include	<QThread>
#include	<QObject>
#include	<atomic>
#include	"stdint.h"
#include	"dab-constants.h"
#include	"ringbuffer.h"
//
class	RadioInterface;
class	fib_processor;
class	dabProcessor;

class etiController: public QThread {
Q_OBJECT
public:
		etiController  	(RadioInterface *,
	                         DabParams	*,
	                         FILE		*,
	                         fib_processor	*,
	                         RingBuffer<int16_t> 	*);
		~etiController	(void);
	void	reset		(void);
	void	stop		(void);
	void	set_audioChannel	(audiodata);
	void	set_dataChannel		(packetdata);
private:
	RadioInterface	*myRadioInterface;
	DabParams	*params;
	FILE		*input;
	RingBuffer<fibElement> *the_fibBuffer;
	RingBuffer<int16_t>    *audioBuffer;
	fib_processor	*my_fibProcessor;
	dabProcessor	*the_Processor;
	std::atomic<bool>	running;
	std::atomic<bool>	newAudio;
	std::atomic<bool>	newData;
	audiodata	audioDescription;
	packetdata	packetDescription;
protected:	
virtual	void		run			(void);
private:
};
#endif

