/*
 *  File:       XButtonHelp.cpp
 *  Summary:   	IHelpText implementation used with buttons.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XButtonHelp.cpp,v $
 *		Revision 1.4  2001/03/09 10:20:26  jesjones
 *		GetMessage takes an XPoint instead of an XMouseMovedEvent.
 *		
 *		Revision 1.3  2000/12/14 08:32:57  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:27:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IHelpText.h>
#include <ISetCheckedHelp.h>
#include <ISetDisabledHelp.h>
#include <ISetHelp.h>

#include <IControl.h>
#include <IValueControl.h>

namespace Whisper {


// ===================================================================================
//	class XButtonHelp
//!		IHelpText implementation used with buttons.
// ===================================================================================
class XButtonHelp : public IHelpText, public ISetHelp, public ISetDisabledHelp, public ISetCheckedHelp {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XButtonHelp();
	
						XButtonHelp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual std::wstring GetMessage(const XPoint& localPt) const;

	virtual void 		SetDefaultHelp(const std::wstring& text);

	virtual void 		SetDisabledHelp(const std::wstring& text);

	virtual void 		SetCheckedHelp(const std::wstring& text);

//-----------------------------------
//	Member Data
//
private:
	std::wstring		mEnabled;
	std::wstring		mDisabled;
	std::wstring		mChecked;
};

DEFINE_INTERFACE_FACTORY(XButtonHelp)

//---------------------------------------------------------------
//
// XButtonHelp::~XButtonHelp
//
//---------------------------------------------------------------
XButtonHelp::~XButtonHelp()
{
}


//---------------------------------------------------------------
//
// XButtonHelp::XButtonHelp
//
//---------------------------------------------------------------
XButtonHelp::XButtonHelp(XBoss* boss) 
{	
	IHelpText::DoSetBoss(boss);
	ISetHelp::DoSetBoss(boss);
	ISetDisabledHelp::DoSetBoss(boss);
	ISetCheckedHelp::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XButtonHelp::GetMessage
//
//---------------------------------------------------------------
std::wstring XButtonHelp::GetMessage(const XPoint& localPt) const
{
	UNUSED(localPt);
	
	std::wstring text;
	
	const IHelpText* thisPtr = this;
	IConstControlPtr control(thisPtr);
	if (!control->IsEnabled())
		text = mDisabled;
		
	IConstValueControlPtr value(thisPtr);
	if (text.length() == 0 && value->GetValue() == 1)
		text = mChecked;

	if (text.length() == 0)
		text = mEnabled;
		
	return text;
}


//---------------------------------------------------------------
//
// XButtonHelp::SetDefaultHelp
//
//---------------------------------------------------------------
void XButtonHelp::SetDefaultHelp(const std::wstring& text)
{
	mEnabled  = text;
}


//---------------------------------------------------------------
//
// XButtonHelp::SetDisabledHelp
//
//---------------------------------------------------------------
void XButtonHelp::SetDisabledHelp(const std::wstring& text)
{
	mDisabled = text;
}


//---------------------------------------------------------------
//
// XButtonHelp::SetCheckedHelp
//
//---------------------------------------------------------------
void XButtonHelp::SetCheckedHelp(const std::wstring& text)
{
	mChecked  = text;
}


}	// namespace Whisper
