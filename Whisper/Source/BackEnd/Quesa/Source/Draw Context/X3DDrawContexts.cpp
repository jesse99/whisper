/*
 *  File:       X3DDrawContexts.cpp
 *  Summary:	QuickDraw 3D draw contexts.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DDrawContexts.cpp,v $
 *		Revision 1.4  2001/04/21 03:35:36  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/13 08:00:53  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 12:15:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DDrawContexts.h>

#include <X3DUtils.h>
#include <XConstants.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateWin32DCDrawContext
//
//---------------------------------------------------------------
#if TARGET_OS_WIN32
static X3DObject* CreateWin32DCDrawContext(TQ3Object inObject)
{
	X3DWin32DCDrawContext* object = new X3DWin32DCDrawContext(inObject);
	
	return object;
}
#endif


//---------------------------------------------------------------
//
// CreateMacDrawContext
//
//---------------------------------------------------------------
#if TARGET_OS_MAC
static X3DObject* CreateMacDrawContext(TQ3Object inObject)
{
	X3DMacDrawContext* object = new X3DMacDrawContext(inObject);
	
	return object;
}
#endif


//---------------------------------------------------------------
//
// CreatePixmapDrawContext
//
//---------------------------------------------------------------
static X3DObject* CreatePixmapDrawContext(TQ3Object inObject)
{
	X3DPixmapDrawContext* object = new X3DPixmapDrawContext(inObject);
	
	return object;
}

#pragma mark -

// ===================================================================================
//	struct SMacDrawContextData
// ===================================================================================

#if TARGET_OS_MAC

//---------------------------------------------------------------
//
// SMacDrawContextData::SMacDrawContextData ()
//
//---------------------------------------------------------------
SMacDrawContextData::SMacDrawContextData()
{
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) false;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	window   = nil;
	library  = kQ3Mac2DLibraryNone;
	viewPort = nil;
	grafPort = nil;
}


//---------------------------------------------------------------
//
// SMacDrawContextData::SMacDrawContextData (TQ3MacDrawContextData)
//
//---------------------------------------------------------------
SMacDrawContextData::SMacDrawContextData(const TQ3MacDrawContextData& data)
{
	drawContextData.clearImageMethod  = data.drawContextData.clearImageMethod;
	drawContextData.clearImageColor   = data.drawContextData.clearImageColor;
	drawContextData.pane.min          = data.drawContextData.pane.min;
	drawContextData.pane.max          = data.drawContextData.pane.max;
	drawContextData.paneState         = data.drawContextData.paneState;
	drawContextData.maskState         = data.drawContextData.maskState;
	drawContextData.doubleBufferState = data.drawContextData.doubleBufferState;

	drawContextData.mask.image    = data.drawContextData.mask.image;
	drawContextData.mask.width    = data.drawContextData.mask.width;
	drawContextData.mask.height   = data.drawContextData.mask.height;
	drawContextData.mask.rowBytes = data.drawContextData.mask.rowBytes;
	drawContextData.mask.bitOrder = data.drawContextData.mask.bitOrder;

	window   = data.window;
	library  = data.library;
	viewPort = data.viewPort;
	grafPort = data.grafPort;
}


//---------------------------------------------------------------
//
// SMacDrawContextData::SMacDrawContextData (OSWindow, X3DColorARGB, bool)
//
//---------------------------------------------------------------
SMacDrawContextData::SMacDrawContextData(OSWindow wptr, const X3DColorARGB& clearColor, bool doubleBuffer)
{
	PRECONDITION(wptr != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	window   = wptr;
	library  = kQ3Mac2DLibraryNone;
	viewPort = nil;
	grafPort = nil;
}


//---------------------------------------------------------------
//
// SMacDrawContextData::SMacDrawContextData (OSWindow, X2DRect, bool)
//
//---------------------------------------------------------------
SMacDrawContextData::SMacDrawContextData(OSWindow wptr, const X2DRect& paneArea, bool doubleBuffer)
{
	PRECONDITION(wptr != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	window   = wptr;
	library  = kQ3Mac2DLibraryNone;
	viewPort = nil;
	grafPort = nil;
}


//---------------------------------------------------------------
//
// SMacDrawContextData::SMacDrawContextData (OSWindow, X3DColorARGB, X2DRect, bool)
//
//---------------------------------------------------------------
SMacDrawContextData::SMacDrawContextData(OSWindow wptr, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer)
{
	PRECONDITION(wptr != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	window   = wptr;
	library  = kQ3Mac2DLibraryNone;
	viewPort = nil;
	grafPort = nil;
}
#endif	// TARGET_OS_MAC

#pragma mark -

// ===================================================================================
//	struct SWin32DCDrawContextData
// ===================================================================================

#if TARGET_OS_WIN32

//---------------------------------------------------------------
//
// SWin32DCDrawContextData::SWin32DCDrawContextData ()
//
//---------------------------------------------------------------
SWin32DCDrawContextData::SWin32DCDrawContextData()
{
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) false;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	hdc   = nil;
}


//---------------------------------------------------------------
//
// SWin32DCDrawContextData::SWin32DCDrawContextData (TQ3Win32DCDrawContextData)
//
//---------------------------------------------------------------
SWin32DCDrawContextData::SWin32DCDrawContextData(const TQ3Win32DCDrawContextData& data)
{
	drawContextData.clearImageMethod  = data.drawContextData.clearImageMethod;
	drawContextData.clearImageColor   = data.drawContextData.clearImageColor;
	drawContextData.pane.min          = data.drawContextData.pane.min;
	drawContextData.pane.max          = data.drawContextData.pane.max;
	drawContextData.paneState         = data.drawContextData.paneState;
	drawContextData.maskState         = data.drawContextData.maskState;
	drawContextData.doubleBufferState = data.drawContextData.doubleBufferState;

	drawContextData.mask.image    = data.drawContextData.mask.image;
	drawContextData.mask.width    = data.drawContextData.mask.width;
	drawContextData.mask.height   = data.drawContextData.mask.height;
	drawContextData.mask.rowBytes = data.drawContextData.mask.rowBytes;
	drawContextData.mask.bitOrder = data.drawContextData.mask.bitOrder;

	hdc   = data.hdc;
}


//---------------------------------------------------------------
//
// TQ3Win32DCDrawContextData::TQ3Win32DCDrawContextData (HDC, X3DColorARGB, bool)
//
//---------------------------------------------------------------
SWin32DCDrawContextData::SWin32DCDrawContextData(HDC dc, const X3DColorARGB& clearColor, bool doubleBuffer)
{
	PRECONDITION(dc != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	hdc = dc;
}


//---------------------------------------------------------------
//
// SWin32DCDrawContextData::SWin32DCDrawContextData (HDC, X2DRect, bool)
//
//---------------------------------------------------------------
SWin32DCDrawContextData::SWin32DCDrawContextData(HDC dc, const X2DRect& paneArea, bool doubleBuffer)
{
	PRECONDITION(dc != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	hdc = dc;
}


//---------------------------------------------------------------
//
// SWin32DCDrawContextData::SWin32DCDrawContextData (HDC, X3DColorARGB, X2DRect, bool)
//
//---------------------------------------------------------------
SWin32DCDrawContextData::SWin32DCDrawContextData(HDC dc, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer)
{
	PRECONDITION(dc != nil);
	
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	hdc = dc;
}
#endif	// TARGET_OS_WIN32

#pragma mark -

// ===================================================================================
//	struct SPixmapDrawContextData
// ===================================================================================

#if TARGET_OS_MAC

//---------------------------------------------------------------
//
// SPixmapDrawContextData::SPixmapDrawContextData ()
//
//---------------------------------------------------------------
SPixmapDrawContextData::SPixmapDrawContextData()
{
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) false;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	pixmap.image     = nil;
	pixmap.width     = 0;
	pixmap.height    = 0;
	pixmap.rowBytes  = 0;
	pixmap.pixelSize = 32;
	pixmap.pixelType = kQ3PixelTypeRGB32;
	pixmap.bitOrder  = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
	pixmap.byteOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;
}


//---------------------------------------------------------------
//
// SPixmapDrawContextData::SPixmapDrawContextData (TQ3PixmapDrawContextData)
//
//---------------------------------------------------------------
SPixmapDrawContextData::SPixmapDrawContextData(const TQ3PixmapDrawContextData& data)
{
	drawContextData.clearImageMethod  = data.drawContextData.clearImageMethod;
	drawContextData.clearImageColor   = data.drawContextData.clearImageColor;
	drawContextData.pane.min          = data.drawContextData.pane.min;
	drawContextData.pane.max          = data.drawContextData.pane.max;
	drawContextData.paneState         = data.drawContextData.paneState;
	drawContextData.maskState         = data.drawContextData.maskState;
	drawContextData.doubleBufferState = data.drawContextData.doubleBufferState;

	drawContextData.mask.image    = data.drawContextData.mask.image;
	drawContextData.mask.width    = data.drawContextData.mask.width;
	drawContextData.mask.height   = data.drawContextData.mask.height;
	drawContextData.mask.rowBytes = data.drawContextData.mask.rowBytes;
	drawContextData.mask.bitOrder = data.drawContextData.mask.bitOrder;

	pixmap.image     = data.pixmap.image;
	pixmap.width     = data.pixmap.width;
	pixmap.height    = data.pixmap.height;
	pixmap.rowBytes  = data.pixmap.rowBytes;
	pixmap.pixelSize = data.pixmap.pixelSize;
	pixmap.pixelType = data.pixmap.pixelType;
	pixmap.bitOrder  = data.pixmap.bitOrder;
	pixmap.byteOrder = data.pixmap.byteOrder;
}


//---------------------------------------------------------------
//
// SPixmapDrawContextData::SPixmapDrawContextData (SPixmap, X3DColorARGB, bool)
//
//---------------------------------------------------------------
SPixmapDrawContextData::SPixmapDrawContextData(const SPixmap& pixels, const X3DColorARGB& clearColor, bool doubleBuffer)
{
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = kZero2DPt;
	drawContextData.pane.max          = kZero2DPt;
	drawContextData.paneState         = (TQ3Boolean) false;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	pixmap.image     = pixels.image;
	pixmap.width     = pixels.width;
	pixmap.height    = pixels.height;
	pixmap.rowBytes  = pixels.rowBytes;
	pixmap.pixelSize = pixels.pixelSize;
	pixmap.pixelType = pixels.pixelType;
	pixmap.bitOrder  = pixels.bitOrder;
	pixmap.byteOrder = pixels.byteOrder;
}


//---------------------------------------------------------------
//
// SPixmapDrawContextData::SPixmapDrawContextData (SPixmap, X2DRect, bool)
//
//---------------------------------------------------------------
SPixmapDrawContextData::SPixmapDrawContextData(const SPixmap& pixels, const X2DRect& paneArea, bool doubleBuffer)
{
	drawContextData.clearImageMethod  = kQ3ClearMethodNone;
	drawContextData.clearImageColor   = X3DColorARGB(kRGBWhite);
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	pixmap.image     = pixels.image;
	pixmap.width     = pixels.width;
	pixmap.height    = pixels.height;
	pixmap.rowBytes  = pixels.rowBytes;
	pixmap.pixelSize = pixels.pixelSize;
	pixmap.pixelType = pixels.pixelType;
	pixmap.bitOrder  = pixels.bitOrder;
	pixmap.byteOrder = pixels.byteOrder;
}


//---------------------------------------------------------------
//
// SPixmapDrawContextData::SPixmapDrawContextData (SPixmap, X3DColorARGB, X2DRect, bool)
//
//---------------------------------------------------------------
SPixmapDrawContextData::SPixmapDrawContextData(const SPixmap& pixels, const X3DColorARGB& clearColor, const X2DRect& paneArea, bool doubleBuffer)
{
	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.clearImageColor   = clearColor;
	drawContextData.pane.min          = paneArea[topLeft];
	drawContextData.pane.max          = paneArea[botRight];
	drawContextData.paneState         = (TQ3Boolean) true;
	drawContextData.maskState         = (TQ3Boolean) false;
	drawContextData.doubleBufferState = (TQ3Boolean) doubleBuffer;

	drawContextData.mask.image    = nil;
	drawContextData.mask.width    = 0;
	drawContextData.mask.height   = 0;
	drawContextData.mask.rowBytes = 0;
	drawContextData.mask.bitOrder = BIG_ENDIAN ? kQ3EndianBig : kQ3EndianLittle;

	pixmap.image     = pixels.image;
	pixmap.width     = pixels.width;
	pixmap.height    = pixels.height;
	pixmap.rowBytes  = pixels.rowBytes;
	pixmap.pixelSize = pixels.pixelSize;
	pixmap.pixelType = pixels.pixelType;
	pixmap.bitOrder  = pixels.bitOrder;
	pixmap.byteOrder = pixels.byteOrder;
}
#endif		// TARGET_OS_MAC

#pragma mark -

// ===================================================================================
//	class X3DMacDrawContext
// ===================================================================================

#if TARGET_OS_MAC

//---------------------------------------------------------------
//
// X3DMacDrawContext::~X3DMacDrawContext
//
//---------------------------------------------------------------
X3DMacDrawContext::~X3DMacDrawContext()
{
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::X3DMacDrawContext (SMacDrawContextData)
//
//---------------------------------------------------------------
X3DMacDrawContext::X3DMacDrawContext(const SMacDrawContextData& data) : X3DDrawContext(Q3MacDrawContext_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::X3DMacDrawContext (TQ3DrawContextObject)
//
//---------------------------------------------------------------
X3DMacDrawContext::X3DMacDrawContext(TQ3DrawContextObject object) : X3DDrawContext(object)
{ 
	this->ConfirmType(kQ3DrawContextTypeMacintosh);
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::Clone
//
//---------------------------------------------------------------
X3DMacDrawContext* X3DMacDrawContext::Clone() const
{
	X3DMacDrawContext* object = new X3DMacDrawContext(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::GetWindow
//
//---------------------------------------------------------------
OSWindow X3DMacDrawContext::GetWindow() const
{
	CWindowPtr wptr;
	
	TQ3Status status = Q3MacDrawContext_GetWindow(mObject, &wptr);
	ThrowIf3DError(status);
	
	return wptr;
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::SetWindow
//
//---------------------------------------------------------------
void X3DMacDrawContext::SetWindow(OSWindow wptr)
{
	PRECONDITION(wptr != nil);
	
	TQ3Status status = Q3MacDrawContext_SetWindow(mObject, wptr);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::Get2DLibrary
//
//---------------------------------------------------------------
TQ3MacDrawContext2DLibrary X3DMacDrawContext::Get2DLibrary() const
{
	TQ3MacDrawContext2DLibrary library;
	
	TQ3Status status = Q3MacDrawContext_Get2DLibrary(mObject, &library);
	ThrowIf3DError(status);
	
	return library;
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::Set2DLibrary
//
//---------------------------------------------------------------
void X3DMacDrawContext::Set2DLibrary(TQ3MacDrawContext2DLibrary library)
{
	TQ3Status status = Q3MacDrawContext_Set2DLibrary(mObject, library);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::GetData
//
//---------------------------------------------------------------
SMacDrawContextData X3DMacDrawContext::GetData() const
{
	SMacDrawContextData data;
	
	data.drawContextData = Inherited::GetData();
	data.window          = this->GetWindow();
	data.library         = this->Get2DLibrary();
	
	return data;
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::SetData
//
//---------------------------------------------------------------
void X3DMacDrawContext::SetData(const SMacDrawContextData& data)
{
	Inherited::SetData(data.drawContextData);
	
	this->SetWindow(data.window);
	this->Set2DLibrary(data.library);
}


//---------------------------------------------------------------
//
// X3DMacDrawContext::IsCompatible 						[static]
//
//---------------------------------------------------------------
bool X3DMacDrawContext::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3DrawContextTypeMacintosh);
	
	return compatible;
}
#endif	// TARGET_OS_MAC

#pragma mark -

// ===================================================================================
//	class X3DWin32DCDrawContext
// ===================================================================================

#if TARGET_OS_WIN32

//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::~X3DWin32DCDrawContext
//
//---------------------------------------------------------------
X3DWin32DCDrawContext::~X3DWin32DCDrawContext()
{
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::X3DWin32DCDrawContext (SWin32DCDrawContextData)
//
//---------------------------------------------------------------
X3DWin32DCDrawContext::X3DWin32DCDrawContext(const SWin32DCDrawContextData& data) : X3DDrawContext(Q3Win32DCDrawContext_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::X3DWin32DCDrawContext (TQ3DrawContextObject)
//
//---------------------------------------------------------------
X3DWin32DCDrawContext::X3DWin32DCDrawContext(TQ3DrawContextObject object) : X3DDrawContext(object)
{ 
	this->ConfirmType(kQ3DrawContextTypeWin32DC);
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::Clone
//
//---------------------------------------------------------------
X3DWin32DCDrawContext* X3DWin32DCDrawContext::Clone() const
{
	X3DWin32DCDrawContext* object = new X3DWin32DCDrawContext(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::GetWindow
//
//---------------------------------------------------------------
HDC X3DWin32DCDrawContext::GetWindow() const
{
	HDC wptr;
	
	TQ3Status status = Q3Win32DCDrawContext_GetDC(mObject, &wptr);
	ThrowIf3DError(status);
	
	return wptr;
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::SetWindow
//
//---------------------------------------------------------------
void X3DWin32DCDrawContext::SetWindow(HDC wptr)
{
	PRECONDITION(wptr != nil);
	
	TQ3Status status = Q3Win32DCDrawContext_SetDC(mObject, wptr);
	ThrowIf3DError(status);
}



//---------------------------------------------------------------
//
// SWin32DCDrawContextData::GetData
//
//---------------------------------------------------------------
SWin32DCDrawContextData X3DWin32DCDrawContext::GetData() const
{
	SWin32DCDrawContextData data;
	
	data.drawContextData = Inherited::GetData();
	data.hdc          = this->GetWindow();
	
	return data;
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::SetData
//
//---------------------------------------------------------------
void X3DWin32DCDrawContext::SetData(const SWin32DCDrawContextData& data)
{
	Inherited::SetData(data.drawContextData);
	
	this->SetWindow(data.hdc);
}


//---------------------------------------------------------------
//
// X3DWin32DCDrawContext::IsCompatible 					[static]
//
//---------------------------------------------------------------
bool X3DWin32DCDrawContext::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3DrawContextTypeWin32DC );
	
	return compatible;
}
#endif	// TARGET_OS_WIN32

#pragma mark -

// ===================================================================================
//	class X3DPixmapDrawContext
// ===================================================================================

#if TARGET_OS_MAC

//---------------------------------------------------------------
//
// X3DPixmapDrawContext::~X3DPixmapDrawContext
//
//---------------------------------------------------------------
X3DPixmapDrawContext::~X3DPixmapDrawContext()
{
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::X3DPixmapDrawContext (SPixmapDrawContextData)
//
//---------------------------------------------------------------
X3DPixmapDrawContext::X3DPixmapDrawContext(const SPixmapDrawContextData& data) : X3DDrawContext(Q3PixmapDrawContext_New(&data))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::X3DPixmapDrawContext (TQ3DrawContextObject)
//
//---------------------------------------------------------------
X3DPixmapDrawContext::X3DPixmapDrawContext(TQ3DrawContextObject object) : X3DDrawContext(object)
{ 
	this->ConfirmType(kQ3DrawContextTypePixmap);
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::Clone
//
//---------------------------------------------------------------
X3DPixmapDrawContext* X3DPixmapDrawContext::Clone() const
{
	X3DPixmapDrawContext* object = new X3DPixmapDrawContext(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::GetPixmap
//
//---------------------------------------------------------------
SPixmap X3DPixmapDrawContext::GetPixmap() const
{
	TQ3Pixmap pixmap;
	
	TQ3Status status = Q3PixmapDrawContext_GetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
	
	return pixmap;
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::SetPixmap
//
//---------------------------------------------------------------
void X3DPixmapDrawContext::SetPixmap(const SPixmap& pixmap)
{	
	TQ3Status status = Q3PixmapDrawContext_SetPixmap(mObject, &pixmap);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::GetData
//
//---------------------------------------------------------------
SPixmapDrawContextData X3DPixmapDrawContext::GetData() const
{
	SPixmapDrawContextData data;
	
	data.drawContextData = Inherited::GetData();
	data.pixmap = this->GetPixmap();
	
	return data;
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::SetData
//
//---------------------------------------------------------------
void X3DPixmapDrawContext::SetData(const SPixmapDrawContextData& data)
{
	Inherited::SetData(data.drawContextData);
	
	this->SetPixmap(data.pixmap);
}


//---------------------------------------------------------------
//
// X3DPixmapDrawContext::IsCompatible 					[static]
//
//---------------------------------------------------------------
bool X3DPixmapDrawContext::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3DrawContextTypePixmap);
	
	return compatible;
}
#endif	// TARGET_OS_MAC


}	// namespace Whisper
