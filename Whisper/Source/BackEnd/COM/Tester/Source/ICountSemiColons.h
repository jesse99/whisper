/*
 *  File:       ICountSemiColons.h
 *  Summary:   	Optional interface used to test interface aggregation.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICountSemiColons.h,v $
 *		Revision 1.2  2000/11/09 09:00:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/30/00	JDJ		Created
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class ICountSemiColons
// ===================================================================================
class ICountSemiColons : public XUnknown {

//-----------------------------------
//	API
//
public:	
	virtual uint32 		CountSemiColons(const std::wstring& text) const = 0;
};

typedef XInterfacePtr<ICountSemiColons> ICountSemiColonsPtr;
typedef XInterfacePtr<const ICountSemiColons> IConstCountSemiColonsPtr;


