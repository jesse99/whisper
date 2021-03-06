/*
 *  File:       ICountLines.h
 *  Summary:   	Interface class for COM Tester plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ICountLines.h,v $
 *		Revision 1.2  2000/11/09 09:00:51  jesjones
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
//	class ICountLines
// ===================================================================================
class ICountLines : public XUnknown {

//-----------------------------------
//	API
//
public:	
	virtual uint32 		CountLines(const std::wstring& text) const = 0;
	
	virtual uint32 		GetNumExtensions() const = 0;
	virtual std::wstring GetExtension(uint32 index) const = 0;
						// Used to iterate over the file extensions supported by this
						// plugin.
};

typedef XInterfacePtr<ICountLines> ICountLinesPtr;
typedef XInterfacePtr<const ICountLines> IConstCountLinesPtr;


