/*
 *  File:       XControlText.cpp
 *  Summary:   	IText implementation that uses the control title.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MControlText.cpp,v $
 *		Revision 1.6  2001/04/27 04:19:23  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.5  2001/03/26 04:17:05  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.4  2000/12/31 04:26:32  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:27:13  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:11:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IText.h>

#include <IControl.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XControlText
//!		IText implementation that uses the control title.
// ===================================================================================
class XControlText : public IText {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControlText();
	
						XControlText(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		IsEmpty() const;
	virtual std::wstring GetText() const;
	virtual void 		SetText(const std::wstring& text, bool redraw = kRedraw);
};

DEFINE_INTERFACE_FACTORY(XControlText)

//---------------------------------------------------------------
//
// XControlText::~XControlText
//
//---------------------------------------------------------------
XControlText::~XControlText()
{
}


//---------------------------------------------------------------
//
// XControlText::XControlText
//
//---------------------------------------------------------------
XControlText::XControlText(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XControlText::IsEmpty
//
//---------------------------------------------------------------
bool XControlText::IsEmpty() const
{
	IConstControlPtr control(this);
	OSControl controlH = control->GetOSControl();

	Str255 title;	
	::GetControlTitle(controlH, title);
	
	return title[0] == 0;
}


//---------------------------------------------------------------
//
// XControlText::GetText
//
//---------------------------------------------------------------
std::wstring XControlText::GetText() const
{
	IConstControlPtr control(this);
	OSControl controlH = control->GetOSControl();

	Str255 title;	
	::GetControlTitle(controlH, title);
	
	return FromPascalStr(title);
}


//---------------------------------------------------------------
//
// XControlText::SetText
//
//---------------------------------------------------------------
void XControlText::SetText(const std::wstring& text, bool redraw)
{
	std::wstring newText = Whisper::Replace(text, '&', '\0', '&');	// delete single ampersands and replace double ampersands with one ampersand

	if (newText != this->GetText()) {
		IControlPtr control(this);
		OSControl controlH = control->GetOSControl();
		GrafPtr port = control->GetOSDrawContext();
		
		::SetPort(port);
		::HidePen();
		SetControlTitle(controlH, ToPascalStr(newText).c_str());
		::ShowPen();
		
		if (redraw)
			control->Invalidate();
	}
}


}	// namespace Whisper
