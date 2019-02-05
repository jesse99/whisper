/*
 *  File:       XPoint.cpp
 *  Summary:	Point class
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XPoint.cpp,v $
 *		Revision 1.4  2001/03/02 11:24:49  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2000/12/10 04:11:05  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:46:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XPoint.h>

#include <XNumbers.h>
#include <XTextConversions.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XPoint kZeroPt(0, 0);


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (wstring, XPoint)
//
//---------------------------------------------------------------
void FromStr(const std::wstring& str, XPoint& value)
{
	FromStr(str, value.x);
	FromStr(After(str, L" "), value.y);
}


//---------------------------------------------------------------
//
// ToStr (XPoint)
//
//---------------------------------------------------------------
std::wstring ToStr(const XPoint& value)
{
	return ToStr(value.x) + L" " + ToStr(value.y);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPoint
// ===================================================================================

//---------------------------------------------------------------
//
// XPoint::GetOSPoint
//
//---------------------------------------------------------------
#if MAC
OSPoint XPoint::GetOSPoint() const			
{
	Point pt;

	pt.h = numeric_cast<int16>(x);
	pt.v = numeric_cast<int16>(y);
	
	return pt;
}
#endif


}	// namespace Whisper

