/*
 *  File:       MRootControl.cpp
 *  Summary:   	The topmost control in a window.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MRootControl.cpp,v $
 *		Revision 1.7  2001/04/27 09:13:14  jesjones
 *		Uses a timer instead of an idler.
 *		
 *		Revision 1.6  2001/04/21 03:28:23  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2000/12/31 04:33:30  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:28:42  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/12 07:54:46  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 09:19:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IRootControl.h>

#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <ITimer.h>
#include <IWindow.h>
#include <MSystemInfo.h>
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

	virtual void 		AddedChild(const IControlPtr& child);
	
	virtual void 		RemovedChild(const IControlPtr& child);
	
//-----------------------------------
//	Member Data
//
private:
			void 		DoIdle();
						
//-----------------------------------
//	Member Data
//
private:
	uint32	mIdleControls;		// count of child controls that want idle time
};

DEFINE_INTERFACE_FACTORY(XRootControl)

//---------------------------------------------------------------
//
// XRootControl::~XRootControl
//
//---------------------------------------------------------------
XRootControl::~XRootControl()
{
	PRECONDITION(mIdleControls == 0);
}


//---------------------------------------------------------------
//
// XRootControl::XRootControl
//
//---------------------------------------------------------------
XRootControl::XRootControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);

	mIdleControls = 0;
}


//---------------------------------------------------------------
//
// XRootControl::Init
//
//---------------------------------------------------------------
void XRootControl::Init(const IHierarchyPtr& parent, const std::wstring& name)
{	
	IWindowPtr window = GetInterface<IWindow>(parent);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = window->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	ControlHandle controlH = nil;
	OSErr err = ::CreateRootControl(window->GetOSWindow(), &controlH);
	
	::ShowPen();
	ThrowIfOSErr(err);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
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


//---------------------------------------------------------------
//
// XRootControl::AddedChild
//
//---------------------------------------------------------------
void XRootControl::AddedChild(const IControlPtr& child)
{
	PRECONDITION(mIdleControls >= 0);

	uint32 features = 0;
	OSErr err = ::GetControlFeatures(child->GetOSControl(), &features);
	if (err == noErr) {
		if ((features & kControlWantsIdle) != 0) {
			if (++mIdleControls == 1) {
				ITimerPtr timer(L"Application");
				
				XCallback0<void> callback(this, &XRootControl::DoIdle);
				timer->AddTimer(callback, 200);
			}
		}
		
	} else
		DEBUGSTR("Got a ", err, " error calling GetControlFeatures!");
}

	
//---------------------------------------------------------------
//
// XRootControl::RemovedChild
//
//---------------------------------------------------------------
void XRootControl::RemovedChild(const IControlPtr& child)
{
	PRECONDITION(mIdleControls >= 0);
	
	uint32 features = 0;
	OSErr err = ::GetControlFeatures(child->GetOSControl(), &features);
	if (err == noErr) {
		if ((features & kControlWantsIdle) != 0) {
			if (--mIdleControls == 0) {
				ITimerPtr timer(L"Application");
				
				XCallback0<void> callback(this, &XRootControl::DoIdle);
				timer->RemoveTimer(callback);
			}
		}

	} else
		DEBUGSTR("Got a ", err, " error calling GetControlFeatures!");
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XRootControl::DoIdle
//
//---------------------------------------------------------------
void XRootControl::DoIdle()
{
	IControlPtr control(this);
	
	::SetPort(control->GetOSDrawContext());
	::IdleControls(control->GetOSWindow());
}


}	// namespace Whisper
