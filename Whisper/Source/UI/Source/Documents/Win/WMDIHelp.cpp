/*
 *  File:       WMDIHelp.cpp
 *  Summary:   	IShowHelp that uses the MDI status window to display help messages.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMDIHelp.cpp,v $
 *		Revision 1.2  2000/12/14 08:30:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/11/27 03:24:45  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IShowHelp.h>

#include <IFrameWindow.h>

namespace Whisper {


// ===================================================================================
//	class WMDIHelp
//!		IShowHelp that uses the MDI status window to display help messages.
// ===================================================================================
class WMDIHelp : public IShowHelp {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WMDIHelp();
	
						WMDIHelp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Show(const std::wstring& message, const XPoint& where) const;
};

DEFINE_INTERFACE_FACTORY(WMDIHelp)

//---------------------------------------------------------------
//
// WMDIHelp::~WMDIHelp
//
//---------------------------------------------------------------
WMDIHelp::~WMDIHelp()
{
}


//---------------------------------------------------------------
//
// WMDIHelp::WMDIHelp
//
//---------------------------------------------------------------
WMDIHelp::WMDIHelp(XBoss* boss) 
{	
	IShowHelp::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// WMDIHelp::Show
//
//---------------------------------------------------------------
void WMDIHelp::Show(const std::wstring& message, const XPoint& where) const
{		
	UNUSED(where);
	
	IShowHelp* thisPtr = const_cast<WMDIHelp*>(this);
	
	IFrameWindowPtr frame(thisPtr);
	frame->SetStatusMesg(message);
}


}	// namespace Whisper
