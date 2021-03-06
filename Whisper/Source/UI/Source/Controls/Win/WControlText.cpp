/*
 *  File:       WControlText.cpp
 *  Summary:   	IText implementation that uses the control title.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WControlText.cpp,v $
 *		Revision 1.7  2001/04/27 09:18:04  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.6  2001/03/26 04:17:23  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.5  2000/12/31 04:41:32  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:30:29  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:06:33  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:05:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IText.h>

#include <IControl.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

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
	HWND controlH = control->GetOSControl();
	
	int32 length = GetWindowTextLength(controlH);
	
	return length == 0;
}


//---------------------------------------------------------------
//
// XControlText::GetText
//
//---------------------------------------------------------------
std::wstring XControlText::GetText() const
{
	std::wstring text;
	
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	int32 length = ::GetWindowTextLength(controlH);
	if (WSystemInfo::HasUnicode()) {
		XTinyVector<wchar_t> buffer(length+1UL);
		int32 count = GetWindowTextW(controlH, buffer.buffer(), (int32) buffer.size());
		text = std::wstring(buffer.buffer(), numeric_cast<uint32>(count));
	
	} else {
		XTinyVector<char> buffer2(length+1UL);
		int32 count2 = GetWindowTextA(controlH, buffer2.buffer(), (int32) buffer2.size());
		text = FromPlatformStr(buffer2.buffer(), numeric_cast<uint32>(count2));
	}

	return text;
}


//---------------------------------------------------------------
//
// XControlText::SetText
//
//---------------------------------------------------------------
void XControlText::SetText(const std::wstring& text, bool redraw)
{
	if (text != this->GetText()) {
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();

		if (!redraw)
			VERIFY(::LockWindowUpdate(controlH));	
	
		int32 succeeded;
		if (WSystemInfo::HasUnicode())
			succeeded = ::SetWindowTextW(controlH, text.c_str());
		else
			succeeded = ::SetWindowTextA(controlH, ToPlatformStr(text).c_str());
	
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
		ThrowIf(!succeeded);
	}
}


}	// namespace Whisper
