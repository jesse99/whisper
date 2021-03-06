/*
 *  File:       WSeperator.cpp
 *  Summary:   	A control that draws a divider line.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WSeperator.cpp,v $
 *		Revision 1.1  2001/03/23 10:50:31  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <ISeperator.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <WSystemInfo.h>
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
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	if (frame.GetWidth() < frame.GetHeight())
		if ((style & SS_ETCHEDHORZ) == SS_ETCHEDHORZ)
			style = (style & ~SS_ETCHEDHORZ) | SS_ETCHEDVERT;
	
	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("STATIC"),		// class name
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
}


}	// namespace Whisper
