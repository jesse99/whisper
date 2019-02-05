/*
 *  File:       WFont.cpp
 *  Summary:	An object used to describe how text should be drawn.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFont.cpp,v $
 *		Revision 1.7  2001/04/27 09:18:50  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/04/21 03:31:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:41:59  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/09 10:15:29  jesjones
 *		Made member data private.
 *		
 *		Revision 1.3  2000/12/10 04:06:44  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:08:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XFont.h>

#include <WGDIUtils.h>
#include <WSystemInfo.h>
#include <XConstants.h>
#include <XDrawContext.h>
#include <XExceptions.h>
#include <XReferenceCounted.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	External Constants
//
const std::wstring kSystemFont			= L"System"; 
const std::wstring kAppFont				= L"System";
const std::wstring kHelveticaFont		= L"Arial";
const std::wstring kTimesFont			= L"Times New Roman";
const std::wstring kCourierFont			= L"Courier New";
const std::wstring kPalatinoFont		= L"Palatino";

const std::wstring kBigSystemFont		= L"big system";
const std::wstring kSmallSystemFont		= L"small system";
const std::wstring kSmallBoldSystemFont	= L"small bold system";


// ===================================================================================
//	class ZFont
//		Windows doesn't appear to provide any mechanism for duplicating GDI objects so
//		we'll use this class to simplify copying XFont's.
// ===================================================================================
class ZFont : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZFont();		
						
						ZFont(HDC dc, const std::wstring& name, uint16 style, int32 pointSize); 

private:					
						ZFont(const ZFont& rhs);

			ZFont& 	operator=(const ZFont& rhs);
						
//-----------------------------------
//	API
//
public:			
			bool 		IsSelected() const						{return mSelectCount > 0;}
			
			HFONT 		Select(HDC dc);		
			void 		DeSelect(HDC dc, HFONT oldFont);

			HFONT 		GetFont() const							{return mFont;}

//-----------------------------------
//	Member Data
//
private:
	HFONT			mFont;
	XAtomicCounter	mSelectCount;
};


//---------------------------------------------------------------
//
// ZFont::~ZFont
//
// Here's some more Windows lameness: you cannot delete a GDI
// object while it's selected into a DC. You'd think that after
// more than a decade they could add a reference count to an
// opaque data structure and let the OS manage resources like
// it's supposed to! To ensure that users are properly managing
// brushes the code below maintains a selection count.
//
//---------------------------------------------------------------
ZFont::~ZFont()
{	
	if (mSelectCount == 0)
		VERIFY(::DeleteObject(mFont));
	else
		DEBUGSTR("Tried to destroy a font that was still selected!");	// font will leak, but since we don't know which DC it's selected into there's not much we can do about it...

	mFont = nil;
}
		

//---------------------------------------------------------------
//
// ZFont::ZFont
//
//---------------------------------------------------------------
ZFont::ZFont(HDC dc, const std::wstring& name, uint16 style, int32 pointSize)
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(name.length() < LF_FACESIZE);
	PRECONDITION(style < 0xF);
	PRECONDITION(pointSize > 0);
	PRECONDITION(pointSize <= 256);
	
	if (WSystemInfo::HasUnicode()) {
		LOGFONTW logFont;
		memset(&logFont, 0, sizeof(logFont));
		
		logFont.lfHeight = -(pointSize*::GetDeviceCaps(dc, LOGPIXELSY)/72);
		
		BlockMoveData(name.c_str(), logFont.lfFaceName, (name.length() + 1UL)*sizeof(wchar_t));	
	
		if (style & kBoldStyle)
			logFont.lfWeight = FW_BOLD;
		else
			logFont.lfWeight = FW_NORMAL;
		
		if (style & kItalicStyle)
			logFont.lfItalic = 0xFF;
			
		if (style & kUnderlineStyle)
			logFont.lfUnderline = 0xFF;
			
		mFont = ::CreateFontIndirectW(&logFont);
		ThrowIfBadHandle(mFont);
	
	} else {
		LOGFONTA logFont;
		memset(&logFont, 0, sizeof(logFont));
		
		logFont.lfHeight = -(pointSize*::GetDeviceCaps(dc, LOGPIXELSY)/72);
		
		BlockMoveData(ToPlatformStr(name).c_str(), logFont.lfFaceName, (name.length() + 1UL));	
	
		if (style & kBoldStyle)
			logFont.lfWeight = FW_BOLD;
		else
			logFont.lfWeight = FW_NORMAL;
		
		if (style & kItalicStyle)
			logFont.lfItalic = 0xFF;
			
		if (style & kUnderlineStyle)
			logFont.lfUnderline = 0xFF;
			
		mFont = ::CreateFontIndirectA(&logFont);
		ThrowIfBadHandle(mFont);
	}
		
	mSelectCount = 0;
}


//---------------------------------------------------------------
//
// ZFont::Select
//
//---------------------------------------------------------------
HFONT ZFont::Select(HDC dc)
{
	PRECONDITION(dc != nil);
	PRECONDITION(mSelectCount >= 0);
	
	HGDIOBJ oldObject = SelectObject(dc, mFont);
	ThrowIf(oldObject == nil || (uint32) oldObject == GDI_ERROR);
	
	++mSelectCount;
		
	return (HFONT) oldObject;
}


//---------------------------------------------------------------
//
// ZFont::DeSelect
//
//---------------------------------------------------------------
void ZFont::DeSelect(HDC dc, HFONT oldFont)
{
	PRECONDITION(dc != nil);
	PRECONDITION(oldFont != nil);
	PRECONDITION(mSelectCount > 0);				// note that if you're using XSelectAttribute this won't be called if Select fails

	(void) SelectObject(dc, oldFont);		// don't throw here (this is called from dtors)

	--mSelectCount;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFont::iterator
// ===================================================================================

//---------------------------------------------------------------
//
// XFont::iterator::iterator (OSDrawContext, int)
//
//---------------------------------------------------------------
XFont::iterator::iterator(OSDrawContext dc, int dummy)
{
	PRECONDITION(dc != nil);
	UNUSED(dummy);			// only used to distinguish between begin() and end()
	
	mDC = dc;
	
	int32 result = ::EnumFontFamilies(mDC, nil, XFont::iterator::DoEnum, (long) this);
	ThrowIf(result == 0);	
	
	mIndex = 0;
}

		
//---------------------------------------------------------------
//
// XFont::iterator::iterator (OSDrawContext)
//
//---------------------------------------------------------------
XFont::iterator::iterator(OSDrawContext dc)
{
	PRECONDITION(dc != nil);
	
	mDC = dc;
	
	mIndex = ULONG_MAX;
}


//---------------------------------------------------------------
//
// XFont::iterator::operator*
//
//---------------------------------------------------------------
std::wstring XFont::iterator::operator*() const							
{
	PRECONDITION(mIndex < mNames.size()); 
		
	const std::wstring& name = mNames[mIndex];
	
	return name;
}


//---------------------------------------------------------------
//
// XFont::iterator::operator++
//
//---------------------------------------------------------------
XFont::iterator& XFont::iterator::operator++()								
{
	if (mIndex < mNames.size()) 
		++mIndex; 
		
	if (mIndex == mNames.size()) 	// so we compare equal to end()
		mIndex = ULONG_MAX;
		
	return *this;
}


//---------------------------------------------------------------
//
// XFont::iterator::operator==
//
//---------------------------------------------------------------
bool XFont::iterator::operator==(const iterator& rhs) const		
{
	PRECONDITION(mDC == rhs.mDC); 
	
	bool equal = mIndex == rhs.mIndex;
	
	return equal;
}	


//---------------------------------------------------------------
//
// XFont::iterator::DoEnum								[static]
//
//---------------------------------------------------------------
int CALLBACK XFont::iterator::DoEnum(const LOGFONT* logicalData, const TEXTMETRIC* physicalData, DWORD type, LPARAM refCon)
{
	UNUSED(physicalData);
	UNUSED(type);
	
	XFont::iterator* thisPtr = reinterpret_cast<XFont::iterator*>(refCon);
	thisPtr->mNames.push_back(FromPlatformStr(logicalData->lfFaceName));
	
	return 1;		// must be non-zero to continue iterating
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFont
// ===================================================================================

//---------------------------------------------------------------
//
// XFont::~XFont
//
//---------------------------------------------------------------
XFont::~XFont()
{
	PRECONDITION(mDC == nil);
	
	if (mFont != nil)
		mFont->RemoveReference();

	mFont = nil;
	mOldFont = nil;
}
		

//---------------------------------------------------------------
//
// XFont::XFont (wstring, int32)
//
//---------------------------------------------------------------
XFont::XFont(const std::wstring& name, int32 size)
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(name.length() < LF_FACESIZE);
	PRECONDITION(size > 0);
	PRECONDITION(size <= 256);

	mFont = nil;
	
	mName  = name;
	mSize  = size;
	mStyle = kPlainStyle;
	mColor = kRGBBlack;
	
	if (mName == kBigSystemFont) {
		mName  = kSystemFont;
		mSize  = 12;
	
	} else if (mName == kSmallSystemFont) {
		mName  = kAppFont;
		mSize  = 9;
	
	} else if (mName == kSmallBoldSystemFont) {
		mName  = kAppFont;
		mStyle = kBoldStyle;
		mSize  = 9;
	}
	
	mDC      = nil;
	mOldFont = nil;
}


//---------------------------------------------------------------
//
// XFont::XFont (XFont)
//
//---------------------------------------------------------------
XFont::XFont(const XFont& rhs)
{
	mDC      = nil;
	mOldFont = nil;
	
	mFont = rhs.mFont;
	if (mFont != nil)
		mFont->AddReference();
	
	mName  = rhs.mName;
	mSize  = rhs.mSize;
	mStyle = rhs.mStyle;
	mColor = rhs.mColor;
	
	mInfo = rhs.mInfo;
}


//---------------------------------------------------------------
//
// XFont::operator=
//
//---------------------------------------------------------------
XFont& XFont::operator=(const XFont& rhs)
{
	if (this != &rhs) {
		if (mFont != nil)
			mFont->RemoveReference();

		mFont = rhs.mFont;
		if (mFont != nil)
			mFont->AddReference();
	
		mName  = rhs.mName;
		mSize  = rhs.mSize;
		mStyle = rhs.mStyle;
		mColor = rhs.mColor;
		
		mInfo = rhs.mInfo;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XFont::operator==
//
//---------------------------------------------------------------
bool XFont::operator==(const XFont& rhs) const
{
	bool eq = false;
	
	if (mName == rhs.mName)
		if (mSize == rhs.mSize)
			if (mStyle == rhs.mStyle)
				if (mColor == rhs.mColor)
					eq = true;
	
	return eq;
}


//---------------------------------------------------------------
//
// XFont::SetName
//
//---------------------------------------------------------------
void XFont::SetName(const std::wstring& name)
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(name.length() < LF_FACESIZE);
	ASSERT_IF(mFont != nil, !mFont->IsSelected());

	if (name != mName) {
		if (mFont != nil) {
			mFont->RemoveReference();
			mFont = nil;
		}
		
		mName = name;

		if (mName == kBigSystemFont) {
			mName  = kSystemFont;
			mStyle = kPlainStyle;
			mSize  = 12;
		
		} else if (mName == kSmallSystemFont) {
			mName  = kAppFont;
			mStyle = kPlainStyle;
			mSize  = 10;
		
		} else if (mName == kSmallBoldSystemFont) {
			mName  = kAppFont;
			mStyle = kBoldStyle;
			mSize  = 10;
		}	
	}
}


//---------------------------------------------------------------
//
// XFont::SetSize
//
//---------------------------------------------------------------
void XFont::SetSize(int32 size)
{
	PRECONDITION(size > 0);
	PRECONDITION(size <= 256);
	ASSERT_IF(mFont != nil, !mFont->IsSelected());

	if (size != mSize) {
		if (mFont != nil) {
			mFont->RemoveReference();
			mFont = nil;
		}
		
		mSize = size;
	}
}


//---------------------------------------------------------------
//
// XFont::SetStyle
//
//---------------------------------------------------------------
void XFont::SetStyle(uint16 style)
{
	PRECONDITION(style < 0xF);
	ASSERT_IF(mFont != nil, !mFont->IsSelected());

	if (style != mStyle) {
		if (mFont != nil) {
			mFont->RemoveReference();
			mFont = nil;
		}
		
		mStyle = style;
	}
}


//---------------------------------------------------------------
//
// XFont::SetColor
//
//---------------------------------------------------------------
void XFont::SetColor(const XRGBColor& color)
{
	ASSERT_IF(mFont != nil, !mFont->IsSelected());

	mColor = color;
}


//---------------------------------------------------------------
//
// XFont::GetMetrics
//
//---------------------------------------------------------------
SFontMetrics XFont::GetMetrics() const						
{
	if (mFont == nil) {
		WAcquireScreenDC screenDC;
		
		const_cast<XFont*>(this)->DoInit(screenDC);
	}
	
	return mInfo;
}


//---------------------------------------------------------------
//
// XFont::GetStringSize
//
//---------------------------------------------------------------
XSize XFont::GetStringSize(const std::wstring& text) const
{
	PRECONDITION(mFont != nil);
	PRECONDITION(mFont->IsSelected());
	
	XSize size;
	int32 suceeded;
	if (WSystemInfo::HasUnicode())
		suceeded = GetTextExtentPoint32W(mDC, text.c_str(), numeric_cast<int32>(text.length()), size);
	else
		suceeded = GetTextExtentPoint32A(mDC, ToPlatformStr(text).c_str(), numeric_cast<int32>(text.length()), size);
	ThrowIf(!suceeded);
	
	return size;
}


//---------------------------------------------------------------
//
// XFont::GetFont
//
//---------------------------------------------------------------
HFONT XFont::GetFont() const
{
	if (mFont == nil) {
		WAcquireScreenDC screenDC;
		
		const_cast<XFont*>(this)->DoInit(screenDC);
	}
	
	return mFont->GetFont();
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XFont::Invariant
//
//---------------------------------------------------------------
void XFont::Invariant() const
{	
	ASSERT(mFont != nil);
	ASSERT(mSize > 0);
}


//---------------------------------------------------------------
//
// XFont::OnStreamOut
//
//---------------------------------------------------------------
void XFont::OnStreamOut(XXMLDoc& doc, XXMLElement& element) const
{
	UNUSED(doc);
	
	element.AppendAttribute(XXMLAttribute(L"font", mName));
	element.AppendAttribute(XXMLAttribute(L"size", mSize));
	element.AppendAttribute(XXMLAttribute(L"style", (uint32) mStyle));
	element.AppendAttribute(XXMLAttribute(L"color", mColor.GetOSColor()));
}


//---------------------------------------------------------------
//
// XFont::OnStreamIn
//
//---------------------------------------------------------------
void XFont::OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element)
{
	PRECONDITION(element.item_begin() == element.item_end());	// we don't have any child elements
	PRECONDITION(element.attr_begin() != element.attr_end());	// we do have attributes
	PRECONDITION(element.GetName() == element.GetName());		// element doesn't have any child elements so this should never fire
	UNUSED(doc);										// doc is provided in case IDREF attributes are used
 	UNUSED(root);
 	
	XXMLElement::const_attr_iterator iter = element.attr_begin();	
	while (iter != element.attr_end()) {
		XXMLAttribute attr = *iter;
		++iter;
		
		const std::wstring& name = attr.GetName();
		
		if (name == L"font")				
			this->SetName(attr.GetValue());
		else if (name == L"size")				
			mSize = attr.Get<int32>();
		else if (name == L"style")				
			this->SetStyle(numeric_cast<uint16>(attr.Get<uint32>()));
		else if (name == L"color")				
			this->SetColor(attr.Get<XRGBColor>());
		else
			DEBUGSTR(name, " isn't a valid attribute of XFont!");
	}
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XFont::DoInit
//
//---------------------------------------------------------------
void XFont::DoInit(OSDrawContext dc)
{
	PRECONDITION(dc != nil);
	PRECONDITION(mFont == nil);

	// Create the font
	mFont = new ZFont(dc, mName, mStyle, mSize);
	
	// Get the metrics
	HFONT oldFont = mFont->Select(dc);
	
	TEXTMETRIC metrics;
	int32 succeeded = ::GetTextMetrics(dc, &metrics);
	if (!succeeded) {
		mFont->DeSelect(dc, oldFont);
		ThrowErr(GDI_ERROR);
	}
	
	mInfo.ascent  = metrics.tmAscent;
	mInfo.descent = metrics.tmDescent;
	mInfo.widMax  = metrics.tmMaxCharWidth;
	mInfo.leading = metrics.tmExternalLeading;

	mFont->DeSelect(dc, oldFont);
}


//---------------------------------------------------------------
//
// XFont::DoSelect
//
//---------------------------------------------------------------
void XFont::DoSelect(OSDrawContext context) const
{
	PRECONDITION(mDC == nil);
	
	mDC = context;
	
	if (mFont == nil) 
		const_cast<XFont*>(this)->DoInit(mDC);
	
	mOldFont = mFont->Select(mDC);
	
	mOldColor = XRGBColor(::SetTextColor(mDC, mColor.GetColorRef()));
	ThrowIf(mOldColor.GetColorRef() == CLR_INVALID);
}


//---------------------------------------------------------------
//
// XFont::DoDeSelect
//
//---------------------------------------------------------------
void XFont::DoDeSelect(OSDrawContext context) const
{
	PRECONDITION(mDC != nil);
	PRECONDITION(context == mDC);
	
	mFont->DeSelect(mDC, mOldFont);
	mOldFont = nil;

	(void) ::SetTextColor(mDC, mOldColor.GetColorRef());
	mDC = nil;
}




}	// namespace Whisper

