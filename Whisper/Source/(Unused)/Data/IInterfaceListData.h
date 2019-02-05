/*
 *  File:       IInterfaceListData.h
 *  Summary:   	Data interface that contains a list of interface pointers.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInterfaceListData.h,v $
 *		Revision 1.2  2000/11/09 09:02:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/06/00	JDJ		Created
 */

#pragma once

#include <XConstInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IInterfaceListData
// ===================================================================================
class IInterfaceListData : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual uint32 		GetCount() const = 0;
	
	virtual XUnknownPtr& Get(uint32 index) = 0;
	virtual const XUnknownPtr& Get(uint32 index) const = 0;
	
	virtual void 		Append(const XUnknownPtr& ptr) = 0;
	virtual void 		Remove(uint32 index) = 0;
};

typedef XInterfacePtr<IInterfaceListData> IInterfaceListDataDataPtr;
typedef XConstInterfacePtr<IInterfaceListData> IConstInterfaceListDataDataPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

