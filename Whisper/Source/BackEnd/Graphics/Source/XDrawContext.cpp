/*
 *  File:       XDrawContext.cpp
 *  Summary:	Abstract base class for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDrawContext.cpp,v $
 *		Revision 1.2  2000/11/09 12:33:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	  8/06/99	JDJ		Added DrawModeToStr and StrToDrawMode.
 */

#include <XWhisperHeader.h>
#include <XDrawContext.h>

namespace Whisper {


// ===================================================================================
//	class XSaveContext
// ===================================================================================

//---------------------------------------------------------------
//
// DrawModeToStr
//
//---------------------------------------------------------------
std::wstring DrawModeToStr(EDrawMode mode)
{
	std::wstring str;
	
	switch (mode) {
		case kCopyMode:
			str = L"kCopyMode";
			break;
			
		case kNotCopyMode:
			str = L"kNotCopyMode";
			break;

		case kOrMode:
			str = L"kOrMode";
			break;

		case kNotOrMode:
			str = L"kNotOrMode";
			break;

		case kXOrMode:
			str = L"kXOrMode";
			break;

		case kClearMode:
			str = L"kClearMode";
			break;

		case kNotClearMode:
			str = L"kNotClearMode";
			break;

#if MAC
		case kHiliteMode:
			str = L"kHiliteMode";
			break;
#endif

		case kTransparentMode:
			str = L"kTransparentMode";
			break;
		
		default:
			str = L"kCopyMode";
			DEBUGSTR("Bad draw mode in DrawModeToStr");
	}
	
	return str;
}


//---------------------------------------------------------------
//
// StrToDrawMode
//
//---------------------------------------------------------------
EDrawMode StrToDrawMode(const std::wstring& str)
{
	EDrawMode mode;
	
	if (str == L"kCopyMode")
		mode = kCopyMode;
			
	else if (str == L"kNotCopyMode")
		mode = kNotCopyMode;

	else if (str == L"kOrMode")
		mode = kOrMode;

	else if (str == L"kNotOrMode")
		mode = kNotOrMode;

	else if (str == L"kXOrMode")
		mode = kXOrMode;

	else if (str == L"kClearMode")
		mode = kClearMode;

	else if (str == L"kNotClearMode")
		mode = kNotClearMode;

	else if (str == L"kHiliteMode")
		mode = kHiliteMode;

	else if (str == L"kTransparentMode")
		mode = kTransparentMode;
		
	else {
		mode = kCopyMode;
		DEBUGSTR("Bad draw mode in StrToDrawMode");
	}
	
	return mode;
}


}	// namespace Whisper

