/*
 *  File:       XDefaultHelp.cpp
 *  Summary:   	Basic IHelpText implementation.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDefaultHelp.cpp,v $
 *		Revision 1.4  2001/03/09 10:18:38  jesjones
 *		GetMessage takes an XPoint instead of an XMouseMovedEvent.
 *		
 *		Revision 1.3  2000/12/14 08:34:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:31:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IHelpText.h>
#include <ISetHelp.h>

#include <IControl.h>

namespace Whisper {


// ===================================================================================
//	class XDefaultHelp
//!		Basic IHelpText implementation.
// ===================================================================================
class XDefaultHelp : public IHelpText, public ISetHelp {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDefaultHelp();
	
						XDefaultHelp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual std::wstring GetMessage(const XPoint& localPt) const;

	virtual void 		SetDefaultHelp(const std::wstring& text);

//-----------------------------------
//	Member Data
//
private:
	std::wstring		mText;
};

DEFINE_INTERFACE_FACTORY(XDefaultHelp)

//---------------------------------------------------------------
//
// XDefaultHelp::~XDefaultHelp
//
//---------------------------------------------------------------
XDefaultHelp::~XDefaultHelp()
{
}


//---------------------------------------------------------------
//
// XDefaultHelp::XDefaultHelp
//
//---------------------------------------------------------------
XDefaultHelp::XDefaultHelp(XBoss* boss) 
{	
	IHelpText::DoSetBoss(boss);
	ISetHelp::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDefaultHelp::GetMessage
//
//---------------------------------------------------------------
std::wstring XDefaultHelp::GetMessage(const XPoint& localPt) const
{
	UNUSED(localPt);
			
	return mText;
}


//---------------------------------------------------------------
//
// XDefaultHelp::SetDefaultHelp
//
//---------------------------------------------------------------
void XDefaultHelp::SetDefaultHelp(const std::wstring& text)
{
	mText = text;
}


}	// namespace Whisper
