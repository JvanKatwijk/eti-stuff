#
/*
 *    Copyright (C) 2013
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
 */
#
#ifndef	__FIB_PROCESSOR__
#define	__FIB_PROCESSOR__
#
//
#include	<stdint.h>
#include	<stdio.h>
#include	"dab-constants.h"
#include	"dab_tables.h"
#include	"ensemble-handler.h"

	struct dablabel {
//	   uint8_t	label [17];
	   std::string	label;
	   uint8_t	mask;
	   bool		hasName;
	};

	typedef struct dablabel	dabLabel;

	typedef struct subchannelmap channelMap;
//	from FIG1/2
	struct serviceid {
	   uint32_t	serviceId;
	   dabLabel	serviceLabel;
	   bool		inUse;
	   bool		hasPNum;
	   bool		hasLanguage;
	   int16_t	language;
	   int16_t	programType;
	   uint16_t	pNum;
	   
	};
	typedef	struct serviceid serviceId;
//      The service component describes the actual service
//      It really should be a union
        struct servicecomponents {
           bool         inUse;          // just administration
           int8_t       TMid;           // the transport mode
           serviceId    *service;       // belongs to the service
           int16_t      componentNr;    // component

           int16_t      ASCTy;          // used for audio
           int16_t      PS_flag;        // use for both audio and packet
           int16_t      subchannelId;   // used in both audio and packet
           uint16_t     SCId;           // used in packet
           uint8_t      CAflag;         // used in packet (or not at all)
           int16_t      DSCTy;          // used in packet
	   uint8_t	DGflag;		// used for TDC
           int16_t      packetAddress;  // used in packet
        };

        typedef struct servicecomponents serviceComponent;

	struct subchannelmap {
	   bool		inUse;
	   int32_t	id;
	   int32_t	start_cu;
	   bool		uepFlag;
	   int32_t	size;
	   int32_t	uep_index;
	   int32_t	protlev;
	   int32_t	bitrate;
	};

class	RadioInterface;

class	fib_processor {
public:
		fib_processor		(ensembleHandler *the_ensembleHandler);
		~fib_processor		(void);
	void	process_FIB		(uint8_t *, uint16_t);

	void	clearEnsemble		(void);
	void	printActions		(int16_t);
	void	get_channelInfo		(channel_data *, int);
	void	get_CIFCount		(int16_t *, int16_t *);
private:
	ensembleHandler *ensemble;
	int16_t		CIFCount_hi;
	int16_t		CIFCount_lo;
	serviceId	*findServiceId (int32_t);
	serviceComponent *find_packetComponent (int16_t);
        void            bind_audioService (int8_t,
                                           uint32_t, int16_t,
                                           int16_t, int16_t, int16_t);
        void            bind_packetService (int8_t,
                                            uint32_t, int16_t,
                                            int16_t, int16_t, int16_t);
	int32_t		selectedService;
	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
	void		FIG0Extension5		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
	void		FIG0Extension21		(uint8_t *);
	void		FIG0Extension22		(uint8_t *);

	int16_t		HandleFIG0Extension1	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension2	(uint8_t *,
	                                         int16_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension3	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension5	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension8	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension13	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension22	(uint8_t *, int16_t);
	int32_t		dateTime	[8];
	channelMap	ficList [64];
	serviceComponent	components [64];
	serviceId	*listofServices;
	bool	dateFlag;
        bool            firstTime;
        bool            isSynced;
//
//      these were signals
        void            addtoEnsemble   (const std::string &, int32_t);
        void            nameofEnsemble  (int, const std::string &);
        void            changeinConfiguration (void);
};

#endif

