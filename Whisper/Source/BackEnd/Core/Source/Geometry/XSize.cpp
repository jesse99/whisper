/*
 *  File:       XSize.cpp
 *  Summary:	Size class
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XSize.cpp,v $
 *		Revision 1.4  2001/03/02 11:25:19  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 04:11:41  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:52:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XSize.h>

#include <XNumbers.h>
#include <XPoint.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XSize kZeroSize(0, 0);


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (wstring, XSize)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, XSize& value)
{
	FromStr(str, value.width);
	FromStr(After(str, L" "), value.height);
}


//---------------------------------------------------------------
//
// ToStr (XSize)
//
//---------------------------------------------------------------
std::wstring ToStr(const XSize& value)
{
	return ToStr(value.width) + L" " + ToStr(value.height);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XSize
// ===================================================================================

//---------------------------------------------------------------
//
// XSize::XSize (XPoint)
//
//---------------------------------------------------------------
XSize::XSize(const XPoint& pt)
{
	width  = pt.x;
	height = pt.y;
}


//---------------------------------------------------------------
//
// XSize::GetOSSize
//
//---------------------------------------------------------------
#if MAC
OSSize XSize::GetOSSize() const			
{
	Point pt;

	pt.h = numeric_cast<int16>(width);
	pt.v = numeric_cast<int16>(height);

	return pt;
}
#endif


}	// namespace Whisper

