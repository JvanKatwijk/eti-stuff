
#ifndef	__VITERBI_SPIRAL__
#define	__VITERBI_SPIRAL__
/*
 * 	Viterbi.h according to the SPIRAL project
 */
#include	"dab-constants.h"

//	For our particular viterbi decoder, we have
#define	RATE	4
#define NUMSTATES 64
#define DECISIONTYPE uint32_t
//#define DECISIONTYPE uint8_t
//#define DECISIONTYPE_BITSIZE 8
#define DECISIONTYPE_BITSIZE 32
#define COMPUTETYPE uint32_t

//decision_t is a BIT vector
#if defined(MSVC) || defined(_WIN32)
//__declspec(align(16))
typedef union {
	DECISIONTYPE t[NUMSTATES/DECISIONTYPE_BITSIZE];
	uint32_t w[NUMSTATES/32];
	uint16_t s[NUMSTATES/16];
	uint8_t c[NUMSTATES/8];
} decision_t;

//__declspec(align(16))
typedef union {
	COMPUTETYPE t[NUMSTATES];
} metric_t;
#else
typedef union {
	DECISIONTYPE t[NUMSTATES/DECISIONTYPE_BITSIZE];
	uint32_t w[NUMSTATES/32];
	uint16_t s[NUMSTATES/16];
	uint8_t c[NUMSTATES/8];
} decision_t __attribute__ ((aligned (16)));

typedef union {
	COMPUTETYPE t[NUMSTATES];
} metric_t __attribute__ ((aligned (16)));
#endif
/* State info for instance of Viterbi decoder
 */

struct v {
#if defined(MSVC) || defined(_WIN32)
	/* path metric buffer 1 */
	//__declspec(align(16)) 
		metric_t metrics1;
	/* path metric buffer 2 */
	//__declspec(align(16)) 
		metric_t metrics2;
#else
/* path metric buffer 1 */
	__attribute__ ((aligned (16))) metric_t metrics1;
/* path metric buffer 2 */
	__attribute__ ((aligned (16))) metric_t metrics2;
#endif
/* Pointers to path metrics, swapped on every bit */
	metric_t *old_metrics,*new_metrics;
	decision_t *decisions;   /* decisions */
};

class	viterbiSpiral {
public:
		viterbiSpiral	(int16_t);
		~viterbiSpiral	(void);
	void	deconvolve	(int16_t *, uint8_t *);
private:

	struct v	vp;
#if defined(MSVC) || defined(_WIN32)
	//__declspec(align(16)) 
		COMPUTETYPE Branchtab[NUMSTATES / 2 * RATE];
#else
	COMPUTETYPE Branchtab	[NUMSTATES / 2 * RATE] __attribute__ ((aligned (16)));
#endif
//	int	parityb		(uint8_t);
	int	parity		(int);
	void	partab_init	(void);
//	uint8_t	Partab	[256];
	void	init_viterbi	(struct v *, int16_t);
	void	update_viterbi_blk_GENERIC	(struct v *, COMPUTETYPE *,
	                                         int16_t);
	void	update_viterbi_blk_SPIRAL	(struct v *, COMPUTETYPE *,
	                                         int16_t);
	void	chainback_viterbi (struct v *, uint8_t *, int16_t, uint16_t);
	struct v *viterbi_alloc (int32_t);
	void	BFLY		(int32_t, int, COMPUTETYPE *,
	                         struct v *, decision_t *);
//	uint8_t *bits;
	uint8_t *data;
	COMPUTETYPE *symbols;
	int16_t	frameBits;
};

#endif

