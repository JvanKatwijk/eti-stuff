
#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#
#include	"eti-controller.h"
#include	"gui.h"
#include	<QThread>
#include	"fib-processor.h"
#include	"mp2processor.h"
#include	"mp4processor.h"
#include	"data-processor.h"
	etiController::etiController (RadioInterface *mr,
	                              DabParams      *dp,
	                              FILE           *f,
	                              fib_processor	*fb,
	                              RingBuffer<int16_t>    *ab) {
	myRadioInterface	= mr;
	params			= dp;
	input			= f;
	my_fibProcessor		= fb;
	audioBuffer		= ab;
	running			= false;
	newAudio		= false;
	newData			= false;
}

	etiController::~etiController	(void) {
}

void	etiController::stop		(void) {
	running		= false;
	msleep (100);
	if (isRunning ()) terminate ();
	wait ();
}

void	etiController::run		(void) {
uint8_t buffer [6144];
int16_t	n_frame;
fibElement fe;
int16_t	subChannelId;
int16_t	data_offset;
int16_t	data_length	= -1;
int	n;
int16_t	NST, FICF;
bool	channelSearch	= false;
bool	processingData	= false;

	the_Processor	= NULL;
	running	= true;

	while (running) {
	   int16_t	bitRate;
//	If there is a request for a service, setup
//	the appropriate handler
	   if (newAudio) {
	      channelSearch	= true;
	      if (the_Processor != NULL)
	         delete the_Processor;
	      the_Processor		= NULL;
	      subChannelId		= audioDescription. subchId;
	      if (audioDescription. ASCTy == 077)	// 
	         the_Processor	= new mp4Processor (myRadioInterface,
	                                            audioDescription. bitRate,
	                                            audioBuffer);
	      else
	         the_Processor	= new mp2Processor (myRadioInterface,
	                                            audioDescription. bitRate,
	                                            audioBuffer);
	   
	      data_length	= -1;
	      bitRate		= audioDescription. bitRate;
	   }	
	   else
	   if (newData) {
	      channelSearch = true;
	      if (the_Processor != NULL)
	         delete the_Processor;

	       the_Processor	=
	                       new dataProcessor (myRadioInterface,
	                                          packetDescription. bitRate,
	                                          packetDescription. DSCTy,
	                                          packetDescription. DGflag,
	                                          packetDescription. FEC_scheme,
	                                          false);
	      subChannelId		= packetDescription. subchId;
	      data_length	= -1;
	      bitRate		= packetDescription. bitRate;
	   }
//
//	OK, ready to read a frame
	   for (n_frame = 0; n_frame < 6144; n_frame += n) {
	      n = fread (&buffer[n_frame], 1, 6144 - n_frame, input);
	      if (n == 0) {
	         fprintf (stderr, "End of file, exiting\n");
	         return;
	      }

	      if (n < 0) {
	         perror ("Read error, exiting");
	         return;
	      }
	   }
//
//	extract the relevant data
	   FICF			= (buffer [5] & 0x80) >> 7;
	   NST			=  buffer [5] & 0x7F;
	   int16_t fibBase	= 12 + 4 * NST;
	   if (FICF != 0) {
	      memcpy (fe. fibdata, &buffer [fibBase], 96);
	      fe. fibno = buffer [4] & 0x03;
	      my_fibProcessor -> process_FIC (fe. fibdata, fe. fibno);
	   }

//	if there was a request for a service, we have to recompute
//	the data_offset where to find it
	   if (channelSearch) {
	      data_offset	= 0;
	      for (int i = 0; i < NST; i++) {
                 int16_t SCID	= (buffer [8 + 4 * i] & 0xfc) >> 2;
                 int16_t SAD	= ((buffer [8 + 4 * i] & 0x03) << 8) |
	                                              buffer [8 + 4 * i+1];
                 int16_t TPL	= (buffer [8 + 4 * i + 2] & 0xfc) >> 2;
                 int16_t STL	= ((buffer [8 + 4 * i + 2] & 0x03) << 8) |
	                                              buffer [8 + 4 *i+3];
                 if (subChannelId == SCID) {
                    data_length = STL * 8;
                    fprintf (stderr,
                       "Extracting channel: SCID=%d, SAD=%d, TPL=%d, STL=%d\n",SCID,SAD,TPL,STL);
                    break;
                 } else {
                    data_offset += STL * 8;
                 }
              }

	      newAudio		= false;
	      newData		= false;
	      channelSearch	= false;
	      processingData	= true;
//
//	However:
	      if (data_length == -1) {
                 fprintf (stderr,
                          "Could not find sub-channel with ID %d\n",
                          subChannelId);
	         processingData	= false;
              }
	   }
//
//	It is a little silly, but we unpack the packed bits. 
//	This has to do with the compatibility of the mp2, mp4 and
//	data handlers as used in the other dab software, we are
//	now able to share the code of these handlers.
	   if (processingData) {
	      uint8_t dataVector [24 * bitRate];
	      int     index = 12 + 4 * NST + FICF * 96 + data_offset;
	      int16_t	i, j;
	      for (i = 0; i < 24 * bitRate / 8; i ++)
	         for (j = 0; j < 8; j ++)
	            dataVector [8 * i + j] = (buffer [index + i] >> (7 - j)) & 01;
              the_Processor -> addtoFrame (dataVector);
	   }
	}
	msleep (10000);
}

void	etiController::set_audioChannel (audiodata a) {
	audioDescription	= a;
	newAudio		= true;
}

void	etiController::set_dataChannel (packetdata p) {
	packetDescription	= p;
	newData			= true;
}


