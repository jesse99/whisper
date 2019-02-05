/*
 *  File:       MStaticText.cpp
 *  Summary:   	Interface for a control that displays non-editable text.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MStaticText.cpp,v $
 *		Revision 1.6  2001/04/27 04:22:27  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/03/26 04:17:09  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.4  2000/12/17 03:47:08  jesjones
 *		Got rid of the dollar signs around Log.
 *		
 *		Revision 1.3  2000/12/14 08:29:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:20:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IStaticText.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <XExceptions.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class MStaticText
//!		Interface for a control that displays non-editable text.
// ===================================================================================
class MStaticText : public IStaticText, public IText {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MStaticText();
	
						MStaticText(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 const XFont& font = kSmallSystemFont,
							 uint32 style = kDefaultStaticTextStyle,
							 uint32 procID = kDefaultStaticTextProcID);

	virtual const XFont& GetFont() const					{return mFont;}
	virtual void 		SetFont(const XFont& font, bool redraw = kRedraw);

	virtual ETextAlignment GetAlignment() const				{return mAlignment;}
	virtual void 		SetAlignment(ETextAlignment alignment, bool redraw = kRedraw);

	virtual bool 		IsEmpty() const;
	virtual std::wstring GetText() const;
	virtual void 		SetText(const std::wstring& text, bool redraw = kRedraw);
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoSetStyle();
	
//-----------------------------------
//	Member Data
//
private:
	XFont			mFont;
	ETextAlignment	mAlignment;
};

DEFINE_INTERFACE_FACTORY(MStaticText)

//---------------------------------------------------------------
//
// MStaticText::~MStaticText
//
//---------------------------------------------------------------
MStaticText::~MStaticText()
{
}


//---------------------------------------------------------------
//
// MStaticText::MStaticText
//
//---------------------------------------------------------------
MStaticText::MStaticText(XBoss* boss) : mFont(kCourierFont)
{	
	IStaticText::DoSetBoss(boss);
	IText::DoSetBoss(boss);
	
	mAlignment = kLeftTopAlign;
}


//---------------------------------------------------------------
//
// MStaticText::Init
//
//---------------------------------------------------------------
void MStaticText::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& inText, const XFont& font,
					   uint32 style, uint32 procID)
{
	UNUSED(style);
		
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IStaticText* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the font		
	this->SetFont(font, kDontRedraw);
	
	// Set the text		
	this->SetText(inText, kDontRedraw);
}


//---------------------------------------------------------------
//
// MStaticText::IsEmpty
//
//---------------------------------------------------------------
bool MStaticText::IsEmpty() const
{
	const IStaticText* thisPtr = this;
	IConstControlPtr control(thisPtr);
	ControlHandle controlH = control->GetOSControl();

	int32 length = 0;
	OSErr err = GetControlDataSize(controlH, kControlEntireControl, kControlStaticTextTextTag, &length);
	ThrowIfOSErr(err);
		
	return length == 0;
}


//---------------------------------------------------------------
//
// MStaticText::GetText
//
//---------------------------------------------------------------
std::wstring MStaticText::GetText() const
{	
	const IStaticText* thisPtr = this;
	IConstControlPtr control(thisPtr);
	ControlHandle controlH = control->GetOSControl();

	int32 length = 0;
	OSErr err = GetControlDataSize(controlH, kControlEntireControl, kControlStaticTextTextTag, &length);
	ThrowIfOSErr(err);
	
	XTinyVector<char> buffer((uint32) length+1);
	
	int32 actualLength = 0;
	err = GetControlData(controlH, kControlEntireControl, kControlStaticTextTextTag, length, buffer.buffer(), &actualLength);
	ThrowIfOSErr(err);
	
	ASSERT(actualLength <= length);
	
	return FromPlatformStr(buffer.buffer(), (uint32) actualLength);
}


//---------------------------------------------------------------
//
// MStaticText::SetText
//
//---------------------------------------------------------------
void MStaticText::SetText(const std::wstring& text, bool redraw)
{
	std::wstring newText = Whisper::Replace(text, '&', '\0', '&');

	if (newText != this->GetText()) {
		IStaticText* thisPtr = this;
		IControlPtr control(thisPtr);
		ControlHandle controlH = control->GetOSControl();

		::SetPort(control->GetOSDrawContext());
		::HidePen();
		OSErr err = SetControlData(controlH, kControlEntireControl, kControlStaticTextTextTag, (int32) text.length(), (Ptr) ToPlatformStr(text).c_str());
		::ShowPen();
		ThrowIfOSErr(err);
	
		if (redraw)
			control->Invalidate();
	}
}


//---------------------------------------------------------------
//
// MStaticText::SetFont
//
//---------------------------------------------------------------
void MStaticText::SetFont(const XFont& font, bool redraw)
{
	if (font != mFont) {
		mFont = font;
	
		this->DoSetStyle();
		
		if (redraw) {
			IStaticText* thisPtr = this;
			IControlPtr control(thisPtr);
			control->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// MStaticText::SetAlignment
//
//---------------------------------------------------------------
void MStaticText::SetAlignment(ETextAlignment alignment, bool redraw)
{
	if (alignment != mAlignment) {
		mAlignment = alignment;
	
		this->DoSetStyle();
		
		if (redraw) {
			IStaticText* thisPtr = this;
			IControlPtr control(thisPtr);
			control->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// MStaticText::DoSetStyle 
//
// Appearance Manager 1.1 is exceedingly lame: if record.flags is
// set to kControlUseJustMask it will go ahead and reset the text's
// style and color attributes! So, to work around this we funnel
// all style changes through this function...
//
//---------------------------------------------------------------
void MStaticText::DoSetStyle()
{
	ControlFontStyleRec record = mFont.GetFontStyleRecord();

	record.flags |= kControlUseJustMask;
	switch (mAlignment) {
		case kLeftTopAlign:
			record.just = teFlushLeft;
			break;
			
		case kCenterTopAlign:
			record.just = teCenter;
			break;

		case kRightTopAlign:
			record.just = teFlushRight;
			break;

		case kDefaultAlign:
			record.just = teFlushDefault;
			break;

		default:
			record.just = teFlushDefault;
			DEBUGSTR("Unsupported alignment flag!");
	}			

	IStaticText* thisPtr = this;
	IControlPtr control(thisPtr);
	ControlHandle controlH = control->GetOSControl();

	::SetPort(control->GetOSDrawContext());
	::HidePen();
	OSErr err = SetControlFontStyle(controlH, &record);
	::ShowPen();
	ThrowIfOSErr(err);
}


}	// namespace Whisper
