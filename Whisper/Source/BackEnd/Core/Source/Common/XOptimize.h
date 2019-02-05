/*
 *  File:       XOptimize.h
 *  Summary:   	A header that can be included to optimize debug builds.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XOptimize.h,v $
 *		Revision 1.2  2000/11/09 12:45:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/22/99	JDJ	
 */

#pragma once

#if DEBUG									// for release builds we'll forego overriding the user specified optimizations
	#if CODE_WARRIOR
		#pragma peephole			on
		#pragma global_optimizer 	on
		#pragma optimization_level	4
		#pragma auto_inline			on
		
		#if __POWERPC__
			#pragma scheduling  	603
		#endif
		
	#elif MSVC
		#pragma optimize("gty", on)			// enable global optimizations, fast sequences of machine code, and frame pointers on the program stack
	#endif
#endif	
