
#ifndef	__CALLBACK_TYPES__
#define	__CALLBACK_TYPES__

#include	<stdint.h>
#include	<string>
//	typedefs for the callbacks
typedef void    (*etiwriter_t)		(uint8_t *, int32_t, void *);
typedef void    (*ensemblename_t)	(std::string, uint32_t, void *);
typedef void    (*programname_t)	(std::string, int, void *);
typedef	void	(*syncsignal_t)		(bool, void *);
typedef void	(*snrsignal_t)		(int16_t, void *);
typedef void    (*fibquality_t)		(int16_t, void *);
typedef	void	(*inputstopped_t)	(void);

typedef struct {
	etiwriter_t	theWriter;
	ensemblename_t	theEnsemble;
	programname_t	theProgram;
	syncsignal_t	theSyncSignal;
	snrsignal_t	theSnrSignal;
	fibquality_t	theFibQuality;
	inputstopped_t	theInputStopped;
} callbacks;

#endif
