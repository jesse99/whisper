/*
 *  File:       WColorTable.cpp
 *  Summary:	Encapsulates an OS specific array of colors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: WColorTable.cpp,v $
 *		Revision 1.5  2001/04/21 03:30:33  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/05 04:02:25  jesjones
 *		Next seed is no longer static member data (due to order of initialization problems).
 *		
 *		Revision 1.3  2000/11/20 05:42:22  jesjones
 *		Ctor no longer asserts that numColos is a power of 2.
 *		
 *		Revision 1.2  2000/11/09 12:05:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 10/07/00	JDJ		Added a ctor that takes an XXMLElement and a CreateElement method. 
 *		 <4>	  9/10/00	JDJ		Changed DoCompare so that it actually works and is more efficient. 
 *		 <3>	  5/27/99	JDJ		msNextSeed is not an XAtomicCounter.
 *		 <2>	  1/06/99	JDJ		Can now create an XColorTable from an HPALETTE.
 *		 <1>	 11/27/97	JDJ		Created 
 */

#include <XWhisperHeader.h>
#include <XColorTable.h>

#include <XAutoPtr.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator!= (RGBQUAD, RGBQUAD)
//
//---------------------------------------------------------------
inline bool operator!=(const RGBQUAD& lhs, const RGBQUAD& rhs)
{
	return lhs.rgbRed != rhs.rgbRed || lhs.rgbGreen != rhs.rgbGreen || lhs.rgbBlue != rhs.rgbBlue;
}


//---------------------------------------------------------------
//
// operator< (RGBQUAD, RGBQUAD)
//
//---------------------------------------------------------------
static bool operator<(const RGBQUAD& lhs, const RGBQUAD& rhs)
{
	if (lhs.rgbRed < rhs.rgbRed)
		return true;
	else if (rhs.rgbRed < lhs.rgbRed)
		return false;

	if (lhs.rgbGreen < rhs.rgbGreen)
		return true;
	else if (rhs.rgbGreen < lhs.rgbGreen)
		return false;

	if (lhs.rgbBlue < rhs.rgbBlue)
		return true;
	else if (rhs.rgbBlue < lhs.rgbBlue)
		return false;
		
	return false;
}


//---------------------------------------------------------------
//
// GetNextSeed 
//
//---------------------------------------------------------------
static int32 GetNextSeed()
{
	static XAtomicCounter seed;		// $$ not quite thread safe (but can't use a file scoped static because of order of initialization problems)
	
	return ++seed;
}

#if __MWERKS__
#pragma mark -
#endif

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
	delete [] mColors;
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable ()
//
//---------------------------------------------------------------
XColorTable::XColorTable()
{
	mColors = new RGBQUAD[2];
	mSize   = 2;
	mDirty  = true;
	
	mColors[0] = kRGBBlack.GetOSColor();
	mColors[1] = kRGBWhite.GetOSColor();

	mSeed = GetNextSeed();
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (OSColor*, uint32)
//
//---------------------------------------------------------------
XColorTable::XColorTable(const OSColor* colors, uint32 numColors)
{
	PRECONDITION(colors != nil);
	
	mColors = new RGBQUAD[numColors];
	mSize   = numColors;
	mDirty  = true;
	
	BlockMoveData(colors, mColors, numColors*sizeof(RGBQUAD));

	mSeed = GetNextSeed();
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (OSPalette)
//
//---------------------------------------------------------------
XColorTable::XColorTable(OSPalette palette)
{
	PRECONDITION(palette != nil);
	
	// $$ There is apparently no way to find the number of colors
	// $$ in a palette(!) so we'll assume that we're never passed
	// $$ a palette with more than 256 colors...
	PALETTEENTRY entries[256];
	uint32 numEntries = ::GetPaletteEntries(palette, 0, 256, entries);
	ThrowIf(numEntries == 0);
			
	mColors = new RGBQUAD[numEntries];
	mSize   = numEntries;
	mDirty  = true;
	
	// Despite being exactly the same size RGBQUAD and PALETTEENTRY
	// have completely different layouts so we need to copy the colors
	// over one at a time...
	RGBQUAD* color = mColors;
	const PALETTEENTRY* entry = entries;
	for (uint32 index = 0; index < mSize; ++index) {
		color->rgbBlue  = entry->peBlue;
		color->rgbGreen = entry->peGreen;
		color->rgbRed   = entry->peRed;
		
		color->rgbReserved = 0;
		
		++color; ++entry;
	}

	mSeed = GetNextSeed();
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (ResID)
//
// Windows only has a handfull of predefined resource types and
// color tables (or palettes) are not one of the lucky few. This
// code is written for a custom resource type called "clut". The
// format is a DWORD for the number of colors followed by an
// RGBQUAD for each color.
//
//---------------------------------------------------------------
XColorTable::XColorTable(ResID id)
{
	HRSRC rsrc = FindResource(nil, MAKEINTRESOURCE(id), "clut");
	ThrowIfBadHandle(rsrc);
	
	HGLOBAL dataH = LoadResource(nil, rsrc);
	ThrowIfBadHandle(dataH);
	
	uint32* dataP = (uint32*) LockResource(dataH);
	ThrowIfNil(dataP);
	
	mSize   = *dataP;
	mColors = new RGBQUAD[mSize];
	mDirty  = true;
	
	BlockMoveData(dataP + 1, mColors, mSize*sizeof(RGBQUAD));

	mSeed = GetNextSeed();
}


//---------------------------------------------------------------
//
// XColorTable::XColorTable (XColorTable)
//
//---------------------------------------------------------------
XColorTable::XColorTable(const XColorTable& rhs)
{
	mSize   = rhs.mSize;
	mColors = new RGBQUAD[mSize];
	mSeed   = rhs.mSeed;
	mDirty  = rhs.mDirty;
	
	BlockMoveData(rhs.mColors, mColors, mSize*sizeof(RGBQUAD));
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
	
	mColors = new RGBQUAD[0];
	mSize   = 0;
	mDirty  = true;
	
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
		const RGBQUAD& color = mColors[i];
		
		XAutoPtr<XXMLElement> child(new XXMLElement(cName));
		child->AppendAttribute(XXMLAttribute(L"red", color.rgbRed/255.0));
		child->AppendAttribute(XXMLAttribute(L"green", color.rgbGreen/255.0));
		child->AppendAttribute(XXMLAttribute(L"blue", color.rgbBlue/255.0));
		
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
	if (this != &rhs) {
		RGBQUAD* temp = new RGBQUAD[rhs.mSize];
		
		delete [] mColors;

		mColors = temp;
		mSize   = rhs.mSize;
		mSeed   = rhs.mSeed;
		mDirty  = rhs.mDirty;
		
		BlockMoveData(rhs.mColors, mColors, mSize*sizeof(RGBQUAD));
	}

	return *this;
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
		RGBQUAD* temp = new RGBQUAD[newSize];
		BlockMoveData(mColors, temp, Min(mSize, newSize)*sizeof(RGBQUAD));
		
		for (uint32 index = mSize; index < newSize; ++index)
			temp[index].rgbReserved = 0;
		
		delete [] mColors;

		mColors = temp;
		mSize   = newSize;
		mDirty  = true;
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

	mColors[index] = color.GetOSColor();

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

	XRGBColor color = mColors[index];
	
	return color;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XColorTable::DoCompare 
//
//---------------------------------------------------------------
int XColorTable::DoCompare(const XColorTable& rhs) const
{
	int result;
	
	// If the lhs or rhs is dirty update the seed.
	if (mDirty)
		mSeed = GetNextSeed();
	
	if (rhs.mDirty)
		rhs.mSeed = GetNextSeed();
	
	// Compare the colors as quickly as possible.
	if (mSize > rhs.mSize)		
		result = 1;
		
	else if (mSize < rhs.mSize)
		result = -1;
		
	else if (mSeed == rhs.mSeed)
		result = 0;
	
	else {
		uint16 index = 0;
		result = 0;
		
		while (result == 0 && index < mSize) {
			const RGBQUAD& c1 = mColors[index];
			const RGBQUAD& c2 = rhs.mColors[index];
			
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

