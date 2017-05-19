

#include	<stdint.h>
#include	<string>
//	typedefs for the callbacks
typedef void    (*etiwriter_t)		(uint8_t *, int32_t, void *);
typedef void    (*ensemblename_t)	(std::string, void *);
typedef void    (*programname_t)	(std::string, int, void *);
typedef	void	(*syncsignal_t)		(bool, void *);
typedef void	(*snrsignal_t)		(int16_t, void *);
typedef void    (*fibquality_t)		(int16_t, void *);

