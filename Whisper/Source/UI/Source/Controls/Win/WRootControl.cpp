/*
 *  File:       XRootControl.cpp
 *  Summary:   	The topmost control in a window.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WRootControl.cpp,v $
 *		Revision 1.5  2000/12/31 04:44:48  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:32:00  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/13 00:53:11  jesjones
 *		Uses Bind instead of Adapt.
 *		
 *		Revision 1.2  2000/11/09 12:11:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/15/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IRootControl.h>

#include <IBinding.h>
#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XBind.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// MatchName
//
//---------------------------------------------------------------
static bool MatchName(const IHierarchyPtr& node, const std::wstring& name)
{
	IControlPtr control(node);
	bool matches = control && control->GetName() == name;
	
	return matches;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRootControl
//!		The topmost control in a window.
// ===================================================================================
class XRootControl : public IRootControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XRootControl();
	
						XRootControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name);

	virtual IControlPtr FindChild(const std::wstring& name) const;
};

DEFINE_INTERFACE_FACTORY(XRootControl)

//---------------------------------------------------------------
//
// XRootControl::~XRootControl
//
//---------------------------------------------------------------
XRootControl::~XRootControl()
{
}


//---------------------------------------------------------------
//
// XRootControl::XRootControl
//
//---------------------------------------------------------------
XRootControl::XRootControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XRootControl::Init
//
//---------------------------------------------------------------
void XRootControl::Init(const IHierarchyPtr& parent, const std::wstring& name)
{		
	IWindowPtr window = GetInterface<IWindow>(parent);
	IGeometryPtr geometry(window);
	XRect frame = geometry->GetExtent();

	uint32 style = WS_CHILD + WS_VISIBLE + SS_NOTIFY;
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("STATIC"),		// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 window->GetOSWindow(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
	ThrowIfNil(controlH);

	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	IBindingPtr binding(this);
	binding->SetBinding(SBinding(true, true, true, true));
}


//---------------------------------------------------------------
//
// XRootControl::FindChild
//
//---------------------------------------------------------------
IControlPtr XRootControl::FindChild(const std::wstring& name) const
{
	IConstHierarchyPtr node(this);
	
	XCallback2<bool, IHierarchyPtr, std::wstring> function(MatchName);
	XCallback1<bool, IHierarchyPtr> predicate = Bind1(function, kUnbound, name);	
	IHierarchyPtr child = node->FindChild(predicate);
		
	return child;
}


}	// namespace Whisper
