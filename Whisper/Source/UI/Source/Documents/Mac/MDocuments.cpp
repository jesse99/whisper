/*
 *  File:       MDocuments.cpp
 *  Summary:   	Interface an the app that creates new documents and handles stuff like save all. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: MDocuments.cpp,v $
 *		Revision 1.11  2001/04/21 03:25:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.10  2001/04/13 07:49:49  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.9  2001/04/05 04:01:16  jesjones
 *		New returns the new document.
 *		
 *		Revision 1.8  2001/02/07 02:22:52  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.7  2001/01/30 07:19:23  jesjones
 *		Tweaked focus code.
 *		
 *		Revision 1.6  2001/01/21 00:27:22  jesjones
 *		Renamed some methods in IMenuHandler.
 *		
 *		Revision 1.5  2000/12/31 04:26:58  jesjones
 *		Open no longer calls AddChild.
 *		
 *		Revision 1.4  2000/11/27 07:12:12  jesjones
 *		OnCreateNewDoc takes a parent argument.
 *		
 *		Revision 1.3  2000/11/09 09:12:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDocuments.h>

#include <IApplication.h>
#include <ICommandLine.h>
#include <ICreateDoc.h>
#include <ICurrentFocus.h>
#include <IDocument.h>
#include <IDocView.h>
#include <IFocus.h>
#include <IHierarchy.h>
#include <IMenuHandler.h>
#include <IWindow.h>
#include <IWindowMgr.h>
#include <MAppleEvent.h>
#include <XDialogUtils.h>
#include <XError.h>
#include <XEvent.h>
#include <XExceptions.h>
#include <XFileDialog.h>
#include <XFileFilter.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CheckForRequiredAEParams
//
//---------------------------------------------------------------
static OSErr CheckForRequiredAEParams(const AppleEvent* event)
{
	PRECONDITION(event != nil);
	
	DescType	returnedType;
	Size		actualSize;
	OSErr err = AEGetAttributePtr(event, keyMissedKeywordAttr, typeWildCard, &returnedType, nil, 0, &actualSize); 

	if (err == errAEDescNotFound)		
		err = noErr;					// all the parameters were there!		
	else if (err == noErr)				
		err = errAEParamMissed;			// missed parameters

	return err;						
}

#pragma mark -

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
			void 		DoOpenPref(const XFileSpec& file);
			IDocumentPtr DoCreateOldDoc(const XFileSpec&);
			void 		DoOpenWithoutDoc();
												
	static 	pascal OSErr DoAEOpenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon);
	static 	pascal OSErr DoAEOpenDocs(const AppleEvent* event, AppleEvent* reply, int32 refCon);
	static 	pascal OSErr DoAEReopenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon);

//-----------------------------------
//	Member Data
//
private:
	XFileDialogOptions	mOpenOptions;		
	XFileFilter*		mFileFilter;
	
	EStartupAction 		mStartupAction;		
};

DEFINE_INTERFACE_FACTORY(XDocuments)
static XDocuments* sInstance = nil;

//---------------------------------------------------------------
//
// XDocuments::~XDocuments
//
//---------------------------------------------------------------
XDocuments::~XDocuments()
{
	sInstance = nil;
	
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

	sInstance = this;
}


//---------------------------------------------------------------
//
// XDocuments::Init
//
//---------------------------------------------------------------
void XDocuments::Init(EStartupAction action)
{	
	AEEventHandlerUPP handler = NewAEEventHandlerUPP(XDocuments::DoAEOpenApp);
	OSErr err = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, handler, 0, false);
	ThrowIfOSErr(err);
	
	handler = NewAEEventHandlerUPP(XDocuments::DoAEOpenDocs);
	err = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, handler, 0, false);
	ThrowIfOSErr(err);
	
	handler = NewAEEventHandlerUPP(XDocuments::DoAEReopenApp);
	err = AEInstallEventHandler(kCoreEventClass, kAEReopenApplication, handler, 0, false);	
	ThrowIfOSErr(err);

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
		IHierarchyPtr node(thisPtr);
		ICreateDocPtr creator(thisPtr);
		
		doc = creator->OnCreate(node);
				
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
		// If the user is opening a 'pref' call DoOpenPref. 
		if (file.GetType() == 'pref')
			this->DoOpenPref(file);
			
		else {
			// Otherwise the user is opening a document file. If the
			// document is already open we'll bring the appropiate
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
				// If the document isn't already open we'll create a
				// new document using the file (most of the time
				// DoCreateOldDoc won't use file).
				IDocumentPtr doc = this->DoCreateOldDoc(file);
				
				try {					
					doc->Open(file);
				
				} catch (...) {
					doc->ForceClose();
					throw;
				}
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
// XDocuments::OnCommandLine
//
//---------------------------------------------------------------
void XDocuments::OnCommandLine(const std::vector<std::wstring>& args)
{
	UNUSED(args);		// $$$ will this ever be used on OS X?
}


#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocuments::DoOpenPref
//
// This will be called when the user double clicks the app's preference 
// file. According to TN 1134 you should pop up the preference dialog 
// (with the current settings: not those in the file). Default sends 
// kPrefsCmd up the focus chain. 
//
//---------------------------------------------------------------
void XDocuments::DoOpenPref(const XFileSpec& file)
{
	UNUSED(file);
	
	ICurrentFocusPtr current(L"Application");
	IFocusPtr focus = current->GetFocus();
	
	IMenuHandlerPtr handler = GetInterface<IMenuHandler>(focus);
	bool handled = handler->HandleCommand(kPrefsCmd, XEvent());
	ASSERT(handled);
}

		
//---------------------------------------------------------------
//
// XDocuments::DoOpenWithoutDoc
//
// Called when the user double clicks the app. Defaults to creating
// a new document, popping up the open file dialog, or nothing
// depending on EStartupAction. 
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
// XDocuments::DoCreateOldDoc
//
// Called to create an existing document. You may want to override
// this for apps that have a variable number of windows per document. 
//
//---------------------------------------------------------------
IDocumentPtr XDocuments::DoCreateOldDoc(const XFileSpec&)					
{
	IDocuments* thisPtr = this;
	IHierarchyPtr parent(thisPtr);
	ICreateDocPtr creator(thisPtr);
	
	return creator->OnCreate(parent);
}


//---------------------------------------------------------------
//
// XDocuments::DoAEOpenApp								[static]
//
//---------------------------------------------------------------
pascal OSErr XDocuments::DoAEOpenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon)
{
	UNUSED(event);
	UNUSED(reply);
	UNUSED(refCon);
	
	try {
		sInstance->DoOpenWithoutDoc();
		
	} catch (const XCancelException& e) {
		// do nothing
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."));
	}

	return noErr;
}


//---------------------------------------------------------------
//
// XDocuments::DoAEOpenDocs								[static]
//
//---------------------------------------------------------------
pascal OSErr XDocuments::DoAEOpenDocs(const AppleEvent* inEvent, AppleEvent* reply, int32 refCon)
{
	UNUSED(reply);
	UNUSED(refCon);
	
	try {
		MAppleEvent event(*inEvent);
		
		MAEList docList = event.GetDesc(keyDirectObject, typeAEList);
		
		event.CheckRequiredParameters();
		
		uint32 count = docList.GetCount();
		for (uint32 index = 0; index < count; index++) {
			FSSpec spec;
						
			OSErr err = docList.GetItem(index, typeFSS, &spec, sizeof(spec));
			ThrowIfOSErr(err);
			
			sInstance->Open(spec);
		}
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the document list."), e);

	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"There was an error opening the document list."));
	}		

	return noErr;
}


//---------------------------------------------------------------
//
// XDocuments::DoAEReopenApp							[static]
//
//---------------------------------------------------------------
pascal OSErr XDocuments::DoAEReopenApp(const AppleEvent* event, AppleEvent* reply, int32 refCon)
{
	UNUSED(event);
	UNUSED(reply);
	UNUSED(refCon);
	
	try {
		sInstance->New();
		
	} catch (const XCancelException& e) {
		// do nothing
	
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't create the document."));
	}

	return noErr;
}


}	// namespace Whisper
