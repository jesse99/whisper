/*
 *  File:       MSeperator.cpp
 *  Summary:   	A control that draws a divider line.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MSeperator.cpp,v $
 *		Revision 1.1  2001/03/23 10:50:27  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <ISeperator.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XSeperator
//!		A control that draws a divider line.
// ===================================================================================
class XSeperator : public ISeperator {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XSeperator();
	
						XSeperator(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, uint32 style = kDefaultSeperatorStyle,
							 uint32 procID = kDefaultSeperatorProcID);
};

DEFINE_INTERFACE_FACTORY(XSeperator)

//---------------------------------------------------------------
//
// XSeperator::~XSeperator
//
//---------------------------------------------------------------
XSeperator::~XSeperator()
{
}


//---------------------------------------------------------------
//
// XSeperator::XSeperator
//
//---------------------------------------------------------------
XSeperator::XSeperator(XBoss* boss) 
{	
	ISeperator::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XSeperator::Init 
//
//---------------------------------------------------------------
void XSeperator::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, uint32 style, uint32 procID)
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
}


}	// namespace Whisper
