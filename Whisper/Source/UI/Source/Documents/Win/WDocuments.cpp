/*
 *  File:       WDocuments.cpp
 *  Summary:   	Interface an the app that creates new documents and handles stuff like save all. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: WDocuments.cpp,v $
 *		Revision 1.8  2001/04/05 04:01:22  jesjones
 *		New returns the new document.
 *		
 *		Revision 1.7  2001/02/21 08:16:18  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.6  2001/02/07 02:24:26  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2000/11/27 03:36:47  jesjones
 *		Open no longer sets the doc's parent.
 *		
 *		Revision 1.4  2000/11/26 03:33:07  jesjones
 *		More or less minor changes to support MDI apps.
 *		
 *		Revision 1.3  2000/11/25 01:54:55  jesjones
 *		Renamed mDocParent mFrameWindow.
 *		
 *		Revision 1.2  2000/11/09 12:06:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDocuments.h>

#include <IApplication.h>
#include <ICommandLine.h>
#include <ICreateDoc.h>
#include <IDefaultPositionWindow.h>
#include <IDocument.h>
#include <IDocView.h>
#include <IFrameWindow.h>
#include <IHierarchy.h>
#include <IWindow.h>
#include <XDialogUtils.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFileDialog.h>
#include <XFileFilter.h>
#include <XFileSystem.h>

namespace Whisper {


// ===================================================================================
//	class XDocuments
//!		Interface an the app that creates new documents and handles stuff like save all. 
// ===================================================================================
class XDocuments : public IDocuments, public ICommandLine {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDocuments();
	
						XDocuments(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(EStartupAction action);

	virtual IDocumentPtr New();
	virtual void 		Open();
	virtual void 		Open(const XFileSpec& file);

	virtual void 		CloseAll();
	virtual void 		ForceCloseAll();
	virtual void 		SaveAll();
	
	virtual XFileDialogOptions& GetFileOptions() 		{return mOpenOptions;}
	virtual void 		SetFileFilter(XFileFilter* takeFilter);

protected:
	virtual void 		OnCommandLine(const std::vector<std::wstring>& args);

//-----------------------------------
//	Internal API
//
private:
	virtual bool 		DoCanOpen(const XFileSpec& spec) const;
	virtual IWindowPtr 	DoCreateFrameWindow();
	virtual IDocumentPtr DoCreateOldDoc(const XFileSpec&);
	virtual void 		DoOpenWithoutDoc();
												
//-----------------------------------
//	Member Data
//
private:
	XFileDialogOptions	mOpenOptions;		
	XFileFilter*		mFileFilter;

	EStartupAction 		mStartupAction;		
	IHierarchy*			mFrameWindow;		// nil for SDI apps
};

DEFINE_INTERFACE_FACTORY(XDocuments)

//---------------------------------------------------------------
//
// XDocuments::~XDocuments
//
//---------------------------------------------------------------
XDocuments::~XDocuments()
{
	if (mFileFilter != nil)
		mFileFilter->RemoveReference();
}


//---------------------------------------------------------------
//
// XDocuments::XDocuments
//
//---------------------------------------------------------------
XDocuments::XDocuments(XBoss* boss)
{
	IDocuments::DoSetBoss(boss);
	ICommandLine::DoSetBoss(boss);
	
	IApplicationPtr app(L"Application");
	
	mOpenOptions.SetAppName(app->GetName());
	mOpenOptions.SetPrefKey('Doc ');
	
	mStartupAction = kDoNothingOnStartup;
	mFileFilter = nil;
}


//---------------------------------------------------------------
//
// XDocuments::Init
//
//---------------------------------------------------------------
void XDocuments::Init(EStartupAction action)
{	
	IHierarchyPtr temp = this->DoCreateFrameWindow();

	mFrameWindow = temp.Get();

	mStartupAction = action;
}


//---------------------------------------------------------------
//
// XDocuments::New
//
//---------------------------------------------------------------
IDocumentPtr XDocuments::New()
{
	IDocumentPtr doc;
	
	try {
		IDocuments* thisPtr = this;

		IHierarchyPtr parent;
		if (mFrameWindow)
			parent = mFrameWindow;
		else
			parent = thisPtr;

		ICreateDocPtr creator(thisPtr);
		doc = creator->OnCreate(parent);
				
		try {			
			doc->Open();
		
		} catch (...) {
			doc->ForceClose();
			throw;
		}
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."));
	}

	return doc;
}


//---------------------------------------------------------------
//
// XDocuments::Open ()
//
//---------------------------------------------------------------
void XDocuments::Open()
{
	// Let the user pick one or more files
	XOpenReply reply;		
	if (mFileFilter != nil)
		reply = XFileDialog::OpenFile(mOpenOptions, *mFileFilter);		
	else
		reply = XFileDialog::OpenFile(mOpenOptions);		
	
	// Open each file the user selected
	uint32 numFiles = reply.GetNumItems();
	for (uint32 index = 0; index < numFiles; ++index) 
		this->Open(XFileSpec(reply.GetItem(index)));
}


//---------------------------------------------------------------
//
// XDocuments::Open (XFileSpec)
//
//---------------------------------------------------------------
void XDocuments::Open(const XFileSpec& file)
{
	try {
		// If the document is already open we'll bring the appropiate
		// window to the front of its layer.
		IWindowPtr docWind;
		
		IWindowMgrPtr windows(L"Application");
		XWindowIter iter = windows->begin();
		while (iter != windows->end() && !docWind) {
			IWindowPtr window = *iter;
			++iter;
			
			IDocViewPtr candidate(window);
			if (candidate && candidate->GetDoc()->IsOnDisk() && candidate->GetDoc()->GetFile() == file)
				docWind = IWindowPtr(candidate);
		}
		
		if (docWind)
			docWind->MoveToFront();
			
		else {
			// If the document isn't already open we'll create a new 
			// document using the file (most of the time DoCreateOldDoc 
			// won't use file).
			IDocumentPtr doc = this->DoCreateOldDoc(file);
			
			try {					
				doc->Open(file);
			
			} catch (...) {
				doc->ForceClose();
				throw;
			}
		}
			
	} catch (const XCancelException& e) {
		// do nothing
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't open the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't open the document."));
	}
}


//---------------------------------------------------------------
//
// XDocuments::CloseAll
//
//---------------------------------------------------------------
void XDocuments::CloseAll()
{
	bool closing = true;
	
	IWindowMgrPtr windows(L"Application");
	XWindowIter iter = windows->begin();
	while (closing && iter != windows->end()) {
		IWindowPtr window = *iter;
		++iter;
		
		IDocViewPtr docView(window);
		if (docView)
			closing = window->Close(false);
	}
}


//---------------------------------------------------------------
//
// XDocuments::ForceCloseAll
//
//---------------------------------------------------------------
void XDocuments::ForceCloseAll()
{
	if (!AnswerNo(LoadWhisperString(L"Are you sure you want to close the windows without saving them?"))) {
		IWindowMgrPtr windows(L"Application");
		XWindowIter iter = windows->begin();
		while (iter != windows->end()) {
			IWindowPtr window = *iter;
			++iter;
			
			IDocViewPtr docView(window);
			if (docView)
				docView->GetDoc()->ForceClose();
		}
	}
}


//---------------------------------------------------------------
//
// XDocuments::SaveAll
//
//---------------------------------------------------------------
void XDocuments::SaveAll()
{
	bool saving = true;
	
	IWindowMgrPtr windows(L"Application");
	XWindowIter iter = windows->begin();
	while (saving && iter != windows->end()) {
		IWindowPtr window = *iter;
		++iter;
		
		IDocViewPtr docView(window);
		if (docView) {
			IDocumentPtr doc = docView->GetDoc();

			if (doc->IsDirty()) {				
				if (doc->IsOnDisk())
					saving = doc->Save();
				else
					saving = doc->SaveAs();
			}
		}
	}
}


//---------------------------------------------------------------
//
// XDocuments::SetFileFilter
//
//---------------------------------------------------------------
void XDocuments::SetFileFilter(XFileFilter* takeFilter)
{
	if (mFileFilter != nil)
		mFileFilter->RemoveReference();
		
	mFileFilter = takeFilter;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocuments::DoCreateOldDoc
//
//---------------------------------------------------------------
IDocumentPtr XDocuments::DoCreateOldDoc(const XFileSpec&)					
{
	IDocuments* thisPtr = this;

	IHierarchyPtr parent;
	if (mFrameWindow)
		parent = mFrameWindow;
	else
		parent = thisPtr;

	ICreateDocPtr creator(thisPtr);
	return creator->OnCreate(parent);
}


//---------------------------------------------------------------
//
// XDocuments::DoCreateFrameWindow
//
//---------------------------------------------------------------
IWindowPtr XDocuments::DoCreateFrameWindow()
{
	IDocuments* thisPtr = this;
	IApplicationPtr app(thisPtr);
	
	IWindowPtr window(L"MDI Frame Window");

	SWindowStyle style(kDocumentWindow, true);
	window->Init(thisPtr, app->GetName(), XSize(500, 400), style);
	
	IFrameWindowPtr frame(window);
	frame->Init();
	
	IDefaultPositionWindowPtr position(window);
	if (position)
		position->Init(kCenterOnMainMonitor);

	window->Open();
	window->MoveToFront();
	
	return window;
}


//---------------------------------------------------------------
//
// XDocuments::OnCommandLine
//
//---------------------------------------------------------------
void XDocuments::OnCommandLine(const std::vector<std::wstring>& args)
{
	// Get a list of all the files dropped on the app.
	std::vector<XFileSpec> specs;
	specs.reserve(args.size());

	for (uint32 index = 0; index < args.size(); ++index) {	
		XFileSpec spec(args[index]);

		if (spec.GetExtension() != L"exe" && XFileSystem::FileExists(spec))
			specs.push_back(spec);
	}
	
	// Open all the files we can open (we need to check for valid
	// files since the OS isn't smart enough to do that for us).
	bool opened = false;

	std::vector<XFileSpec>::iterator iter = specs.begin();
	while (iter != specs.end()) {
		const XFileSpec& spec = *iter;
		++iter;
		
		if (this->DoCanOpen(spec)) {
			this->Open(spec);
			opened = true;
		}
	}
	
	// If we couldn't find a file to open call the hook function.
	if (!opened)
		this->DoOpenWithoutDoc();
}


//---------------------------------------------------------------
//
// XDocuments::DoOpenWithoutDoc
//
//---------------------------------------------------------------
void XDocuments::DoOpenWithoutDoc()
{
	if (mStartupAction == kNewDocOnStartup)
		this->New();
	
	else if (mStartupAction == kOpenDocOnStartup)
		this->Open();
}


//---------------------------------------------------------------
//
// XDocuments::DoCanOpen
//
//---------------------------------------------------------------
bool XDocuments::DoCanOpen(const XFileSpec& spec) const
{
	bool can = false;
	
	std::wstring extension = spec.GetExtension();
	
	const XFileDialogOptions::TypeList& types = mOpenOptions.GetTypes();
	XFileDialogOptions::TypeList::const_iterator iter = types.begin();
	while (iter != types.end() && !can) {
		const XFileType& type = *iter;
		++iter;
		
		can = type.Match(extension, '??\?\?') > 0;
	}
	
	return can;
}


}	// namespace Whisper
