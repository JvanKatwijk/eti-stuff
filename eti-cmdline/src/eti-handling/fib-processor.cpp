#
/*
 *    Copyright (C) 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti library
 *
 *    eti library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	fib and fig processor
 */
#include	"fib-processor.h"
#include	<cstring>
#include	"charsets.h"
//
//
// Tabelle ETSI EN 300 401 Page 50
// Table is copied from the work of Michael Hoehn
   const int ProtLevel[64][3]   = {{16,5,32},	// Index 0
                                   {21,4,32},
                                   {24,3,32},
                                   {29,2,32},
                                   {35,1,32},	// Index 4
                                   {24,5,48},
                                   {29,4,48},
                                   {35,3,48},
                                   {42,2,48},
                                   {52,1,48},	// Index 9
                                   {29,5,56},
                                   {35,4,56},
                                   {42,3,56},
                                   {52,2,56},
                                   {32,5,64},	// Index 14
                                   {42,4,64},
                                   {48,3,64},
                                   {58,2,64},
                                   {70,1,64},
                                   {40,5,80},	// Index 19
                                   {52,4,80},
                                   {58,3,80},
                                   {70,2,80},
                                   {84,1,80},
                                   {48,5,96},	// Index 24
                                   {58,4,96},
                                   {70,3,96},
                                   {84,2,96},
                                   {104,1,96},
                                   {58,5,112},	// Index 29
                                   {70,4,112},
                                   {84,3,112},
                                   {104,2,112},
                                   {64,5,128},
                                   {84,4,128},	// Index 34
                                   {96,3,128},
                                   {116,2,128},
                                   {140,1,128},
                                   {80,5,160},
                                   {104,4,160},	// Index 39
                                   {116,3,160},
                                   {140,2,160},
                                   {168,1,160},
                                   {96,5,192},
                                   {116,4,192},	// Index 44
                                   {140,3,192},
                                   {168,2,192},
                                   {208,1,192},
                                   {116,5,224},
                                   {140,4,224},	// Index 49
                                   {168,3,224},
                                   {208,2,224},
                                   {232,1,224},
                                   {128,5,256},
                                   {168,4,256},	// Index 54
                                   {192,3,256},
                                   {232,2,256},
                                   {280,1,256},
                                   {160,5,320},
                                   {208,4,320},	// index 59
                                   {280,2,320},
                                   {192,5,384},
                                   {280,3,384},
                                   {416,1,384}};

//
	fib_processor::fib_processor (callbacks	*the_callBacks,
	                              void		*userData) {
	this	-> ensembleName	= the_callBacks -> theEnsemble;
	this	-> programName	= the_callBacks -> theProgram;
	this	-> userData	= userData;
	clearEnsemble	();
	CIFCount_hi	= -1;
	CIFCount_lo	= -1;
}
	
	fib_processor::~fib_processor () {
}
//
//	FIB's are segments of 256 bits. When here, we already
//	passed the crc and we start unpacking into FIGs
//	This is merely a dispatcher
void	fib_processor::process_FIB (uint8_t *p, uint16_t fib) {
uint8_t	FIGtype;
int8_t	processedBytes	= 0;
uint8_t	*d		= p;

	   (void)fib;
	   while (processedBytes  < 30) {
	      FIGtype 		= getBits_3 (d, 0);
	      switch (FIGtype) {
	         case 0:
	            process_FIG0 (d);	
	            break;

	         case 1:
	            process_FIG1 (d);
	            break;

	         case 7:
	            return;

	         default:
	            fprintf (stderr, "FIG%d aanwezig\n", FIGtype);
	            break;
	      }
//
//	Thanks to Ronny Kunze, who discovered that I used
//	a p rather than a d
	      processedBytes += getBits_5 (d, 3) + 1;
//	      processedBytes += getBits (p, 3, 5) + 1;
	      d = p + processedBytes * 8;
	   }
}
//
//	To generate an eti file, we basically only need the
//	subchannel information
//
void	fib_processor::process_FIG0 (uint8_t *d) {
uint8_t	extension	= getBits_5 (d, 8 + 3);
//uint8_t	CN	= getBits_1 (d, 8 + 0);

	switch (extension) {
	   case 0:
	      FIG0Extension0 (d);
	      break;

	   case 1:
	      FIG0Extension1 (d);
	      break;

	   default:
	      break;
	}
}

//
//	FIG0/0 indicated a change in channel organization
//	we are not equipped for that, so we just return
//	control to the init
void	fib_processor::FIG0Extension0 (uint8_t *d) {
uint16_t	EId;
uint8_t		changeflag;
int16_t		occurrenceChange;
uint8_t	CN	= getBits_1 (d, 8 + 0);

	(void)CN;
	changeflag	= getBits_2 (d, 16 + 16);

	EId			= getBits (d, 16, 16);
	(void)EId;
	CIFCount_hi		= getBits_5 (d, 16 + 19) % 20;
	CIFCount_lo		= getBits_8 (d, 16 + 24) % 250;

	if (changeflag == 0)
	   return;
	occurrenceChange	= getBits_8 (d, 16 + 32);
	(void)occurrenceChange;

	fprintf (stderr, "changes in config not supported, choose again\n");
	changeinConfiguration ();
//
}
//
//	FIG0 extension 1 creates a mapping between the
//	sub channel identifications and the positions in the
//	relevant CIF.
void	fib_processor::FIG0Extension1 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
//uint8_t	CN	= getBits_1 (d, 8 + 0);

	while (used < Length - 1)
	   used = HandleFIG0Extension1 (d, used, PD_bit);
}
//
//	defining the channels 
int16_t	fib_processor::HandleFIG0Extension1 (uint8_t *d,
	                                     int16_t offset,
	                                     uint8_t pd) {
int16_t	bitOffset	= offset * 8;
int16_t	SubChId		= getBits_6 (d, bitOffset);
int16_t start_cu	= getBits (d, bitOffset + 6, 10);
int16_t uep_index;
int16_t	option, protLevel, subChanSize;
	(void)pd;		// not used right now, maybe later
	subChannels [SubChId]. id		= SubChId;
	subChannels [SubChId]. start_cu	= start_cu;
	subChannels [SubChId]. uepFlag	= getBits_1 (d, bitOffset + 16) == 0;
	if (subChannels [SubChId]. uepFlag) {	// short form
	   subChannels [SubChId]. uep_index  = getBits_6 (d, bitOffset + 18);
	   uep_index = subChannels [SubChId]. uep_index;
	   subChannels [SubChId]. size  	= ProtLevel [uep_index][0];
	   subChannels [SubChId]. protlev	= ProtLevel [uep_index][1];
	   subChannels [SubChId]. bitrate	= ProtLevel [uep_index][2];
	   bitOffset += 24;
	   subChannels [SubChId]. inUse	= true;
	}
	else { 	// EEP long form
	   option = getBits_3 (d, bitOffset + 17);
	   if (option == 0) { 		// A Level protection
	      protLevel = getBits (d, bitOffset + 20, 2);
//
	      subChannels [SubChId]. protlev = protLevel;
	      subChanSize = getBits (d, bitOffset + 22, 10);
	      subChannels [SubChId]. size	= subChanSize;
	      
	      if (protLevel == 0)
	         subChannels [SubChId]. bitrate	= subChanSize / 12 * 8;
	      if (protLevel == 1)
	         subChannels [SubChId]. bitrate	= subChanSize / 8 * 8;
	      if (protLevel == 2)
	         subChannels [SubChId]. bitrate	= subChanSize / 6 * 8;
	      if (protLevel == 3)
	         subChannels [SubChId]. bitrate	= subChanSize / 4 * 8;
	   }
	   else			// option should be 001
	   if (option == 001) {		// B Level protection
	      protLevel = getBits_2 (d, bitOffset + 20);
//
	      subChannels [SubChId]. protlev = protLevel + (1 << 2);
	      subChanSize = getBits (d, bitOffset + 22, 10);
	      subChannels [SubChId]. size = subChanSize;
	      if (protLevel == 0)
	         subChannels [SubChId]. bitrate	= subChanSize / 27 * 32;
	      if (protLevel == 1)
	         subChannels [SubChId]. bitrate	= subChanSize / 21 * 32;
	      if (protLevel == 2)
	         subChannels [SubChId]. bitrate	= subChanSize / 18 * 32;
	      if (protLevel == 3)
	         subChannels [SubChId]. bitrate	= subChanSize / 15 * 32;
	   }
	   subChannels [SubChId]. inUse	= true;
	   bitOffset += 32;
	}
	return bitOffset / 8;	// we return bytes
}
//

//	FIG 1
//
//	FIG1 gives some names that we show
void	fib_processor::process_FIG1 (uint8_t *d) {
uint8_t		charSet, extension;
uint32_t	SId	= 0;
uint8_t		oe;
int16_t		offset	= 0;
serviceId	*myIndex;
int16_t		i;
uint8_t		pd_flag;
char		label [17];
//
//	from byte 1 we deduce:
	charSet		= getBits_4 (d, 8);
	oe		= getBits_1 (d, 8 + 4);
	extension	= getBits_3 (d, 8 + 5); 
	label [16]	= 0x00;
	if (oe == 01)
	   return;
	switch (extension) {
	   case 0:	// ensemble label
	      SId	= getBits (d, 16, 16);
	      offset	= 32;
	      if ((charSet <= 16)) { // EBU Latin based repertoire
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits_8 (d, offset + 8 * i);
	         }
	         if (!oe) {
	            const std::string name = toStringUsingCharset (
	                                      (const char *) label,
	                                      (CharacterSet) charSet);
	            nameofEnsemble (SId, name);
	         }
	      }
	      break;

	   case 1:	// 16 bit Identifier field for service label
	      SId	= getBits (d, 16, 16);
	      offset	= 32;
	      myIndex	= findServiceId (SId);
	      if ((!myIndex -> serviceLabel. hasName) && (charSet <= 16)) {
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits_8 (d, offset + 8 * i);
	         }
	         myIndex -> serviceLabel. label. append (
	                            toStringUsingCharset (
	                                   (const char *) label,
	                                   (CharacterSet) charSet));
	         addtoEnsemble (myIndex -> serviceLabel. label, SId);
	         myIndex -> serviceLabel. hasName = true;
	      }
	      break;

	   case 4:
	      pd_flag	= getLBits (d, 16, 1);
	      if (pd_flag) {	// 32 bit identifier field for service component label
	         SId	= getLBits (d, 24, 32);
	         offset	= 56;
	      }
	      else {	// 16 bit identifier field for service component label
	         SId	= getLBits (d, 24, 16);
	         offset	= 40;
	      }

	      for (i = 0; i < 16; i ++)
	         label [i] = getBits_8 (d, offset + 8 * i);
	      break;
//
	   case 5:	// 32 bit Identifier field for service label
	      SId	= getLBits (d, 16, 32);
	      offset	= 48;
	      myIndex   = findServiceId (SId);
	      if ((!myIndex -> serviceLabel. hasName) && (charSet <= 16)) {
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits_8 (d, offset + 8 * i);
	         }
	         myIndex -> serviceLabel. label. append (
	                           toStringUsingCharset (
	                                           (const char *) label,
                                                    (CharacterSet) charSet));
	         myIndex -> serviceLabel. label. append (
	                           toStringUsingCharset (
	                                 " (data)",
                                         (CharacterSet) charSet));
	         addtoEnsemble (myIndex -> serviceLabel. label, SId);
	         myIndex -> serviceLabel. hasName = true;
	      }
	      break;

	   default:
//	      fprintf (stderr, "FIG1/%d: not handled now\n", extension);
	      break;
	}
}
//
//	locate - and create if needed - a reference to the entry
//	for the serviceId serviceId
serviceId	*fib_processor::findServiceId (int32_t serviceId) {
int16_t	i;

	for (i = 0; i < 64; i ++)
	   if ((listofServices [i]. inUse) &&
	        ((int32_t)(listofServices [i]. serviceId) == serviceId))
	      return &listofServices [i];

	for (i = 0; i < 64; i ++)
	   if (!listofServices [i]. inUse) {
	      listofServices [i]. inUse = true;
	      listofServices [i]. serviceLabel. hasName = false;
	      listofServices [i]. serviceId = serviceId;
	      return &listofServices [i];
	   }

	return &listofServices [0];	// should not happen
}


//	bind_audioService is the main processor for - what the name suggests -
void	fib_processor::clearEnsemble () {
int16_t i;
	memset (subChannels, 0, sizeof (subChannels));
	for (i = 0; i < 64; i ++) {
	   listofServices [i]. inUse = false;
	   listofServices [i]. serviceId = -1;
	   listofServices [i]. serviceLabel. label = std::string ();
	   subChannels	[i]. inUse	= false;
	}
}

void	fib_processor::get_channelInfo (channel_data *d, int n) {
	d	-> in_use	= subChannels [n]. inUse;
	d	-> id		= subChannels [n]. id;
	d	-> start_cu	= subChannels [n]. start_cu;
	d	-> protlev	= subChannels [n]. protlev;
	d	-> size		= subChannels [n]. size;
	d	-> bitrate	= subChannels [n]. bitrate;
	d	-> uepFlag	= subChannels [n]. uepFlag;
}

void	fib_processor::get_CIFCount	(int16_t *high, int16_t *lo) {
	*high	= CIFCount_hi;
	*lo	= CIFCount_lo;
}

//      and now for the would-be signals

void    fib_processor::addtoEnsemble    (const std::string &s, int32_t SId) {
	programName (s, SId, userData);
}

void    fib_processor::nameofEnsemble  (int id, const std::string &s) {
	ensembleName (s, id, userData);
        isSynced        = true;
}

void    fib_processor::changeinConfiguration () {
}


