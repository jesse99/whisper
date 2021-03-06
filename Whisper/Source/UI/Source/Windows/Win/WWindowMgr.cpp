/*
 *  File:       WWindowMgr.cpp
 *  Summary:   	Window manager desktop functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WWindowMgr.cpp,v $
 *		Revision 1.9  2001/04/21 03:34:50  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.8  2001/02/21 08:17:29  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.7  2000/12/31 04:45:30  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.6  2000/12/27 01:11:08  jesjones
 *		Rewrote GetMyTopWindow to work better when dialogs are up.
 *		
 *		Revision 1.5  2000/12/14 08:32:29  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/12/10 04:08:29  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/26 03:34:07  jesjones
 *		Added a DoAddWindows method to XWindowIterator.
 *		
 *		Revision 1.2  2000/11/09 12:13:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 8/05/00	JDJ		Moved AutoPosition and SendToBack to IWindow. 
 *		 <2>	 5/11/99	JDJ		Implemented SendToBack.
 *		 <1>	10/12/98	JDJ		Created (from Raven)
 */
										
#include <XWhisperHeader.h>
#include <IWindowMgr.h>

#include <IFocus.h>
#include <IFrameWindow.h>
#include <IGeometry.h>
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XDesktop.h>
#include <XExceptions.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// MatchLayer
//
//---------------------------------------------------------------
class MatchLayer : public std::binary_function<IWindowPtr, EWindowLayer, bool> {
public:
	bool operator()(const IWindowPtr& window, EWindowLayer layer) const	{return window->GetLayer() == layer;}
};


//---------------------------------------------------------------
//
// GetMyTopWindow
//
//---------------------------------------------------------------
static HWND GetMyTopWindow()
{
	HWND windH = ::GetTopWindow(nil);

	// Loop thru all the top-level windows in the system,
	while (windH != nil) {
		IWindowPtr window = IWindow::GetWindow(windH);
		IFrameWindowPtr frame(window);
		
		// if we find an MDI frame window then get the first child
		// of the client window,
		if (frame) {
			HWND clientH = frame->GetClientWindow();	
			if (clientH != nil)
				windH = ::GetWindow(clientH, GW_CHILD);
				
		// if we've found one of our windows then we're done,
		} else if (window)
			break;
			
		// otherwise try the next window.
		else
			windH = ::GetNextWindow(windH, GW_HWNDNEXT);
	}
	
	return windH;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XWindowIter
// ===================================================================================

//---------------------------------------------------------------
//
// XWindowIter::XWindowIter ()
//
//---------------------------------------------------------------
XWindowIter::XWindowIter()
{
	mIter = mWindows.end();
}


//---------------------------------------------------------------
//
// XWindowIter::XWindowIter (bool, OSWindow)
//
//---------------------------------------------------------------
XWindowIter::XWindowIter(bool visibleOnly, OSWindow windH)
{
	if (windH == nil) 
		windH = GetMyTopWindow();
		
	if (windH != nil)
		this->DoAddWindows(visibleOnly, windH);
	
	mIter = mWindows.begin();
}


//---------------------------------------------------------------
//
// XWindowIter::XWindowIter (XWindowIter)
//
//---------------------------------------------------------------
XWindowIter::XWindowIter(const XWindowIter& rhs) : mWindows(rhs.mWindows)
{
	int32 offset = rhs.mIter - rhs.mWindows.begin();
	
	mIter = mWindows.begin() + offset;
}


//---------------------------------------------------------------
//
// XWindowIter::operator=
//
//---------------------------------------------------------------
XWindowIter& XWindowIter::operator=(const XWindowIter& rhs)
{
	if (this != &rhs) {
		mWindows = rhs.mWindows;

		int32 offset = rhs.mIter - rhs.mWindows.begin();
		mIter = mWindows.begin() + offset;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XWindowIter::DoAddWindows
//
//---------------------------------------------------------------
void XWindowIter::DoAddWindows(bool visibleOnly, OSWindow windH)
{
	PRECONDITION(windH != nil) ;
	
	while (windH != nil) {
		IWindowPtr window = IWindow::GetWindow(windH);
		IFrameWindowPtr frame(window);			

		if (((visibleOnly && ::IsWindowVisible(windH)) || !visibleOnly) && !frame) {
			if (window) 
				mWindows.push_back(window);
		}
		
		if (frame) {
			HWND clientH = frame->GetClientWindow();	// dialogs are siblings of the frame window
			if (clientH != nil)
				windH = ::GetWindow(clientH, GW_CHILD);
				
		} else
			windH = ::GetNextWindow(windH, GW_HWNDNEXT);
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XWindowMgr
//!		Window manager desktop functions.
// ===================================================================================
class XWindowMgr : public IWindowMgr {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual			   ~XWindowMgr();
	
						XWindowMgr(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual IWindowPtr GetTop(EWindowLayer layer) const;

	virtual IWindowPtr GetTopRegular() const;
	virtual IWindowPtr GetBottomRegular() const;
	
	virtual IWindowPtr GetTopFloater() const;
	virtual IWindowPtr GetBottomFloater() const;
	
	virtual IWindowPtr GetTopModal() const;
	virtual IWindowPtr GetBottomModal() const;

	virtual iterator 	begin(bool visibleOnly = true, OSWindow wptr = nil) const;
	virtual iterator 	end() const;

	virtual reverse_iterator rbegin() const;
	virtual reverse_iterator rend(bool visibleOnly = true, OSWindow wptr = nil) const;

#if DEBUG
	virtual void 		DumpWindows() const;
#endif
};

DEFINE_INTERFACE_FACTORY(XWindowMgr)

//---------------------------------------------------------------
//
// XWindowMgr::~XWindowMgr
//
//---------------------------------------------------------------
XWindowMgr::~XWindowMgr()
{
}


//---------------------------------------------------------------
//
// XWindowMgr::XWindowMgr
//
//---------------------------------------------------------------
XWindowMgr::XWindowMgr(XBoss* boss)
{
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XWindowMgr::DumpWindows
//
//---------------------------------------------------------------
#if DEBUG
void XWindowMgr::DumpWindows() const
{
	HWND windH = GetMyTopWindow();
	while (windH != nil) {
		std::wstring title;
		
		int32 length = GetWindowTextLength(windH);
		if (WSystemInfo::HasUnicode()) {
			XTinyVector<wchar_t> buffer(length+1UL);
			int32 count = GetWindowTextW(windH, buffer.buffer(), numeric_cast<int32>(buffer.size()));
			title = std::wstring(buffer.buffer(), numeric_cast<uint32>(count));
		
		} else {
			XTinyVector<char> buffer2(length+1UL);
			int32 count2 = GetWindowTextA(windH, buffer2.buffer(), numeric_cast<int32>(buffer2.size()));
			title = FromPlatformStr(buffer2.buffer(), numeric_cast<uint32>(count2));
		}
		
		bool visible = ::IsWindowVisible(windH) != 0;
			
		IConstWindowPtr wind = IWindow::GetWindow(windH);
		if (wind != nil) {			
			if (wind->GetType() == kDocumentWindow)
				TRACE(visible ? "Visible" : "Invisible", " regular window: ", title, "\n");

			else if (wind->GetType() == kFloatingWindow)
				TRACE(visible ? "Visible" : "Invisible", " floating window: ", title, "\n");

			else
				TRACE(visible ? "Visible" : "Invisible", " window: ", title, "\n");

		} else 
			TRACE(visible ? "Visible" : "Invisible", " OS window: ", title, "\n");

		windH = ::GetNextWindow(windH, GW_HWNDNEXT);
	}
	TRACE("\n");
}
#endif


//---------------------------------------------------------------
//
// XWindowMgr::GetTop
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetTop(EWindowLayer layer) const
{
	iterator iter = std::find_if(this->begin(), this->end(), std::bind2nd(MatchLayer(), layer));

	IWindowPtr wind;
	if (iter != this->end())
		wind = *iter;
			
	return wind;
}


//---------------------------------------------------------------
//
// XWindowMgr::GetTopRegular
//
// Returns the frontmost window that is visible, has a window
// object, and is in the regular layer.
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetTopRegular() const
{
	iterator iter = std::find_if(this->begin(), this->end(), std::bind2nd(MatchLayer(), kRegularLayer));

	IWindowPtr wind;
	if (iter != this->end())
		wind = *iter;
			
	return wind;
}


//---------------------------------------------------------------
//
// XWindowMgr::GetBottomRegular
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetBottomRegular() const
{
	reverse_iterator iter = std::find_if(this->rbegin(), this->rend(), std::bind2nd(MatchLayer(), kRegularLayer));

	IWindowPtr wind;
	if (iter != this->rend())
		wind = *iter;
			
	return wind;
}

	
//---------------------------------------------------------------
//
// XWindowMgr::GetTopFloater
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetTopFloater() const
{
	iterator iter = std::find_if(this->begin(), this->end(), std::bind2nd(MatchLayer(), kFloatingLayer));

	IWindowPtr wind;
	if (iter != this->end())
		wind = *iter;
			
	return wind;
}


//---------------------------------------------------------------
//
// XWindowMgr::GetBottomFloater
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetBottomFloater() const
{
	reverse_iterator iter = std::find_if(this->rbegin(), this->rend(), std::bind2nd(MatchLayer(), kFloatingLayer));

	IWindowPtr wind;
	if (iter != this->rend())
		wind = *iter;
			
	return wind;
}
	

//---------------------------------------------------------------
//
// XWindowMgr::GetTopModal
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetTopModal() const
{
	iterator iter = std::find_if(this->begin(), this->end(), std::bind2nd(MatchLayer(), kModalLayer));

	IWindowPtr wind;
	if (iter != this->end())
		wind = *iter;
			
	return wind;
}


//---------------------------------------------------------------
//
// XWindowMgr::GetBottomModal
//
//---------------------------------------------------------------
IWindowPtr XWindowMgr::GetBottomModal() const
{
	reverse_iterator iter = std::find_if(this->rbegin(), this->rend(), std::bind2nd(MatchLayer(), kModalLayer));

	IWindowPtr wind;
	if (iter != this->rend())
		wind = *iter;
			
	return wind;
}
	

//---------------------------------------------------------------
//
// XWindowMgr::begin
//
//---------------------------------------------------------------
XWindowMgr::iterator XWindowMgr::begin(bool visibleOnly, OSWindow wptr) const
{
	return iterator(visibleOnly, wptr);
}


//---------------------------------------------------------------
//
// XWindowMgr::end
//
//---------------------------------------------------------------
XWindowMgr::iterator XWindowMgr::end() const
{
	return iterator();
}


//---------------------------------------------------------------
//
// XWindowMgr::rbegin
//
//---------------------------------------------------------------
XWindowMgr::reverse_iterator XWindowMgr::rbegin() const
{
	return reverse_iterator(this->end());
}


//---------------------------------------------------------------
//
// XWindowMgr::rend
//
//---------------------------------------------------------------
XWindowMgr::reverse_iterator XWindowMgr::rend(bool visibleOnly, OSWindow wptr) const
{
	return reverse_iterator(this->begin(visibleOnly, wptr));
}


}	// namespace Whisper

