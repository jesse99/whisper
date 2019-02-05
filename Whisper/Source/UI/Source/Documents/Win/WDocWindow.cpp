/*
 *  File:       WDocWindow.cpp
 *  Summary:   	A window attached to a document.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WDocWindow.cpp,v $
 *		Revision 1.6  2001/02/21 08:16:22  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.5  2000/11/26 03:32:10  jesjones
 *		Init uses GetInterface to find the frame window.
 *		
 *		Revision 1.4  2000/11/25 01:56:43  jesjones
 *		Made SetName a no-op. Tweaked the invariant.
 *		
 *		Revision 1.3  2000/11/17 08:50:38  jesjones
 *		Calls SetWindowProc.
 *		
 *		Revision 1.2  2000/11/09 12:06:37  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IInit.h>

#include <ICanClose.h>
#include <IDocument.h>
#include <IFrameWindow.h>
#include <IHierarchy.h>
#include <IPersistDoc.h>
#include <ITimer.h>
#include <IWindow.h>
#include <IUndoContext.h>
#include <XFileSpec.h>
#include <XMouseEvents.h>

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

	IFrameWindowPtr frame = GetInterface<IFrameWindow>(thisPtr);
	if (frame) {
		IWindowPtr window(thisPtr);
		window->SetWindowProc(DefMDIChildProc);
	}
	
	IDocument::Callback callback(this, &XDocWindow::DoDocChange);
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);	
	doc->AddCallback(callback);
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
}

			
//---------------------------------------------------------------
//
// XDocWindow::OnCanClose
//
//---------------------------------------------------------------
bool XDocWindow::OnCanClose(bool quitting) const
{	
	const IInit* thisPtr = this;

	IConstDocumentPtr doc = GetParent<IDocument>(thisPtr);
	bool can = const_cast<IDocument*>(doc.Get())->CanClose(quitting ? kQuittingApplication : kClosingDocument);
		
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
			}
			break;
	}
}


}	// namespace Whisper
