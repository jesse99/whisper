/*
 *  File:       X3DStrings.cpp
 *  Summary:	QuickDraw 3D string objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStrings.cpp,v $
 *		Revision 1.4  2001/04/27 04:22:56  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/14 07:32:12  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:21:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DStrings.h>

#include <QuesaString.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class X3DCString
// ===================================================================================

//---------------------------------------------------------------
//
// X3DCString::~X3DCString
//
//---------------------------------------------------------------
X3DCString::~X3DCString()
{
}


//---------------------------------------------------------------
//
// X3DCString::X3DCString (wstring)
//
//---------------------------------------------------------------
X3DCString::X3DCString(const std::wstring& str) : X3DString(Q3CString_New(ToPlatformStr(str).c_str()))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCString::X3DCString (TQ3StringObject)
//
//---------------------------------------------------------------
X3DCString::X3DCString(TQ3StringObject object) : X3DString(object)
{
	this->ConfirmType(kQ3StringTypeCString);
}


//---------------------------------------------------------------
//
// X3DCString::GetLength
//
//---------------------------------------------------------------
uint32 X3DCString::GetLength() const
{
	uint32 length;
	
	TQ3Status status = Q3CString_GetLength(mObject, &length);
	ThrowIf3DError(status);

	return length;	
}


//---------------------------------------------------------------
//
// X3DCString::SetString
//
//---------------------------------------------------------------
void X3DCString::SetString(const std::wstring& str)
{
	TQ3Status status = Q3CString_SetString(mObject, ToPlatformStr(str).c_str());
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DCString::GetString
//
//---------------------------------------------------------------
std::wstring X3DCString::GetString() const
{
	char* data = nil;

	TQ3Status status = Q3CString_GetString(mObject, &data);
	ThrowIf3DError(status);
	
	std::wstring str = FromPlatformStr(data);
	
	status = Q3CString_EmptyData(&data);
	ASSERT(status == kQ3Success);
	
	return str;
}


//---------------------------------------------------------------
//
// X3DCString::IsCompatible		 						[static]
//	
//---------------------------------------------------------------
bool X3DCString::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StringTypeCString);
	
	return compatible;
}


}	// namespace Whisper

