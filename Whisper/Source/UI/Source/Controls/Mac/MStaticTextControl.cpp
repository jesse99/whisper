/*
 *  File:       MStaticTextControl.cpp
 *  Summary:   	A control that displays non-editable text.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MStaticTextControl.cpp,v $
 *		Revision 1.4  2001/04/21 03:28:46  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:05:33  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:20:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	12/23/99	JDJ		OnStreamIn updates mFont if the XFont element is missing.
 *		 <6>	 8/04/99	JDJ		Ampersands are now stripped. OnStreamIn replaces
 *									single undescores with ampersands.
 *		 <5>	 7/20/99	JDJ		Tweaked for Carbon.
 *		 <4>	 6/29/99	JDJ		Added streaming support.
 *		 <3>	 6/22/99	JDJ		Added support for streaming.
 *		 <2>	 6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <1>	11/05/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XStaticTextControl.h>

#include <XAutoPtr.h>
#include <XExceptions.h>
#include <XIntConversions.h>
#include <XNestableControl.h>
#include <XTinyVector.h>
#include <XXMLDoc.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateStaticTextControl
//
//---------------------------------------------------------------
static ControlHandle CreateStaticTextControl(XNestableControl* parent, const XRect& frame, uint32 procID) 
{
	PRECONDITION(parent != nil);
	
	XRect bounds = parent->LocalToWindow(frame);
		
	::SetPort(parent->GetOSContext());
	::HidePen();
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = NewControl(parent->GetOSWindow(), &temp, "\p", true, 0, 0, 0, numeric_cast<int16>(procID), 0);
	::ShowPen();
	ThrowIfNil(controlH);
	
	return controlH;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XStaticTextControl
// ===================================================================================

//---------------------------------------------------------------
//
// XStaticTextControl::~XStaticTextControl
//
//---------------------------------------------------------------
XStaticTextControl::~XStaticTextControl()
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XStaticTextControl::XStaticTextControl
//
//---------------------------------------------------------------
XStaticTextControl::XStaticTextControl(const std::wstring& name, XNestableControl* parent, const XRect& frame, const std::wstring& text, const XFont& font, uint32 style, uint32 procID) : XControl(name, parent, CreateStaticTextControl(parent, frame, procID), style, procID), mFont(font)
{
	this->SetClassName(L"XStaticTextControl");
	mClass = L"STATIC";
	
	mAlignment = kLeftTopAlign;

	std::wstring newText = Whisper::Replace(text, '&', '\0', '&');

	this->OnSetText(newText, kDontRedraw);
	this->OnSetFont(font); 
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XStaticTextControl::Create 							[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XStaticTextControl::Create(XCreateByNameMixin* parent)
{
	XNestableControl* super = dynamic_cast<XNestableControl*>(parent);
	ASSERT(super != nil);
	
	XControl* control = new XStaticTextControl(L"", super, XRect(0, 0, 64, 64), L"");
	
	return control;
}

						
//---------------------------------------------------------------
//
// XStaticTextControl::GetText
//
//---------------------------------------------------------------
std::wstring XStaticTextControl::GetText() const
{
	int32 length = 0;
	
	OSErr err = GetControlDataSize(mControl, kControlEntireControl, kControlStaticTextTextTag, &length);
	ThrowIfOSErr(err);
	
	XTinyVector<char> buffer((uint32) length+1);
	
	int32 actualLength = 0;
	err = GetControlData(mControl, kControlEntireControl, kControlStaticTextTextTag, length, buffer.buffer(), &actualLength);
	ThrowIfOSErr(err);
	
	ASSERT(actualLength <= length);
	
	return std::wstring(buffer.buffer(), (uint32) actualLength);
}


//---------------------------------------------------------------
//
// XStaticTextControl::SetText
//
//---------------------------------------------------------------
void XStaticTextControl::SetText(const std::wstring& text, bool redraw)
{
	CHECK_INVARIANT;
	
	std::wstring newText = Whisper::Replace(text, '&', '\0', '&');

	if (newText != this->GetText()) 
		this->OnSetText(newText, redraw);
}


//---------------------------------------------------------------
//
// XStaticTextControl::SetFont 
//
//---------------------------------------------------------------
void XStaticTextControl::SetFont(const XFont& font, bool redraw)
{
	CHECK_INVARIANT;
	
	if (font != mFont) {
		this->OnSetFont(font);
		
		if (redraw)
			this->Invalidate();
	}
}


//---------------------------------------------------------------
//
// XStaticTextControl::SetAlignment 
//
//---------------------------------------------------------------
void XStaticTextControl::SetAlignment(ETextAlignment alignment, bool redraw)
{
	CHECK_INVARIANT;
	
	if (alignment != mAlignment) {
		this->OnSetAlignment(alignment);
		
		if (redraw)
			this->Invalidate();
	}
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XStaticTextControl::OnStreamOut
//
//---------------------------------------------------------------
void XStaticTextControl::OnStreamOut(XXMLDoc& doc, XXMLElement& parent) const
{
	Inherited::OnStreamOut(doc, parent);
	
	XAutoPtr<XXMLElement> info(new XXMLElement(L"XStaticTextControlInfo"));
	
	info->AppendAttribute(XXMLAttribute(L"text", this->GetText()));	
	
	switch (mAlignment) {
		case kLeftTopAlign:
			info->AppendAttribute(XXMLAttribute(L"alignment", L"left_top"));	
			break;
			
		case kCenterTopAlign:
			info->AppendAttribute(XXMLAttribute(L"alignment", L"center_top"));	
			break;
			
		case kRightTopAlign:
			info->AppendAttribute(XXMLAttribute(L"alignment", L"right_top"));	
			break;
			
		case kDefaultAlign:
			info->AppendAttribute(XXMLAttribute(L"alignment", L"default"));	
			break;
			
		default:
			DEBUGSTR("Bad mAlignment in XStaticTextControl::OnStreamOut"); 
	}
		
	XAutoPtr<XXMLElement> font(new XXMLElement(L"XFont"));
	mFont.HandleStreamOut(doc, *font.Get());
	info->AppendItem(font.Release());
	
	parent.AppendItem(info.Release());
}


//---------------------------------------------------------------
//
// XStaticTextControl::OnStreamIn
//
//---------------------------------------------------------------
void XStaticTextControl::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLElement& element)
{
	Inherited::OnStreamIn(doc, parent, element);
	
	if (element.GetName() == L"XStaticTextControlInfo") {
		ASSERT(element.attr_begin() != element.attr_end());		// we have required attributes
			
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"text") {			
				std::wstring newValue = Whisper::Replace(value, '_', '&', '_');
				this->SetText(LoadAppString(newValue), kDontRedraw);
							  
			} else if (name == L"alignment") {			
				if (value == L"left_top")
					this->SetAlignment(kLeftTopAlign, kDontRedraw);
				else if (value == L"center_top")
					this->SetAlignment(kCenterTopAlign, kDontRedraw);
				else if (value == L"right_top")
					this->SetAlignment(kRightTopAlign, kDontRedraw);
				else if (value == L"default")
					this->SetAlignment(kDefaultAlign, kDontRedraw);
				else 
					DEBUGSTR(value, " isn't a valid XStaticTextControlInfo alignment attribute!");
			
			} else
				DEBUGSTR(name, " isn't a valid attribute of XStaticTextControlInfo!");
		}	
		
		XFont font(kSystemFont);
		if (element.item_begin() != element.item_end()) {
			const XXMLItem* item = element.item_front();
			const XXMLElement* child = dynamic_cast<const XXMLElement*>(item);
			ASSERT(child != nil);
			ASSERT(child->GetName() == L"XFont");
			
			font.HandleStreamIn(doc, *child);
		}
		this->SetFont(font, kDontRedraw);
	}
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XStaticTextControl::OnSetText
//
//---------------------------------------------------------------
void XStaticTextControl::OnSetText(const std::wstring& text, bool redraw)
{		
	::SetPort(this->GetOSContext());
	::HidePen();
	OSErr err = SetControlData(mControl, kControlEntireControl, kControlStaticTextTextTag, (int32) text.length(), (Ptr) ToPlatformStr(text));
	::ShowPen();
	ThrowIfOSErr(err);

	if (redraw)
		this->Invalidate();
}


//---------------------------------------------------------------
//
// XStaticTextControl::OnSetFont 
//
//---------------------------------------------------------------
void XStaticTextControl::OnSetFont(const XFont& font)
{
	mFont = font;
	
	this->DoSetStyle();
}


//---------------------------------------------------------------
//
// XStaticTextControl::OnSetAlignment 
//
//---------------------------------------------------------------
void XStaticTextControl::OnSetAlignment(ETextAlignment alignment)
{
	mAlignment = alignment;
	
	this->DoSetStyle();
}


//---------------------------------------------------------------
//
// XStaticTextControl::DoSetStyle 
//
// Appearance Manager 1.1 is exceedingly lame: if record.flags is
// set to kControlUseJustMask it will go ahead and reset the text's
// style and color attributes! So, to work around this we funnel
// all style changes through this function...
//
//---------------------------------------------------------------
void XStaticTextControl::DoSetStyle()
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

	::SetPort(this->GetOSContext());
	::HidePen();
	OSErr err = SetControlFontStyle(mControl, &record);
	::ShowPen();
	ThrowIfOSErr(err);
}


}	// namespace Whisper
