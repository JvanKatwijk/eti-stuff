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

static uint16_t calc_crc (unsigned char *data,
	                  int length,
	                  uint16_t const *crctab, unsigned int crc) { 
int count;
unsigned int temp;

	for (count = 0; count < length; ++count) {
	   temp = (*data++ ^ (crc >> 8)) & 0xff;
	   crc = crctab [temp] ^ (crc << 8);
	}

	return crc & 0xffff;
} 

int16_t cif_In    [55296];
int16_t	cifVector [16][55296];
uint8_t	fibVector [16][96];
bool	fibValid  [16];

#define	CUSize	(4 * 16)

std::vector<protDesc> protTable;

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
	index_Out		= 0;
	BitsperBlock		= 2 * params. get_carriers ();
	numberofblocksperCIF	= 18;	// mode I
	running. store (false);
	amount.  store (0);
	processing. store (false);
	expected_block		= 2;
	CIFCount_hi		= -1;
	CIFCount_lo		= -1;
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
	amount.  store (0);
	processing. store (false);
	expected_block		= 2;
	CIFCount_hi		= -1;
	CIFCount_lo		= -1;
	start ();
}

void	etiGenerator::newFrame		(void) {
}

void	etiGenerator::processBlock	(int16_t *softbits, int16_t blkno) { 
bufferElement s;
	s. blkno = blkno;
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
int16_t	temp [55296];
uint8_t	theVector [6144];
int16_t	Minor	= 0;
const int16_t interleaveMap[] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};

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
	      index_Out	= 0;
	      amount. store (0);
	      Minor	= 0;
	      continue;
	   }
//
//	Note, we officially count from 1 .. params -> L
	   expected_block ++;
	   if (expected_block > params. get_L ()) {
	      expected_block = 2;
	   }
//
//	The blocks 2 .. 4 are the FIC blocks
//	index_Out is the index in the fib/cif combines where to
//	put the next fib/cif.
	   if (2 <= b. blkno && b. blkno <= 4) {
	      memcpy (&(fibInput [(b. blkno - 2) * BitsperBlock]),
	              b. data, 
	              BitsperBlock * sizeof (int16_t));
	      if (b. blkno == 4) {
	         bool	valid [4];
	         uint8_t fibs_bytes [4 * 768];
	         my_ficHandler. process_ficBlock (fibInput. data (),
	                                             fibs_bytes, valid);
	         for (i = 0; i < 4; i ++) {
	            fibValid [index_Out + i] = valid [i];
//	            fprintf (stderr, "fib [%d] = %d\n", i, valid [i]);
	            for (j = 0; j < 96; j ++) {
	               fibVector [(index_Out + i) & 017][j] = 0;
	                  for (k = 0; k < 8; k ++) {
	                     fibVector [(index_Out + i) & 017][j] <<= 1;
	                     fibVector [(index_Out + i) & 017][j] |=
	                                (fibs_bytes [i * 768 + 8 * j + k] & 01);
	               }
	            }
	         }
	         Minor	= 0;
	         my_ficHandler. get_CIFCount (&CIFCount_hi, &CIFCount_lo);
	      }
	      continue;
	   }
//
//	Now for real

//	adding the MSC blocks
	   CIF_index	= (b. blkno - 5) % numberofblocksperCIF;
	   memcpy (&cif_In [CIF_index * BitsperBlock],
	           b. data,
	           BitsperBlock * sizeof (int16_t));
	   if (CIF_index == numberofblocksperCIF - 1) {
	      for (i = 0; i < 3072 * 18; i++) {
	         int index = interleaveMap [i & 017];
	         temp [i] = cifVector [(index_Out + index) & 017] [i];
	         cifVector [index_Out & 0xF] [i] = cif_In [i];
	      }
//	we have to wait until the interleave matrix is filled
	      if (amount. load () < 15) {
	         amount. store (amount. load () + 1);
	         index_Out	= (index_Out + 1) & 017;
//	Minor is introduced to inform the init_eti function
//	anout the CIF number in the dab frame, it runs from 0 .. 3
	         Minor		= 0;
	         continue;
	      }
//
//	Otherwise, it becomes serious
	      if ((CIFCount_hi < 0) || (CIFCount_lo < 0))
	         continue;

	      int offset	= init_eti (theVector, CIFCount_hi,
	                                               CIFCount_lo, Minor);
	      int base		= offset;
	      memcpy (&theVector [offset], fibVector [index_Out], 96);
	      offset += 96;
//
//
//	oef, here we go for handling the CIF
	      if (processing. load ()) {
	         offset	= process_CIF (temp, theVector, offset);
//
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
//
//	Padding
	         memset (&theVector [offset], 0x55, 6144 - offset);
//	         fwrite (theVector, 1, 6144, outputFile);
	         etiWriter (theVector, 6144, userData);
//	 static int cnt	= 0;
//	         fprintf (stderr, "%d\r", ++cnt);
	      }
//	at the end, go for a new eti vector
	      index_Out	= (index_Out + 1) & 017;
	      Minor ++;
	   }
	}
}

//	In process_CIF we iterate over the data in the CIF and map that
//	upon a segment in the eti vector
//
//	Since from the subchannel data we know the location in
//	the input vector, the output vector and the
//	parameters for deconvolution, we can do
//	the processing in parallel. So, for each subchannel
//	we just launch a task
class parameter {
public:
	int16_t	*input;
	bool	uepFlag;
	int	bitRate;
	int	protLevel;
	int	start_cu;
	int	size;
	uint8_t	*output;
};

static void     process_subCh (parameter *p);

int32_t	etiGenerator::process_CIF (int16_t *input,
	                           uint8_t *output, int32_t offset) {
int16_t	i;
std::vector<std::thread> theThreads;
std::vector<parameter *> theParameters;

	for (i = 0; i < 64; i ++) {
	   channel_data data;
	   my_ficHandler. get_channelInfo (&data, i);
	   if (data. in_use) {
	      parameter *t	= new parameter;
	      t -> input	= input;
	      t -> uepFlag	= data. uepFlag;
	      t -> bitRate	= data. bitrate;
	      t -> protLevel	= data. protlev;
	      t -> start_cu	= data. start_cu;
	      t -> size		= data. size;
	      t -> output	= &output [offset];
	      offset 		+= data. bitrate * 24 / 8;
//
//	we need to save a reference to the parameters
//	since we have to delete the instance later on
	      theParameters. push_back (t);
	      theThreads . push_back (std::thread (process_subCh, t));
	   }
	}

	for (std::thread &th : theThreads) {
	   th. join ();
	}
	for (parameter*t: theParameters)
	   delete t;
	return offset;
}

static void	process_subCh (parameter *p) {
uint8_t outVector [24 * p -> bitRate];
int	j, k;

bool	uepFlag		= p -> uepFlag;
int	bitRate		= p -> bitRate;
int	protLevel	= p -> protLevel;
int	size		= p -> size;
int	start_cu	= p -> start_cu;
//protDesc theProtector;
//bool	found		= false;
	memset (outVector, 0, 24 * bitRate);

//	for (int i = 0; i < protTable. size (); i ++) {
//	   if (protTable. at (i). uepFlag != uepFlag) 
//	      continue;
//	   if (protTable. at (i). bitRate != bitRate) 
//	      continue;
//	   if (protTable. at (i). protLevel != protLevel) 
//	      continue;
//	   found = true;
//	   theProtector	= protTable. at (i);
//	   break;
//	}
//
//	if (!found) {
//	   theProtector. uepFlag = uepFlag;
//	   theProtector. bitRate = bitRate;
//	   theProtector. protLevel = protLevel;
//	   if (uepFlag)
//	      theProtector. protector = new uep_protection (bitRate, protLevel);
//	   else
//	      theProtector. protector = new eep_protection (bitRate, protLevel);
//	   protTable. push_back (theProtector);
//	}
//
//	theProtector.
//	         protector -> deconvolve (&p -> input [start_cu * CUSize],
//	                                  size * CUSize,
//	                                  outVector);
	if (uepFlag) {
	   uep_protection uepProtector (bitRate, protLevel);
	   uepProtector. deconvolve (&p -> input [start_cu * CUSize],
	                             size * CUSize,
	                             outVector);
	}
	else {
	   eep_protection  eepProtector (bitRate, protLevel);
	   eepProtector.  deconvolve (&p -> input [start_cu * CUSize],
	                              size * CUSize,
	                              outVector);
	}

	uint8_t	shiftRegister [9];
	memset (shiftRegister, 1, 9);

	for (j = 0; j < 24 * bitRate; j ++) {
	   uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	   for (k = 8; k > 0; k--)
	      shiftRegister [k] = shiftRegister [k - 1];
	   shiftRegister [0] = b;
	   outVector [j] ^= b;
        }
//
//	and the storage:
	for (j = 0; j < 24 * bitRate / 8; j ++) {
	   int temp = 0;
	   for (k = 0; k < 8; k ++)
	      temp = (temp << 1) | (outVector [j * 8 + k] & 01);
	   p -> output [j] = temp;
	}
}

void	etiGenerator::postProcess (uint8_t *theVector, int32_t offset){
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
	                        int16_t CIFCount_hi,
	                        int16_t CIFCount_lo,
	                        int16_t minor) {
int	fillPointer = 0;
int	j;
channel_data data;

	CIFCount_lo += minor;
	if (CIFCount_lo >= 250) {
	   CIFCount_lo = CIFCount_lo % 250;
	   CIFCount_hi ++;
	}
	if (CIFCount_hi >= 20)
	   CIFCount_hi = 20;

//	SYNC()
//	ERR
//	if (fibValid [index_Out + minor])
	   eti [fillPointer ++] = 0xFF;		// error level 0
//	else
//	   eti [fillPointer ++] = 0x0F;		// error level 2, fib errors
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
	eti [fillPointer ++]	= CIFCount_lo; // FCT from CIFCount_lo
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

