/*
 *  File:       WHTMLHelp.cpp
 *  Summary:   	IShowHelp that uses HtmlHelp.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WHTMLHelp.cpp,v $
 *		Revision 1.3  2001/04/27 09:18:58  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.2  2001/04/21 03:32:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/03/09 10:02:11  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IShowHelp.h>

#include <HtmlHelp.h>

#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class WHTMLHelp
//!		IShowHelp implementation that uses HtmlHelp.
// ===================================================================================
class WHTMLHelp : public IShowHelp {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WHTMLHelp();
	
						WHTMLHelp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Show(const std::wstring& message, const XPoint& where) const;
};

DEFINE_INTERFACE_FACTORY(WHTMLHelp)

//---------------------------------------------------------------
//
// WHTMLHelp::~WHTMLHelp
//
//---------------------------------------------------------------
WHTMLHelp::~WHTMLHelp()
{
}


//---------------------------------------------------------------
//
// WHTMLHelp::WHTMLHelp
//
//---------------------------------------------------------------
WHTMLHelp::WHTMLHelp(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// WHTMLHelp::Show
//
//---------------------------------------------------------------
void WHTMLHelp::Show(const std::wstring& message, const XPoint& where) const
{		
	PRECONDITION(message.length() > 0);
	
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	HH_POPUP info;
	info.cbStruct      = sizeof(info);
	info.hinst         = nil;
	info.idString      = nil;
	info.pt            = where;
	info.clrForeground = (uint32) -1;
	info.clrBackground = (uint32) -1;
	info.rcMargins     = XRect(-1, -1, -1, -1);
	info.pszFont       = nil;
	
	if (WSystemInfo::HasUnicode()) {
		info.pszText = (char*) message.c_str();
		VERIFY(HtmlHelpW(controlH,				// hwndCaller
						 nil,					// pszFile
						 HH_DISPLAY_TEXT_POPUP, // uCommand
						 (uint32) &info));		// dwData
	
	} else {
		PlatformString str = ToPlatformStr(message);
		info.pszText = (char*) str.c_str();
		VERIFY(HtmlHelpA(controlH,				// hwndCaller
						 nil,					// pszFile
						 HH_DISPLAY_TEXT_POPUP, // uCommand
						 (uint32) &info));		// dwData
	}
}


}	// namespace Whisper
