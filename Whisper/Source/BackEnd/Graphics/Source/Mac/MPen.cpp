/*
 *  File:       MPen.cpp
 *  Summary:	An object used when drawing lines or framing shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPen.cpp,v $
 *		Revision 1.6  2001/04/21 03:27:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:41:13  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/04/13 07:52:49  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:04:42  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:17:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	  8/09/99	JDJ		Added streaming support. 
 *		 <3>	  1/16/99	JDJ		Disabled XPen::XPen (XPixMap, EDrawMode). Replaced
 * 									selection pens with selection brushes.
 *		 <2>	 12/24/98	JDJ		Added XPen::XPen (Pattern, XRGBColor, int32, EDrawMode)
 *									and ZSelectionPenInitter. Added SetDrawMode.
 *		 <1>	 11/30/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XPen.h>

#include <XDrawContext.h>
#include <XReferenceCounted.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const XPen kBlackPen(XRGBColor(0.0,  0.0,  0.0));		// we don't use the XRGBColor constants because of order of initialization problems
const XPen kWhitePen(XRGBColor(1.0,  1.0,  1.0));
const XPen kRedPen(XRGBColor(0.87, 0.0,  0.0));
const XPen kYellowPen(XRGBColor(1.0,  1.0,  0.0));
const XPen kBluePen(XRGBColor(0.0,  0.0,  0.87));
const XPen kGreenPen(XRGBColor(0.0,  0.93, 0.0));
const XPen kBrownPen(XRGBColor(0.6,  0.4,  0.0));
const XPen kPurplePen(XRGBColor(0.4,  0.0,  0.6));
const XPen kLightBluePen(XRGBColor(0.0,  0.6,  1.0));
const XPen kGrayPen(XRGBColor(0.53,  0.53,  0.53));
const XPen kLightGrayPen(XRGBColor(0.75, 0.75, 0.75));
const XPen kDarkGrayPen(XRGBColor(0.25, 0.25, 0.25));


// ===================================================================================
//	class ZPen
// ===================================================================================
class ZPen : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZPen();		
						
						ZPen(const XBrush& brush, int16 width);
						
private:		
						ZPen(const ZPen& rhs);
						
			ZPen&		operator=(const ZPen& rhs);
						
//-----------------------------------
//	API
//
public:
			void 		SetDrawMode(EDrawMode mode)				{mBrush.SetDrawMode(mode);}

			int32 		GetWidth() const						{return mWidth;}

			void 		Select(OSDrawContext context);
			void 		DeSelect(OSDrawContext context);
						
			bool 		operator==(const ZPen& rhs) const;

			void 		StreamOut(XXMLDoc& doc, XXMLElement& element) const;
			void 		StreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element);

//-----------------------------------
//	Member Data
//
protected:
	int32		mWidth;
	XBrush		mBrush;
};


//---------------------------------------------------------------
//
// ZPen::~ZPen
//
//---------------------------------------------------------------
ZPen::~ZPen()
{
}
		

//---------------------------------------------------------------
//
// ZPen::ZPen
//
//---------------------------------------------------------------
ZPen::ZPen(const XBrush& brush, int16 width) : mBrush(brush)
{
	PRECONDITION(width >= 0);
	
	mWidth = width;
}


//---------------------------------------------------------------
//
// ZPen::operator==
//
//---------------------------------------------------------------
bool ZPen::operator==(const ZPen& rhs) const
{
	bool eq = mWidth == rhs.mWidth && mBrush == rhs.mBrush;
	
	return eq;
}


//---------------------------------------------------------------
//
// ZPen::StreamOut
//
//---------------------------------------------------------------
void ZPen::StreamOut(XXMLDoc& doc, XXMLElement& element) const
{
	mBrush.HandleStreamOut(doc, element);
	
	element.AppendAttribute(XXMLAttribute(L"width", mWidth));
}


//---------------------------------------------------------------
//
// ZPen::StreamIn
//
//---------------------------------------------------------------
void ZPen::StreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element)
{
	UNUSED(root);
	
	mBrush.HandleStreamIn(doc, element);
	 	
	const XXMLAttribute& attr = element.FindAttribute(L"width");
	attr.Get(0L, mWidth, (int32) SHRT_MAX);
}


//---------------------------------------------------------------
//
// ZPen::Select
//
//---------------------------------------------------------------
void ZPen::Select(OSDrawContext context)
{
	::PenSize(numeric_cast<int16>(mWidth), numeric_cast<int16>(mWidth));	
	mBrush.DoSelect(context);
}


//---------------------------------------------------------------
//
// ZPen::DeSelect
//
//---------------------------------------------------------------
void ZPen::DeSelect(OSDrawContext context)
{
	mBrush.DoDeSelect(context);
}

#pragma mark -

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
	mPen->RemoveReference();
}


//---------------------------------------------------------------
//
// XPen::XPen (XRGBColor, int32, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(const XRGBColor& color, int32 width, EDrawMode mode)
{
	PRECONDITION(width >= 0);
	
	mPen = new ZPen(XBrush(color, mode), numeric_cast<int16>(width));
}


//---------------------------------------------------------------
//
// XPen::XPen (EHatchBrush, XRGBColor, int32, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(EHatchBrush brush, const XRGBColor& color, int32 width, EDrawMode mode)
{	
	PRECONDITION(width >= 0);
	
	mPen = new ZPen(XBrush(brush, color, mode), numeric_cast<int16>(width));
}


//---------------------------------------------------------------
//
// XPen::XPen (XPixMap, EDrawMode)
//
//---------------------------------------------------------------
#if 0												// not supported on Win98...
XPen::XPen(const XPixMap& pixMap, EDrawMode mode) 
{
	mPen = new ZPen(XBrush(pixMap, mode), 1);
}
#endif


//---------------------------------------------------------------
//
// XPen::XPen (Pattern, XRGBColor, int32, EDrawMode)
//
//---------------------------------------------------------------
XPen::XPen(const Pattern& pattern, const XRGBColor& color, int32 width, EDrawMode mode)
{
	PRECONDITION(width >= 0);
	
	mPen = new ZPen(XBrush(pattern, color, mode), numeric_cast<int16>(width));
}


//---------------------------------------------------------------
//
// XPen::XPen (XPen)
//
//---------------------------------------------------------------
XPen::XPen(const XPen& rhs)
{
	mPen = rhs.mPen;
	mPen->AddReference();
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

		mPen = rhs.mPen;
		mPen->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XPen::SetDrawMode
//
//---------------------------------------------------------------
void XPen::SetDrawMode(EDrawMode mode)				
{
	mPen->SetDrawMode(mode);
}


//---------------------------------------------------------------
//
// XPen::GetWidth
//
//---------------------------------------------------------------
int32 XPen::GetWidth() const						
{	
	int32 width = mPen->GetWidth();
	
	return width;
}


//---------------------------------------------------------------
//
// XPen::operator==
//
//---------------------------------------------------------------
bool XPen::operator==(const XPen& rhs) const
{
	bool eq = mPen == rhs.mPen || *mPen == *rhs.mPen;
			
	return eq;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XPen::Invariant
//
//---------------------------------------------------------------
void XPen::Invariant() const
{	
	ASSERT(mPen != nil);
}


//---------------------------------------------------------------
//
// XPen::OnStreamOut
//
//---------------------------------------------------------------
void XPen::OnStreamOut(XXMLDoc& doc, XXMLElement& element) const
{
	mPen->StreamOut(doc, element);
}


//---------------------------------------------------------------
//
// XPen::OnStreamIn
//
//---------------------------------------------------------------
void XPen::OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element)
{
	// Since we're mutating an existing ZPen object we need to
	// make sure it's not being shared.
	if (mPen->GetRefCount() > 1) {
		ZPen* newPen = new ZPen(XBrush(kRGBBlack, kCopyMode), 1);
		
		mPen->RemoveReference();
		mPen = newPen;
	}
	
	mPen->StreamIn(doc, root, element);
}


//---------------------------------------------------------------
//
// XPen::DoSelect
//
//---------------------------------------------------------------
void XPen::DoSelect(OSDrawContext context) const
{
	PRECONDITION(context != nil);
	
	::SetGWorld(context, GetMainDevice());	
	
	PenState state;
	GetPenState(&state);
	mOldWidth = state.pnSize.h;
	
	mPen->Select(context);
}


//---------------------------------------------------------------
//
// XPen::DoDeSelect
//
//---------------------------------------------------------------
void XPen::DoDeSelect(OSDrawContext context) const
{
	PRECONDITION(context != nil);
	
	::SetGWorld(context, GetMainDevice());	
	
	 mPen->DeSelect(context);

	::PenSize(mOldWidth, mOldWidth);	
}


}	// namespace Whisper

