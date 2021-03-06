/*
 *  File:       IErrorData.h
 *  Summary:   	Data interface used to initialize things like command objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IErrorData.h,v $
 *		Revision 1.2  2000/11/09 09:01:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/29/00	JDJ		Created
 */

#pragma once

#include <XConstInterfacePtr.h>
#include <XTypes.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IErrorData
// ===================================================================================
class IErrorData : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual std::wstring GetSummary() const = 0;
	virtual std::wstring GetNarrative() const = 0;
							
	virtual void 		SetSummary(const std::wstring& text) = 0;
	virtual void 		SetNarrative(const std::wstring& text) = 0;
	virtual void 		SetNarrative(const std::exception* e) = 0;
	virtual void 		SetNarrative(OSError err) = 0;
						// e may be nil
};

typedef XInterfacePtr<IErrorData> IErrorDataPtr;
typedef XConstInterfacePtr<IErrorData> IConstErrorDataPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

