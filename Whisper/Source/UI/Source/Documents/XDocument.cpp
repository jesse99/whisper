/*
 *  File:       XDocument.cpp
 *  Summary:   	An object containing the data for an application. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XDocument.cpp,v $
 *		Revision 1.19  2001/08/29 01:04:53  jesjones
 *		Wrapped XDisableInvariant with #if DEBUG.
 *		
 *		Revision 1.18  2001/04/27 09:20:29  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.17  2001/04/27 04:32:20  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.16  2001/04/17 01:43:35  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.15  2001/03/25 06:12:34  jesjones
 *		kReverted message now includes the file spec.
 *		
 *		Revision 1.14  2001/03/15 09:33:26  jesjones
 *		No longer calls ReplaceFile directly (so apps run on Win95).
 *		
 *		Revision 1.13  2001/03/06 07:32:29  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.12  2001/02/21 08:12:41  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.11  2001/02/12 09:49:47  jesjones
 *		Uses MFileSystem::ChangeCreatorType instead of using MoreFiles directly (so DLL targets link).
 *		
 *		Revision 1.10  2001/02/08 03:56:08  jesjones
 *		ForceClose no longer class PRECONDITION and POSTCONDITION.
 *		
 *		Revision 1.9  2001/01/03 05:57:05  jesjones
 *		No longer building with Win2K targeted so had to define the ReplaceFile prototypes.
 *		
 *		Revision 1.8  2000/12/14 08:34:37  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.7  2000/11/27 03:32:23  jesjones
 *		DoSave uses ReplaceFile on Windows 2000.
 *		
 *		Revision 1.6  2000/11/26 03:31:29  jesjones
 *		DoClose no longer uses a command to remove itself from its parent (causes problems with MDI apps).
 *		
 *		Revision 1.5  2000/11/25 01:55:46  jesjones
 *		GetName no longer calls PRECONDITION or POSTCONDITION.
 *		
 *		Revision 1.4  2000/11/20 05:47:53  jesjones
 *		XCallback again uses IsValid.
 *		
 *		Revision 1.3  2000/11/12 07:55:28  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:32:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/10/99	JDJ		Added an undo context.
 *		 <4>	 6/25/99	JDJ		Uses new kInvalidFileSpec constant.
 *		 <3>	 6/21/99	JDJ		HandleSaveAs strips leading and trailing spaces on Windows.
 *		 <2>	 5/27/99	JDJ		Made thread safe.
 *		 <1>	 5/10/99	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <IDocument.h>

#include <IActiveUndoContext.h>
#include <ICommand.h>
#include <ICommands.h>
#include <ICommandQueue.h>
#include <IHierarchy.h>
#include <IPersistDoc.h>
#include <IUndoContext.h>
#include <XBind.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFile.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XIntConversions.h>
#include <XLocker.h>
#include <XStringUtils.h>

#if MAC
	#include <MFileSystem.h>
#elif WIN
	#include <WSystemInfo.h>
#endif

#if WIN
	#if (_WIN32_WINNT < 0x0500)		// $$ WWhisperHeader.h used to define this to 5.0, but that somehow caused CoCreateInstance to fail with error E_NOINTERFACE
		#define REPLACEFILE_WRITE_THROUGH       0x00000001
		#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002

		extern "C" {
		WINBASEAPI
		BOOL
		WINAPI
		ReplaceFileA(
		    LPCSTR  lpReplacedFileName,
		    LPCSTR  lpReplacementFileName,
		    LPCSTR  lpBackupFileName,
		    DWORD   dwReplaceFlags,
		    LPVOID  lpExclude,
		    LPVOID  lpReserved
		    );
		WINBASEAPI
		BOOL
		WINAPI
		ReplaceFileW(
		    LPCWSTR lpReplacedFileName,
		    LPCWSTR lpReplacementFileName,
		    LPCWSTR lpBackupFileName,
		    DWORD   dwReplaceFlags,
		    LPVOID  lpExclude,
		    LPVOID  lpReserved
		    );
		}
		#ifdef UNICODE
			#define ReplaceFile  ReplaceFileW
		#else
			#define ReplaceFile  ReplaceFileA
		#endif 	
	#endif 		

	static BOOL MyReplaceFileW(LPCWSTR lpReplacedFileName, LPCWSTR lpReplacementFileName, LPCWSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved)
	{
		BOOL succeeded = false;

		typedef BOOL (WINAPI *IsReplaceFileWProc)(LPCWSTR, LPCWSTR, LPCWSTR, DWORD, LPVOID, LPVOID);

		HINSTANCE kernelH = LoadLibrary("KERNEL32.DLL");
		if (kernelH != nil) {								// should never fail			
			IsReplaceFileWProc proc = (IsReplaceFileWProc) ::GetProcAddress(kernelH, "ReplaceFileW");
			if (proc != nil)								// only present in NT and Win 98
				succeeded = proc(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
		}

		return succeeded;
	}

	static BOOL MyReplaceFileA(LPCSTR lpReplacedFileName, LPCSTR lpReplacementFileName, LPCSTR lpBackupFileName, DWORD dwReplaceFlags, LPVOID lpExclude, LPVOID lpReserved)
	{
		BOOL succeeded = false;

		typedef BOOL (WINAPI *IsReplaceFileAProc)(LPCSTR, LPCSTR, LPCSTR, DWORD, LPVOID, LPVOID);

		HINSTANCE kernelH = LoadLibrary("KERNEL32.DLL");
		if (kernelH != nil) {								// should never fail			
			IsReplaceFileAProc proc = (IsReplaceFileAProc) ::GetProcAddress(kernelH, "ReplaceFileA");
			if (proc != nil)								// only present in NT and Win 98
				succeeded = proc(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);
		}

		return succeeded;
	}
#endif 			

namespace Whisper {


// ===================================================================================
//	class XDocument
//!		An object containing the data for an application. 
// ===================================================================================
class XDocument : public IDocument {
	
	friend class ZNamesMatch;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDocument();
	
  						XDocument(XBoss* boss);
  						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Open();
	virtual void 		Open(const XFileSpec& file);
						
	virtual	bool 		CanClose(ESaveChanges reason);
	virtual void		ForceClose();

	virtual bool		Save();
	virtual bool		SaveAs();
	virtual void		Revert();

	virtual bool		IsDirty() const						{return mDirty;}
	virtual void		SetDirty(bool dirty = true, uint32 message = kChangedDocument, const XAnyValue& value = XAnyValue());
						
	virtual std::wstring GetName() const;
	virtual void 		SetName(const std::wstring& newName);

	virtual bool		IsOnDisk() const					{return mOnDisk;}
	virtual XFileSpec	GetFile() const						{return mFileSpec;}
	virtual bool 		SetFile(const XFileSpec& spec, bool onDisk);

	virtual void 		AddCallback(Callback callback, uint32 change = 0);
	virtual void 		RemoveCallback(Callback callback);

protected:
	virtual void 		Invariant() const;
	
//-----------------------------------
//	Internal API
//
protected:
			void 		DoSave(const XFileSpec& target, OSType creator, OSType type);		
			void 		DoClose();
			std::wstring DoGetInitialName() const;
			void 		DoBroadcast(uint32 message, const XAnyValue& value = XAnyValue());

//-----------------------------------
//	Internal Types
//
protected:
	struct SCallback {
		Callback	callback;
		uint32		change;
		
					SCallback(Callback c, uint32 m) : callback(c), change(m) {}
			bool 	operator==(const Callback& rhs) const	{return callback == rhs;}
		};
		
	typedef std::list<XDocument*> DocList;
	typedef std::vector<SCallback> Callbacks;

//-----------------------------------
//	Member Data
//
private:	
	mutable std::wstring	mName;
	bool					mDirty;				// needs to be saved?

	bool					mOnDisk;			// does the document have a file on disk?
	XFileSpec				mFileSpec;			// only valid if mOnDisk is set
	
	Callbacks				mCallbacks;
	bool					mClosing;

	static DocList			msDocuments;
};

// ===================================================================================
//	class ZNamesMatch
//		Predicate function used for searching within the document list. Note that this 
//		is a friend of XDocument so it can directly access mName (calling GetName can 
//		lead to an infinite loop).
// ===================================================================================
class ZNamesMatch {

public:
  						ZNamesMatch(const std::wstring& name)	{mName = name;}
  						
			bool		operator()(const XDocument* doc) const	{PRECONDITION(doc != nil); return doc->mName == mName;}
			
private:
	std::wstring	mName;
};
	
DEFINE_INTERFACE_FACTORY(XDocument)

XDocument::DocList XDocument::msDocuments;

//---------------------------------------------------------------
//
// XDocument::~XDocument
//
//---------------------------------------------------------------
XDocument::~XDocument()
{
	CALL_INVARIANT;
	PRECONDITION(mCallbacks.empty());
		
	IUndoContextPtr myContext(this);
	if (myContext) {
		IActiveUndoContextPtr activeContext(L"Application");
		if (activeContext && activeContext->Get() == myContext) {
			IUndoContext* nilContext = nil;
			activeContext->Set(nilContext);
		}
	}
	
	msDocuments.remove(this);
}


//---------------------------------------------------------------
//
// XDocument::XDocument
//
//---------------------------------------------------------------
XDocument::XDocument(XBoss* boss) 
{	
	this->DoSetBoss(boss);
		
	mDirty  = false;
	mOnDisk = false;
	
	mClosing = false;
			
	msDocuments.push_back(this); 

//	CALL_INVARIANT;			// wait until we're opened
}


//---------------------------------------------------------------
//
// XDocument::Open ()
//
//---------------------------------------------------------------
void XDocument::Open()
{
	PRECONDITION(!mOnDisk);
	
	{
#if DEBUG
	XDisableInvariant disabler(this);		
#endif
		this->DoBroadcast(kOpenedDocument);
	}
	
	POSTCONDITION(!mOnDisk);
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XDocument::Open (XFileSpec)
//
//---------------------------------------------------------------
void XDocument::Open(const XFileSpec& spec)
{
	PRECONDITION(!mOnDisk);
	PRECONDITION(!mDirty);
	
	{
#if DEBUG
	XDisableInvariant disabler(this);		
#endif
		IPersistDocPtr io(this);
		io->HandleRead(spec);
				
		this->SetDirty(false);
		this->SetFile(spec, true);		
	
		this->DoBroadcast(kOpenedDocument, XAnyValue(spec));
		this->DoBroadcast(kChangedDocument);
		this->DoBroadcast(kChangedDocumentFileSpec);
	
		this->SetName(spec.GetName());	// need to do this after broadcasting so doc window has a chance to open, $$$ rewrite comment?	
	}
	
	POSTCONDITION(mOnDisk);
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XDocument::CanClose
//
//---------------------------------------------------------------
bool XDocument::CanClose(ESaveChanges reason)
{
	CHECK_INVARIANT;
	
	bool canClose = true;
	
	if (mDirty) {
		std::wstring docName = this->GetName();
		EAskSaveResult result = XFileDialog::AskSaveChanges(docName, reason);
		
		if (result == kSaveChanges) {
			if (mOnDisk)
				canClose = this->Save();
			else
				canClose = this->SaveAs();
				
		} else if (result == kCancelSave)
			canClose = false;
	}
	 
	return canClose;
}


//---------------------------------------------------------------
//
// XDocument::ForceClose
//
//---------------------------------------------------------------
void XDocument::ForceClose()
{
	// Everyone who has a reference to the document needs to listen
	// for kForceCloseDocument and release their reference.
	this->DoBroadcast(kForceCloseDocument);
}


//---------------------------------------------------------------
//
// XDocument::Save
//
//---------------------------------------------------------------
bool XDocument::Save()
{
	PRECONDITION(mOnDisk);
	PRECONDITION(mDirty);
	CHECK_INVARIANT;
	
	bool saved = false;
		
	try {
#if MAC
		this->DoSave(mFileSpec, mFileSpec.GetCreator(), mFileSpec.GetType());
#else
		this->DoSave(mFileSpec, 0, 0);
#endif
						
		this->SetDirty(false);

		this->DoBroadcast(kSavedDocument, XAnyValue(mFileSpec));
		
		saved = true;
		
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the document."));
	}
	
	POSTCONDITION(!mDirty);
	
	return saved;
}


//---------------------------------------------------------------
//
// XDocument::SaveAs
//
//---------------------------------------------------------------
bool XDocument::SaveAs()
{
	CHECK_INVARIANT;
	
	bool saved = false;
	
	try {		
		std::wstring prompt  = LoadWhisperString(L"Save File As:");
		std::wstring docName = this->GetName();
				
		IPersistDocPtr io(this);
		XSaveReply reply = io->SaveFile(prompt, docName);
							
		if (reply.IsValid()) {		
			XFileSpec spec = reply.GetTarget();
			
#if WIN
			std::wstring prefix = StripLeading(StripTrailing(spec.GetPrefix()));	// logo requirements require leading and trailing spaces to be stripped
			std::wstring name = prefix + L"." + spec.GetExtension();

			spec = XFileSpec(spec.GetParent(), name);				
#endif
			
			if (reply.IsReplacing())
				if (!io->CanReplace(reply))
					throw std::range_error(ToUTF8Str(LoadWhisperString(L"You can't save over this file.")));
			
#if MAC
			this->DoSave(spec, reply.GetCreator(), reply.GetType());
#else
			this->DoSave(spec, 0, 0);
#endif
			
			this->SetFile(spec, true);	
			this->SetName(mFileSpec.GetName());
			this->SetDirty(false);
		
			saved = true;

			this->DoBroadcast(kSavedDocument, XAnyValue(mFileSpec));
			this->DoBroadcast(kChangedDocumentFileSpec);
		}
			
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the document."), e);
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't save the document."));
	}
	
	return saved;
}


//---------------------------------------------------------------
//
// XDocument::Revert
//
// Note that if an error occurs we close the document. We do this
// because it's not obvious how to restore the user's changes and
// because the user has indicated his willingness to throw away
// his changes.
//
//---------------------------------------------------------------
void XDocument::Revert()
{
	PRECONDITION(mOnDisk);
	PRECONDITION(mDirty);
	CHECK_INVARIANT;
	
	try {
		std::wstring docName = this->GetName();
		ERevertResult result = XFileDialog::AskRevertChanges(docName);
		if (result == kDiscardChanges) {
			IPersistDocPtr io(this);
			io->HandleRead(mFileSpec);

			this->SetDirty(false);

			this->DoBroadcast(kChangedDocument);
			this->DoBroadcast(kRevertedDocument, XAnyValue(mFileSpec));
		}
		
	} catch (const std::exception& e) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't revert the document."), e);
		this->ForceClose();
	
	} catch (...) {
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't revert the document."));
		this->ForceClose();
	}
}


//---------------------------------------------------------------
//
// XDocument::SetDirty
//
//---------------------------------------------------------------
void XDocument::SetDirty(bool dirty, uint32 message, const XAnyValue& value)
{
	CHECK_INVARIANT;
	
	if (dirty != mDirty) {		
		mDirty = dirty;
				
		this->DoBroadcast(kDocumentDirtyChanged);
	}

	if (dirty && message != 0)
		this->DoBroadcast(message, value);

	POSTCONDITION(dirty == this->IsDirty());
}


//---------------------------------------------------------------
//
// XDocument::GetName
//
//---------------------------------------------------------------
std::wstring XDocument::GetName() const
{
	if (mName.empty())
		mName = this->DoGetInitialName();		// we'll call this here to allow people to override DoGetInitialName
	
	POSTCONDITION(!mName.empty());
		
	return mName;
}


//---------------------------------------------------------------
//
// XDocument::SetName
//
//---------------------------------------------------------------
void XDocument::SetName(const std::wstring& newName)
{
	PRECONDITION(!newName.empty());
	CHECK_INVARIANT;
	
	if (this->GetName() != newName) {			
		mName = newName;
		
		this->DoBroadcast(kChangedDocumentName);
	}

	POSTCONDITION(newName == this->GetName());
}


//---------------------------------------------------------------
//
// XDocument::SetFile
//
//---------------------------------------------------------------
bool XDocument::SetFile(const XFileSpec& spec, bool onDisk)
{
	CHECK_INVARIANT;
	
	bool changed = false;
	
	if (spec != mFileSpec || onDisk != mOnDisk) {
		mFileSpec = spec;
		mOnDisk   = onDisk;
		
		this->DoBroadcast(kChangedDocumentFileSpec);
		changed = true;
	}
	
	return changed;
}


//---------------------------------------------------------------
//
// XDocument::AddCallback
//
//---------------------------------------------------------------
void XDocument::AddCallback(Callback callback, uint32 change)
{
	PRECONDITION(callback.IsValid());

#if DEBUG
	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	PRECONDITION(iter == mCallbacks.end());
#endif

	mCallbacks.push_back(SCallback(callback, change));	
}


//---------------------------------------------------------------
//
// XDocument::RemoveCallback
//
//---------------------------------------------------------------
void XDocument::RemoveCallback(Callback callback)
{
	Callbacks::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	PRECONDITION(iter != mCallbacks.end());
	
	mCallbacks.erase(iter);

	if (mCallbacks.empty())
		this->DoClose();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDocument::Invariant
//
//---------------------------------------------------------------
void XDocument::Invariant() const
{			
	if (mOnDisk)
		ASSERT(mFileSpec != kInvalidFileSpec);
	else
		ASSERT(mFileSpec == kInvalidFileSpec);	
	
	ASSERT(!msDocuments.empty());

	XBoss* boss = this->GetBoss();
	int32 count = boss->GetRefCount();
	if (count != 0) {						// if count is zero we're being deleted so our parent link has been broken
		IConstHierarchyPtr node(this);
		IConstHierarchyPtr parent = node->GetParent();
		ASSERT(parent);
	}
	
#if DEBUG	
	if (gIntenseDebugging)
		ASSERT(std::find(msDocuments.begin(), msDocuments.end(), this) != msDocuments.end());
#endif
}


//---------------------------------------------------------------
//
// XDocument::DoClose
//
//---------------------------------------------------------------
void XDocument::DoClose()
{	
	PRECONDITION(!mClosing);
	
	// Release our references to our children (we need to do this
	// to break cyclic references).
	IHierarchyPtr node(this);
//	node->RemoveChildren();

	// Post a command that removes 'this' from our parent. If garbage
	// collection is off we'll be deleted in RemoveChild (this is why
	// we use a command: we don't want to be executing XDocument code
	// after we're deleted).
	IHierarchyPtr parent = node->GetParent();
#if 1
	parent->RemoveChild(this);
	
#else
	XCallback1<void, IHierarchyPtr> function(parent.Get(), &IHierarchy::RemoveChild);
	XCallback0<void> proc = Bind1(function, this);	

	ICommandPtr command(L"Callback Command");
	ICallbackCommandPtr callback(command);
	callback->Init(proc);
	
	ICommandQueuePtr queue(L"Application");
	queue->Post(command);
#endif

	mClosing = true;
}


//---------------------------------------------------------------
//
// XDocument::DoSave
//
//---------------------------------------------------------------
void XDocument::DoSave(const XFileSpec& target, OSType creator, OSType type)
{
#if MAC
	OSErr err = noErr;
	
	// If the target file already exists we'll save to a temporary 
	// file to avoid disturbing the original file until everything
	// has been saved successfully. If the target file exists but
	// isn't writable we'll throw an exception (we have to do this
	// explicitly since the File Manager won't check 'ckid' resources).
	XFileSpec temp = target;
	if (XFileSystem::FileExists(target)) {
		if (XFileSystem::IsWritable(target)) {	
			temp = XFileSystem::GetTempFile(target.GetName(),target.GetExtension());
		
		} else {
			int16 refNum;
			err = FSpOpenDF(&target.GetOSSpec(), fsWrPerm, &refNum);
			if (err == noErr) {
				(void) FSClose(refNum);
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"File isn't writable (according to the 'ckid' resource).")));
	
			} else
				ThrowErr(err);
		}
	}
		
	try {
		// If the original file has a resource fork we'll copy
		// the contents to our new file.
		if (temp != target && MFileSystem::HasRsrcFork(target)) {
			XFile srcFile(target);
			XFile dstFile(temp);	
				
			srcFile.Open(kReadPermission, kRsrcFork);
			
			uint32 bytes = srcFile.GetLength();
			XHandle hand(bytes);
			XLocker lock(hand);
			
			srcFile.Read(hand.GetPtr(), bytes);
			srcFile.Close();

			dstFile.Open(creator, type, kWritePermission, kRsrcFork);
			dstFile.Write(hand.GetPtr(), bytes);
			dstFile.Close();
		}
		
		// Let the subclass save the data.
		IPersistDocPtr io(this);
		io->HandleWrite(type, temp);
				
		// We're all done saving so we can swap the old and new files
		// and flush out changes to the disk.
		if (temp != target) {
			OSType creator = temp.GetCreator();
			OSType type    = temp.GetType();
			
			err = FSpExchangeFiles(&temp.GetOSSpec(), &target.GetOSSpec());
			ThrowIfOSErr(err);
			
			MFileSystem::ChangeCreatorType(target, creator, type);	// FSpExchangeFiles doesn't update the file type...
		}

		MFileSystem::FlushVolume(target.GetVolume());

		// The original file can now be deleted.		
		if (temp != target) {
			err = FSpDelete(&temp.GetOSSpec());
			ASSERT(err == noErr);
		}

		// Save the preview.
		try {
			io->SavePreview(target);
		} catch (...) {
		}	

	} catch (...) {
		(void) FSpDelete(&temp.GetOSSpec());
		throw;
	}

#elif WIN
	UNUSED(creator);
	
	if (WSystemInfo::IsWin2000()) {
		// If the target file already exists we'll save to a temporary 
		// file to avoid disturbing the original file until everything
		// has been saved successfully.
		XFileSpec temp = target;
		if (XFileSystem::FileExists(target))
			temp = XFileSystem::GetTempFile(target.GetName(),target.GetExtension());
			
		try {
			// Let the subclass save the data.
			IPersistDocPtr io(this);
			io->HandleWrite(type, temp);
					
			// We're all done saving so we can repalce the old file.
			if (temp != target) {
				int32 succeeded;					
				if (WSystemInfo::HasUnicode())
					succeeded = ::MyReplaceFileW(target.GetPath().c_str(),	// file name
											   temp.GetPath().c_str(),		// replacement file
											   nil,							// optional backup file ($$$ use this? $$$)
											   REPLACEFILE_WRITE_THROUGH,	// replace options
											   nil,							// reserved
											   nil);						// reserved
				else 
					succeeded = ::MyReplaceFileA(ToPlatformStr(target.GetPath()).c_str(),	// file name
											   ToPlatformStr(temp.GetPath()).c_str(),	// replacement file
											   nil,								// optional backup file ($$$ use this? $$$)
											   REPLACEFILE_WRITE_THROUGH,		// replace options
											   nil,								// reserved
											   nil);							// reserved
				ThrowIf(!succeeded);
			}

		} catch (...) {
			if (WSystemInfo::HasUnicode())
				(void) ::DeleteFileW(temp.GetPath().c_str());
			else
				(void) ::DeleteFileA(ToPlatformStr(temp.GetPath()).c_str());
			throw;
		}
	
	} else {
		IPersistDocPtr io(this);
		io->HandleWrite(type, target);
	}
#endif
}


//---------------------------------------------------------------
//
// XDocument::DoGetInitialName
//
//---------------------------------------------------------------
std::wstring XDocument::DoGetInitialName() const
{
	std::wstring initial = LoadWhisperString(L"untitled (default doc name)");
	std::wstring name    = initial;
	
	DocList::iterator iter;
		
	int32 num = 0;
	while (std::find_if(msDocuments.begin(), msDocuments.end(), ZNamesMatch(name)) != msDocuments.end()) {
		num++;
		name = initial + L" " + Int32ToStr(num);
	}
	
	return name;
}


//---------------------------------------------------------------
//
// XDocument::DoBroadcast
//
// Note that this is a rather delicate loop: callbacks may remove
// themselves and possibly even their children as they execute.
// This means that we cannot use iterators and the call to size()
// cannot be moved outside the loop.
//
//---------------------------------------------------------------
void XDocument::DoBroadcast(uint32 message, const XAnyValue& value)
{
	SDocumentMessage mesg(this, message, value);
	
	for (uint32 i = 0; i < mCallbacks.size(); ++i) {
		SCallback callback = mCallbacks[i];
		
		if (callback.change == 0 || callback.change == message)
			callback.callback(mesg);
	}
}


}	// namespace Whisper
