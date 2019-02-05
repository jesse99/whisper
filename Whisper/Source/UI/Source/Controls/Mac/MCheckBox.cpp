/*
 *  File:       MCheckBox.cpp
 *  Summary:   	System checkbox control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MCheckBox.cpp,v $
 *		Revision 1.5  2001/04/01 04:53:42  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.4  2001/02/04 03:35:14  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.3  2000/12/31 04:25:06  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.2  2000/12/14 08:26:47  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:56  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ICheckBox.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IHierarchy.h>
#include <IText.h>
#include <IValueControl.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XCheckBox
//!		System checkbox control.
// ===================================================================================
class XCheckBox : public ICheckBox, public IControlPressedHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCheckBox();
	
						XCheckBox(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultCheckBoxStyle,
							 uint32 procID = kDefaultCheckBoxProcID);
	
	virtual bool		IsChecked() const;

	virtual bool		IsIndeterminate() const;

protected:
	virtual void 		OnPressed(const XMouseDownEvent& event);
};

DEFINE_INTERFACE_FACTORY(XCheckBox)

//---------------------------------------------------------------
//
// XCheckBox::~XCheckBox
//
//---------------------------------------------------------------
XCheckBox::~XCheckBox()
{
}


//---------------------------------------------------------------
//
// XCheckBox::XCheckBox
//
//---------------------------------------------------------------
XCheckBox::XCheckBox(XBoss* boss)
{	
	ICheckBox::DoSetBoss(boss);
	IControlPressedHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XCheckBox::Init 
//
//---------------------------------------------------------------
void XCheckBox::Init(const IHierarchyPtr& parent, const std::wstring& name, 
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
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 2, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	ICheckBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the control's title		
	ITextPtr text(thisPtr);
	text->SetText(title, kDontRedraw);
}


//---------------------------------------------------------------
//
// XCheckBox::IsChecked
//
//---------------------------------------------------------------
bool XCheckBox::IsChecked() const
{
	const ICheckBox* thisPtr = this;
	IConstValueControlPtr value(thisPtr);
	int32 val = value->GetValue();
	
	return val > 0;
}


//---------------------------------------------------------------
//
// XCheckBox::IsIndeterminate
//
//---------------------------------------------------------------
bool XCheckBox::IsIndeterminate() const
{
	const ICheckBox* thisPtr = this;
	IConstValueControlPtr value(thisPtr);
	int32 val = value->GetValue();
	
	return val == 2;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XCheckBox::OnPressed
//
//---------------------------------------------------------------
void XCheckBox::OnPressed(const XMouseDownEvent& event)
{
	UNUSED(event);
	
	ICheckBox* thisPtr = this;
	IValueControlPtr value(thisPtr);
	int32 val = value->GetValue();
	ASSERT(val >= 0);
	ASSERT(val <= 2);
	
	if (val == 0 || val == 2)
		val = 1;
	else
		val = 0;
		
	value->SetValue(val, kRedraw);
}


}	// namespace Whisper
