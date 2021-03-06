/*
 *  File:       MDocWindowClick.cpp
 *  Summary:   	IWindowClick implementation that handles clicks in document windows.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDocWindowClick.cpp,v $
 *		Revision 1.1  2001/02/07 02:32:11  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IWindowClick.h>

#include <IDocument.h>
#include <IHierarchy.h>
#include <IWindow.h>
#include <MProcess.h>
#include <MSystemInfo.h>
#include <XMouseEvents.h>

namespace Whisper {


// ===================================================================================
//	class MDocWindowClick
//!		IWindowClick implementation that handles clicks in document windows.
// ===================================================================================
class MDocWindowClick : public IWindowClick {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MDocWindowClick();
	
						MDocWindowClick(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnClick(const XMouseDownEvent& globalEvent, int16 partCode);

//-----------------------------------
//	Internal API
//
private:
			OSStatus 	DoClickInPath(const XMouseDownEvent& globalEvent);
			OSStatus 	DoClickInProxy(const XMouseDownEvent& globalEvent);
};

DEFINE_INTERFACE_FACTORY(MDocWindowClick)

//---------------------------------------------------------------
//
// MDocWindowClick::~MDocWindowClick
//
//---------------------------------------------------------------
MDocWindowClick::~MDocWindowClick()
{
}


//---------------------------------------------------------------
//
// MDocWindowClick::MDocWindowClick
//
//---------------------------------------------------------------
MDocWindowClick::MDocWindowClick(XBoss* boss) 
{	
	IWindowClick::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MDocWindowClick::OnClick
//
//---------------------------------------------------------------
bool MDocWindowClick::OnClick(const XMouseDownEvent& globalEvent, int16 partCode)
{	
	bool handled = true;
	
	OSStatus err = noErr;
	switch (partCode) {
		case inDrag:
			err = this->DoClickInPath(globalEvent);
			handled = err == noErr;
			break;
			
		case inProxyIcon:
			err = this->DoClickInProxy(globalEvent);
			if (err == errUserWantsToDragWindow)
				handled = false;
			break;

		default:
			handled = false;
	}
	
	return handled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MDocWindowClick::DoClickInPath
//
//---------------------------------------------------------------
OSStatus MDocWindowClick::DoClickInPath(const XMouseDownEvent& globalEvent)
{	
	OSStatus err = userCanceledErr;

	if (MSystemInfo::HasWindowMgr2()) {	
		IDocumentPtr doc = GetParent<IDocument>(this);	
		
		IWindowPtr window(this);
		WindowRef wptr = window->GetOSWindow();

		if (doc->IsOnDisk()) {
			EventRecord temp = globalEvent.GetOSEvent();
			if (::IsWindowPathSelectClick(wptr, &temp)) {
				err = ::WindowPathSelect(wptr, nil, nil);
				if (err == noErr) {
					const OSType kFinder = 'MACS';
					
					if (MProcess::IsRunning(kFinder)) {
						ProcessSerialNumber psn = MProcess::GetSerialNumber(kFinder);
						MProcess::PullToFront(psn);	
					}
					
				} else
					ASSERT(err == userCanceledErr);		// don't throw for this!
			}
		}
	}
	
	return err;
}


//---------------------------------------------------------------
//
// MDocWindowClick::DoClickInProxy
//
//---------------------------------------------------------------
OSStatus MDocWindowClick::DoClickInProxy(const XMouseDownEvent& globalEvent)
{
	OSStatus err = userCanceledErr;
	
	if (MSystemInfo::HasWindowMgr2()) {	
		IDocumentPtr doc = GetParent<IDocument>(this);	

		IWindowPtr window(this);
		WindowRef wptr = window->GetOSWindow();

		if (doc->IsOnDisk()) {
			err = ::TrackWindowProxyDrag(wptr, globalEvent.GetGlobalPosition().GetOSPoint());
			ASSERT(err == noErr || err == userCanceledErr || err == errUserWantsToDragWindow);	// don't throw for this!
		}
	}
	
	return err;
}


}	// namespace Whisper
