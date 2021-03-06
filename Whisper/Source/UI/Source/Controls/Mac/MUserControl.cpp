/*
 *  File:       MUserControl.cpp
 *  Summary:   	Control used with custom controls.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MUserControl.cpp,v $
 *		Revision 1.6  2001/04/13 07:53:53  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/03 23:57:56  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.4  2000/12/14 08:29:13  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:05:44  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:21:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IUserControl.h>

#include <IControl.h>
#include <IFocus.h>
#include <IHierarchy.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XUserControl
//!		Control used with custom controls.
// ===================================================================================
class XUserControl : public IUserControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XUserControl();
	
						XUserControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultUserStyle,
							 uint32 procID = kDefaultUserProcID);
	
//-----------------------------------
//	Internal API
//
protected:
	static pascal ControlPartCode DoFocus(ControlHandle controlH, ControlFocusPart action);
	
//-----------------------------------
//	Member Data
//
protected:	
	ControlUserPaneFocusUPP	mFocusUPP;
};

DEFINE_INTERFACE_FACTORY(XUserControl)

//---------------------------------------------------------------
//
// XUserControl::~XUserControl
//
//---------------------------------------------------------------
XUserControl::~XUserControl()
{
	if (mFocusUPP != nil) {
#if TARGET_CARBON
		DisposeControlUserPaneFocusUPP(mFocusUPP);
#else
		DisposeRoutineDescriptor(mFocusUPP);
#endif
	}
}


//---------------------------------------------------------------
//
// XUserControl::XUserControl
//
//---------------------------------------------------------------
XUserControl::XUserControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XUserControl::Init
//
//---------------------------------------------------------------
void XUserControl::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	mFocusUPP = nil;
	
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	uint32 features = kControlSupportsEmbedding | kControlWantsActivate;
	
	IFocusPtr focus(this);	
	if (focus)
		features |= kControlSupportsFocus + kControlGetsFocusOnClick;	
	
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, numeric_cast<int16>(features), 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);

	// Register the focus callback
	if (focus) {
		mFocusUPP = NewControlUserPaneFocusUPP(XUserControl::DoFocus);
		ThrowIfNil(mFocusUPP);
		
		OSErr err = SetControlData(controlH, kControlEntireControl, kControlUserPaneFocusProcTag, sizeof(mFocusUPP), (Ptr) &mFocusUPP);
		ThrowIfOSErr(err);
	}
}


//---------------------------------------------------------------
//
// XUserControl::DoFocus								[static]
//
//---------------------------------------------------------------
pascal ControlPartCode XUserControl::DoFocus(ControlHandle controlH, ControlFocusPart action)
{
	ControlPartCode code = kControlFocusNoPart;
	
	try {
		IControlPtr control = IControl::GetControl(controlH);
		IFocusPtr focus(control);	
			
		if (action != kControlFocusNoPart && focus)
			if (focus->WantsFocus())
				code = 1;
		
	} catch (...) {
		DEBUGSTR("Got an exception in XUserControl::DoFocus");	
	}
	
	return code; 
}


}	// namespace Whisper
