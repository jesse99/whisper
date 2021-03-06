/*
 *  File:       WCheckBox.cpp
 *  Summary:   	System checkbox control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WCheckBox.cpp,v $
 *		Revision 1.4  2001/02/21 08:15:36  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/31 04:40:34  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.2  2000/12/14 08:30:02  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/07 09:49:05  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <ICheckBox.h>

#include <windowsx.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IHierarchy.h>
#include <IText.h>
#include <WSystemInfo.h>
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
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("BUTTON"),		// class name
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
	IConstControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	int32 value = Button_GetCheck(controlH);
	bool checked = value != BST_UNCHECKED;

	return checked;
}


//---------------------------------------------------------------
//
// XCheckBox::IsIndeterminate
//
//---------------------------------------------------------------
bool XCheckBox::IsIndeterminate() const
{
	const ICheckBox* thisPtr = this;
	IConstControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	int32 value = Button_GetCheck(controlH);
	bool indeterminate = value == BST_INDETERMINATE;

	return indeterminate;
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
	IControlPtr control(thisPtr);
	HWND controlH = control->GetOSControl();
	
	int32 value = Button_GetCheck(controlH);
	if (value == BST_UNCHECKED || value == BST_INDETERMINATE)
		value = BST_CHECKED;
	else
		value = BST_UNCHECKED;
		
	Button_SetCheck(controlH, value);
}


}	// namespace Whisper
