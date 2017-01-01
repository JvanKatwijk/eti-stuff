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
 * 	The deconvolution for both uep and eep
 */
#include	"dab-constants.h"
#include	"deconvolve.h"
#include	"protTables.h"

struct protectionProfile {
	int16_t	bitRate;
	int16_t	subChannelsize;
	int16_t	protLevel;
	int16_t L1;
	int16_t	L2;
	int16_t	L3;
	int16_t	L4;
	int16_t	PI1;
	int16_t	PI2;
	int16_t	PI3;
	int16_t	PI4;
} profileTable [] = {
	{32,	16,	5,	3, 4, 17, 0,	5, 3, 2, -1},
	{32,	21,	4,	3, 3, 18, 0,	11, 6, 5, -1},
	{32,	24,	3,	3, 4, 14, 3,	15, 9, 6, 8},
	{32,	29,	2,	3, 4, 14, 3,	22, 13, 8, 13},
	{32,	35,	1,	3, 5, 13, 3,	24, 17, 12, 17},

	{48,	24,	5,	4, 3, 26, 3,	5, 4, 2, 3},
	{48,	29,	4,	3, 4, 26, 3,	9, 6, 4, 6},
	{48,	35,	3,	3, 4, 26, 3,	15, 10, 6, 9},
	{48,	42,	2,	3, 4, 26, 3,	24, 14, 8, 15},
	{48,	52,	1,	3, 5, 25, 3,	24, 18, 13, 18},

	{56,	29,	5,	6, 10, 23, 3,	4, 3, 1, 2},
        {56,	35,	4,	6, 10, 23, 3,   8, 5, 3, 4},
        {56,	42,	3,	6, 12, 21, 3,	15, 6, 5, 8},
        {56,	52,	2,	6, 10, 23, 3,	22, 12, 7, 12},

	{64,	32,	5,	6, 9, 31, 2,	5, 3, 2, 3},
	{64,	42,	4,	6, 9, 33, 0,	11, 6, 6, -1},
	{64,	48,	3,	6, 12, 27, 3,	16, 8, 6, 9},
	{64,	58,	2,	6, 10, 29, 3,	23, 13, 8, 13},
	{64,	70,	1,	6, 11, 28, 3,	24, 18, 12, 18},

	{80,	40,	5,	6, 10, 41, 3,	6, 3, 2, 3},
	{80,	52,	4,	6, 10, 41, 3,	11, 6, 5, 6},
	{80,	58,	3,	6, 11, 40, 3,	16, 8, 6, 7},
	{80,	70,	2,	6, 10, 41, 3,	23, 13, 8, 13},
	{80,	84,	1,	6, 10, 41, 3,	24, 7, 12, 18},

	{96,	48,	5,	7, 9, 53, 3,	5, 4, 2, 4},
	{96,	58,	4,	7, 10, 52, 3,	9, 6, 4, 6},
	{96,	70,	3,	6, 12, 51, 3,	16, 9, 6, 10},
	{96,	84,	2,	6, 10, 53, 3,	22, 12, 9, 12},
	{96,	104,	1,	6, 13, 50, 3,	24, 18, 13, 19},
//
//	Thanks to Kalle Riis, who found that the "112" was missing
	{112,	58,	5,	14, 17, 50, 3,	5, 4, 2, 5},
	{112,	70,	4,	11, 21, 49, 3,	9, 6, 4, 8},
	{112,	84,	3,	11, 23, 47, 3,	16, 8, 6, 9},
	{112,	104,	2,	11, 21, 49, 3,	23, 12, 9, 14},

	{128,	64,	5,	12, 19, 62, 3,	5, 3, 2, 4},
	{128,	84,	4,	11, 21, 61, 3,	11, 6, 5, 7},
	{128,	96,	3,	11, 22, 60, 3,	16, 9, 6, 10},
	{128,	116,	2,	11, 21, 61, 3,	22, 12, 9, 14},
	{128,	140,	1,	11, 20, 62, 3,	24, 17, 13, 19},

	{160,	80,	5,	11, 19, 87, 3,	5, 4, 2, 4},
	{160,	104,	4,	11, 23, 83, 3,	11, 6, 5, 9},
	{160,	116,	3,	11, 24, 82, 3,	16, 8, 6, 11},
	{160,	140,	2,	11, 21, 85, 3,	22, 11, 9, 13},
	{160,	168,	1,	11, 22, 84, 3,	24, 18, 12, 19},

	{192,	96,	5,	11, 20, 110, 3,	6, 4, 2, 5},
	{192,	116,	4,	11, 22, 108, 3,	10, 6, 4, 9},
	{192,	140,	3,	11, 24, 106, 3, 16, 10, 6, 11},
	{192,	168,	2,	11, 20, 110, 3, 22, 13, 9, 13},
	{192,	208,	1,	11, 21, 109, 3,	24, 20, 13, 24},

	{224,	116,	5,	12, 22, 131, 3,	8,  6, 2, 6},
	{224,	140,	4,	12, 26, 127, 3,	12, 8, 4, 11},
	{224,	168,	3,	11, 20, 134, 3, 16, 10, 7, 9},
	{224,	208,	2,	11, 22, 132, 3,	24, 16, 10, 15},
	{224,	232,	1,	11, 24, 130, 3,	24, 20, 12, 20},

	{256,	128,	5,	11, 24, 154, 3,	6, 5, 2, 5},
	{256,	168,	4,	11, 24, 154, 3,	12, 9, 5, 10},
	{256,	192,	3,	11, 27, 151, 3,	16, 10, 7, 10},
	{256,	232,	2,	11, 22, 156, 3,	24, 14, 10, 13},
	{256,	280,	1,	11, 26, 152, 3,	24, 19, 14, 18},

	{320,	160,	5,	11, 26, 200, 3,	8, 5, 2, 6},
	{320,	208,	4,	11, 25, 201, 3,	13, 9, 5, 10},
	{320,	280,	2,	11, 26, 200, 3,	24, 17, 9, 17},
	
	{384,	192,	5,	11, 27, 247, 3,	8, 6, 2, 7},
	{384,	280,	3,	11, 24, 250, 3,	16, 9, 7, 10},
	{384,	416,	1,	12, 28, 245, 3,	24, 20, 14, 23},
	{0,	-1,	-1, -1, -1, -1,	-1, -1, -1, -1}
};

static
uint8_t	PI_X [24] = {
	1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
	1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0
};

static
int16_t	findIndex (int16_t bitRate, int16_t protLevel) {
int16_t	i;

	for (i = 0; profileTable [i].bitRate != 0; i ++)
	   if ((profileTable [i]. bitRate == bitRate) &&
	       (profileTable [i]. protLevel == protLevel))
	      return i;
	return -1;
}

/**
  *	the table is based on chapter 11 of the DAB standard.
  *
  *	\brief uep_deconvolve
  *
  *	The bitRate and the protectionLevel determine the 
  *	depuncturing scheme.
  */
	uep_deconvolve::uep_deconvolve (int16_t bitRate,
	                                int16_t protLevel):
	                                  viterbi (24 * bitRate) {
int16_t	index;

	this	-> bitRate		= bitRate;
	index	= findIndex (bitRate, protLevel);
	if (index == -1) {
	   fprintf (stderr, "%d (%d) has a problem\n", bitRate, protLevel);
	   index = 1;
	}

	outSize		= 24 * bitRate;
	viterbiBlock	= new int16_t [outSize * 4 + 24];
	L1	= profileTable [index]. L1;
	L2	= profileTable [index]. L2;
	L3	= profileTable [index]. L3;
	L4	= profileTable [index]. L4;

	PI1	= get_PCodes (profileTable [index]. PI1 -1);
	PI2	= get_PCodes (profileTable [index]. PI2 -1);
	PI3	= get_PCodes (profileTable [index]. PI3 -1);
	if ((profileTable [index]. PI4 - 1) != -1)
	   PI4	= get_PCodes (profileTable [index]. PI4 -1);
	else
	   PI4	= NULL;
}

	uep_deconvolve::~uep_deconvolve (void) {
	delete[]	viterbiBlock;
}

bool	uep_deconvolve::deconvolve (int16_t *v,
	                            int32_t size, uint8_t *outBuffer) {
int16_t	i, j;
int16_t	inputCounter	= 0;
int32_t	viterbiCounter	= 0;
	(void)size;			// currently unused

//	according to the standard we process the logical frame
//	with a pair of tuples
//	(L1, PI1), (L2, PI2), (L3, PI3), (L4, PI4)

///	clear the bits in the viterbiBlock,
///	only the non-punctured ones are set
	memset (viterbiBlock, 0, (outSize * 4 + 24) * sizeof (int16_t)); 
	for (i = 0; i < L1; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI1 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;
	   }
	}

	for (i = 0; i < L2; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI2 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;
	   }
	}

	for (i = 0; i < L3; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI3 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;	
	   }
	}

	for (i = 0; i < L4; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI4 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;	
	   }
	}

/**
  *	we have a final block of 24 bits  with puncturing according to PI_X
  *	This block constitues the 6 * 4 bits of the register itself.
  */
	for (i = 0; i < 24; i ++) {
	   if (PI_X [i] == 1)  {
	      viterbiBlock [viterbiCounter] = v [inputCounter ++];
	   }
	   viterbiCounter ++;
	}
//
///	The actual deconvolution is done by the viterbi decoder

	viterbi::deconvolve (viterbiBlock, outBuffer);
	return true;
}

/**
  *	\brief eep_deconvolve
  *	equal error protection, bitRate and protLevel
  *	define the puncturing table
  */
	eep_deconvolve::eep_deconvolve (int16_t bitRate,
	                                int16_t protLevel):viterbi (24 * bitRate) {
	this	-> bitRate = bitRate;
	outSize		= 24 * bitRate;
	viterbiBlock	= new int16_t [outSize * 4 + 24];
	if ((protLevel & 04) == 0) {	// set A profiles
	   switch (protLevel & 07) {
	      case 1:
	         L1	= 6 * bitRate / 8 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (24 - 1);
	         PI2	= get_PCodes (23 - 1);
	         break;

	      case 2:
	         if (bitRate == 8) {
	            L1	= 5;
	            L2	= 1;
	            PI1	= get_PCodes (13 - 1);
	            PI2	= get_PCodes (12 - 1);
	         } else {
	            L1	= 2 * bitRate / 8 - 3;
	            L2	= 4 * bitRate / 8 + 3;
	            PI1	= get_PCodes (14 - 1);
	            PI2	= get_PCodes (13 - 1);
	         }
	         break;

	      case 3:
	         L1	= 6 * bitRate / 8 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (8 - 1);
	         PI2	= get_PCodes (7 - 1);
	         break;

	      case 4:
	         L1	= 4 * bitRate / 8 - 3;
	         L2	= 2 * bitRate / 8 + 3;
	         PI1	= get_PCodes (3 - 1);
	         PI2	= get_PCodes (2 - 1);
	         break;
	   }
	}
	else
	if (protLevel & 004) {		// B series
	   switch ((protLevel & 07)) {
	      case 4:
	         L1	= 24 * bitRate / 32 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (2 - 1);
	         PI2	= get_PCodes (1 - 1);
	         break;

	      case 3:
	         L1	= 24 * bitRate / 32 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (4 - 1);
	         PI2	= get_PCodes (3 - 1);
	         break;

	      case 2:
	         L1	= 24 * bitRate / 32 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (6 - 1);
	         PI2	= get_PCodes (5 - 1);
	         break;

	      case 1:
	         L1	= 24 * bitRate / 32 - 3;
	         L2	= 3;
	         PI1	= get_PCodes (10 - 1);
	         PI2	= get_PCodes (9 - 1);
	         break;
	   }
	}
}

	eep_deconvolve::~eep_deconvolve (void) {
	delete[]	viterbiBlock;
}

bool	eep_deconvolve::deconvolve (int16_t *v,
	                            int32_t size, uint8_t *outBuffer) {

int16_t	i, j;
int32_t	inputCounter	= 0;
int32_t	viterbiCounter	= 0;
	(void)size;			// currently unused
	memset (viterbiBlock, 0, (outSize * 4 + 24) * sizeof (int16_t)); 
//
//	according to the standard we process the logical frame
//	with a pair of tuples
//	(L1, PI1), (L2, PI2), (L3, PI3), (L4, PI4)
//
	for (i = 0; i < L1; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI1 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;	
	   }
	}

	for (i = 0; i < L2; i ++) {
	   for (j = 0; j < 128; j ++) {
	      if (PI2 [j % 32] == 1) {
	         viterbiBlock [viterbiCounter] = v [inputCounter ++];
	      }
	      viterbiCounter ++;	
	   }
	}
//	we had a final block of 24 bits  with puncturing according to PI_X
//	This block constitues the 6 * 4 bits of the register itself.
	for (i = 0; i < 24; i ++) {
	   if (PI_X [i] == 1) {
	      viterbiBlock [viterbiCounter] = v [inputCounter ++];
	   }
	   viterbiCounter ++;
	}
	viterbi::deconvolve (viterbiBlock, outBuffer);
	return true;
}

