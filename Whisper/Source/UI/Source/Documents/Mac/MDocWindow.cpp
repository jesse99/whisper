/*
 *  File:       MDocWindow.cpp
 *  Summary:   	A window attached to a document.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDocWindow.cpp,v $
 *		Revision 1.5  2001/04/27 09:12:04  jesjones
 *		Updated for the new timer code.
 *		
 *		Revision 1.4  2001/02/08 03:51:01  jesjones
 *		Renamed a method.
 *		
 *		Revision 1.3  2001/02/07 02:28:15  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 09:12:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInit.h>

#include <ICanClose.h>
#include <IDocument.h>
#include <IHierarchy.h>
#include <IPersistDoc.h>
#include <ITimer.h>
#include <IUndoContext.h>
#include <IWindow.h>
#include <MFileSystem.h>
#include <MSystemInfo.h>

namespace Whisper {


// ===================================================================================
//	class XDocWindow
//!		A window attached to a document.
// ===================================================================================
class XDocWindow : public IInit, public ICanClose {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDocWindow();
	
						XDocWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnInit();
	virtual void 		OnCleanup();
	
	virtual bool 		OnCanClose(bool quitting) const;

//-----------------------------------
//	Internal API
//
private:
			void 		DoDocChange(const SDocumentMessage& mesg);
			void 		DoSyncWithFinder();
	
			void 		DoSetProxySpec();
			void 		DoSetDirtyProxy();
};

DEFINE_INTERFACE_FACTORY(XDocWindow)

//---------------------------------------------------------------
//
// XDocWindow::~XDocWindow
//
//---------------------------------------------------------------
XDocWindow::~XDocWindow()
{
}


//---------------------------------------------------------------
//
// XDocWindow::XDocWindow
//
//---------------------------------------------------------------
XDocWindow::XDocWindow(XBoss* boss) 
{	
	IInit::DoSetBoss(boss);
	ICanClose::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocWindow::OnInit
//
//---------------------------------------------------------------
void XDocWindow::OnInit()
{
	IInit* thisPtr = this;

	IDocument::Callback callback(this, &XDocWindow::DoDocChange);
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);	
	doc->AddCallback(callback);
	
	ITimerPtr timer(L"Application");	
	XCallback0<void> callback2(this, &XDocWindow::DoSyncWithFinder);
	timer->AddTimer(callback2, 250);

	this->DoSetProxySpec();
	this->DoSetDirtyProxy();
}


//---------------------------------------------------------------
//
// XDocWindow::OnCleanup
//
//---------------------------------------------------------------
void XDocWindow::OnCleanup()
{
	IInit* thisPtr = this;

	IDocument::Callback callback(this, &XDocWindow::DoDocChange);
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);	
	doc->RemoveCallback(callback);	
		
	ITimerPtr timer(L"Application");	
	XCallback0<void> callback2(this, &XDocWindow::DoSyncWithFinder);
	timer->RemoveTimer(callback2);
}


//---------------------------------------------------------------
//
// XDocWindow::OnCanClose
//
//---------------------------------------------------------------
bool XDocWindow::OnCanClose(bool quitting) const
{	
	const IInit* thisPtr = this;

	IDocumentPtr doc = GetParent<IDocument>(thisPtr);
	bool can = doc->CanClose(quitting ? kQuittingApplication : kClosingDocument);
		
	return can;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocWindow::DoDocChange
//
//---------------------------------------------------------------
void XDocWindow::DoDocChange(const SDocumentMessage& mesg)
{	
	IInit* thisPtr = this;
	IWindowPtr window(thisPtr);
	
	switch (mesg.change) {
		case kOpenedDocument:
			ASSERT(!window->IsVisible());
			this->DoSetDirtyProxy();
			window->SetName(mesg.doc->GetName());
			window->Open();
			window->MoveToFront();
			break;
			
		case kForceCloseDocument:
			VERIFY(window->Close(false, true));	// don't bother asking the doc if it can close
			break;
		
		case kChangedDocumentName:
			window->SetName(mesg.doc->GetName());
			break;
		
		case kChangedDocument:
			window->Invalidate();
			break;
			
		case kRevertedDocument:					// will also get a kChangedDocument so don't need to Invalidate
		case kSavedDocument:
			{
			IUndoContextPtr undo(thisPtr);
			if (undo)
				undo->ClearHistory();			// $$$ do we really want to do this?
			this->DoSetDirtyProxy();			// in case the user saves a new, unchanged document
			}
			break;
		
		case kChangedDocumentFileSpec:
			this->DoSetProxySpec();
			break;
		
		case kDocumentDirtyChanged:
			this->DoSetDirtyProxy();
			break;
	}
}


//---------------------------------------------------------------
//
// XDocWindow::DoSyncWithFinder
//
// The document needs to be kept informed of proxy changes so that
// it can do things like rename itself when the user changes the 
// proxies name, save in the correct location if the user moves the
// proxy, or delete itself if the user trashes the proxy. Currently
// we handle this via a timer which kind of blows. $$$ Hopefully
// the Carbon Event Manager will have a better solution. $$$
//
//---------------------------------------------------------------
void XDocWindow::DoSyncWithFinder()
{	
	if (MSystemInfo::HasWindowMgr2()) {	
		IInit* thisPtr = this;
		IDocumentPtr doc = GetParent<IDocument>(thisPtr);	

		IWindowPtr window(thisPtr);
		WindowRef wptr = window->GetOSWindow();

		if (doc->IsOnDisk()) {
			FSSpec spec;
			OSStatus err = ::GetWindowProxyFSSpec(wptr, &spec);
			if (err == noErr) {
				XFileSpec theSpec(spec);
				if (doc->SetFile(theSpec, true)) {		// returns true if the spec changed
					doc->SetName(theSpec.GetName());
	
					if (MFileSystem::IsInTrash(theSpec))
						doc->ForceClose();
				}
							
			} else
				DEBUGSTR("Got an error calling GetWindowProxyFSSpec!");
		}
	}
}


//---------------------------------------------------------------
//
// XDocWindow::DoSetProxySpec
//
//---------------------------------------------------------------
void XDocWindow::DoSetProxySpec()
{
	if (MSystemInfo::HasWindowMgr2()) {	
		IInit* thisPtr = this;
		IDocumentPtr doc = GetParent<IDocument>(thisPtr);	
		IPersistDocPtr persist(doc);

		IWindowPtr window(thisPtr);
		WindowRef wptr = window->GetOSWindow();

		OSStatus err;
		if (doc->IsOnDisk())
			err = ::SetWindowProxyFSSpec(wptr, &doc->GetFile().GetOSSpec());
		else
			err = ::SetWindowProxyCreatorAndType(wptr,
										 	  	 persist->GetDefaultCreator(),
										 	  	 persist->GetDefaultType(),
										 	  	 kOnSystemDisk);
		ASSERT(err == noErr);		// don't throw for this!
	}
}


//---------------------------------------------------------------
//
// XDocWindow::DoSetDirtyProxy
//
//---------------------------------------------------------------
void XDocWindow::DoSetDirtyProxy()
{
	if (MSystemInfo::HasWindowMgr2()) {	
		IInit* thisPtr = this;
		IDocumentPtr doc = GetParent<IDocument>(thisPtr);	

		IWindowPtr window(thisPtr);
		WindowRef wptr = window->GetOSWindow();

		Boolean dirty = doc->IsDirty();
		if (dirty || doc->IsOnDisk()) {		// only clear the dirty bit if the doc is on disk (we don't want to allow the user to drag the proxy if the doc isn't on the disk)
			OSStatus err = ::SetWindowModified(wptr, dirty);
			ASSERT(err == noErr);			// don't throw for this!
		}
	}
}


}	// namespace Whisper
