/*
 *  File:       XProgressBar.cpp
 *  Summary:   	A control that displays a system progress bar.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MProgressBar.cpp,v $
 *		Revision 1.4  2000/12/31 04:32:45  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:28:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:18:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/14/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IProgressBar.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IValueControl.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XProgressBar
//!		A control that displays a system progress bar.
// ===================================================================================
class XProgressBar : public IProgressBar {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XProgressBar();
	
						XProgressBar(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, int32 minValue, int32 maxValue,
							 uint32 style = kDefaultProgressBarStyle,
							 uint32 procID = kDefaultProgressBarProcID);

	virtual void 		MakeIndeterminate(bool indeterminate = true);
};

DEFINE_INTERFACE_FACTORY(XProgressBar)

//---------------------------------------------------------------
//
// XProgressBar::~XProgressBar
//
//---------------------------------------------------------------
XProgressBar::~XProgressBar()
{
}


//---------------------------------------------------------------
//
// XProgressBar::XProgressBar
//
//---------------------------------------------------------------
XProgressBar::XProgressBar(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XProgressBar::Init 
//
//---------------------------------------------------------------
void XProgressBar::Init(const IHierarchyPtr& parent, const std::wstring& name, 
						const XRect& frame, int32 minValue, int32 maxValue,
						uint32 style, uint32 procID)
{
	UNUSED(style);
	
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the min and max values
	IValueControlPtr value(this);
	value->SetMinMaxValue(minValue, maxValue, kDontRedraw);
}


//---------------------------------------------------------------
//
// XProgressBar::MakeIndeterminate
//
//---------------------------------------------------------------
void XProgressBar::MakeIndeterminate(bool indeterminate)
{
	IControlPtr control(this);

	Boolean value = indeterminate;
	OSErr err = SetControlData(control->GetOSControl(), kControlEntireControl, kControlProgressBarIndeterminateTag, sizeof(value), (char*) &value);
	ThrowIfOSErr(err);
}


}	// namespace Whisper
