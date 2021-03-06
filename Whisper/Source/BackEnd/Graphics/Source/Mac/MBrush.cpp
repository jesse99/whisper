/*
 *  File:       MBrush.cpp
 *  Summary:	An object used when filling shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	 
 *
 *		$Log: MBrush.cpp,v $
 *		Revision 1.6  2001/04/21 03:24:08  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:40:43  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/04/13 07:48:51  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:02:40  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:10:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	  8/06/99	JDJ		Added support for streaming and kSolidBrush.
 *		 <6>	  7/19/99	JDJ		Tweaked for Carbon.
 *		 <5>	  6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <4>	  4/10/99	JDJ		Fixed (XPixMap, EDrawMode) ctor so that int16_cast no longer fires when
 *									assigning to rowBytes and it works if depth is larger than 8.
 *		 <3>	  1/16/99	JDJ		Replaced selection pens with selection brushes.
 *		 <2>	 12/24/98	JDJ		Added XBrush::XBrush (Pattern, XRGBColor, EDrawMode).
 *		 <1>	 11/30/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XBrush.h>

#include <MQuickDrawUtils.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XPixMap.h>
#include <XReferenceCounted.h>
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
//	class ZBrush
// ===================================================================================
class ZBrush : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZBrush();		
						
						ZBrush(const Pattern& pattern, const XRGBColor& color); 

						ZBrush(PixPatHandle pixPat); 

private:					
						ZBrush(const ZBrush& rhs);

			ZBrush& 	operator=(const ZBrush& rhs);
						
//-----------------------------------
//	API
//
public:
			void 		Select();	
			void 		DeSelect();
			
			bool 		HasPixPat() const					{return mPixPat != nil;}
			
			bool 		MatchPattern(int16 resID) const;
			XRGBColor 	GetColor() const					{return mColor;}

			bool 		operator==(const ZBrush& rhs) const;

//-----------------------------------
//	Member Data
//
protected:
	XAtomicCounter	mSelectCount;
	
	Pattern			mPattern;
	XRGBColor		mColor;
	PixPatHandle	mPixPat;		// color and pattern are ignored if this is non-nil
};


//---------------------------------------------------------------
//
// ZBrush::~ZBrush
//
// Windows' GDI objects can't be deleted while they're selected
// into a device context. In order to assist writing cross platform 
// code the code below burps if you try to delete an object that 
// is still selected.
//
//---------------------------------------------------------------
ZBrush::~ZBrush()
{	
	if (mSelectCount == 0) {
		if (mPixPat != nil)
			DisposePixPat(mPixPat);
	
	} else
		DEBUGSTR("Tried to destroy a brush that was still selected!");	// QuickDraw will delete the pixPat when it's finished with it
}
		

//---------------------------------------------------------------
//
// ZBrush::ZBrush (Pattern, XRGBColor)
//
//---------------------------------------------------------------
ZBrush::ZBrush(const Pattern& pattern, const XRGBColor& color)
{	
	mSelectCount = 0;
	
	mPattern = pattern;
	mColor   = color;
	mPixPat  = nil;
}


//---------------------------------------------------------------
//
// ZBrush::ZBrush (PixPatHandle)
//
//---------------------------------------------------------------
ZBrush::ZBrush(PixPatHandle pixPat)
{	
	PRECONDITION(pixPat != nil);
	
	mSelectCount = 0;
	
	mPixPat = pixPat;
}


//---------------------------------------------------------------
//
// ZBrush::operator==
//
//---------------------------------------------------------------
bool ZBrush::operator==(const ZBrush& rhs) const
{
	bool equal = false;
	
	// If lhs and rhs are using pixel patterns
	if (mPixPat != nil && rhs.mPixPat != nil) {
		PixMapHandle& lhsPixels = (*mPixPat)->patMap;
		PixMapHandle& rhsPixels = (*rhs.mPixPat)->patMap;
		
		// then if the depths match
		if ((*lhsPixels)->pixelSize == (*rhsPixels)->pixelSize)
		
			// and the sizes match
			if ((*lhsPixels)->bounds.right == (*rhsPixels)->bounds.right)
				if ((*lhsPixels)->bounds.bottom == (*rhsPixels)->bounds.bottom)
				
					// and the pixel data matches
					if (EqualHandle((*mPixPat)->patData, (*rhs.mPixPat)->patData))
					
						// and the color tables match then they compare equal.
						equal = CompareColorTable((*lhsPixels)->pmTable, (*rhsPixels)->pmTable) == 0;
	
	// If the lhs and rhs aren't using pixel patterns
	} else if (mPixPat == nil && rhs.mPixPat == nil) {
	
		// then we have a match if the color and b/w pattern compare equal.
		if (mColor == rhs.mColor)
			equal = EqualMemory(&mPattern, &rhs.mPattern, sizeof(Pattern));
			
	// Otherwise one brush is using a pixel pattern and the other is
	// not so we compare unequal.
	}
		
	return equal;
}


//---------------------------------------------------------------
//
// ZBrush::MatchPattern
//
//---------------------------------------------------------------
bool ZBrush::MatchPattern(int16 resID) const
{
	Pattern pattern;
	::GetIndPattern(&pattern, sysPatListID, resID);

	bool matches = EqualMemory(&mPattern, &pattern, sizeof(Pattern));
	
	return matches;
}


//---------------------------------------------------------------
//
// ZBrush::Select
//
//---------------------------------------------------------------
void ZBrush::Select()
{
	PRECONDITION(mSelectCount >= 0);
	
	if (mPixPat != nil) {
		::PenPixPat(mPixPat);	
	
	} else {
		::PenPat(&mPattern);
		
		RGBColor temp = mColor.GetOSColor();
		::RGBForeColor(&temp);
	}
	
	++mSelectCount;
}


//---------------------------------------------------------------
//
// ZBrush::DeSelect
//
//---------------------------------------------------------------
void ZBrush::DeSelect()
{
	PRECONDITION(mSelectCount > 0);

	--mSelectCount;
}

#pragma mark -

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
	PRECONDITION(mOldPixPat == nil);

	mBrush->RemoveReference();
}
		

//---------------------------------------------------------------
//
// XBrush::XBrush (XRGBColor, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(const XRGBColor& color, EDrawMode mode)
{
	mOldPixPat = nil;
	
	mDrawMode = mode;
	
	Pattern pattern;
	::GetIndPattern(&pattern, sysPatListID, 1);
	
	mBrush = new ZBrush(pattern, color);
}


//---------------------------------------------------------------
//
// XBrush::XBrush (EHatchBrush, XRGBColor, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(EHatchBrush brush, const XRGBColor& color, EDrawMode mode)
{		
	Pattern pattern;
	
	switch (brush) {
		case kSolidBrush:
			::GetIndPattern(&pattern, sysPatListID, 1);
			break;
			
		case kDiagnalBrush:
			::GetIndPattern(&pattern, sysPatListID, 26);
			break;
			
		case kCrossBrush:
			::GetIndPattern(&pattern, sysPatListID, 11);
			break;
			
		case kHorzBrush:
			::GetIndPattern(&pattern, sysPatListID, 25);
			break;
			
		case kVertBrush:
			::GetIndPattern(&pattern, sysPatListID, 6);
			break;
			
		default:
			DEBUGSTR("Bad hatched brush in XBrush ctor");
			::GetIndPattern(&pattern, sysPatListID, 1);
	}

	mOldPixPat = nil;
	
	mDrawMode = mode;
	mBrush = new ZBrush(pattern, color);
}


//---------------------------------------------------------------
//
// XBrush::XBrush (XPixMap, EDrawMode)
//
// This is derived from FW_CPrivColorPatternRep::MacNewPixPat in ODF.
//
//---------------------------------------------------------------
XBrush::XBrush(const XPixMap& pixels, EDrawMode mode)
{
	PixPatHandle pixPat = nil;

	try {
		int32 depth  = pixels.GetDepth();
		int32 width  = pixels.GetWidth();
		int32 height = pixels.GetHeight();
		
		// Make sure width and height are a power of two
		ASSERT(width == 1 || width == 2 || width == 4 || width == 8 || width == 16 || width == 32 || width == 64);
		ASSERT(height == 1 || height == 2 || height == 4 || height == 8 || height == 16 || height == 32 || height == 64);

		// Create the pixel pattern
		pixPat = ::NewPixPat();
		ThrowIfNil(pixPat);
		
		(**pixPat).patType = 1;
			
		// Create the pixMap
		PixMapHandle pixMap = (**pixPat).patMap;
		
		(**pixMap).rowBytes      = numeric_cast<int16>(pixels.GetRowBytes());
		(**pixMap).bounds.left   = 0;
		(**pixMap).bounds.top    = 0;
		(**pixMap).bounds.right  = numeric_cast<int16>(width);
		(**pixMap).bounds.bottom = numeric_cast<int16>(height);
		(**pixMap).pixelType     = numeric_cast<int16>(depth < 16 ? 0 : 16);
		(**pixMap).pixelSize     = numeric_cast<int16>(depth);
		(**pixMap).cmpCount      = numeric_cast<int16>(depth < 16 ? 1 : 3);
		(**pixMap).cmpSize       = numeric_cast<int16>(depth < 16 ? depth : (depth == 16 ? 5 : 8));
		
		(**pixMap).rowBytes |= 0x8000;					// I'm a pixmap

		// Setup the color table
		OSPixMap osPixels = pixels.GetOSPixMap();
		CTabHandle colorsH = (*osPixels)->pmTable;		// use a temporary because HandToHand may move memory
		if (colorsH != nil)	{
			OSErr err = HandToHand((Handle*) &colorsH);
			ThrowIfOSErr(err);
		}
		if ((**pixMap).pmTable != nil)
			DisposeHandle((Handle) (**pixMap).pmTable);
		(**pixMap).pmTable = colorsH;
			
		// Create the pattern data
		uint32 dataSize = pixels.GetBufferSize();
		Handle patData = (**pixPat).patData;
		::SetHandleSize(patData, numeric_cast<int32>(dataSize));
		ThrowIfMemFail(patData);
		
		BlockMoveData(pixels.GetUnsafeBuffer(), *patData, dataSize);
		
		if ((**pixMap).pmTable != nil)
			CTabChanged((**pixMap).pmTable);
		PixPatChanged(pixPat);
	
	} catch (...) {
		if (pixPat != nil)
			DisposePixPat(pixPat);

		throw;
	}

	mOldPixPat = nil;
	
	mDrawMode = mode;
	mBrush = new ZBrush(pixPat);
}
 

//---------------------------------------------------------------
//
// XBrush::XBrush (Pattern, XRGBColor, EDrawMode)
//
//---------------------------------------------------------------
XBrush::XBrush(const Pattern& pattern, const XRGBColor& color, EDrawMode mode)
{
	mOldPixPat = nil;
	
	mDrawMode = mode;
	mBrush = new ZBrush(pattern, color);
}


//---------------------------------------------------------------
//
// XBrush::XBrush (XBrush)
//
//---------------------------------------------------------------
XBrush::XBrush(const XBrush& rhs)
{
	mOldPixPat = nil;
	
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
// XBrush::operator==
//
//---------------------------------------------------------------
bool XBrush::operator==(const XBrush& rhs) const
{
	bool equal = false;
	
	if (mDrawMode == rhs.mDrawMode)
		if (mBrush == rhs.mBrush || *mBrush == *rhs.mBrush)
			equal = true;
			
	return equal;
}

#pragma mark ~

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
	
	if (mBrush->HasPixPat()) 
		DEBUGSTR("XBrush::OnStreamOut doesn't support streaming out pixmap brushes.");
	
	if (mBrush->MatchPattern(1))
		element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));

	else if (mBrush->MatchPattern(26))
		element.AppendAttribute(XXMLAttribute(L"brush", L"kDiagnalBrush"));

	else if (mBrush->MatchPattern(11))
		element.AppendAttribute(XXMLAttribute(L"brush", L"kCrossBrush"));

	else if (mBrush->MatchPattern(25))
		element.AppendAttribute(XXMLAttribute(L"brush", L"kHorzBrush"));

	else if (mBrush->MatchPattern(6))
		element.AppendAttribute(XXMLAttribute(L"brush", L"kVertBrush"));
		
	else {
		DEBUGSTR("Couldn't find a match for the pattern in XBrush::OnStreamOut.");
		element.AppendAttribute(XXMLAttribute(L"brush", L"kSolidBrush"));
	}
	
	element.AppendAttribute(XXMLAttribute(L"color", mBrush->GetColor().GetOSColor()));
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
		} 
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
	PRECONDITION(mOldPixPat == nil);
	PRECONDITION(context != nil);
	
	CGrafPtr port = context;
	::SetGWorld(port, GetMainDevice());	

	RGBColor oldColor;
	GetForeColor(&oldColor);
	mOldColor = oldColor;
	
	PenState state;
	GetPenState(&state);
	mOldPattern = state.pnPat;
	mOldMode = state.pnMode;
	
	::PenMode((int16) mDrawMode);

	if (mBrush->HasPixPat()) {
#if TARGET_CARBON
		mOldPixPat = NewPixPat();
		ThrowIfNil(mOldPixPat);

		GetPortPenPixPat(port, mOldPixPat);
		ThrowIfQDError();
#else
		PixPatHandle temp = port->pnPixPat;
		if (temp != nil) {
			mOldPixPat = NewPixPat();
			ThrowIfNil(mOldPixPat);

			CopyPixPat(temp, mOldPixPat);
			ThrowIfQDError();
		}
#endif
	}
	
	mBrush->Select();
}


//---------------------------------------------------------------
//
// XBrush::DoDeSelect
//
//---------------------------------------------------------------
void XBrush::DoDeSelect(OSDrawContext context) const
{
	PRECONDITION(context != nil);
	
	::SetGWorld(context, GetMainDevice());	
	
	 mBrush->DeSelect();

	::PenMode(mOldMode);

	if (mOldPixPat != nil) {
		::PenPixPat(mOldPixPat);	
		
		::DisposePixPat(mOldPixPat);
		mOldPixPat = nil;
	
	} else {
		::PenPat(&mOldPattern);

		RGBColor temp = mOldColor.GetOSColor();
		::RGBForeColor(&temp);
	}
}


}	// namespace Whisper

