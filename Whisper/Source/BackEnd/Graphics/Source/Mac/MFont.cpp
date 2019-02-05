/*
 *  File:       MFont.cpp
 *  Summary:	An object used to describe how text should be drawn.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFont.cpp,v $
 *		Revision 1.8  2001/04/27 04:20:56  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.7  2001/04/21 03:26:26  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/17 01:40:59  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.5  2001/04/13 07:50:36  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.4  2000/12/17 03:46:36  jesjones
 *		GetFontStyleRecord only sets the fore color flag if the color isn't black (so static text controls properly dim when deactivated).
 *		
 *		Revision 1.3  2000/12/10 04:03:56  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:14:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	  7/19/99	JDJ		Tweaked for Carbon.
 *		 <5>	  6/25/99	JDJ		Now descends from XXMLPersistentMixin.
 *		 <4>	  6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <3>	 12/23/98	JDJ		XFont::SetName was using == instead of != when comparing names.
 *									GetMetrics and GetStringSize weren't selecting the font.
 *		 <2>	 12/23/98	JDJ		Added XFont::iterator. Added HasSize.
 *		 <1>	 11/30/97	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <XFont.h>

#include <MQuickDrawUtils.h>
#include <MResUtils.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XReferenceCounted.h>
#include <XStringUtils.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	External Constants
//
const std::wstring kSystemFont			= L"systemFont"; 
const std::wstring kAppFont				= L"applFont";
const std::wstring kHelveticaFont		= L"Helvetica";
const std::wstring kTimesFont			= L"Times";
const std::wstring kCourierFont			= L"Courier";
const std::wstring kPalatinoFont	  		= L"Palatino";

const std::wstring kBigSystemFont		= L"big system";
const std::wstring kSmallSystemFont		= L"small system";
const std::wstring kSmallBoldSystemFont	= L"small bold system";


// ===================================================================================
//	class ZFont
// ===================================================================================
class ZFont : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZFont();		
						
						ZFont(); 

private:					
						ZFont(const ZFont& rhs);

			ZFont& 	operator=(const ZFont& rhs);
						
//-----------------------------------
//	API
//
public:			
			bool 		IsSelected() const						{return mSelectCount > 0;}
			
			void 		Select();
			
			void 		DeSelect();

//-----------------------------------
//	Member Data
//
protected:
	XAtomicCounter	mSelectCount;
};


//---------------------------------------------------------------
//
// ZFont::~ZFont
//
// Windows' GDI objects can't be deleted while they're selected.
// In order to assist writing cross platform code the code below
// burps if you try to delete an object that is still selected.
//
//---------------------------------------------------------------
ZFont::~ZFont()
{	
	if (mSelectCount != 0)
		DEBUGSTR("Tried to destroy a font that was still selected!");
}
		

//---------------------------------------------------------------
//
// ZFont::ZFont
//
//---------------------------------------------------------------
ZFont::ZFont()
{
	mSelectCount = 0;
}


//---------------------------------------------------------------
//
// ZFont::Select
//
//---------------------------------------------------------------
void ZFont::Select()
{
	PRECONDITION(mSelectCount >= 0);
	
	++mSelectCount;
}


//---------------------------------------------------------------
//
// ZFont::DeSelect
//
//---------------------------------------------------------------
void ZFont::DeSelect()
{
	PRECONDITION(mSelectCount > 0);

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
	
	mIndex = 0;
	
	int16 count = ::CountResources('FOND');
	mNames.reserve(count);
	
	for (int16 index = 1; index <= count; ++index) {
		Handle hand = ::GetIndResource('FOND', index);
		if (hand != nil) {
			std::wstring name = Whisper::GetResourceInfo(hand).name;
			if (name[0] != '.' && name[0] != '%')
				mNames.push_back(name);
				
			::ReleaseResource(hand);
		}
	}
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
	if (mFont != nil)
		mFont->RemoveReference();
}
		

//---------------------------------------------------------------
//
// XFont::XFont (string, int32)
//
//---------------------------------------------------------------
XFont::XFont(const std::wstring& name, int32 size)
{
	PRECONDITION(name.length() > 0);
	PRECONDITION(name.length() < 256);
	PRECONDITION(size > 0);
	PRECONDITION(size <= 256);

	mFont = new ZFont;
	
	mName  = name;
	mSize  = size;
	mStyle = kPlainStyle;
	mColor = kRGBBlack;
	
	if (mName == kSystemFont) {	
		mFontNum = 0;
	
	} else if (mName == kAppFont) {
		mFontNum = 1;
	
	} else if (mName == kBigSystemFont) {
		mFontNum = kControlFontBigSystemFont;
		mSize  = 12;
	
	} else if (mName == kSmallSystemFont) {
		mFontNum = kControlFontSmallSystemFont;
		mSize  = 10;
	
	} else if (mName == kSmallBoldSystemFont) {
		mFontNum = kControlFontSmallBoldSystemFont;
		mStyle = kBoldStyle;
		mSize  = 10;
	
	} else
		::GetFNum(ToPascalStr(mName).c_str(), &mFontNum);
	
	mValidInfo = false;
}


//---------------------------------------------------------------
//
// XFont::XFont (XFont)
//
//---------------------------------------------------------------
XFont::XFont(const XFont& rhs)
{	
	mFont = rhs.mFont;
	mFont->AddReference();
	
	mName  = rhs.mName;
	mSize  = rhs.mSize;
	mStyle = rhs.mStyle;
	mColor = rhs.mColor;
	
	mFontNum = rhs.mFontNum;
	
	mInfo = rhs.mInfo;
	mValidInfo = rhs.mValidInfo;
}


//---------------------------------------------------------------
//
// XFont::operator=
//
//---------------------------------------------------------------
XFont& XFont::operator=(const XFont& rhs)
{
	if (this != &rhs) {
		mFont->RemoveReference();

		mFont = rhs.mFont;
		mFont->AddReference();
	
		mName  = rhs.mName;
		mSize  = rhs.mSize;
		mStyle = rhs.mStyle;
		mColor = rhs.mColor;
		
		mFontNum = rhs.mFontNum;
		
		mInfo = rhs.mInfo;
		mValidInfo = rhs.mValidInfo;
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
	PRECONDITION(name.length() < 256);
	ASSERT_IF(mFont != nil, !mFont->IsSelected());

	if (name != mName) {
		mName = name;

		if (mName == kSystemFont) {		
			mFontNum = 0;
		
		} else if (mName == kAppFont) {
			mFontNum = 1;
			
		} else if (mName == kBigSystemFont) {
			mFontNum = kControlFontBigSystemFont;
			mStyle = kPlainStyle;
			mSize  = 12;
		
		} else if (mName == kSmallSystemFont) {
			mFontNum = kControlFontSmallSystemFont;
			mStyle = kPlainStyle;
			mSize  = 10;
		
		} else if (mName == kSmallBoldSystemFont) {
			mFontNum = kControlFontSmallBoldSystemFont;
			mStyle = kBoldStyle;
			mSize  = 10;
	
		} else
			::GetFNum(ToPascalStr(mName).c_str(), &mFontNum);

		mValidInfo = false;
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
		mSize = size;

		mValidInfo = false;
	}
}


//---------------------------------------------------------------
//
// XFont::HasSize
//
//---------------------------------------------------------------
bool XFont::HasSize(int32 pointSize) const
{
	PRECONDITION(pointSize > 0);
	
	bool has = ::RealFont(mFontNum, numeric_cast<int16>(pointSize));
	
	return has;
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
		mStyle = style;

		mValidInfo = false;
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
	if (!mValidInfo) {
		MSetPort setter(GetWorkPort());
		this->DoSelect(GetWorkPort());
		
		FontInfo info;
		::GetFontInfo(&info);
		
		mInfo.ascent  = info.ascent;
		mInfo.descent = info.descent;
		mInfo.widMax  = info.widMax;
		mInfo.leading = info.leading;
		
		mValidInfo = true;
		this->DoDeSelect(GetWorkPort());
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
	PRECONDITION(mFont->IsSelected());			// required on Windows
	
	MSetPort setter(GetWorkPort());
	this->DoSelect(GetWorkPort());
	
	SFontMetrics metrics = this->GetMetrics();
	
	XSize size;
	size.width  = (int16) (::TextWidth(ToPlatformStr(text).c_str(), 0, (int16) text.length()) + 1);	// NeoTextBox likes an extra pixel
	size.height = (int16) (metrics.ascent + metrics.descent);

	this->DoDeSelect(GetWorkPort());

	return size;
}


//---------------------------------------------------------------
//
// XFont::GetFontStyleRecord
//
//---------------------------------------------------------------
ControlFontStyleRec XFont::GetFontStyleRecord() const
{
	ControlFontStyleRec record;
	
	record.flags     = kControlUseAllMask - (kControlUseJustMask + kControlUseModeMask + kControlUseForeColorMask + kControlUseBackColorMask);
	record.font      = mFontNum;
	record.size      = numeric_cast<int16>(mSize);
	record.style     = numeric_cast<int16>(mStyle);
	record.mode      = srcOr;						// Appearance Manager 1.1 isn't handling the flags field correctly (with static text controls) so we'll init the unused fields to reasonable values just in case...
	record.just      = teFlushLeft;
	record.foreColor = mColor.GetOSColor();
//	record.backColor = mTraits.color;

	if (mColor != kRGBBlack)
		record.flags += kControlUseForeColorMask;	// static text controls don't dim when they're deactivated when custom colors are used

	return record;
}

#pragma mark ~

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
	element.AppendAttribute(XXMLAttribute(L"color", mColor.GetOSColor()));
	
	std::wstring value;
	if (mStyle == kPlainStyle)
		value = L"plain ";
	if (mStyle & kBoldStyle)
		value += L"bold+";
	if (mStyle & kItalicStyle)
		value += L"italic+";
	if (mStyle & kUnderlineStyle)
		value += L"underline+";
	value.resize(value.length() - 1);
	
	element.AppendAttribute(XXMLAttribute(L"style", value));
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
			
		else if (name == L"color")				
			this->SetColor(attr.Get<XRGBColor>());
			
		else if (name == L"style") {
			uint32 style = 0;			

			std::wstring str = attr.GetValue();
			while (str.length() > 0) {
				std::wstring temp = Whisper::Parse(str, L" +");
			
				if (temp == L"plain")
					style = kPlainStyle;
				else if (temp == L"bold")
					style += kBoldStyle;
				else if (temp == L"italic")
					style += kItalicStyle;
				else if (temp == L"underline")
					style += kUnderlineStyle;
		
				else 
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: #1 isn't a valid font style.", temp)));
			}

			this->SetStyle(numeric_cast<uint16>(style));
			
		} else
			DEBUGSTR(name, " isn't a valid attribute of XFont!");
	}
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFont::DoSelect
//
//---------------------------------------------------------------
void XFont::DoSelect(OSDrawContext context) const
{
	PRECONDITION(context != nil);
	
	CGrafPtr port = context;
	::SetGWorld(port, GetMainDevice());	
	
	RGBColor color;
	GetForeColor(&color);
	mOldColor = color;
	
#if TARGET_CARBON
	mOldFontNum = ::GetPortTextFont(port);
	mOldStyle   = numeric_cast<uint8>(::GetPortTextSize(port));
	mOldStyle   = ::GetPortTextFace(port);
#else
	mOldFontNum = port->txFont;
	mOldSize    = port->txSize;
	mOldStyle   = port->txFace;
#endif

	mFont->Select();
	
	if (mName == kBigSystemFont) 				// $$$ not sure if TextFont supports the new kControlFontBigSystemFont constants
		::TextFont(0);	
	else if (mName == kSmallSystemFont) 
		::TextFont(1);	
	else if (mName == kSmallBoldSystemFont) 
		::TextFont(1);
	else
		::TextFont(mFontNum);

	::TextSize(numeric_cast<int16>(mSize));

	RGBColor temp = mColor.GetOSColor();
	::RGBForeColor(&temp);

	Style style = normal;
	if (mStyle & kBoldStyle)
		style += bold;
	
	if (mStyle & kItalicStyle)
		style += italic;
		
	if (mStyle & kUnderlineStyle)
		style += underline;
		
	::TextFace(style);
}


//---------------------------------------------------------------
//
// XFont::DoDeSelect
//
//---------------------------------------------------------------
void XFont::DoDeSelect(OSDrawContext context) const
{
	PRECONDITION(context != nil);
	
	::SetGWorld(context, GetMainDevice());	
	
	mFont->DeSelect();

	::TextFont(mOldFontNum);
	::TextSize(mOldSize);
	::TextFace(mOldStyle);

	RGBColor temp = mOldColor.GetOSColor();
	::RGBForeColor(&temp);
}


}	// namespace Whisper

