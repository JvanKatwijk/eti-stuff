#
/*
 * ## Copyright
 *
 * dabtools is written by Dave Chapman <dave@dchapman.com> 
 *
 * Large parts of the code are copied verbatim (or with trivial
 * modifications) from David Crawley's OpenDAB and hence retain his
 * copyright.
 *
 *	Parts of this software are copied verbatim (or with trivial
 *	Modifications) from David Chapman's dabtools and hence retain
 *	his copyright. In particular, the crc, descramble and init_eti
 *	functions are - apart from naming - a verbatim copy. Thanks
 *	for the nice work
 *
 *    Copyright (C) 2016
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *	partly rewritten and somewhat altered to allow integration
 *	with the already existing parts, taken from dab-cmdline
 *
 *    This file is part of the eti library
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
 */
#
#include	"dab-constants.h"
#include	"eti-generator.h"
#include	"eep-protection.h"
#include	"uep-protection.h"

static uint16_t const crctab_1021[256] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static inline
uint16_t calc_crc (unsigned char *data,
	                  int length,
	                  uint16_t const *crctab, unsigned int crc) { 
int count;
unsigned int temp;

	for (count = 0; count < length; count ++) {
	   temp = (*data++ ^ (crc >> 8)) & 0xff;
	   crc = crctab [temp] ^ (crc << 8);
	}

	return crc & 0xffff;
} 

int16_t	raw_CIF	  [55296];
int16_t cif_In    [55296];
int16_t	cifVector [16][55296];
uint8_t	fibVector [16][96];
bool	fibValid  [4];

#define	CUSize	(4 * 16)

//
//	fibvector contains the processed fics, i.e ready for addition
//	to the eti frame.
//	Since there are 4 fibs, one for each CIF, the index to be used for
//	filling - and processing - indicates at the end of the frame
//	again the last fib.
//	In between there are more fib fields filled than CIFS
//
//	Since the odpprocessor (together with the decoder) takes quite an
//	amount of cycles, the eti-generation is done in a different thread
//	Note CIF counts from 0 .. 3
//
		etiGenerator::etiGenerator	(uint8_t	dabMode,
	                                         void		*userData,
	                                         ensemblename_t ensembleName,
	                                         programname_t	programName,
	                                         fibquality_t	set_fibQuality,
	                                         etiwriter_t	etiWriter):
	                                            params (dabMode),
	                                            my_ficHandler (&params,
	                                                           userData,
	                                                           ensembleName,
	                                                           programName,
	                                                           set_fibQuality) {
	this	-> userData	= userData;
	this	-> etiWriter	= etiWriter;
	
	fibInput		.resize (3 * 2 * params. get_carriers ());
	dataBuffer		= new RingBuffer<bufferElement> (512);
	index_In		= 0;
	BitsperBlock		= 2 * params. get_carriers ();
	numberofblocksperCIF	= 18;	// mode I
	running. store (false);
	deinterleaverFilled	= 0;
	processing. store (false);
	expected_block		= 2;
	currentCIF		= 0;
	lastGenerated		= -1;
	start ();
}

		etiGenerator::~etiGenerator	(void) {
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
	delete		dataBuffer;
}

void	etiGenerator::start	(void) {
	running. store (true);
        threadHandle    = std::thread (&etiGenerator::run, this);
}

//
void	etiGenerator::reset	(void) { 
	if (running. load ()) {
	   running. store (false);
	   threadHandle. join ();
	}
	running. store (false);
	deinterleaverFilled	= 0;
	processing. store (false);
	expected_block		= 2;
	currentCIF		= 0;
	lastGenerated		= -1;
	start ();
}

void	etiGenerator::newFrame		(void) {
}

void	etiGenerator::processBlock	(int16_t *softbits,
	                                 int16_t blkno, bool syncFlag) { 
bufferElement s;
	s. blkno	= blkno;
	s. syncFlag	= syncFlag;
	memcpy (s. data, softbits,
	                 2 * params. get_carriers () * sizeof (int16_t));
	while (dataBuffer ->  GetRingBufferWriteAvailable () < 1) 
	   usleep (1000);
	dataBuffer -> putDataIntoBuffer (&s, 1);
}

void	etiGenerator::run		(void) {
int	i, j, k;
bufferElement b;
int16_t	CIF_index;
int CIFCount_hi, CIFCount_lo;

	running. store (true);
	while (running. load ()) {
	   while (running. load () &&
	             (dataBuffer -> GetRingBufferReadAvailable () < 1)) 
	      usleep (1000);
	   if (!running. load ())
	      break;

	   dataBuffer -> getDataFromBuffer (&b, 1);
	   if (b. blkno != expected_block) {
	      fprintf (stderr, "got %d, expected %d\n", b.blkno, expected_block);
	      expected_block = 2;
	      index_In	= 0;
	      deinterleaverFilled	= 0;
	      continue;
	   }
//
//	Note, we officially count from 1 .. params -> L
	   expected_block ++;
	   if (expected_block > params. get_L ()) {
	      expected_block = 2;
	   }

	   switch (b. blkno) {
	      case 2:
	      case 3:
	         memcpy (&(fibInput [(b. blkno - 2) * BitsperBlock]),
	                 b. data, 
	                 BitsperBlock * sizeof (int16_t));
	         break;

	      case 4:
	         memcpy (&(fibInput [(b. blkno - 2) * BitsperBlock]),
	                 b. data, 
	                 BitsperBlock * sizeof (int16_t));
//	we have the 3 blocks with FIC data in,
//	let us decode the 4 FIC blocks
	         bool	valid [4];
	         uint8_t fibs_bytes [4 * 768];
	         my_ficHandler. process_ficBlock (fibInput. data (),
	                                             fibs_bytes, valid);
	         for (i = 0; i < 4; i ++) {
	            fibValid [i] = valid [i];
//	            fprintf (stderr, "fib [%d] = %d\n", i, valid [i]);
	            for (j = 0; j < 96; j ++) {
	               fibVector [(index_In + i) & 017][j] = 0;
	                  for (k = 0; k < 8; k ++) {
	                     fibVector [(index_In + i) & 017][j] <<= 1;
	                     fibVector [(index_In + i) & 017][j] |=
	                                (fibs_bytes [i * 768 + 8 * j + k] & 01);
	               }
	            }
	         }

	         my_ficHandler. get_CIFCount (&CIFCount_hi, &CIFCount_lo);
	         currentCIF	= CIFCount_hi * 250 + CIFCount_lo;
	         break;

//	Now for real
//	adding the MSC blocks
	      case  5: case  6: case  7: case  8: case  9: case 10:
	      case 11: case 12: case 13: case 14: case 15: case 16:
	      case 17: case 18: case 19: case 20: case 21: 
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));
	         break;
//
//	Laast block for first CIF
	      case 22:
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));

	         process_CIF (currentCIF, 0);
	         break;

	      case 23: case 24: case 25: case 26: case 27: case 28:
	      case 29: case 30: case 31: case 32: case 33: case 34:
	      case 35: case 36: case 37: case 38: case 39: 
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));
	         break;
//
//	last block for second CIF
	      case 40:
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));

	         process_CIF (currentCIF, 1);
	         break;

	      case 41: case 42: case 43: case 44: case 45: case 46:
	      case 47: case 48: case 49: case 50: case 51: case 52:
	      case 53: case 54: case 55: case 56: case 57: 
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));
	         break;

//	last block for CIF 3
	      case 58:
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));

	         process_CIF (currentCIF, 2);
	         break;

	      case 59: case 60: case 61: case 62: case 63: case 64:
	      case 65: case 66: case 67: case 68: case 69: case 70:
	      case 71: case 72: case 73: case 74: case 75: 
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));
	         break;

//	last block for CIF 4
	      case 76:
	         CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	         memcpy (&cif_In [CIF_index * BitsperBlock],
	                 b. data,
	                 BitsperBlock * sizeof (int16_t));

	         process_CIF (currentCIF, 3);
	         break;
	   }
	}
}

//
//	The 
void	etiGenerator::process_CIF (int currentCIF, int minor) {
int32_t	i;
const int16_t interleaveMap[] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
uint8_t theVector [6144];

	if (currentCIF < 0) {	// some error
	   index_In = (index_In + 1) & 0xF;
	   return;
	}

//
//	always handle the de-interleaving
	for (i = 0; i < 3072 * 18; i++) {
	   int index = interleaveMap [i & 017];
	   raw_CIF [i] = cifVector [(index_In + index) & 017] [i];
	   cifVector [index_In & 0xF] [i] = cif_In [i];
	}

//	we have to wait until the interleave matrix is filled
	if (deinterleaverFilled < 15) {
	   deinterleaverFilled ++;
	   index_In	= (index_In + 1) & 017;
	   return;
	}
//
//	oef, here we go for handling the CIF
	if (!processing. load ()) {
	   index_In	= (index_In + 1) % 0xF;
	   return;
	}
//
//	we assume we have a sufficient amount of correct data
//	in the FIB handlers, so this might be a little superfluous
//	if (!fibValid [minor]) {
//	   index_In	= (index_In + 1) & 017;
//	   return;
//	}

	if (lastGenerated == -1)
	   lastGenerated = (currentCIF + minor) % 5000;
	else	// a consequence of erroneous fibs might be:
	if ((currentCIF + minor) % 5000 < (lastGenerated + 1) % 5000) {
	   index_In = (index_In + 1) % 5000;
	   return;
	}
	else {
	   if ((currentCIF + minor) % 5000 > (lastGenerated + 1) % 5000) {
//	we are missing a number of frames, insert error frames
	      lastGenerated = (lastGenerated + 1) % 5000;
	      fprintf (stderr, "currentCIF = %d, lastGenerated = %d\n",
	                          currentCIF + minor, lastGenerated);
	      while (lastGenerated != (currentCIF + minor) % 5000) {
	         generate_error_Frame (theVector, lastGenerated);
	         postProcess (theVector, 6144);
	         lastGenerated = (lastGenerated + 1) % 5000;
	      }
	   }
	   lastGenerated = (currentCIF + minor) % 5000;
	}

	generate_ETI_Frame (theVector, currentCIF, minor);
	postProcess (theVector, 6144);
//
//	and prepare for the next one from the deinterleaving matrix
	index_In	= (index_In + 1) & 0xF;
}
//
//	The ETI frame we are generating is "minor" out of 4,
void	etiGenerator::generate_ETI_Frame (uint8_t *theVector,
	                                  int32_t  currentCIF,
	                                  int32_t  minor) {
	int offset	= init_eti (theVector, currentCIF, minor);
	int base	= offset;	// for use later on
//
//	first copy the fib vector
	memcpy (&theVector [offset], fibVector [(index_In + minor) & 0xF], 96);
	offset += 96;
//

	offset	= process_CIF_data (raw_CIF, theVector, offset);

//	and now the header
//	EOF - CRC
//	The "data bytes" are stored in the range base .. offset
	int crc = calc_crc (&(theVector [base]),
	                    offset - base,
	                    crctab_1021, 0xFFFF);
	crc = ~crc;
	theVector [offset ++] = (crc & 0xFF00) >> 8;
	theVector [offset ++] = crc & 0xFF;
//
//	EOF - RFU
	theVector [offset ++] = 0xFF;
	theVector [offset ++] = 0xFF;
//
//	TIST	- 0xFFFFFFFF means time stamp not used
	theVector [offset ++] = 0xFF;
	theVector [offset ++] = 0xFF;
	theVector [offset ++] = 0xFF;
	theVector [offset ++] = 0xFF;

//      Padding
        memset (&theVector [offset], 0x55, 6144 - offset);
}

//
//	In process_MSC we iterate over the data in the CIF and map that
//	upon a segment in the eti vector
int32_t	etiGenerator::process_CIF_data (int16_t *input,
	                                uint8_t *output,
	                                int32_t offset) {
int16_t	i, j, k;
uint8_t	shiftRegister [9];

	for (i = 0; i < 64; i ++) {
	   channel_data data;
	   my_ficHandler. get_channelInfo (&data, i);
	   if (!data. in_use)
	      continue;

	   uint8_t outVector [24 * data. bitrate];
	   memset (outVector, 0, 24 * data. bitrate);

//	Apply appropriate depuncturing/deconvolution for each subchannel 
//
//	An "optimization" here could be to maintain a "cache" of
//	xep_protection handlers, since it is most likely that
//	more than one service is protected with the same parameters.
//	(Note time deinterleaving is done already)
	   if (data. uepFlag) {
	      uep_protection uep_deconvolver (data. bitrate, data. protlev);
	      uep_deconvolver.
	          deconvolve (&input [data. start_cu * CUSize],
	                      data. size * CUSize,
	                      outVector);
	   }
	   else {
	      eep_protection eep_deconvolver (data. bitrate,
	                                                data. protlev);
	      eep_deconvolver.
	         deconvolve (&input [data. start_cu * CUSize],
	                     data. size * CUSize,
	                     outVector);
	   }
//
//	What remains is dispersion of the bits, packing then as bytes
//	and adding these bytes to the output vector.
	   memset (shiftRegister, 1, 9);
           for (j = 0; j < 24 * data. bitrate; j ++) {
              uint8_t b = shiftRegister [8] ^ shiftRegister [4];
              for (k = 8; k > 0; k--)
                 shiftRegister [k] = shiftRegister [k - 1];
              shiftRegister [0] = b;
              outVector [j] ^= b;
           }
	   for (j = 0; j < 24 * data. bitrate / 8; j ++) {
              int temp = 0;
              for (k = 0; k < 8; k ++)
                 temp = (temp << 1) | (outVector [j * 8 + k] & 01);
              output [offset + j] = temp;
	   }
	   offset += 24 * data. bitrate / 8;
	}
	return offset;
}


void	etiGenerator::postProcess (uint8_t *theVector, int32_t size){
	etiWriter (theVector, 6144, userData);
}

void	etiGenerator::startProcessing	(void) {
	processing. store (true);
	fprintf (stderr, "yes, here we go\n");
	amount. store (0);
}

void	etiGenerator::stop	(void) {
	processing. store (false);
	if (outputFile != NULL)
	   fclose (outputFile);
	outputFile	= NULL;
}

//	Copied  from dabtools:
int32_t	etiGenerator::init_eti (uint8_t* eti,
	                        int16_t currentCIF,
	                        int16_t minor) {
int	fillPointer = 0;
int	j;
channel_data data;
	currentCIF	= (currentCIF + minor) % 5000;
int	CIFCount_hi	= currentCIF / 250;
int	CIFCount_lo	= currentCIF % 250;

//	SYNC()
//	ERR
	if (fibValid [minor])
	   eti [fillPointer ++] = 0xFF;		// error level 0
	else
	   eti [fillPointer ++] = 0x0F;		// error level 2, fib errors
//	FSYNC
	if (CIFCount_lo & 1) {
	   eti [fillPointer ++] = 0xf8;
	   eti [fillPointer ++] = 0xc5;
	   eti [fillPointer ++] = 0x49;
	} else {
	   eti [fillPointer ++] = 0x07;
	   eti [fillPointer ++] = 0x3a;
	   eti [fillPointer ++] = 0xb6;
	}
//	LIDATA()
//	FC()
	eti [fillPointer ++]	= CIFCount_lo;  // FCT from CIFCount_lo
	int FICF	= 1;			// FIC present in MST
	int NST		= 0;			// number of streams
	int FL		= 0;			// Frame Length
	for (j = 0; j < 64; j++) {
	   my_ficHandler. get_channelInfo (&data, j);
	   if (data. in_use) {
	      NST++;
	      FL += (data. bitrate * 3) / 4;		// words remember
	   }
 	}
//
	FL	+= NST + 1 + 24; // STC + EOH + MST (FIC data, Mode 1!)
	eti [fillPointer ++] = (FICF << 7) | NST;
//
//	The FP is computed as remainder of the total CIFCount,
	uint8_t FP = ((CIFCount_hi * 250) + CIFCount_lo) % 8;
//
	int MID	= 0x01; // We only support Mode 1
	eti [fillPointer ++] = (FP << 5) | (MID << 3) | ((FL & 0x700) >> 8);
 	eti [fillPointer ++] = FL & 0xff;
//	Now for each of the streams in the FIC we add information
//	on how to get it
//	STC ()
	for (j = 0; j < 64; j++) {
	   my_ficHandler. get_channelInfo (&data, j);
	   if (data. in_use) {
	      int SCID	= data. id;
	      int  SAD	= data. start_cu;
	      int  TPL;
	      if (data. uepFlag) 
	         TPL = 0x10 | (data. protlev - 1);
	      else
	         TPL = 0x20 | data. protlev;
	      int STL = data. bitrate * 3 / 8;
	      eti [fillPointer ++] = (SCID << 2) | ((SAD & 0x300) >> 8);
	      eti [fillPointer ++] = SAD & 0xFF;
	      eti [fillPointer ++] = (TPL << 2) | ((STL & 0x300) >> 8);
	      eti [fillPointer ++] = STL & 0xFF;
	   }
	}
//	EOH ()
//	MNSC
	eti [fillPointer ++] = 0xFF;
	eti [fillPointer ++] = 0xFF;
//	HCRC
	int HCRC = calc_crc (&eti [4], fillPointer - 4, crctab_1021, 0xffff);
	HCRC =~ HCRC;
	eti [fillPointer ++] = (HCRC & 0xff00) >> 8;
	eti [fillPointer ++] = HCRC & 0xff;

	return fillPointer;
}

void	etiGenerator::generate_error_Frame (uint8_t *eti,
	                                    int32_t lastGenerated) {
int	fillPointer = 0;
int	j;
int	CIFCount_hi	= lastGenerated / 250;
int	CIFCount_lo	= lastGenerated % 250;

//	SYNC()
//	ERR
	eti [fillPointer ++] = 0x00;		// error level 3, unusable frame
//	FSYNC
	if (CIFCount_lo & 1) {
	   eti [fillPointer ++] = 0xf8;
	   eti [fillPointer ++] = 0xc5;
	   eti [fillPointer ++] = 0x49;
	} else {
	   eti [fillPointer ++] = 0x07;
	   eti [fillPointer ++] = 0x3a;
	   eti [fillPointer ++] = 0xb6;
	}
//	LIDATA()
//	FC()
	eti [fillPointer ++]	= CIFCount_lo;  // FCT from CIFCount_lo
	memset (&eti [fillPointer], 0xFF, 6144 - fillPointer);
}

