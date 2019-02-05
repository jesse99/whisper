/*
 *  File:       XControlHelp.cpp
 *  Summary:   	IHelpText implementation used with generic controls (also see XButtonHelpText).
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XControlHelp.cpp,v $
 *		Revision 1.4  2001/03/09 10:20:34  jesjones
 *		GetMessage takes an XPoint instead of an XMouseMovedEvent.
 *		
 *		Revision 1.3  2000/12/14 08:33:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:29:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IHelpText.h>
#include <ISetDisabledHelp.h>
#include <ISetHelp.h>

#include <IControl.h>

namespace Whisper {


// ===================================================================================
//	class XControlHelp
//!		IHelpText implementation used with generic controls (also see XButtonHelpText).
// ===================================================================================
class XControlHelp : public IHelpText, public ISetHelp, public ISetDisabledHelp {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XControlHelp();
	
						XControlHelp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual std::wstring GetMessage(const XPoint& localPt) const;

	virtual void 		SetDefaultHelp(const std::wstring& text);

	virtual void 		SetDisabledHelp(const std::wstring& text);

//-----------------------------------
//	Member Data
//
private:
	std::wstring		mEnabled;
	std::wstring		mDisabled;
};

DEFINE_INTERFACE_FACTORY(XControlHelp)

//---------------------------------------------------------------
//
// XControlHelp::~XControlHelp
//
//---------------------------------------------------------------
XControlHelp::~XControlHelp()
{
}


//---------------------------------------------------------------
//
// XControlHelp::XControlHelp
//
//---------------------------------------------------------------
XControlHelp::XControlHelp(XBoss* boss) 
{	
	IHelpText::DoSetBoss(boss);
	ISetHelp::DoSetBoss(boss);
	ISetDisabledHelp::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XControlHelp::GetMessage
//
//---------------------------------------------------------------
std::wstring XControlHelp::GetMessage(const XPoint& localPt) const
{
	UNUSED(localPt);
	
	std::wstring text;
	
	const IHelpText* thisPtr = this;
	IConstControlPtr control(thisPtr);
	if (!control->IsEnabled())
		text = mDisabled;
		
	if (text.length() == 0)
		text = mEnabled;
		
	return text;
}


//---------------------------------------------------------------
//
// XControlHelp::SetDefaultHelp
//
//---------------------------------------------------------------
void XControlHelp::SetDefaultHelp(const std::wstring& text)
{
	mEnabled  = text;
}


//---------------------------------------------------------------
//
// XControlHelp::SetDisabledHelp
//
//---------------------------------------------------------------
void XControlHelp::SetDisabledHelp(const std::wstring& text)
{
	mDisabled = text;
}


}	// namespace Whisper
