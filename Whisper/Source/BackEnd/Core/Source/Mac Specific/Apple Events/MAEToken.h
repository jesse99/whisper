/*
 *  File:       MAEToken.h
 *  Summary:   	An AEDesc that is used when resolving scripting descriptors.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEToken.h,v $
 *		Revision 1.3  2000/12/10 08:48:16  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:09:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MAEToken.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:48:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 9/17/98	JDJ		Created
 */

#pragma once

#include <MAEDesc.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAEToken
//!		An AEDesc that is used when resolving scripting descriptors. $$$ For now this is just a place holder.
// ===================================================================================
class CORE_EXPORT MAEToken : public MAEDesc {

	typedef MAEDesc Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MAEToken();
						
						MAEToken();

						MAEToken(const AEDesc& objectSpecifier, int16 callbackFlags);
						/**< CallbackFlags can be kAEIDoMinimum, kAEIDoWhose, and kAEIDoMarking. */
											
private:
						MAEToken(const MAEToken& rhs);
						/**< Tokens shouldn't be copied with AEDuplicateDesc so, for now,
						we'll disable copying altogether. */
						
			MAEToken& 	operator=(const MAEToken& rhs);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
