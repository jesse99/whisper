/*
 *  File:       WBrush.cpp
 *  Summary:	An object used when filling shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WBrush.cpp,v $
 *		Revision 1.4  2001/04/21 03:30:22  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/17 01:41:45  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:04:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	  8/09/99	JDJ		Added support for streaming and kSolidBrush.
 *		 <6>	  8/09/99	JDJ		Uses new CreatePackedDIB function.
 *		 <5>	  8/06/99	JDJ		Rewrote XBrush::operator== so that it works even if the HBRUSH
 *									handles differ.
 *		 <4>	  8/06/99	JDJ		Added support for streaming and kSolidBrush.
 *		 <3>	  1/15/99	JDJ		Added kSelectionBrushes.
 *		 <2>	 12/24/98	JDJ		Enabled XBrush::XBrush (XPixMap, EDrawMode).
 *		 <1>	 11/30/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XBrush.h>

#include <WGDIUtils.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XMiscUtils.h>
#include <XPixMap.h>
#include <XPointer.h>
#include <XReferenceCounted.h>
#include <WSystemInfo.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XBrush kBlackBrush(XRGBColor(0.0,  0.0,  0.0));		// we don't use the XRGBColor constants because of order of initialization problems
const XBrush kWhiteBrush(XRGBColor(1.0,  1.0,  1.0));
const XBrush kRedBrush(XRGBColor(0.87, 0.0,  0.0));
const XBrush kYellowBrush(XRGBColor(1.0,  1.0,  0.0));
const XBrush kBlueBrush(XRGBColor(0.0,  0.0,  0.87));
const XBrush kGreenBrush(XRGBColor(0.0,  0.93, 0.0));
const XBrush kBrownBrush(XRGBColor(0.6,  0.4,  0.0));
const XBrush kPurpleBrush(XRGBColor(0.4,  0.0,  0.6));
const XBrush kLightBlueBrush(XRGBColor(0.0,  0.6,  1.0));
const XBrush kGrayBrush(XRGBColor(0.5,  0.5,  0.5));
const XBrush kLightGrayBrush(XRGBColor(0.75, 0.75, 0.75));
const XBrush kDarkGrayBrush(XRGBColor(0.25, 0.25, 0.25));

const XBrush sSelectionBrushes[kNumSelectionBrushes];	
const XBrush* kSelectionBrushes = sSelectionBrushes;		


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RotateRight
// 
//---------------------------------------------------------------
static uint8 RotateRight(uint8& n)
{	
	if (n & 1) 
		n = (uint8) ((n >> 1) + 0x80);
	else
		n = (uint8) (n >> 1);

	return n;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZSelectionBrushInitter
// ===================================================================================
class ZSelectionBrushInitter {

public:
						~ZSelectionBrushInitter();		
						
						ZSelectionBrushInitter(); 
};

static ZSelectionBrushInitter sBrushInitter;

//---------------------------------------------------------------
//
// ZSelectionBrushInitter::~ZSelectionBrushInitter
//
//---------------------------------------------------------------
ZSelectionBrushInitter::~ZSelectionBrushInitter()
{	
}
		

//---------------------------------------------------------------
//
// ZSelectionBrushInitter::ZSelectionBrushInitter
// 
//---------------------------------------------------------------
ZSelectionBrushInitter::ZSelectionBrushInitter()
{	
	uint8 seed = 0x87;
	
	XPixMap pixmap(XSize(8, 8), nil, 1);
		
	for (uint32 index = 0; index < kNumSelectionBrushes; ++index) {
		uint8 pixel = seed;
		*pixmap.GetUnsafeBufferAt(0, 0) = pixel;
		*pixmap.GetUnsafeBufferAt(0, 1) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 2) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 3) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 4) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 5) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 6) = RotateRight(pixel);
		*pixmap.GetUnsafeBufferAt(0, 7) = RotateRight(pixel);

		XBrush temp(pixmap);
		const_cast<XBrush&>(kSelectionBrushes[index]) = temp;
//		const_cast<XBrush&>(kSelectionBrushes[index]) = XPen(pixmap);	// MSVC 5 won't compile this...
		
		RotateRight(seed);
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZBrush
//		Windows doesn't appear to provide any mechanism for duplicating GDI objects so
//		we'll use this class to simplify copying XBrush's.
// ===================================================================================
class ZBrush : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZBrush();		
						
						ZBrush(HBRUSH brush); 

private:					
						ZBrush(const ZBrush& rhs);

			ZBrush& 	operator=(const ZBrush& rhs);
						
//-----------------------------------
//	API
//
public:
			HBRUSH 		GetBrush() const							{return mBrush;}

			HBRUSH 		Select(HDC dc);
			void 		DeSelect(HDC dc, HBRUSH oldBrush);

//-----------------------------------
//	Member Data
//
protected:
	XAtomicCounter	mSelectCount;
	HBRUSH			mBrush;
};


//---------------------------------------------------------------
//
// ZBrush::~ZBrush
//
// Here's some more Windows lameness: you cannot delete a GDI
// object while it's selected into a DC. You'd think that after
// more than a decade they could add a reference count to an
// opaque data structure and let the OS manage resources like
// it's supposed to! To ensure that users are properly managing
// brushes the code below maintains a selection count.
//
//---------------------------------------------------------------
ZBrush::~ZBrush()
{	
	if (mSelectCount == 0)
		VERIFY(::DeleteObject(mBrush));
	else
		DEBUGSTR("Tried to destroy a brush that was still selected!");	// brush will leak, but since we don't know which DC it's selected into there's not much we can do about it...

	mBrush = nil;
}
		

//---------------------------------------------------------------
//
// ZBrush::ZBrush
//
//---------------------------------------------------------------
ZBrush::ZBrush(HBRUSH brush)
{
	ThrowIfBadHandle(brush);
	
	mSelectCount = 0;
	mBrush = brush;
}


//---------------------------------------------------------------
//
// ZBrush::Select
//
// $$ The docs say that NT "automatically tracks the origin of all 
// $$ window-managed device contexts and adjusts their brushes as 
// $$ necessary to maintain an alignment of patterns on the surface."
// $$ I'm not certain what a window-managed device context is, but
// $$ NT doesn't appear to be updating the origin of my selection
// $$ brush...
//
//---------------------------------------------------------------
HBRUSH ZBrush::Select(HDC dc)
{
	PRECONDITION(dc != nil);
	PRECONDITION(mSelectCount >= 0);
	
//	if (!WSystemInfo::IsNT()) {
		XPoint origin;
		VERIFY(::GetViewportOrgEx(dc, origin));
		VERIFY(::UnrealizeObject(mBrush));
		VERIFY(::SetBrushOrgEx(dc, origin.x, origin.y, nil));
//	}

	HGDIOBJ oldObject = SelectObject(dc, mBrush);
	ThrowIf(oldObject == nil || (uint32) oldObject == GDI_ERROR);
	
	++mSelectCount;
	
	return (HBRUSH) oldObject;
}


//---------------------------------------------------------------
//
// ZBrush::DeSelect
//
//---------------------------------------------------------------
void ZBrush::DeSelect(HDC dc, HBRUSH oldBrush)
{
	PRECONDITION(dc != nil);
	PRECONDITION(oldBrush != nil);
	PRECONDITION(mSelectCount > 0);				// note that if you're using XSelectAttribute this won't be called if Select fails

	(void) SelectObject(dc, oldBrush);		// don't throw here (this is called from dtors)

	--mSelectCount;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XBrush
// ===================================================================================

//---------------------------------------------------------------
//
// XBrush::~XBrush
//
//---------------------------------------------------------------
XBrush::~XBrush()
{
	PRECONDITION(mOldBrush == nil);
	
	mBrush->RemoveReference();
}
		

//---------------------------------------------------------------
//
// XBrush::XBrush (XRGBColor, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(const XRGBColor& color, EDrawMode mode)
{
	mOldBrush = nil;
	
	mDrawMode = mode;
	mBrush = new ZBrush(CreateSolidBrush(color.GetColorRef()));
}


//---------------------------------------------------------------
//
// XBrush::XBrush (EHatchBrush, XRGBColor, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(EHatchBrush brush, const XRGBColor& color, EDrawMode mode)
{		
	mOldBrush = nil;
	mDrawMode = mode;

	switch (brush) {
		case kSolidBrush:
			mBrush = new ZBrush(CreateSolidBrush(color.GetColorRef()));
			break;
			
		case kDiagnalBrush:
			mBrush = new ZBrush(CreateHatchBrush(HS_FDIAGONAL, color.GetColorRef()));
			break;
			
		case kCrossBrush:
			mBrush = new ZBrush(CreateHatchBrush(HS_CROSS, color.GetColorRef()));
			break;
			
		case kHorzBrush:
			mBrush = new ZBrush(CreateHatchBrush(HS_HORIZONTAL, color.GetColorRef()));
			break;
			
		case kVertBrush:
			mBrush = new ZBrush(CreateHatchBrush(HS_VERTICAL, color.GetColorRef()));
			break;
			
		default:
			mBrush = new ZBrush(CreateSolidBrush(color.GetColorRef()));
			DEBUGSTR("Bad brush in XBrush ctor");
	}
}


//---------------------------------------------------------------
//
// XBrush::XBrush (XPixMap, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(const XPixMap& pixMap, EDrawMode mode)
{
	PRECONDITION(pixMap.GetWidth() <= 8);		// Windows 98 doesn't support pattern brushes larger than 8x8
	PRECONDITION(pixMap.GetHeight() <= 8);

	// CreateDIBPatternBrush wants a DIB, but doesn't work with 
	// DIB-sections so we have to convert our pixmap into a DIB...
	XLocker lock(pixMap);
	XPointer dib = Whisper::CreatePackedDIB(pixMap.GetInfo(), pixMap.GetBuffer());

	// Now that we have the DIB we can create the brush.
	mBrush = new ZBrush(CreateDIBPatternBrushPt(dib.GetPtr(), DIB_RGB_COLORS));

	mOldBrush = nil;
	mDrawMode = mode;
}


//---------------------------------------------------------------
//
// XBrush::XBrush (HBRUSH, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(HBRUSH takeBrush, EDrawMode mode)
{
	PRECONDITION(takeBrush != nil);
	
	mOldBrush = nil;
	
	mDrawMode = mode;
	mBrush = new ZBrush(takeBrush);
}


//---------------------------------------------------------------
//
// XBrush::XBrush (XBrush)
//
//---------------------------------------------------------------
XBrush::XBrush(const XBrush& rhs)
{
	mOldBrush = nil;
	
	mDrawMode = rhs.mDrawMode;
	
	mBrush = rhs.mBrush;
	mBrush->AddReference();
}


//---------------------------------------------------------------
//
// XBrush::operator=
//
//---------------------------------------------------------------
XBrush& XBrush::operator=(const XBrush& rhs)
{
	if (this != &rhs) {
		mBrush->RemoveReference();

		mDrawMode = rhs.mDrawMode;
	
		mBrush = rhs.mBrush;
		mBrush->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XBrush::operator HBRUSH
//
//---------------------------------------------------------------
XBrush::operator HBRUSH() const
{
	return mBrush->GetBrush();
}


//---------------------------------------------------------------
//
// XBrush::operator==
//
//---------------------------------------------------------------
bool XBrush::operator==(const XBrush& rhs) const
{
	bool equal = mDrawMode == rhs.mDrawMode;	// If draw modes and,
	
	if (equal) {
		equal = mBrush == rhs.mBrush;			// letters are the same we're done,
		
		if (!equal) {							// otherwise we need to compare the hard way.
			LOGBRUSH info1;						// $$ this code will work if you use the cross-platform API, but if you use the HBRUSH ctor lbStyle may be something not yet checked...
			int32 result = ::GetObject(mBrush->GetBrush(), sizeof(info1), &info1);
			ThrowIf(result <= 0);
			
			LOGBRUSH info2;
			result = ::GetObject(rhs.mBrush->GetBrush(), sizeof(info2), &info2);
			ThrowIf(result <= 0);

			if (info1.lbStyle == BS_SOLID && info2.lbStyle == BS_SOLID) 
				equal = info1.lbColor == info2.lbColor;
				
			else if (info1.lbStyle == BS_HATCHED && info2.lbStyle == BS_HATCHED) 
				equal = info1.lbHatch == info2.lbHatch;
				
			else if (info1.lbStyle == BS_DIBPATTERNPT && info2.lbStyle == BS_DIBPATTERNPT) {
				const BITMAPINFOHEADER* header1 = reinterpret_cast<const BITMAPINFOHEADER*>(info1.lbStyle);
				const BITMAPINFOHEADER* header2 = reinterpret_cast<const BITMAPINFOHEADER*>(info2.lbStyle);

				if (header1 != nil && header2 != nil) 
					equal = EqualHeader(header1, header2) && EqualPixels(header1, header2);
			}
		}
	}
			
	return equal;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XBrush::Invariant
//
//---------------------------------------------------------------
void XBrush::Invariant() const
{	
	ASSERT(mBrush != nil);
}


//---------------------------------------------------------------
//
// XBrush::OnStreamOut
//
//---------------------------------------------------------------
void XBrush::OnStreamOut(XXMLDoc& doc, XXMLElement& element) const
{
	UNUSED(doc);
	
	element.AppendAttribute(XXMLAttribute(L"mode", DrawModeToStr(mDrawMode)));
	
	LOGBRUSH info;
	int32 result = ::GetObject(mBrush->GetBrush(), sizeof(info), &info);
	ThrowIf(result <= 0);

	if (info.lbStyle == BS_SOLID) 
		element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
		
	else if (info.lbStyle == BS_HATCHED) 
		if (info.lbHatch == HS_FDIAGONAL)
			element.AppendAttribute(XXMLAttribute(L"brush", L"kDiagnalBrush"));
	
		else if (info.lbHatch == HS_CROSS)
			element.AppendAttribute(XXMLAttribute(L"brush", L"kCrossBrush"));
	
		else if (info.lbHatch == HS_HORIZONTAL)
			element.AppendAttribute(XXMLAttribute(L"brush", L"kHorzBrush"));
	
		else if (info.lbHatch == HS_VERTICAL)
			element.AppendAttribute(XXMLAttribute(L"brush", L"kVertBrush"));
			
		else {
			DEBUGSTR("Couldn't find a match for the hatch pattern in XBrush::OnStreamOut.");
			element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
		}
		
	else {
		element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
		DEBUGSTR("This brush cannot be streamed out.");
	}
	
	element.AppendAttribute(XXMLAttribute(L"color", XRGBColor(info.lbColor).GetOSColor()));
}


//---------------------------------------------------------------
//
// XBrush::OnStreamIn
//
//---------------------------------------------------------------
void XBrush::OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element)
{
	PRECONDITION(element.item_begin() == element.item_end());	// we don't have any child elements
	PRECONDITION(element.attr_begin() != element.attr_end());	// we do have attributes
	UNUSED(doc);										
 	UNUSED(root);
 	
 	EHatchBrush brush = kSolidBrush;
 	XRGBColor color = kRGBBlack;
 	EDrawMode mode = kCopyMode;
 	
	XXMLElement::const_attr_iterator iter = element.attr_begin();	
	while (iter != element.attr_end()) {
		XXMLAttribute attr = *iter;
		++iter;
		
		const std::wstring& name = attr.GetName();
		const std::wstring& value = attr.GetValue();
		
		if (name == L"mode") {			
			mode = StrToDrawMode(value);
			
		} else if (name == L"brush") {
			if (value == L"kSolidBrush")
				brush = kSolidBrush;
			
			else if (value == L"kDiagnalBrush")
				brush = kDiagnalBrush;
			
			else if (value == L"kCrossBrush")
				brush = kCrossBrush;
			
			else if (value == L"kHorzBrush")
				brush = kHorzBrush;
			
			else if (value == L"kVertBrush")
				brush = kVertBrush;
			
			else {
				brush = kSolidBrush;
				DEBUGSTR(value, " isn't a valid brush value!");
			}
			
		} else if (name == L"color") {			
			color = attr.Get<XRGBColor>();
						
		} else
			DEBUGSTR(name, " isn't a valid attribute of XBrush!");
	}
	
	XBrush temp(brush, color, mode);
	*this = temp;
}


//---------------------------------------------------------------
//
// XBrush::DoSelect
//
//---------------------------------------------------------------
void XBrush::DoSelect(OSDrawContext context) const
{
	PRECONDITION(mOldBrush == nil);
		
	mOldBrush = mBrush->Select(context);
	mOldMode  = SetROP2(context, (int32) mDrawMode);
}


//---------------------------------------------------------------
//
// XBrush::DoDeSelect
//
//---------------------------------------------------------------
void XBrush::DoDeSelect(OSDrawContext context) const
{
	mBrush->DeSelect(context, mOldBrush);
	mOldBrush = nil;

	(void) SetROP2(context, mOldMode);
}


}	// namespace Whisper

