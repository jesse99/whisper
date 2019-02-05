/*
 *  File:       MGroupBox.cpp
 *  Summary:   	System group box control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MGroupBox.cpp,v $
 *		Revision 1.5  2001/03/20 12:02:49  jesjones
 *		Fixed a comment.
 *		
 *		Revision 1.4  2000/12/31 04:27:29  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:27:52  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/12/10 04:04:02  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.1  2000/12/05 09:02:04  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IGroupBox.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XGroupBox
//!		System group box control.
// ===================================================================================
class XGroupBox : public IGroupBox {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XGroupBox();
	
						XGroupBox(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultGroupBoxStyle,
							 uint32 procID = kDefaultGroupBoxProcID);
};

DEFINE_INTERFACE_FACTORY(XGroupBox)

//---------------------------------------------------------------
//
// XGroupBox::~XGroupBox
//
//---------------------------------------------------------------
XGroupBox::~XGroupBox()
{
}


//---------------------------------------------------------------
//
// XGroupBox::XGroupBox
//
//---------------------------------------------------------------
XGroupBox::XGroupBox(XBoss* boss) 
{	
	IGroupBox::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XGroupBox::Init 
//
//---------------------------------------------------------------
void XGroupBox::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& title, 
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
	uint32 features = kControlSupportsEmbedding + kControlWantsActivate;	
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, numeric_cast<int16>(features), 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the control's title		
	ITextPtr text(this);
	text->SetText(title, kDontRedraw);
}


}	// namespace Whisper
