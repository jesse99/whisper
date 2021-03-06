/*
 *  File:       XWindowMgr.cpp
 *  Summary:   	Window manager desktop functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWindowMgr.cpp,v $
 *		Revision 1.5  2001/01/21 00:37:03  jesjones
 *		Hooked up MDeactivateDesktop.
 *		
 *		Revision 1.4  2000/12/31 04:36:37  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:29:30  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:22:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/05/00	JDJ		Moved AutoPosition and SendToBack to IWindow. 
 *		 <3>	 7/20/99	JDJ		Rewrote for Carbon.
 *		 <2>	 3/31/99	JDJ		Implemented Suspend and Resume.
 *		 <1>	10/12/98	JDJ		Created (from Raven)
 */
										
#include <XWhisperHeader.h>
#include <IWindowMgr.h>

#include <IGeometry.h>
#include <IWindow.h>
#include <MDialogUtils.h>
#include <MSystemInfo.h>
#include <XDesktop.h>

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

#pragma mark -

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
XWindowIter::XWindowIter(bool visibleOnly, OSWindow wptr)
{	
#if TARGET_CARBON
	if (wptr == nil) 
		wptr = ::GetFrontWindowOfClass(kAllWindowClasses, visibleOnly);
	
	while (wptr != nil) {
		IWindowPtr wind = IWindow::GetWindow(wptr);
		if (wind)
			mWindows.push_back(wind);

		wptr = ::GetNextWindowOfClass(wptr, kAllWindowClasses, visibleOnly);
	}

#else
	if (wptr == nil) {
		if (visibleOnly)
			wptr = ::FrontWindow();
		else
			wptr = ::LMGetWindowList();
	}
	
	while (wptr != nil) {
		if ((visibleOnly && IsWindowVisible(wptr)) || !visibleOnly) {
			IWindowPtr wind = IWindow::GetWindow(wptr);
			if (wind)
				mWindows.push_back(wind);
		}
		
		wptr = GetNextWindow(wptr);
	}
#endif

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

#pragma mark -

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

	virtual void 		Suspend();
	virtual void 		Resume();

#if DEBUG
	virtual void 		DumpWindows() const;
#endif

//-----------------------------------
//	Internal API
//
private:
	static	void 		DoActivate();
	static	void 		DoDeactivate();
};

DEFINE_INTERFACE_FACTORY(XWindowMgr)

//---------------------------------------------------------------
//
// XWindowMgr::~XWindowMgr
//
//---------------------------------------------------------------
XWindowMgr::~XWindowMgr()
{	
	MDeativateDesktop::SetActivateProc(nil);
	MDeativateDesktop::SetDeactivateProc(nil);
}


//---------------------------------------------------------------
//
// XWindowMgr::XWindowMgr
//
//---------------------------------------------------------------
XWindowMgr::XWindowMgr(XBoss* boss)
{
	IWindowMgr::DoSetBoss(boss);

	MDeativateDesktop::SetActivateProc(XWindowMgr::DoActivate);
	MDeativateDesktop::SetDeactivateProc(XWindowMgr::DoDeactivate);
}


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


//---------------------------------------------------------------
//
// XWindowMgr::Suspend
//
//---------------------------------------------------------------
void XWindowMgr::Suspend()
{		
	IWindowPtr wind;

	iterator iter = this->begin(false);
	while (iter != this->end()) {
		wind = *iter;
		++iter;
		
		wind->HandleSuspend();
	}
	
	wind = this->GetTopModal();
	if (!wind)
		wind = this->GetTopRegular();

	if (wind)
		wind->HandleDeactivate();
}


//---------------------------------------------------------------
//
// XWindowMgr::Resume
//
//---------------------------------------------------------------
void XWindowMgr::Resume()
{		
	IWindowPtr wind;

	iterator iter = this->begin(false);
	while (iter != this->end()) {
		wind = *iter;
		++iter;
		
		wind->HandleResume();
	}
	
	wind = this->GetTopModal();
	if (!wind)
		wind = this->GetTopRegular();

	if (wind)
		wind->HandleActivate();
}


//---------------------------------------------------------------
//
// XWindowMgr::DumpWindows
//
//---------------------------------------------------------------
#if DEBUG
void XWindowMgr::DumpWindows() const
{
	XWindowIter iter = this->begin(false);
	while (iter != this->end()) {
		IWindowPtr wind = *iter;
		++iter;
		
		if (wind->GetLayer() == kModalLayer)
			TRACE(wind->IsVisible() ? "Visible" : "Invisible", " modal window: '", wind->GetName(), "'\n");

		else if (wind->GetLayer() == kFloatingLayer)
			TRACE(wind->IsVisible() ? "Visible" : "Invisible", " floating window: '", wind->GetName(), "'\n");

		else
			TRACE(wind->IsVisible() ? "Visible" : "Invisible", " regular window: '", wind->GetName(), "'\n");
	}
	TRACE("\n");
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XWindowMgr::DoActivate								[static]
//
//---------------------------------------------------------------
void XWindowMgr::DoActivate()
{
#if 0
	IWindowMgrPtr mgr(L"Application");

	IWindowPtr wind = mgr->GetTopModal();
	if (wind)
		wind->HandleActivate();			// don't need to activate because an activate event will be in the queue

	else {
		wind = mgr->GetTopRegular();	
		if (wind)
			wind->HandleActivate();
	}
#endif
}


//---------------------------------------------------------------
//
// XWindowMgr::DoDeactivate								[static]
//
//---------------------------------------------------------------
void XWindowMgr::DoDeactivate()
{	
	IWindowMgrPtr mgr(L"Application");
	
	IWindowPtr wind = mgr->GetTopModal();
	if (wind)
		wind->HandleDeactivate();		// need to do deactivate because our code isn't called unless our event loop is executed

	else {
		wind = mgr->GetTopRegular();
		if (wind)
			wind->HandleDeactivate();
	}
}


}	// namespace Whisper
