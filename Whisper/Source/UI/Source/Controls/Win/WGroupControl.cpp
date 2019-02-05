/*
 *  File:       XGroupControl.cpp
 *  Summary:   	A control into which other controls can be nested.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WGroupControl.cpp,v $
 *		Revision 1.4  2000/12/31 04:41:55  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:30:52  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:08:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/15/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IGroupControl.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <WSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XGroupControl
//!		A control into which other controls can be nested.
// ===================================================================================
class XGroupControl : public IGroupControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XGroupControl();
	
						XGroupControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultGroupStyle, 
							 uint32 procID = kDefaultGroupProcID);
};

DEFINE_INTERFACE_FACTORY(XGroupControl)

//---------------------------------------------------------------
//
// XGroupControl::~XGroupControl
//
//---------------------------------------------------------------
XGroupControl::~XGroupControl()
{
}


//---------------------------------------------------------------
//
// XGroupControl::XGroupControl
//
//---------------------------------------------------------------
XGroupControl::XGroupControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XGroupControl::Init
//
//---------------------------------------------------------------
void XGroupControl::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

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

	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
}


}	// namespace Whisper
