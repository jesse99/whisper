/*
 *  File:       WStaticText.cpp
 *  Summary:   	Interface for a control that displays non-editable text.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WStaticText.cpp,v $
 *		Revision 1.4  2001/03/05 05:40:18  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:32:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:12:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IStaticText.h>

#include <windowsx.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class WStaticText
//!		Interface for a control that displays non-editable text.
// ===================================================================================
class WStaticText : public IStaticText {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WStaticText();
	
						WStaticText(XBoss* boss);
						
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
	
//-----------------------------------
//	Member Data
//
private:
	XFont			mFont;
	ETextAlignment	mAlignment;
};

DEFINE_INTERFACE_FACTORY(WStaticText)

//---------------------------------------------------------------
//
// WStaticText::~WStaticText
//
//---------------------------------------------------------------
WStaticText::~WStaticText()
{
}


//---------------------------------------------------------------
//
// WStaticText::WStaticText
//
//---------------------------------------------------------------
WStaticText::WStaticText(XBoss* boss) : mFont(kCourierFont)
{	
	this->DoSetBoss(boss);
	
	mAlignment = kLeftTopAlign;
}


//---------------------------------------------------------------
//
// WStaticText::Init
//
//---------------------------------------------------------------
void WStaticText::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& inText, const XFont& font,
					   uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("STATIC"),		// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 super->GetOSControl(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the font		
	this->SetFont(font, kDontRedraw);
	
	// Set the control's text		
	ITextPtr text(this);
	text->SetText(inText, kDontRedraw);
}


//---------------------------------------------------------------
//
// WStaticText::SetFont
//
//---------------------------------------------------------------
void WStaticText::SetFont(const XFont& font, bool redraw)
{
	if (font != mFont) {
		mFont = font;
	
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();
		SetWindowFont(controlH, font.GetFont(), kDontRedraw);
		
		if (redraw)
			control->Invalidate();
	}
}


//---------------------------------------------------------------
//
// WStaticText::SetAlignment
//
//---------------------------------------------------------------
void WStaticText::SetAlignment(ETextAlignment alignment, bool redraw)
{	
	if (alignment != mAlignment) {
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();

		int32 style = ::GetWindowLong(controlH, GWL_STYLE);
		ThrowIf(style == 0);
		
		style &= ~(SS_LEFT | SS_CENTER | SS_RIGHT);
	
		switch (alignment) {
			case kLeftTopAlign:
				style = style | SS_LEFT;		// get "expression has no side effect" warnings if |= is used with CW 5.3
				break;
				
			case kCenterTopAlign:
				style = style | SS_CENTER;
				break;
	
			case kRightTopAlign:
				style = style | SS_RIGHT;
				break;
	
			case kDefaultAlign:
				style = style | SS_LEFT;		// $$$ is there a way to find out if we're running on a right-justified system?
				break;
	
			default:
				style = style | SS_LEFT;
				DEBUGSTR("Unsupported alignment flag!");
		}			
		
		if (!redraw)
			VERIFY(::LockWindowUpdate(controlH));	
	
		::SetLastError(0);
		int32 oldValue = ::SetWindowLong(controlH, GWL_STYLE, style);
		uint32 err = GetLastError();
	
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
		ThrowIf(oldValue == 0 && err != noErr);
	
		mAlignment = alignment;
	}
}


}	// namespace Whisper
