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
 *		$Log: MGroupControl.cpp,v $
 *		Revision 1.5  2000/12/31 04:27:43  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.4  2000/12/14 08:27:56  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:04:05  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:14:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/12/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IGroupControl.h>

#include <IControl.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IWindow.h>
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
	uint32 features = kControlSupportsEmbedding + kControlWantsActivate;	
	ControlHandle controlH = NewControl(super->GetOSWindow(), &temp, "\p", true, numeric_cast<int16>(features), 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
}


}	// namespace Whisper
