#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
	fib_processor::fib_processor (void		*userData,
	                              ensemblename_t	ensembleName,
	                              programname_t	programName) {
	this	-> userData	= userData;
	this	-> ensembleName	= ensembleName;
	this	-> programName	= programName;
	memset (dateTime, 0, sizeof (dateTime));
	dateFlag	= false;
	selectedService		= -1;
	clearEnsemble	();
	CIFCount_hi	= -1;
	CIFCount_lo	= -1;
}
	
	fib_processor::~fib_processor (void) {
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
//	Handle ensemble is all through FIG0
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

	   case 2:
	      FIG0Extension2 (d);
	      break;

	   case 3:
	      FIG0Extension3 (d);
	      break;

	   case 5:
	      FIG0Extension5 (d);
	      break;

	   case 8:
	      FIG0Extension8 (d);
	      break;

	   case 9:
	      FIG0Extension9 (d);
	      break;

	   case 10:
	      FIG0Extension10 (d);
	      break;

	   case 14:
	      FIG0Extension14 (d);
	      break;

	   case 13:
	      FIG0Extension13 (d);
	      break;

	   case 16:
	      FIG0Extension16 (d);
	      break;

	   case 17:
	      FIG0Extension17 (d);
	      break;

	   case 18:
	      FIG0Extension18 (d);
	      break;

	   case 19:
	      FIG0Extension19 (d);
	      break;

	   case 21:
	      FIG0Extension21 (d);
	      break;

	   case 22:
	      FIG0Extension22 (d);
	      break;

	   default:
//	      fprintf (stderr, "FIG0/%d passed by\n", extension);
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
//	
void	fib_processor::FIG0Extension2 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
uint8_t	PD_bit	= getBits_1 (d, 8 + 2);
uint8_t	CN	= getBits_1 (d, 8 + 0);

	while (used < Length) {
	   used = HandleFIG0Extension2 (d, used, CN, PD_bit);
	}
}
//
//	Note Offset is in bytes
//	With FIG0/2 we bind the channels to Service Ids
//
int16_t	fib_processor::HandleFIG0Extension2 (uint8_t *d,
	                                     int16_t offset,
	                                     uint8_t cn,
	                                     uint8_t pd) {
int16_t		lOffset	= 8 * offset;
int16_t		i;
uint8_t		ecc;
uint8_t		cId;
uint32_t	SId;
int16_t		numberofComponents;

	(void)cn;
	if (pd == 1) {		// long Sid
	   ecc	= getBits_8 (d, lOffset);	(void)ecc;
	   cId	= getBits_4 (d, lOffset + 1);
	   SId	= getLBits (d, lOffset, 32);
	   lOffset	+= 32;
	}
	else {
	   cId	= getBits_4 (d, lOffset);	(void)cId;
	   SId	= getBits (d, lOffset + 4, 12);
	   SId	= getBits (d, lOffset, 16);
	   lOffset	+= 16;
	}

	numberofComponents	= getBits_4 (d, lOffset + 4);
	lOffset	+= 8;

	for (i = 0; i < numberofComponents; i ++) {
	   uint8_t	TMid	= getBits_2 (d, lOffset);
	   if (TMid == 00)  {	// Audio
	      uint8_t	ASCTy	= getBits_6 (d, lOffset + 2);
	      uint8_t	SubChId	= getBits_6 (d, lOffset + 8);
	      uint8_t	PS_flag	= getBits_1 (d, lOffset + 14);
	      bind_audioService (TMid, SId, i, SubChId, PS_flag, ASCTy);
	   }
	   else
	   if (TMid == 3) { // MSC packet data
	      int16_t SCId	= getBits (d, lOffset + 2, 12);
	      uint8_t PS_flag	= getBits_1 (d, lOffset + 14);
	      uint8_t CA_flag	= getBits_1 (d, lOffset + 15);
	      bind_packetService (TMid, SId, i, SCId, PS_flag, CA_flag);
           }
	   else
	      {;}		// for now
	   lOffset += 16;
	}
	return lOffset / 8;		// in Bytes
}

//      The Extension 3 of FIG type 0 (FIG 0/3) gives
//      additional information about the service component
//      description in packet mode.
//      manual: page 55
void	fib_processor::FIG0Extension3 (uint8_t *d) {
int16_t	used	= 2;
int16_t	Length	= getBits_5 (d, 3);

	while (used < Length)
	   used = HandleFIG0Extension3 (d, used);
}

//
//      DSCTy   DataService Component Type
int16_t fib_processor::HandleFIG0Extension3 (uint8_t *d, int16_t used) {
int16_t SCId            = getBits (d, used * 8, 12);
int16_t CAOrgflag       = getBits_1 (d, used * 8 + 15);
int16_t DGflag          = getBits_1 (d, used * 8 + 16);
int16_t DSCTy           = getBits_6 (d, used * 8 + 18);
int16_t SubChId         = getBits_6 (d, used * 8 + 24);
int16_t packetAddress   = getBits (d, used * 8 + 30, 10);
uint16_t        CAOrg   = getBits (d, used * 8 + 40, 16);

serviceComponent *packetComp = find_packetComponent (SCId);

	(void)CAOrgflag;
	(void)CAOrg;
        used += 56 / 8;
        if (packetComp == NULL)		// no serviceComponent yet
           return used;
        packetComp      -> subchannelId = SubChId;
        packetComp      -> DSCTy        = DSCTy;
	packetComp	-> DGflag	= DGflag;
        packetComp      -> packetAddress        = packetAddress;
        return used;
}

void	fib_processor::FIG0Extension5 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);

	while (used < Length) {
	   used = HandleFIG0Extension5 (d, used);
	}
}

int16_t	fib_processor::HandleFIG0Extension5 (uint8_t* d, int16_t offset) {
int16_t	loffset	= offset * 8;
uint8_t	lsFlag	= getBits_1 (d, loffset);
int16_t	subChId, serviceComp, language;

	if (lsFlag == 0) {	// short form
	   if (getBits_1 (d, loffset + 1) == 0) {
	      subChId	= getBits_6 (d, loffset + 2);
	      language	= getBits_8 (d, loffset + 8);
	   }
	   loffset += 16;
	}
	else {			// long form
	   serviceComp	= getBits (d, loffset + 4, 12);
	   language	= getBits_8 (d, loffset + 16);
	   loffset += 24;
	}
	(void)serviceComp;
	(void)language;
	(void)subChId;
	return loffset / 8;
}

void	fib_processor::FIG0Extension8 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) {
	   used = HandleFIG0Extension8 (d, used, PD_bit);
	}
}

int16_t	fib_processor::HandleFIG0Extension8 (uint8_t *d, int16_t used,
	                                     uint8_t pdBit) {
int16_t	lOffset	= used * 8;
uint32_t	SId	= getLBits (d, lOffset, pdBit == 1 ? 32 : 16);
uint8_t		lsFlag;
uint16_t	SCIds;
int16_t		SCid;
int16_t		MSCflag;
int16_t		SubChId;
uint8_t		extensionFlag;

	lOffset += pdBit == 1 ? 32 : 16;
        extensionFlag   = getBits_1 (d, lOffset);
        SCIds   = getBits_4 (d, lOffset + 4);
        lOffset += 8;

        lsFlag  = getBits_1 (d, lOffset + 8);
        if (lsFlag == 1) {
           SCid = getBits (d, lOffset + 4, 12);
           lOffset += 16;
//           if (find_packetComponent ((SCIds << 4) | SCid) != NULL) {
//              fprintf (stderr, "packet component bestaat !!\n");
//           }
        }
	else {
	   MSCflag	= getBits_1 (d, lOffset + 1);
	   SubChId	= getBits_6 (d, lOffset + 2);
	   lOffset += 8;
	}
	if (extensionFlag)
	   lOffset += 8;	// skip Rfa
	(void)SId;
	(void)SCIds;
	(void)SCid;
	(void)SubChId;
	(void)MSCflag;
	return lOffset / 8;
}
//
//	FIG0/9 and FIG0/10 are copied from the work of
//	Michael Hoehn
void fib_processor::FIG0Extension9 (uint8_t *d) {
int16_t	offset	= 16;

	dateTime [6] = (getBits_1 (d, offset + 2) == 1)?
	                -1 * getBits_4 (d, offset + 3):
	                     getBits_4 (d, offset + 3);
	dateTime [7] = (getBits_1 (d, offset + 7) == 1)? 30 : 0;
}
//
void fib_processor::FIG0Extension10 (uint8_t *fig) {
int16_t		offset = 16;
int32_t		mjd	= getLBits (fig, offset + 1, 17);
// Modified Julian Date umrechnen (Nach wikipedia)
int32_t J	= mjd + 2400001;
int32_t j	= J + 32044;
int32_t g	= j / 146097; 
int32_t	dg	= j % 146097;
int32_t c	= ((dg / 36524) + 1) * 3 / 4; 
int32_t dc	= dg - c * 36524;
int32_t b	= dc / 1461;
int32_t db	= dc%1461;
int32_t a	= ((db / 365) + 1) * 3 / 4; 
int32_t da	= db - a * 365;
int32_t y	= g * 400 + c * 100 + b * 4 + a;
int32_t m	= ((da * 5 + 308) / 153) - 2;
int32_t d	= da - ((m + 4) * 153 / 5) + 122;
int32_t Y	= y - 4800 + ((m + 2) / 12); 
int32_t M	= ((m + 2) % 12) + 1; 
int32_t D	= d + 1;
	
	dateTime [0] = Y;	// Jahr
	dateTime [1] = M;	// Monat
	dateTime [2] = D;	// Tag
	dateTime [3] = getBits_5 (fig, offset + 21);	// Stunden
	if (getBits_6 (fig, offset + 26) != dateTime [4]) 
	   dateTime [5] =  0;	// Sekunden (Uebergang abfangen)

	dateTime [4] = getBits_6 (fig, offset + 26);	// Minuten
	if (fig [offset + 20] == 1)
	   dateTime [5] = getBits_6 (fig, offset + 32);	// Sekunden
	dateFlag	= true;
}

void	fib_processor::FIG0Extension13 (uint8_t *d) {
int16_t	used	= 2;		// offset in bytes
int16_t	Length	= getBits_5 (d, 3);
uint8_t	PD_bit	= getBits_1 (d, 8 + 2);

	while (used < Length) 
	   used = HandleFIG0Extension13 (d, used, PD_bit);
}

int16_t	fib_processor::HandleFIG0Extension13 (uint8_t *d,
	                                     int16_t used,
	                                     uint8_t pdBit) {
int16_t	lOffset		= used * 8;
uint32_t	SId	= getLBits (d, lOffset, pdBit == 1 ? 32 : 16);
uint16_t	SCIds;
int16_t		NoApplications;
int16_t		i;

	lOffset		+= pdBit == 1 ? 32 : 16;
	SCIds		= getBits_4 (d, lOffset);
	NoApplications	= getBits_4 (d, lOffset + 4);
	lOffset += 8;

	for (i = 0; i < NoApplications; i ++) {
	   int16_t appType	= getBits (d, lOffset, 11);
	   int16_t length	= getBits_5 (d, lOffset + 11);
	   lOffset += (11 + 5 + 8 * length);
	   switch (appType) {
	      case 0x000:		// reserved for future use
	      case 0x001:		// not used
	         break;

	      case 0x002:		// MOT slideshow
	      case 0x003:		// MOT Broadcast Web Site
	      case 0x004:		// TPEG
	      case 0x005:		// DGPS
	      case 0x006:		// TMC
	      case 0x007:		// EPG
	      case 0x008:		// DAB Java
	         break;

	      case 0x44a:		// Journaline
//	         fprintf (stderr, "Journaline\n");
	         break;

	      default:
	         break;
	   }
	}

	(void)SId;
	(void)SCIds;
	return lOffset / 8;
}

void	fib_processor::FIG0Extension14 (uint8_t *d) {
	(void)d;
}

void	fib_processor::FIG0Extension16 (uint8_t *d) {
int16_t	length 	= getBits_5 (d, 3);	// in bytes
int16_t	offset	= 16;			// in bits
serviceId	*s;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   s	= findServiceId (SId);
	   if (!s -> hasPNum) {
	      uint8_t PNum = getBits (d, offset + 16, 16);
	      s -> pNum		= PNum;
	      s -> hasPNum	= true;
//	      fprintf (stderr, "Program number info SId = %.8X, PNum = %d\n",
//	      	                               SId, PNum);
	   }
	   offset += 72;
	}
}

void	fib_processor::FIG0Extension17 (uint8_t *d) {
int16_t	length	= getBits_5 (d, 3);
int16_t	offset	= 16;
serviceId	*s;

	while (offset < length * 8) {
	   uint16_t	SId	= getBits (d, offset, 16);
	   bool	L_flag	= getBits_1 (d, offset + 18);
	   bool	CC_flag	= getBits_1 (d, offset + 19);
	   int16_t type;
	   int16_t Language = 0x00;	// init with unknown language
	   s	= findServiceId (SId);
	   if (L_flag) {		// language field present
	      Language = getBits_8 (d, offset + 24);
	      s -> language = Language;
	      s -> hasLanguage = true;
	      offset += 8;
	   }

	   type	= getBits_5 (d, offset + 27);
	   s	-> programType	= type;
	   if (CC_flag)			// cc flag
	      offset += 40;
	   else
	      offset += 32;
	}
}

void	fib_processor::FIG0Extension18 (uint8_t *d) {
int16_t	offset	= 16;		// bits
uint16_t	SId, AsuFlags;
int16_t		Length	= getBits_5 (d, 3);

	while (offset / 8 < Length - 1 ) {
	   int16_t NumClusters = getBits_5 (d, offset + 35);
	   SId	= getBits (d, offset, 16);
	   AsuFlags	= getBits (d, offset + 16, 16);
//	   fprintf (stderr, "Announcement %d for SId %d with %d clusters\n",
//	                    AsuFlags, SId, NumClusters);
	   offset += 40 + NumClusters * 8;
	}
	(void)SId;
	(void)AsuFlags;
}

void	fib_processor::FIG0Extension19 (uint8_t *d) {
int16_t		offset	= 16;		// bits
uint16_t	AswFlags;
int16_t		Length	= getBits_5 (d, 3);
uint8_t		region_Id_Lower;

	while (offset / 8 < Length - 1) {
	   uint8_t ClusterId	= getBits_8 (d, offset);
	   bool new_flag	= getBits_1(d, offset + 24);
	   bool	region_flag	= getBits_1 (d, offset + 25);
	   uint8_t SubChId	= getBits_6 (d, offset + 26);

	   AswFlags	= getBits (d, offset + 8, 16);
//	   fprintf (stderr,
//	          "%s %s Announcement %d for Cluster %2u on SubCh %2u ",
//	              ((new_flag==1)?"new":"old"),
//	              ((region_flag==1)?"regional":""),
//	              AswFlags, ClusterId,SubChId);
	   if (region_flag) {
	      region_Id_Lower = getBits_6 (d, offset + 34);
	      offset += 40;
//           fprintf(stderr,"for region %u",region_Id_Lower);
	   }
	   else
	      offset += 32;

//	   fprintf(stderr,"\n");
	   (void)ClusterId;
	   (void)new_flag;
	   (void)SubChId;
	}
	(void)AswFlags;
	(void)region_Id_Lower;
}

void	fib_processor::FIG0Extension21 (uint8_t *d) {
//	fprintf (stderr, "Frequency information\n");
	(void)d;
}

void	fib_processor::FIG0Extension22 (uint8_t *d) {
int16_t	Length	= getBits_5 (d, 3);
int16_t	offset	= 16;		// on bits
int16_t	used	= 2;

	while (used < Length) 
	   used = HandleFIG0Extension22 (d, used);
	(void)offset;
}

int16_t	fib_processor::HandleFIG0Extension22 (uint8_t *d, int16_t used) {
uint8_t MS;
int16_t	mainId;
int16_t	noSubfields;

	mainId	= getBits_7 (d, used * 8 + 1);
	(void)mainId;
	MS	= getBits_1 (d, used * 8);
	if (MS == 0) {		// fixed size
	   int16_t latitudeCoarse = getBits (d, used * 8 + 8, 16);
	   int16_t longitudeCoarse = getBits (d, used * 8 + 24, 16);
//	   fprintf (stderr, "Id = %d, (%d %d)\n", mainId,
//	                              latitudeCoarse, longitudeCoarse);
	   (void)latitudeCoarse;
	   (void)longitudeCoarse;
	   return used + 48 / 6;
	}
	//	MS == 1

	noSubfields = getBits_3 (d, used * 8 + 13);
//	fprintf (stderr, "Id = %d, subfields = %d\n", mainId, noSubfields);
	used += (16 + noSubfields * 48) / 8;

	return used;
}

//	FIG 1
//
void	fib_processor::process_FIG1 (uint8_t *d) {
uint8_t		charSet, extension;
uint32_t	SId	= 0;
uint8_t		oe;
int16_t		offset	= 0;
serviceId	*myIndex;
int16_t		i;
uint8_t		pd_flag;
uint8_t		SCidS;
uint8_t		XPAD_aid;
uint8_t		flagfield;
uint8_t		region_id;
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
/*
	   default:
	      return;
 */
	   case 0:	// ensemble label
	      SId	= getBits (d, 16, 16);
	      offset	= 32;
	      if ((charSet <= 16)) { // EBU Latin based repertoire
	         for (i = 0; i < 16; i ++) {
	            label [i] = getBits_8 (d, offset + 8 * i);
//	            addEnsembleChar (label [i], i);
	         }
//	         fprintf (stderr, "Ensemblename: %16s\n", label);
	         if (!oe) {
	            const std::string name = toStringUsingCharset (
	                                      (const char *) label,
	                                      (CharacterSet) charSet);
	            nameofEnsemble (SId, name);
	         }
	      }
//	      fprintf (stderr,
//	               "charset %d is used for ensemblename\n", charSet);
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
//	         fprintf (stderr, "FIG1/1: SId = %4x\t%s\n", SId, label);
	         addtoEnsemble (myIndex -> serviceLabel. label, SId);
	         myIndex -> serviceLabel. hasName = true;
	      }
	      break;

	   case 3:
	      // region label
	      region_id = getBits_6 (d, 16 + 2);
	      (void)region_id;
	      offset = 24;
	      for (i = 0; i < 16; i ++) 
	         label [i] = getBits_8 (d, offset + 8 * i);

//	      fprintf (stderr, "FIG1/3: RegionID = %2x\t%s\n", region_id, label);
	      break;

	   case 4:
	      pd_flag	= getLBits (d, 16, 1);
	      SCidS	= getLBits (d, 20, 4);
	      if (pd_flag) {	// 32 bit identifier field for service component label
	         SId	= getLBits (d, 24, 32);
	         offset	= 56;
	      }
	      else {	// 16 bit identifier field for service component label
	         SId	= getLBits (d, 24, 16);
	         offset	= 40;
	      }

	      flagfield	= getLBits (d, offset + 128, 16);
	      for (i = 0; i < 16; i ++)
	         label [i] = getBits_8 (d, offset + 8 * i);
//	      fprintf (stderr, "FIG1/4: Sid = %8x\tp/d=%d\tSCidS=%1X\tflag=%8X\t%s\n",
//	                        SId, pd_flag, SCidS, flagfield, label);
	      break;


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
#ifdef	MSC_DATA__
	         addtoEnsemble (myIndex -> serviceLabel. label);
#endif
                 myIndex -> serviceLabel. hasName = true;
              }
	      break;

	   case 6:	// XPAD label
	      pd_flag	= getLBits (d, 16, 1);
	      SCidS	= getLBits (d, 20, 4);
	      if (pd_flag) {	// 32 bits identifier for XPAD label
	         SId		= getLBits (d, 24, 32);
	         XPAD_aid	= getLBits (d, 59, 5);
	         offset 	= 64;
	      }
	      else {	// 16 bit identifier for XPAD label
	         SId		= getLBits (d, 24, 16);
	         XPAD_aid	= getLBits (d, 43, 5);
	         offset		= 48;
	      }

	      for (i = 0; i < 16; i ++)
	         label [i] = getBits_8 (d, offset + 8 * i);

//	      fprintf (stderr, "FIG1/6: SId = %8x\tp/d = %d\t SCidS = %1X\tXPAD_aid = %2u\t%s\n",
//		       SId, pd_flag, SCidS, XPAD_aid, label);
	      break;

	   default:
//	      fprintf (stderr, "FIG1/%d: not handled now\n", extension);
	      break;
	}
	(void)SCidS;
	(void)XPAD_aid;
	(void)flagfield;
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

serviceComponent *fib_processor::find_packetComponent (int16_t SCId) {
int16_t i;

        for (i = 0; i < 64; i ++) {
           if (!serviceComps [i]. inUse)
              continue;
           if (serviceComps [i]. TMid != 03)
              continue;
           if (serviceComps [i]. SCId == SCId)
              return &serviceComps [i];
        }
        return NULL;
}

//	bind_audioService is the main processor for - what the name suggests -
//	connecting the description of an audioservice to the Service Identifier
void	fib_processor::bind_audioService (int8_t TMid,
	                                  uint32_t SId,
	                                  int16_t compnr,
	                                  int16_t subChId,
	                                  int16_t ps_flag,
	                                  int16_t ASCTy) {
serviceId *s	= findServiceId	(SId);
int16_t	i;
int16_t	firstFree	= -1;

	for (i = 0; i < 64; i ++) {
           if (!serviceComps [i]. inUse) {
              if (firstFree == -1)
                 firstFree = i;
              continue;
           }
           if ((serviceComps [i]. service == s) &&
               (serviceComps [i]. componentNr == compnr))
              return;
        }
        serviceComps [firstFree]. inUse = true;
        serviceComps [firstFree]. TMid    = TMid;
        serviceComps [firstFree]. componentNr = compnr;
        serviceComps [firstFree]. service = s;
        serviceComps [firstFree]. subchannelId = subChId;
        serviceComps [firstFree]. PS_flag = ps_flag;
        serviceComps [firstFree]. ASCTy = ASCTy;
//	addtoEnsemble (s -> serviceLabel. label , SId);
//	fprintf (stderr, "service %8x (comp %d) is audio\n", subChId, firstFree);
}


//	bind_packetService is the main processor for - what the name suggests -
//	connecting the service component defining the service to the SId,
///	Note that the subchannel is assigned through a FIG0/3
void	fib_processor::bind_packetService (int8_t TMid,
	                                   uint32_t SId,
	                                   int16_t compnr,
	                                   int16_t subChId,
	                                   int16_t ps_flag,
	                                   int16_t CAflag) {
serviceId *s    = findServiceId (SId);
int16_t i;
int16_t	firstFree	= -1;

       for (i = 0; i < 64; i ++) {
	   if (!serviceComps [i]. inUse) {
	      if (firstFree == -1)
	         firstFree = i;
	      continue;
	   }
	   if ((serviceComps [i]. service == s) && 
	       (serviceComps [i]. componentNr == compnr))
	      return;
	}
	serviceComps [firstFree]. inUse  = true;
	serviceComps [firstFree]. TMid   = TMid;
	serviceComps [firstFree]. service = s;
	serviceComps [firstFree]. componentNr = compnr;
	serviceComps [firstFree]. SCId   = SId;
	serviceComps [firstFree]. PS_flag = ps_flag;
	serviceComps [firstFree]. CAflag = CAflag;
//	fprintf (stderr, "service %8x (comp %d) is packet\n", SId, compnr);
}


//	bind_audioService is the main processor for - what the name suggests -
void	fib_processor::clearEnsemble (void) {
int16_t i;
	memset (serviceComps, 0, sizeof (serviceComps));
	memset (subChannels, 0, sizeof (subChannels));
	for (i = 0; i < 64; i ++) {
	   listofServices [i]. inUse = false;
	   listofServices [i]. serviceId = -1;
	   listofServices [i]. serviceLabel. label = std::string ();
	   serviceComps [i]. inUse = false;
	   subChannels	[i]. inUse	= false;
	}
	selectedService	= -1;
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
	ensembleName (s, userData);
        isSynced        = true;
}

void    fib_processor::changeinConfiguration (void) {
}


