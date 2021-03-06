/*
 *  File:       X3DStyles.cpp
 *  Summary:	QuickDraw 3D style objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DStyles.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:13  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:24  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:21:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DStyles.h>

#include <X3DUtils.h>

namespace Whisper {


// ===================================================================================
//	Registration
// ===================================================================================

//---------------------------------------------------------------
//
// CreateSubdivisionStyle (TQ3SubdivisionMethod, float, float)
//
//---------------------------------------------------------------
static TQ3Object CreateSubdivisionStyle(TQ3SubdivisionMethod method, float c1, float c2)
{
	PRECONDITION(c1 >= 0.0);
	PRECONDITION(c2 >= 0.0);
	
	TQ3SubdivisionStyleData data;
	data.method = method;
	data.c1     = c1;
	data.c2     = c2;
	
	TQ3Object object = Q3SubdivisionStyle_New(&data);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	class X3DSubdivisionStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DSubdivisionStyle::~X3DSubdivisionStyle
//
//---------------------------------------------------------------
X3DSubdivisionStyle::~X3DSubdivisionStyle()
{
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::X3DSubdivisionStyle (TQ3SubdivisionMethod, float, float)
//
//---------------------------------------------------------------
X3DSubdivisionStyle::X3DSubdivisionStyle(TQ3SubdivisionMethod method, float c1, float c2) : X3DStyle(CreateSubdivisionStyle(method, c1, c2))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::X3DSubdivisionStyle (TQ3SubdivisionStyleData)
//
//---------------------------------------------------------------
X3DSubdivisionStyle::X3DSubdivisionStyle(const TQ3SubdivisionStyleData& data) : X3DStyle(Q3SubdivisionStyle_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::X3DSubdivisionStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DSubdivisionStyle::X3DSubdivisionStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeSubdivision);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::Clone
//
//---------------------------------------------------------------
X3DSubdivisionStyle* X3DSubdivisionStyle::Clone() const
{
	X3DSubdivisionStyle* object = new X3DSubdivisionStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::Submit (TQ3SubdivisionMethod, float, float, view)	[static]
//
//---------------------------------------------------------------
void X3DSubdivisionStyle::Submit(TQ3SubdivisionMethod method, float c1, float c2, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3SubdivisionStyleData data;
	data.method = method;
	data.c1     = c1;
	data.c2     = c2;
	
	TQ3Status status = Q3SubdivisionStyle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::Submit (TQ3SubdivisionStyleData, view)	[static]
//
//---------------------------------------------------------------
void X3DSubdivisionStyle::Submit(const TQ3SubdivisionStyleData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3SubdivisionStyle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::GetData
//
//---------------------------------------------------------------
TQ3SubdivisionStyleData X3DSubdivisionStyle::GetData() const
{
	TQ3SubdivisionStyleData data;
	
	TQ3Status status = Q3SubdivisionStyle_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::SetData (TQ3SubdivisionMethod, float, float)
//
//---------------------------------------------------------------
void X3DSubdivisionStyle::SetData(TQ3SubdivisionMethod method, float c1, float c2)
{
	TQ3SubdivisionStyleData data;
	data.method = method;
	data.c1     = c1;
	data.c2     = c2;
		
	TQ3Status status = Q3SubdivisionStyle_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::SetData (TQ3SubdivisionStyleData)
//
//---------------------------------------------------------------
void X3DSubdivisionStyle::SetData(const TQ3SubdivisionStyleData& data)
{
	TQ3Status status = Q3SubdivisionStyle_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DSubdivisionStyle::IsCompatible		 			[static]
//	
//---------------------------------------------------------------
bool X3DSubdivisionStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeSubdivision);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DPickIDStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPickIDStyle::~X3DPickIDStyle
//
//---------------------------------------------------------------
X3DPickIDStyle::~X3DPickIDStyle()
{
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::X3DPickIDStyle (uint32)
//
//---------------------------------------------------------------
X3DPickIDStyle::X3DPickIDStyle(uint32 id) : X3DStyle(Q3PickIDStyle_New(id))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::X3DPickIDStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DPickIDStyle::X3DPickIDStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypePickID);
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::Clone
//
//---------------------------------------------------------------
X3DPickIDStyle* X3DPickIDStyle::Clone() const
{
	X3DPickIDStyle* object = new X3DPickIDStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::Submit								[static]
//
//---------------------------------------------------------------
void X3DPickIDStyle::Submit(uint32 id, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3PickIDStyle_Submit(id, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::Get
//
//---------------------------------------------------------------
uint32 X3DPickIDStyle::Get() const
{
	uint32 id;
	
	TQ3Status status = Q3PickIDStyle_Get(mObject, &id);
	ThrowIf3DError(status);
	
	return id;
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::Set
//
//---------------------------------------------------------------
void X3DPickIDStyle::Set(uint32 id)
{
	TQ3Status status = Q3PickIDStyle_Set(mObject, id);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickIDStyle::IsCompatible		 					[static]
//	
//---------------------------------------------------------------
bool X3DPickIDStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypePickID);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DPickPartsStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DPickPartsStyle::~X3DPickPartsStyle
//
//---------------------------------------------------------------
X3DPickPartsStyle::~X3DPickPartsStyle()
{
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::X3DPickPartsStyle (TQ3PickParts)
//
//---------------------------------------------------------------
X3DPickPartsStyle::X3DPickPartsStyle(TQ3PickParts parts) : X3DStyle(Q3PickPartsStyle_New(parts))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::X3DPickPartsStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DPickPartsStyle::X3DPickPartsStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypePickParts);
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::Clone
//
//---------------------------------------------------------------
X3DPickPartsStyle* X3DPickPartsStyle::Clone() const
{
	X3DPickPartsStyle* object = new X3DPickPartsStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DPickPartsStyle::Submit(TQ3PickParts parts, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3PickPartsStyle_Submit(parts, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::Get
//
//---------------------------------------------------------------
TQ3PickParts X3DPickPartsStyle::Get() const
{
	TQ3PickParts parts;
	
	TQ3Status status = Q3PickPartsStyle_Get(mObject, &parts);
	ThrowIf3DError(status);
	
	return parts;
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::Set
//
//---------------------------------------------------------------
void X3DPickPartsStyle::Set(TQ3PickParts parts)
{
	TQ3Status status = Q3PickPartsStyle_Set(mObject, parts);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPickPartsStyle::IsCompatible		 				[static]
//	
//---------------------------------------------------------------
bool X3DPickPartsStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypePickParts);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DReceiveShadowsStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::~X3DReceiveShadowsStyle
//
//---------------------------------------------------------------
X3DReceiveShadowsStyle::~X3DReceiveShadowsStyle()
{
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::X3DReceiveShadowsStyle (bool)
//
//---------------------------------------------------------------
X3DReceiveShadowsStyle::X3DReceiveShadowsStyle(bool receives) : X3DStyle(Q3ReceiveShadowsStyle_New((TQ3Boolean) receives))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::X3DReceiveShadowsStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DReceiveShadowsStyle::X3DReceiveShadowsStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeReceiveShadows);
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::Clone
//
//---------------------------------------------------------------
X3DReceiveShadowsStyle* X3DReceiveShadowsStyle::Clone() const
{
	X3DReceiveShadowsStyle* object = new X3DReceiveShadowsStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::Submit						[static]
//
//---------------------------------------------------------------
void X3DReceiveShadowsStyle::Submit(bool receives, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3ReceiveShadowsStyle_Submit((TQ3Boolean) receives, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::Get
//
//---------------------------------------------------------------
bool X3DReceiveShadowsStyle::Get() const
{
	TQ3Boolean receives;
	
	TQ3Status status = Q3ReceiveShadowsStyle_Get(mObject, &receives);
	ThrowIf3DError(status);
	
	return receives;
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::Set
//
//---------------------------------------------------------------
void X3DReceiveShadowsStyle::Set(bool receives)
{
	TQ3Status status = Q3ReceiveShadowsStyle_Set(mObject, (TQ3Boolean) receives);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DReceiveShadowsStyle::IsCompatible		 			[static]
//	
//---------------------------------------------------------------
bool X3DReceiveShadowsStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeReceiveShadows);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DFillStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFillStyle::~X3DFillStyle
//
//---------------------------------------------------------------
X3DFillStyle::~X3DFillStyle()
{
}


//---------------------------------------------------------------
//
// X3DFillStyle::X3DFillStyle (TQ3FillStyle)
//
//---------------------------------------------------------------
X3DFillStyle::X3DFillStyle(TQ3FillStyle style) : X3DStyle(Q3FillStyle_New(style))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFillStyle::X3DFillStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DFillStyle::X3DFillStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeFill);
}


//---------------------------------------------------------------
//
// X3DFillStyle::Clone
//
//---------------------------------------------------------------
X3DFillStyle* X3DFillStyle::Clone() const
{
	X3DFillStyle* object = new X3DFillStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DFillStyle::Submit									[static]
//
//---------------------------------------------------------------
void X3DFillStyle::Submit(TQ3FillStyle style, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3FillStyle_Submit(style, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFillStyle::Get
//
//---------------------------------------------------------------
TQ3FillStyle X3DFillStyle::Get() const
{
	TQ3FillStyle style;
	
	TQ3Status status = Q3FillStyle_Get(mObject, &style);
	ThrowIf3DError(status);
	
	return style;
}


//---------------------------------------------------------------
//
// X3DFillStyle::Set
//
//---------------------------------------------------------------
void X3DFillStyle::Set(TQ3FillStyle style)
{
	TQ3Status status = Q3FillStyle_Set(mObject, style);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFillStyle::IsCompatible		 					[static]
//	
//---------------------------------------------------------------
bool X3DFillStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeFill);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DBackfacingStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DBackfacingStyle::~X3DBackfacingStyle
//
//---------------------------------------------------------------
X3DBackfacingStyle::~X3DBackfacingStyle()
{
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::X3DBackfacingStyle (TQ3BackfacingStyle)
//
//---------------------------------------------------------------
X3DBackfacingStyle::X3DBackfacingStyle(TQ3BackfacingStyle style) : X3DStyle(Q3BackfacingStyle_New(style))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::X3DBackfacingStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DBackfacingStyle::X3DBackfacingStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeBackfacing);
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::Clone
//
//---------------------------------------------------------------
X3DBackfacingStyle* X3DBackfacingStyle::Clone() const
{
	X3DBackfacingStyle* object = new X3DBackfacingStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DBackfacingStyle::Submit(TQ3BackfacingStyle style, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3BackfacingStyle_Submit(style, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::Get
//
//---------------------------------------------------------------
TQ3BackfacingStyle X3DBackfacingStyle::Get() const
{
	TQ3BackfacingStyle style;
	
	TQ3Status status = Q3BackfacingStyle_Get(mObject, &style);
	ThrowIf3DError(status);
	
	return style;
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::Set
//
//---------------------------------------------------------------
void X3DBackfacingStyle::Set(TQ3BackfacingStyle style)
{
	TQ3Status status = Q3BackfacingStyle_Set(mObject, style);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DBackfacingStyle::IsCompatible		 				[static]
//	
//---------------------------------------------------------------
bool X3DBackfacingStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeBackfacing);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DInterpolationStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DInterpolationStyle::~X3DInterpolationStyle
//
//---------------------------------------------------------------
X3DInterpolationStyle::~X3DInterpolationStyle()
{
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::X3DInterpolationStyle (TQ3InterpolationStyle)
//
//---------------------------------------------------------------
X3DInterpolationStyle::X3DInterpolationStyle(TQ3InterpolationStyle style) : X3DStyle(Q3InterpolationStyle_New(style))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::X3DInterpolationStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DInterpolationStyle::X3DInterpolationStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeInterpolation);
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::Clone
//
//---------------------------------------------------------------
X3DInterpolationStyle* X3DInterpolationStyle::Clone() const
{
	X3DInterpolationStyle* object = new X3DInterpolationStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::Submit						[static]
//
//---------------------------------------------------------------
void X3DInterpolationStyle::Submit(TQ3InterpolationStyle style, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3InterpolationStyle_Submit(style, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::Get
//
//---------------------------------------------------------------
TQ3InterpolationStyle X3DInterpolationStyle::Get() const
{
	TQ3InterpolationStyle style;
	
	TQ3Status status = Q3InterpolationStyle_Get(mObject, &style);
	ThrowIf3DError(status);
	
	return style;
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::Set
//
//---------------------------------------------------------------
void X3DInterpolationStyle::Set(TQ3InterpolationStyle style)
{
	TQ3Status status = Q3InterpolationStyle_Set(mObject, style);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DInterpolationStyle::IsCompatible		 			[static]
//	
//---------------------------------------------------------------
bool X3DInterpolationStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeInterpolation);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DHighlightStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DHighlightStyle::~X3DHighlightStyle
//
//---------------------------------------------------------------
X3DHighlightStyle::~X3DHighlightStyle()
{
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::X3DHighlightStyle (TQ3AttributeSet)
//
//---------------------------------------------------------------
X3DHighlightStyle::X3DHighlightStyle(TQ3AttributeSet attributes) : X3DStyle(Q3HighlightStyle_New(attributes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::X3DHighlightStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DHighlightStyle::X3DHighlightStyle(TQ3StyleObject object, bool) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeHighlight);
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::Clone
//
//---------------------------------------------------------------
X3DHighlightStyle* X3DHighlightStyle::Clone() const
{
	X3DHighlightStyle* object = new X3DHighlightStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DHighlightStyle::Submit(TQ3AttributeSet attributes, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3HighlightStyle_Submit(attributes, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::Get
//
//---------------------------------------------------------------
TQ3AttributeSet X3DHighlightStyle::Get() const
{
	TQ3AttributeSet attributes;
	
	TQ3Status status = Q3HighlightStyle_Get(mObject, &attributes);
	ThrowIf3DError(status);
	
	return attributes;
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::Set
//
//---------------------------------------------------------------
void X3DHighlightStyle::Set(TQ3AttributeSet attributes)
{
	TQ3Status status = Q3HighlightStyle_Set(mObject, attributes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHighlightStyle::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DHighlightStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeHighlight);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DOrientationStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DOrientationStyle::~X3DOrientationStyle
//
//---------------------------------------------------------------
X3DOrientationStyle::~X3DOrientationStyle()
{
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::X3DOrientationStyle (TQ3OrientationStyle)
//
//---------------------------------------------------------------
X3DOrientationStyle::X3DOrientationStyle(TQ3OrientationStyle direction) : X3DStyle(Q3OrientationStyle_New(direction))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::X3DOrientationStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DOrientationStyle::X3DOrientationStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeOrientation);
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::Clone
//
//---------------------------------------------------------------
X3DOrientationStyle* X3DOrientationStyle::Clone() const
{
	X3DOrientationStyle* object = new X3DOrientationStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DOrientationStyle::Submit(TQ3OrientationStyle direction, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3OrientationStyle_Submit(direction, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::Get
//
//---------------------------------------------------------------
TQ3OrientationStyle X3DOrientationStyle::Get() const
{
	TQ3OrientationStyle direction;
	
	TQ3Status status = Q3OrientationStyle_Get(mObject, &direction);
	ThrowIf3DError(status);
	
	return direction;
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::Set
//
//---------------------------------------------------------------
void X3DOrientationStyle::Set(TQ3OrientationStyle direction)
{
	TQ3Status status = Q3OrientationStyle_Set(mObject, direction);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DOrientationStyle::IsCompatible			 		[static]
//	
//---------------------------------------------------------------
bool X3DOrientationStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeOrientation);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DAntiAliasStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DAntiAliasStyle::~X3DAntiAliasStyle
//
//---------------------------------------------------------------
X3DAntiAliasStyle::~X3DAntiAliasStyle()
{
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::X3DAntiAliasStyle (TQ3AntiAliasStyleData)
//
//---------------------------------------------------------------
X3DAntiAliasStyle::X3DAntiAliasStyle(const TQ3AntiAliasStyleData& data) : X3DStyle(Q3AntiAliasStyle_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::X3DAntiAliasStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DAntiAliasStyle::X3DAntiAliasStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeAntiAlias);
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::Clone
//
//---------------------------------------------------------------
X3DAntiAliasStyle* X3DAntiAliasStyle::Clone() const
{
	X3DAntiAliasStyle* object = new X3DAntiAliasStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DAntiAliasStyle::Submit(const TQ3AntiAliasStyleData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3AntiAliasStyle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::GetData
//
//---------------------------------------------------------------
TQ3AntiAliasStyleData X3DAntiAliasStyle::GetData() const
{
	TQ3AntiAliasStyleData data;
	
	TQ3Status status = Q3AntiAliasStyle_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::SetData
//
//---------------------------------------------------------------
void X3DAntiAliasStyle::SetData(const TQ3AntiAliasStyleData& data)
{
	TQ3Status status = Q3AntiAliasStyle_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DAntiAliasStyle::IsCompatible			 			[static]
//	
//---------------------------------------------------------------
bool X3DAntiAliasStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeAntiAlias);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DFogStyle
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFogStyle::~X3DFogStyle
//
//---------------------------------------------------------------
X3DFogStyle::~X3DFogStyle()
{
}


//---------------------------------------------------------------
//
// X3DFogStyle::X3DFogStyle (TQ3FogStyleData)
//
//---------------------------------------------------------------
X3DFogStyle::X3DFogStyle(const TQ3FogStyleData& data) : X3DStyle(Q3FogStyle_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFogStyle::X3DFogStyle (TQ3StyleObject)
//
//---------------------------------------------------------------
X3DFogStyle::X3DFogStyle(TQ3StyleObject object) : X3DStyle(object)
{
	this->ConfirmType(kQ3StyleTypeOrientation);
}


//---------------------------------------------------------------
//
// X3DFogStyle::Clone
//
//---------------------------------------------------------------
X3DFogStyle* X3DFogStyle::Clone() const
{
	X3DFogStyle* object = new X3DFogStyle(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DFogStyle::Submit							[static]
//
//---------------------------------------------------------------
void X3DFogStyle::Submit(const TQ3FogStyleData& data, TQ3ViewObject view)
{
	PRECONDITION(view != nil);
	
	TQ3Status status = Q3FogStyle_Submit(&data, view);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFogStyle::Get
//
//---------------------------------------------------------------
TQ3FogStyleData X3DFogStyle::Get() const
{
	TQ3FogStyleData data;
	
	TQ3Status status = Q3FogStyle_GetData(mObject, &data);
	ThrowIf3DError(status);
	
	return data;
}


//---------------------------------------------------------------
//
// X3DFogStyle::Set
//
//---------------------------------------------------------------
void X3DFogStyle::Set(const TQ3FogStyleData& data)
{
	TQ3Status status = Q3FogStyle_SetData(mObject, &data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFogStyle::IsCompatible			 		[static]
//	
//---------------------------------------------------------------
bool X3DFogStyle::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StyleTypeOrientation);
	
	return compatible;
}


}	// namespace Whisper

