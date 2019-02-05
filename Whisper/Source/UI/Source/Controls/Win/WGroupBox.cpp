/*
 *  File:       WGroupBox.cpp
 *  Summary:   	System group box control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WGroupBox.cpp,v $
 *		Revision 1.4  2001/03/23 10:58:53  jesjones
 *		Added a comment.
 *		
 *		Revision 1.3  2001/03/20 12:02:53  jesjones
 *		Fixed a comment.
 *		
 *		Revision 1.2  2000/12/31 04:41:44  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.1  2000/12/08 08:05:18  jesjones
 *		Checked in.
 */

#include <XWhisperHeader.h>
#include <IGroupBox.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IText.h>
#include <WSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XGroupBox
//!		A control that displays a labeled frame around it's children.
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
	IControlPtr control(this);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the control's title		
	ITextPtr text(this);
	text->SetText(title, kDontRedraw);
}


}	// namespace Whisper
