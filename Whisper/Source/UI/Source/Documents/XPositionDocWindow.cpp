/*
 *  File:       XPositionDocWindow.cpp
 *  Summary:   	IPositionWindow interface that saves the window position with the doc.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPositionDocWindow.cpp,v $
 *		Revision 1.3  2001/04/27 04:35:21  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:47:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDefaultPositionWindow.h>

#include <IDocument.h>
#include <IDocView.h>
#include <IGeometry.h>
#include <IHierarchy.h>
//#include <IWindow.h>
//#include <IWindowMgr.h>
//#include <MSystemInfo.h>
#include <XChildNode.h>
//#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class XPositionDocWindow
// ===================================================================================
class XPositionDocWindow : public XDefaultPositionWindow, public XChildNode {

	typedef XDefaultPositionWindow Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPositionDocWindow();
	
						XPositionDocWindow(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Position();
	
protected:
	virtual void 		OnAdopted();
						
	virtual void 		OnOrphaned();
	
//-----------------------------------
//	Internal API
//
protected:
			void 		DoSaveState(const SDocumentMessage& mesg);

//-----------------------------------
//	Member Data
//
private:
};


DEFINE_INTERFACE_FACTORY(XPositionDocWindow)

//---------------------------------------------------------------
//
// XPositionDocWindow::~XPositionDocWindow
//
//---------------------------------------------------------------
XPositionDocWindow::~XPositionDocWindow()
{
}


//---------------------------------------------------------------
//
// XPositionDocWindow::XPositionDocWindow
//
//---------------------------------------------------------------
XPositionDocWindow::XPositionDocWindow(XBoss* boss) : XDefaultPositionWindow(boss), XChildNode(boss)
{	
}


//---------------------------------------------------------------
//
// XPositionDocWindow::Position
//
//---------------------------------------------------------------
void XPositionDocWindow::Position()
{
	IPositionWindow* thisPtr = this;
	IDocViewPtr view = GetInterface<IDocView>(thisPtr);
	IDocumentPtr doc = view->GetDoc();
	
	bool updated = false;
	if (doc->IsOnDisk()) {
		XFileSpec spec = doc->GetFile();
		
		try {
			XRect frame;
			spec.GetMetaData(L"Window Position", &frame, sizeof(frame));
			
			IGeometryPtr geometry(thisPtr);
			geometry->SetFrame(frame);
			
			updated = true;
			
		} catch (...) {
			// eat exceptions
		}
	} 
	
	if (!updated)
		Inherited::Position();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XPositionDocWindow::OnAdopted
//
//---------------------------------------------------------------
void XPositionDocWindow::OnAdopted()
{
	IPositionWindow* thisPtr = this;
	IDocViewPtr view = GetInterface<IDocView>(thisPtr);
	IDocumentPtr doc = view->GetDoc();
	
	IDocument::Callback callback(this, &XPositionDocWindow::DoSaveState);
	doc->AddCallback(callback, kSavedDocument);
}

						
//---------------------------------------------------------------
//
// XPositionDocWindow::OnOrphaned
//
//---------------------------------------------------------------
void XPositionDocWindow::OnOrphaned()
{
	IPositionWindow* thisPtr = this;
	IDocViewPtr view = GetInterface<IDocView>(thisPtr);
	IDocumentPtr doc = view->GetDoc();
	
	IDocument::Callback callback(this, &XPositionDocWindow::DoSaveState);
	doc->RemoveCallback(callback);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XPositionDocWindow::DoSaveState
//
//---------------------------------------------------------------
void XPositionDocWindow::DoSaveState(const SDocumentMessage& mesg)
{	
	PRECONDITION(mesg.change == kSavedDocument);
	
	XFileSpec spec = mesg.data.Get<XFileSpec>();

	IPositionWindow* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
			
	try {
		XRect frame = geometry->GetFrame();
		spec.SetMetaData(L"Window Position", &frame, sizeof(frame));	// meta-data isn't portable so padding and bytes-ordering shouldn't be much of an issue...
		
	} catch (...) {
		// eat exceptions
	}
}


}	// namespace Whisper
