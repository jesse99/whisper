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
 *		$Log: WProgressBar.cpp,v $
 *		Revision 1.4  2000/12/31 04:44:10  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:31:24  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:11:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/15/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IProgressBar.h>

#include <commctrl.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IValueControl.h>
#include <WSystemInfo.h>
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
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 PROGRESS_CLASS,		// class name
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
	UNUSED(indeterminate);
	
	// only implemented on Mac
}


}	// namespace Whisper
