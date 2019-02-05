/*
 *  File:       IIntData.h
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IIntData.h,v $
 *		Revision 1.2  2000/11/09 09:02:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#pragma once

#include <XConstInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IIntData
// ===================================================================================
class IIntData : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual int32 		Get() const = 0;
							
	virtual void 		Set(int32 value) = 0;
};

typedef XInterfacePtr<IIntData> IIntDataPtr;
typedef XConstInterfacePtr<IIntData> IConstIntDataPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

