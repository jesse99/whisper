/*
 *  File:       WPen.cpp
 *  Summary:	An object used when drawing lines or framing shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WPen.cpp,v $
 *		Revision 1.5  2001/04/21 03:33:01  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/17 01:42:26  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/10 04:07:28  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:10:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  8/09/99	JDJ		Uses new CreatePackedDIB function. Rewrote XPen::operator== 
 *									so that it works even if the HPEN handles differ.
 *		 <4>	  1/15/99	JDJ		Replaced kSelectionPens array with kSelectionBrushes.
 *		 <3>	  1/15/99	JDJ		Disabled XPen ctor taking an XPixMap.
 *		 <2>	 12/24/98	JDJ		Added XPen::XPen (HPEN, EDrawMode) and ZSelectionPenInitter.
 *		 <1>	 11/30/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XPen.h>

#include <WGDIUtils.h>
#include <XDrawContext.h>
#include <XExceptions.h>
#include <XPixMap.h>
#include <XPointer.h>
#include <XReferenceCounted.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kPenStyle = PS_GEOMETRIC   | 		// allows width to vary
						 PS_INSIDEFRAME | 		// so Windows frames shapes like the Mac
						 PS_JOIN_MITER;			// so corners are drawn like the Mac(?)

const XPen kBlackPen(XRGBColor(0.0,  0.0,  0.0));		// we don't use the XRGBColor constants because of order of initialization problems
const XPen kWhitePen(XRGBColor(1.0,  1.0,  1.0));
const XPen kRedPen(XRGBColor(0.87, 0.0,  0.0));
const XPen kYellowPen(XRGBColor(1.0,  1.0,  0.0));
const XPen kBluePen(XRGBColor(0.0,  0.0,  0.87));
const XPen kGreenPen(XRGBColor(0.0,  0.93, 0.0));
const XPen kBrownPen(XRGBColor(0.6,  0.4,  0.0));
const XPen kPurplePen(XRGBColor(0.4,  0.0,  0.6));
const XPen kLightBluePen(XRGBColor(0.0,  0.6,  1.0));
const XPen kGrayPen(XRGBColor(0.5,  0.5,  0.5));
const XPen kLightGrayPen(XRGBColor(0.75, 0.75, 0.75));
const XPen kDarkGrayPen(XRGBColor(0.25, 0.25, 0.25));


// ===================================================================================
//	class ZPen
//		Windows doesn't appear to provide any mechanism for duplicating GDI objects so
//		we'll use this class to simplify copying XPen's.
// ===================================================================================
class ZPen : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZPen();		
						
						ZPen(HPEN pen);
						
private:		
						ZPen(const ZPen& rhs);
						
			ZPen&		operator=(const ZPen& rhs);
						
//-----------------------------------
//	API
//
public:
			HPEN 		GetPen() const							{return mPen;}

			HPEN 		Select(HDC dc);
			
			void 		DeSelect(HDC dc, HPEN oldPen);
						
//-----------------------------------
//	Member Data
//
protected:
	XAtomicCounter	mSelectCount;
	HPEN			mPen;
};


//---------------------------------------------------------------
//
// ZPen::~ZPen
//
// Here's some more Windows lameness: you cannot delete a GDI
// object while it's selected into a DC. You'd think that after
// more than a decade they could add a reference count to an
// opaque data structure and let the OS manage resources like
// it's supposed to! To ensure that users are properly managing
// brushes the code below maintains a selection count.
//
//---------------------------------------------------------------
ZPen::~ZPen()
{
	if (mSelectCount == 0)
		VERIFY(::DeleteObject(mPen));
	else
		DEBUGSTR("Tried to destroy a pen that was still selected!");	// pen will leak, but since we don't know which DC it's selected into there's not much we can do about it...

	mPen = nil;
}
		

//---------------------------------------------------------------
//
// ZPen::ZPen (HPEN)
//
//---------------------------------------------------------------
ZPen::ZPen(HPEN pen)
{
	ThrowIfBadHandle(pen);
	
	mSelectCount = 0;
	mPen = pen;
}


//---------------------------------------------------------------
//
// ZPen::Select
//
//---------------------------------------------------------------
HPEN ZPen::Select(HDC dc)
{
	PRECONDITION(dc != nil);
	PRECONDITION(mSelectCount >= 0);
	
	HGDIOBJ oldObject = ::SelectObject(dc, mPen);
	ThrowIf(oldObject == nil || (uint32) oldObject == GDI_ERROR);
	
	++mSelectCount;
	
	return (HPEN) oldObject;
}


//---------------------------------------------------------------
//
// ZPen::DeSelect
//
//---------------------------------------------------------------
void ZPen::DeSelect(HDC dc, HPEN oldPen)
{
	PRECONDITION(dc != nil);
	PRECONDITION(oldPen != nil);
	PRECONDITION(mSelectCount > 0);				// note that if you're using XSelectAttribute this won't be called if Select fails

	(void) ::SelectObject(dc, oldPen);		// don't throw here (this is called from dtors)

	--mSelectCount;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XPen
// ===================================================================================

//---------------------------------------------------------------
//
// XPen::~XPen
//
//---------------------------------------------------------------
XPen::~XPen()
{
	PRECONDITION(mOldPen == nil);
	
	mPen->RemoveReference();
	mPen = nil;
}


//---------------------------------------------------------------
//
// XPen::XPen (XRGBColor, int32, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(const XRGBColor& color, int32 width, EDrawMode mode)
{
	PRECONDITION(width >= 0);
	
	LOGBRUSH brush;
	brush.lbStyle = BS_SOLID;
	brush.lbColor = color.GetColorRef();
	brush.lbHatch = 0;					// ignored with BS_SOLID
		
	mOldPen = nil;
	
	mDrawMode = mode;
	mPen = new ZPen(ExtCreatePen(kPenStyle, numeric_cast<uint32>(width), &brush, 0, nil));

	mWidth = width;
}


//---------------------------------------------------------------
//
// XPen::XPen (EHatchBrush, XRGBColor, int32, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(EHatchBrush hatch, const XRGBColor& color, int32 width, EDrawMode mode)
{
	PRECONDITION(width >= 0);
	
	LOGBRUSH brush;
	brush.lbStyle = (uint32) (hatch == kSolidBrush ? BS_SOLID : BS_HATCHED);
	brush.lbColor = color.GetColorRef();
	brush.lbHatch = hatch;				// ignored with BS_SOLID
		
	mOldPen = nil;
	
	mDrawMode = mode;
	mPen = new ZPen(ExtCreatePen(kPenStyle, numeric_cast<uint32>(width), &brush, 0, nil));

	mWidth = width;
}

 
//---------------------------------------------------------------
//
// XPen::XPen (XPixMap, EDrawMode)
//
// $$ All the documentation I've seen (including that on Microsoft's 
// $$ web site as of 1/15/99) says that pens can be created from 8x8
// $$ DIBs in Win95. However no matter what I tried ExtCreatePen
// $$ always returned nil under Win98 (although NT appeared to work
// $$ OK). I also tried the W32PEN sample app from MSDN under Win98
// $$ and it too wound up with a null pen when using a pattern.
//
//---------------------------------------------------------------
#if 0
XPen::XPen(const XPixMap& pixMap, EDrawMode mode)
{
	// ExtCreatePen wants a DIB, but doesn't work with DIB-sections
	// so we have to convert our pixmap into a DIB...
	XLocker lock(pixMap);
	XPointer dib = Whisper::CreatePackedDIB(pixMap.GetInfo(), pixMap.GetBuffer());

	// Now that we have the DIB we can create the pen.
	LOGBRUSH brush;
	brush.lbStyle = BS_DIBPATTERNPT;
	brush.lbColor = DIB_RGB_COLORS;				
	brush.lbHatch = (int32) dib.GetPtr();	// pretty slick how an int32 can be a handle to a DIB, a pointer to a DIB, or a hatch constant :-)
		
	mOldPen = nil;
	
	mDrawMode = mode;
	mPen = new ZPen(ExtCreatePen(kPenStyle, 1, &brush, 0, nil));

	mWidth = 1;
}
#endif


//---------------------------------------------------------------
//
// XPen::XPen (HPEN, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(HPEN takePen, EDrawMode mode)
{
	PRECONDITION(takePen != nil);
	
	mOldPen = nil;
	
	mDrawMode = mode;
	mPen = new ZPen(takePen);

	mWidth = 1;
}


//---------------------------------------------------------------
//
// XPen::XPen (XPen)
//
//---------------------------------------------------------------
XPen::XPen(const XPen& rhs)
{
	mOldPen = nil;
	
	mDrawMode = rhs.mDrawMode;

	mPen = rhs.mPen;
	mPen->AddReference();

	mWidth = rhs.mWidth;
}


//---------------------------------------------------------------
//
// XPen::operator HPEN
//
//---------------------------------------------------------------
XPen::operator HPEN() const
{
	return mPen->GetPen();
}


//---------------------------------------------------------------
//
// XPen::operator=
//
//---------------------------------------------------------------
XPen& XPen::operator=(const XPen& rhs)
{
	if (this != &rhs) {
		mPen->RemoveReference();

		mDrawMode = rhs.mDrawMode;

		mPen = rhs.mPen;
		mPen->AddReference();

		mWidth = rhs.mWidth;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XPen::operator==
//
// The only function that I can find that provides information
// about the contents of a brush is GetObject and that only
// returns width, height, and color info. So the only reliable
// test for equality we can rely on is to test if the brushes
// point to the exact same object (note that MFC's CPen class
// doesn't even have an operator==).
//
//---------------------------------------------------------------	
bool XPen::operator==(const XPen& rhs) const
{
	bool equal = mDrawMode == rhs.mDrawMode && 	// If draw modes, width, and,
				 mWidth == rhs.mWidth;	
	
	if (equal) {
		equal = mPen == rhs.mPen;				// letters are the same we're done,
		
		if (!equal) {							// otherwise we need to compare the hard way.
			int32 bytes1 = ::GetObject(mPen->GetPen(), 0, nil);	// $$ this code will work if you use the cross-platform API, but if you use the HPEN ctor elpBrushStyle may be something not yet checked...
			int32 bytes2 = ::GetObject(rhs.mPen->GetPen(), 0, nil);
			
			if (bytes1 == bytes2 && bytes1 > 0) {
				if (bytes1 >= sizeof(EXTLOGPEN)) {
					EXTLOGPEN info1;						
					int32 result = ::GetObject(mPen->GetPen(), sizeof(info1), &info1);
					ThrowIf(result <= 0);
					
					EXTLOGPEN info2;
					result = ::GetObject(rhs.mPen->GetPen(), sizeof(info2), &info2);
					ThrowIf(result <= 0);
					
					equal = info1.elpPenStyle == info2.elpPenStyle && 
							info1.elpWidth == info2.elpWidth && 	
							info1.elpBrushStyle == info2.elpBrushStyle;
							
					if (equal) {
						if (info1.elpBrushStyle == BS_HATCHED) {
							equal = info1.elpColor == info2.elpColor && info1.elpHatch == info2.elpHatch;
						
						} else if (info1.elpBrushStyle == BS_SOLID) {
							equal = info1.elpColor == info2.elpColor;

						} else if (info1.elpBrushStyle == BS_DIBPATTERNPT) {
							equal = LOWORD(info1.elpColor) == LOWORD(info2.elpColor);
							if (equal) {
								const BITMAPINFOHEADER* header1 = reinterpret_cast<const BITMAPINFOHEADER*>(info1.elpHatch);
								const BITMAPINFOHEADER* header2 = reinterpret_cast<const BITMAPINFOHEADER*>(info2.elpHatch);
				
								if (header1 != nil && header2 != nil) 				
									equal = EqualHeader(header1, header2) && EqualPixels(header1, header2);
							}
							
						} else
							equal = false;
					}
				
				} else {
					LOGPEN info1;						
					int32 result = ::GetObject(mPen->GetPen(), sizeof(info1), &info1);
					ThrowIf(result <= 0);
					
					LOGPEN info2;
					result = ::GetObject(rhs.mPen->GetPen(), sizeof(info2), &info2);
					ThrowIf(result <= 0);
					
					equal = info1.lopnStyle == info2.lopnStyle && 
							info1.lopnWidth.x == info2.lopnWidth.x && 	// y component isn't used
							info1.lopnColor == info2.lopnColor;
				}
			}
		}
	}
			
	return equal;
}


//---------------------------------------------------------------
//
// XPen::SetDrawMode
//
//---------------------------------------------------------------
void XPen::SetDrawMode(EDrawMode mode)				
{
	mDrawMode = mode;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XPen::Invariant
//
//---------------------------------------------------------------
void XPen::Invariant() const
{	
	ASSERT(mPen != nil);
	ASSERT(mWidth >= 0);
}


//---------------------------------------------------------------
//
// XPen::OnStreamOut
//
//---------------------------------------------------------------
void XPen::OnStreamOut(XXMLDoc& doc, XXMLElement& element) const
{
	UNUSED(doc);
	
	element.AppendAttribute(XXMLAttribute(L"mode", DrawModeToStr(mDrawMode)));
	
	int32 bytes = ::GetObject(mPen->GetPen(), 0, nil);
	ThrowIf(bytes <= 0);
	
	if (bytes >= sizeof(EXTLOGPEN)) {
		EXTLOGPEN info;						
		int32 result = ::GetObject(mPen->GetPen(), sizeof(info), &info);
		ThrowIf(result <= 0);
					
		if (info.elpBrushStyle == BS_SOLID) 
			element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
			
		else if (info.elpBrushStyle == BS_HATCHED) 
			if (info.elpHatch == HS_FDIAGONAL)
				element.AppendAttribute(XXMLAttribute(L"brush", L"kDiagnalBrush"));
		
			else if (info.elpHatch == HS_CROSS)
				element.AppendAttribute(XXMLAttribute(L"brush", L"kCrossBrush"));
		
			else if (info.elpHatch == HS_HORIZONTAL)
				element.AppendAttribute(XXMLAttribute(L"brush", L"kHorzBrush"));
		
			else if (info.elpHatch == HS_VERTICAL)
				element.AppendAttribute(XXMLAttribute(L"brush", L"kVertBrush"));
				
			else {
				DEBUGSTR("Couldn't find a match for the hatch pattern in XBrush::OnStreamOut.");
				element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
			}
			
		else {
			element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
			DEBUGSTR("This pen cannot be streamed out.");
		}
		
		element.AppendAttribute(XXMLAttribute(L"color", XRGBColor(info.elpColor).GetOSColor()));
		element.AppendAttribute(XXMLAttribute(L"width", info.elpWidth));
	
	} else {
		LOGPEN info;						
		int32 result = ::GetObject(mPen->GetPen(), sizeof(info), &info);
		ThrowIf(result <= 0);
					
		if (info.lopnStyle == PS_SOLID) 
			element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
			
		else {
			element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
			DEBUGSTR("This pen cannot be streamed out.");
		}
		
		element.AppendAttribute(XXMLAttribute(L"color", XRGBColor(info.lopnColor).GetOSColor()));
		element.AppendAttribute(XXMLAttribute(L"width", info.lopnWidth.x));
	}
}


//---------------------------------------------------------------
//
// XPen::OnStreamIn
//
//---------------------------------------------------------------
void XPen::OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element)
{
	PRECONDITION(element.item_begin() == element.item_end());	// we don't have any child elements
	PRECONDITION(element.attr_begin() != element.attr_end());	// we do have attributes
	UNUSED(doc);										
 	UNUSED(root);
 	
 	EHatchBrush brush = kSolidBrush;
 	XRGBColor color = kRGBBlack;
 	EDrawMode mode = kCopyMode;
 	int32 width = 1;
 	
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
						
		} else if (name == L"width") {			
			width = attr.Get<int32>();
						
		} else
			DEBUGSTR(name, " isn't a valid attribute of XPen!");
	}
	
	XPen temp(brush, color, width, mode);
	*this = temp;
}


//---------------------------------------------------------------
//
// XPen::DoSelect
//
//---------------------------------------------------------------
void XPen::DoSelect(OSDrawContext context) const
{
	PRECONDITION(mOldPen == nil);
		
	mOldPen  = mPen->Select(context);
	mOldMode = SetROP2(context, (int32) mDrawMode);
}


//---------------------------------------------------------------
//
// XPen::DoDeSelect
//
//---------------------------------------------------------------
void XPen::DoDeSelect(OSDrawContext context) const
{
	mPen->DeSelect(context, mOldPen);
	mOldPen = nil;

	(void) SetROP2(context, mOldMode);
}


}	// namespace Whisper

