
// "FastTiming.h"

#ifndef __FASTTIMING__
#define __FASTTIMING__

#ifndef __TYPES__
	#include <Types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


Boolean InitFastTiming( void );
void ShutdownFastTiming( void );
void FastMicroseconds( register unsigned long * hiPtr, register unsigned long * loPtr );


#ifdef __cplusplus
}
#endif

#endif	// __FASTTIMING__
