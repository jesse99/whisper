/*
 *  File:       MColorTable.cpp
 *  Summary:	Encapsulates an OS specific array of colors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MColorTable.cpp,v $
 *		Revision 1.6  2001/05/30 01:15:00  jesjones
 *		(OSColor*, uint32) ctor no longer asserts that count is a power of two.
 *		
 *		Revision 1.5  2001/04/21 03:24:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/16 10:19:24  jesjones
 *		Included <vector>.
 *		
 *		Revision 1.3  2000/12/10 04:02:47  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:10:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XColorTable.h>

#include <vector>

#include <QDOffscreen.h>

#include <MQuickDrawUtils.h>
#include <XAutoPtr.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	Helper Functions	
// ===================================================================================

inline bool operator!=(const RGBColor& lhs, const RGBColor& rhs)
{
	return lhs.red != rhs.red || lhs.green != rhs.green || lhs.blue != rhs.blue;
}

static bool operator<(const RGBColor& lhs, const RGBColor& rhs)
{
	if (lhs.red < rhs.red)
		return true;
	else if (rhs.red < lhs.red)
		return false;

	if (lhs.green < rhs.green)
		return true;
	else if (rhs.green < lhs.green)
		return false;

	if (lhs.blue < rhs.blue)
		return true;
	else if (rhs.blue < lhs.blue)
		return false;
		
	return false;
}

//---------------------------------------------------------------
//
// SetComponants
//
//---------------------------------------------------------------
inline void SetComponants(RGBColor& a, uint16 componant)	
{ 
	a.red = a.green = a.blue = componant; 
}


//---------------------------------------------------------------
//
// CalcColorTableSize (uint32)
//
//---------------------------------------------------------------
inline long	CalcColorTableSize(uint32 numColors)						
{
	PRECONDITION(numColors <= 16384);
	
	long bytes = (long) (sizeof(ColorTable) + sizeof(ColorSpec)*(numColors - 1));
	
	return bytes;
}
	

//---------------------------------------------------------------
//
// CalcColorTableSize (CTabHandle)
//
//---------------------------------------------------------------
inline long	CalcColorTableSize(CTabHandle colorsH)					
{
	PRECONDITION(colorsH != nil);
	PRECONDITION(*colorsH != nil);
	
	long bytes = (long) (sizeof(ColorTable) + sizeof(ColorSpec)*(**colorsH).ctSize);
	
	return bytes;
}


//---------------------------------------------------------------
//
// RoundUpColorSize
//
// Rounds up a number to the nearest power of two, pegging at 256.
//
//---------------------------------------------------------------
static uint32 RoundUpColorSize(uint32 inCount)
{
	PRECONDITION(inCount <= 16384);
	
	uint32 outCount = 256;

	if (inCount <= 2)
		outCount = 2;
	else if (inCount <= 4)
		outCount = 4;
	else if (inCount <= 16)
		outCount = 16;

	return outCount;
}


//---------------------------------------------------------------
//
// MakeColorTable
//
//---------------------------------------------------------------
static CTabHandle MakeColorTable(uint32 numColors)
{
	PRECONDITION(numColors < 16384);

	CTabHandle colors = (CTabHandle) NewHandle(8 + numColors*sizeof(ColorSpec));
	ThrowIfMemFail(colors);

	(**colors).ctSeed  = GetCTSeed();
	(**colors).ctFlags = 0;
	(**colors).ctSize  = numeric_cast<int16>(numColors ? numColors - 1 : 0);

	if (numColors > 0) {
		for (int16 index = 0; index < numColors; index++)
			(**colors).ctTable[index].value = index;

		(**colors).ctTable[0].rgb = kRGBWhite.GetOSColor();
		(**colors).ctTable[numColors - 1].rgb = kRGBBlack.GetOSColor();
	}

	return colors;
}


//---------------------------------------------------------------
//
// CloneColorTable
//
//---------------------------------------------------------------
static CTabHandle CloneColorTable(CTabHandle srcColors)
{
	PRECONDITION(srcColors != nil);
	PRECONDITION(!IsPurgeable((Handle) srcColors));

	int32 srcSize = GetHandleSize((Handle) srcColors);
	REQUIRE(srcSize >= (sizeof(ColorTable) - sizeof(ColorSpec)) && srcSize >= ((**srcColors).ctSize * sizeof(ColorSpec) + sizeof(ColorTable)));
		
	int32 newSize = CalcColorTableSize(srcColors);
	CTabHandle colorsH = (CTabHandle) ::NewHandle(newSize);
	ThrowIfNil(colorsH);
	
	::BlockMoveData(*srcColors, *colorsH, newSize);			

	int16 numColors = numeric_cast<int16>((**colorsH).ctSize + 1);			
	for (int16 n = 0; n < numColors; n++)
		(**colorsH).ctTable[n].value = n;

	(**colorsH).ctFlags = 0;						

	return colorsH;
}

#pragma mark -

// ===================================================================================
//	XColorTable
// ===================================================================================

//---------------------------------------------------------------
//
// XColorTable::~XColorTable 
//
//---------------------------------------------------------------
XColorTable::~XColorTable()
{
	::DisposeCTable(mColors);
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable ()
//
//---------------------------------------------------------------
XColorTable::XColorTable()
{
	CTabHandle colorsH = GetCTable(33);		// Create a black and white color table.
	ThrowIfResFail(colorsH);
	
	mColors = CloneColorTable(colorsH);
	mSize   = (**mColors).ctSize + 1UL;
	mDirty  = false;
	
	DisposeCTable(colorsH);
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (OSColor*, uint32)
//
//---------------------------------------------------------------
XColorTable::XColorTable(const OSColor* colors, uint32 numColors)
{
	PRECONDITION(colors != nil);

	mColors = MakeColorTable(numColors);
	mSize   = numColors;
	mDirty  = false;

	for (uint32 index = 0; index < numColors; ++index) {
		(**mColors).ctTable[index].rgb   = colors[index];
		(**mColors).ctTable[index].value = 1;
	}
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (CTabHandle)
//
//---------------------------------------------------------------
XColorTable::XColorTable(CTabHandle colorsH)
{
	mColors = CloneColorTable(colorsH);
	mSize   = (**mColors).ctSize + 1UL;
	mDirty  = false;
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (OSPalette)
//
// This code is from Paul Elseth.
//
//---------------------------------------------------------------
XColorTable::XColorTable(OSPalette palH)
{
	PRECONDITION(palH != nil);
	PRECONDITION(*palH != nil);
	
	int16 index;

	mColors = MakeColorTable(RoundUpColorSize((**palH).pmEntries));
	mSize   = (**mColors).ctSize + 1UL;
	mDirty  = false;

	// Mark all colors as unitialized to start:
	for (index = 0; index <= (**mColors).ctSize; index++)
		(**mColors).ctTable[index].value = 0;

	// Set first and last colors to white and black, respectively:
	SetComponants((**mColors).ctTable[0].rgb, 0xFFFF);
	(**mColors).ctTable[0].value = 1;
	SetComponants((**mColors).ctTable[(**mColors).ctSize].rgb, 0x0000);
	(**mColors).ctTable[(**mColors).ctSize].value = 1;

	for (index = 0; index < (**palH).pmEntries; ++index) {
		if ((**palH).pmInfo[index].ciUsage & pmInhibitC8)					// ignore 8-bit inhibited colors
			;
			
		else if ((**palH).pmInfo[index].ciUsage & pmExplicit) {				// handle explicit colors
			(**mColors).ctTable[index].rgb   = (**palH).pmInfo[index].ciRGB;
			(**mColors).ctTable[index].value = 1;
			
		} else {
			for (int m = 0; m <= (**mColors).ctSize; ++m) {
				if ((**mColors).ctTable[m].value == 0) {
					(**mColors).ctTable[m].rgb   = (**palH).pmInfo[index].ciRGB;
					(**mColors).ctTable[m].value = 1;
				}
			}
		}
	}

	// Fill in uninitialized entries with black, and fix the color table's values:
	for (index = 0; index <= (**mColors).ctSize; index++) {
		if ((**mColors).ctTable[index].value == 0)
			SetComponants((**mColors).ctTable[index].rgb, 0x0000);
		(**mColors).ctTable[index].value = index;
	}
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (XColorTable)
//
//---------------------------------------------------------------
XColorTable::XColorTable(const XColorTable& rhs)
{
	if (rhs.mDirty) {
		(**rhs.mColors).ctSeed = GetCTSeed();
		CTabChanged(rhs.mColors);
		
		rhs.mDirty = false;
	}

	mColors = CloneColorTable(rhs.mColors);
	mSize   = (**mColors).ctSize + 1UL;
	mDirty  = false;
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (ResID)
//
//---------------------------------------------------------------
XColorTable::XColorTable(ResID rsrcID)
{
	CTabHandle colorsH = GetCTable(rsrcID);
	ThrowIfResFail(colorsH);
	
	mColors = CloneColorTable(colorsH);
	mSize   = (**mColors).ctSize + 1UL;
	mDirty  = false;
	
	DisposeCTable(colorsH);
} 


//---------------------------------------------------------------
//
// XColorTable::XColorTable (XXMLElement, const wchar_t*, const wchar_t*)
//
//---------------------------------------------------------------
XColorTable::XColorTable(const XXMLElement& element, const wchar_t* pName, const wchar_t* cName)
{
	PRECONDITION(pName != nil);
	PRECONDITION(cName != nil);
	
	if (element.GetName() != pName)		// may not have a DTD so we can't count on the parser to handle these types of errors
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"#1 isn't a valid palette element name", element.GetName())));

	std::vector<XRGBColor> colors;
	colors.reserve(256);

	XXMLElement::const_item_iterator iter = element.item_begin();
	while (iter != element.item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {	// may not have a DTD so the parser may not strip out whitespace elements
			if (child->GetName() != cName)		
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"#1 isn't a valid palette entry name", child->GetName())));
			
			const XXMLAttribute& r = child->FindAttribute(L"red");
			const XXMLAttribute& g = child->FindAttribute(L"green");
			const XXMLAttribute& b = child->FindAttribute(L"blue");
			
			XRGBColor color;
			r.Get(color.red);
			g.Get(color.green);
			b.Get(color.blue);
			
			colors.push_back(color);
		}
	}
	
	mColors = MakeColorTable(0);
	mSize   = 0;
	mDirty  = false;

	uint32 count = colors.size();
	this->SetSize(count);
	
	for (uint32 i = 0; i < count; ++i) 
		this->SetColor(i, colors[i]);
}


//---------------------------------------------------------------
//
// XColorTable::CreateElement
//
//---------------------------------------------------------------
XXMLElement* XColorTable::CreateElement(const wchar_t* pName, const wchar_t* cName) const
{
	PRECONDITION(pName != nil);
	PRECONDITION(cName != nil);
	
	XAutoPtr<XXMLElement> element(new XXMLElement(pName));
	
	uint32 count = this->GetSize();
	for (uint32 i = 0; i < count; ++i) {
		const RGBColor& color = (**mColors).ctTable[i].rgb;
		
		XAutoPtr<XXMLElement> child(new XXMLElement(cName));
		child->AppendAttribute(XXMLAttribute(L"red", color.red/65535.0));
		child->AppendAttribute(XXMLAttribute(L"green", color.green/65535.0));
		child->AppendAttribute(XXMLAttribute(L"blue", color.blue/65535.0));
		
		element->AppendItem(child.Release());
	}

	return element.Release();
}


//---------------------------------------------------------------
//
// XColorTable::operator= 
//
//---------------------------------------------------------------
XColorTable& XColorTable::operator=(const XColorTable& rhs)
{
	if (mColors != rhs.mColors) {
		CTabHandle colorsH = CloneColorTable(rhs.mColors);

		if (rhs.mDirty) {
			(**rhs.mColors).ctSeed = GetCTSeed();
			CTabChanged(rhs.mColors);
			
			rhs.mDirty = false;
		}

		DisposeCTable(mColors);
		mColors = colorsH;
		mSize   = (**mColors).ctSize + 1UL;
		mDirty  = false;
	}

	return *this;
}


//---------------------------------------------------------------
//
// XColorTable::operator CTabHandle
//
//---------------------------------------------------------------
XColorTable::operator CTabHandle() const					
{
	if (mDirty) {
		(**mColors).ctSeed = GetCTSeed();
		CTabChanged(mColors);
		
		mDirty = false;
	}

	return mColors;
}


//---------------------------------------------------------------
//
// XColorTable::CreatePalette 
//
//---------------------------------------------------------------
PaletteHandle XColorTable::CreatePalette(int16 usage, int16 tolerance) const
{
	PaletteHandle palette = NewPalette(numeric_cast<int16>(this->GetSize()), mColors, usage, tolerance);
	
	return palette;
}


//---------------------------------------------------------------
//
// XColorTable::SetSize 
//
//---------------------------------------------------------------
void XColorTable::SetSize(uint32 newSize)
{
	PRECONDITION(newSize < 16384);
		
	if (newSize != mSize) {
		::SetHandleSize((Handle) mColors, CalcColorTableSize(newSize));
		ThrowIfMemError();
		
		(**mColors).ctSize = numeric_cast<int16>(newSize - 1);
		
		mSize = newSize;
		mDirty = true;
	}
}


//---------------------------------------------------------------
//
// XColorTable::SetColor 
//
//---------------------------------------------------------------
void XColorTable::SetColor(uint32 index, const XRGBColor& color)
{
	PRECONDITION(index < this->GetSize());

	(**mColors).ctTable[index].value = numeric_cast<int16>(index);
	(**mColors).ctTable[index].rgb = color.GetOSColor();

	mDirty = true;
}
	

//---------------------------------------------------------------
//
// XColorTable::GetColor 
//
//---------------------------------------------------------------
XRGBColor XColorTable::GetColor(uint32 index) const
{
	PRECONDITION(index < this->GetSize());

	XRGBColor color = (**mColors).ctTable[index].rgb;
	
	return color;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XColorTable::DoCompare
//
//---------------------------------------------------------------
int XColorTable::DoCompare(const XColorTable& rhs) const
{
	int result;
	
	if (mSize > rhs.mSize)		
		result = 1;
		
	else if (mSize < rhs.mSize)
		result = -1;
		
	else if (!mDirty && !rhs.mDirty && (**mColors).ctSeed == (**rhs.mColors).ctSeed)
		result = 0;
	
	else {
		uint16 index = 0;
		result = 0;
		
		while (result == 0 && index < mSize) {
			const RGBColor& c1 = (**mColors).ctTable[index].rgb;
			const RGBColor& c2 = (**rhs.mColors).ctTable[index].rgb;
			
			if (c1 < c2)
				result = -1;
			else if (c1 != c2)
				result = +1;
			
			index++;
		}
	}
	
	return result;
}


}	// namespace Whisper

