/*
 *  File:       X3DStyle.cpp
 *  Summary:	Base class for QuickDraw 3D style objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStyle.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:07  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:15  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:21:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DStyle.h>

#include <X3DUtils.h>
#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class X3DStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DStyle::~X3DStyle
//
//---------------------------------------------------------------
X3DStyle::~X3DStyle()
{
}


//---------------------------------------------------------------
//
// X3DStyle::X3DStyle
//
//---------------------------------------------------------------
X3DStyle::X3DStyle(TQ3StyleObject object) : X3DShape(object)
{
	this->ConfirmType(kQ3ShapeTypeStyle);
}


//---------------------------------------------------------------
//
// X3DStyle::Clone
//
//---------------------------------------------------------------
X3DStyle* X3DStyle::Clone() const
{
	X3DStyle* object = new X3DStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DStyle::Submit
//
//---------------------------------------------------------------
void X3DStyle::Submit(TQ3ViewObject view) const
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3Style_Submit(mObject, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DStyle::IsCompatible		 						[static]
//	
//---------------------------------------------------------------
bool X3DStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3ShapeTypeStyle);
	
	return compatible;
}


}	// namespace Whisper

