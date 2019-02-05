/*
 *  File:		XFileDialog.h
 *  Summary:	File open and save dialogs.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998--2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileDialog.h,v $
 *		Revision 1.4  2001/02/21 08:11:52  jesjones
 *		No longer includes <shlobj.h> (to avoid pulling in an absolutely ridiculuous number of lines of code).
 *		
 *		Revision 1.3  2000/12/10 08:52:01  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:35:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/21/99	JDJ		Added XFileDialogOptions::SetDefaultFolder (Win). 
 *		 <3>	 5/07/99	JDJ		Types to open and save are specified in XFileDialogOptions using
 *									XFileType objects.
 *		 <2>	 1/20/99	JDJ		Added ChooseFolder overload taking only an XFileDialogOptions.
 *		 <1>	 8/10/98	JDJ		Created
 */

#pragma once

#include <vector>

#include <XFileType.h>
#include <XTranscode.h>

#if MAC
	#include <Navigation.h>

#elif WIN
	#include <XFileSpec.h>

	struct _ITEMIDLIST;			// in order to forward reference we have to use the sucky struct names instead of the (slightly less sucky) typedefs
	struct tagOFNA;				// note that avoiding the <shlobj.h> include saves all of our clients from including around 400K lines of code...
	struct tagOFNW;
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;
class XFileFilter;
class XFileSystemSpec;
class XFolderSpec;
class XFolderFilter;


//-----------------------------------
//	Types
//
#if MAC
	typedef NavDialogOptions OSFileDialogOptions;
#elif WIN
	typedef tagOFNW OSFileDialogOptions;			// on Win95 the string pointers will point to ANSI strings...
#endif


//-----------------------------------
//	Constants
//
enum ESaveChanges {
	kGenericSave		 = 0,
	kClosingDocument 	 = 1,
	kQuittingApplication = 2
};

enum EAskSaveResult {
	kSaveChanges	= 1,
	kCancelSave		= 2,
	kDontSave		= 3
};

enum ERevertResult {
	kDiscardChanges	= 1,
	kCancelRevert	= 2
};


// ===================================================================================
//	class XOpenReply
//!		Returned by XFileDialog when selecting a file or folder to be read.
// ===================================================================================
class FILES_EXPORT XOpenReply {

//-----------------------------------
//	Initialization/Destruction
//
public:	
						~XOpenReply();

						XOpenReply();

#if WIN
						XOpenReply(const tagOFNA* reply, bool succeeded);
						XOpenReply(const tagOFNA* reply, bool succeeded, const XFileFilter& fileFilter);

						XOpenReply(const tagOFNW* reply, bool succeeded);
						XOpenReply(const tagOFNW* reply, bool succeeded, const XFileFilter& fileFilter);
						
						XOpenReply(_ITEMIDLIST* reply, const XFolderFilter& folderFilter);
#elif MAC
						XOpenReply(const NavReplyRecord& reply);

						XOpenReply(const StandardFileReply& reply);
						
						XOpenReply(const XOpenReply& rhs);
						
			XOpenReply& operator=(const XOpenReply& rhs);
#endif

//-----------------------------------
//	API
//
public:
			bool		FilteredFiles() const;
						/**< Returns true if one or more of the files the user selected
						were filtered out. This will happen only on Windows and
						only if you use an XFileFilter subclass (Win95 only supports
						filtering via file extensions). */

			uint32 		GetNumItems() const;
						/**< Returns the number of items the user wants to open. Note that
						this will return zero if the user canceled out of the dialog
						or all the files he selected were filtered out (Windows). */
						
			XFileSystemSpec GetItem(uint32 index) const;
									
#if MAC
			bool		WasTranslated() const				{return mNavReply != nil && mNavReply->translationNeeded;}
						/**< Returns true if the file will be translated (by Easy Open). */
#endif	

//-----------------------------------
//	Member Data
//
protected:		
#if MAC
	NavReplyRecord*					mNavReply;
	int32*							mRefCount;
	
	StandardFileReply				mStdReply;

#elif WIN
	bool							mFiltered;
	std::vector<XFileSystemSpec>	mSpecs;
#endif
};


// ===================================================================================
//	class XSaveReply
//!		Returned by XFileDialog when selecting a file to be written to.
// ===================================================================================
class FILES_EXPORT XSaveReply {

//-----------------------------------
//	Initialization/Destruction
//
public:	
						~XSaveReply();

						XSaveReply();

#if WIN
						XSaveReply(const tagOFNA* reply, bool succeeded);

						XSaveReply(const tagOFNW* reply, bool succeeded);
#elif MAC
						XSaveReply(const NavReplyRecord& reply, OSType creator, OSType type);

						XSaveReply(const StandardFileReply& reply, OSType creator, OSType type);
						
						XSaveReply(const XSaveReply& rhs);
						
			XSaveReply& operator=(const XSaveReply& rhs);
#endif

//-----------------------------------
//	API
//
public:
			bool		IsValid() const							{return mIsValid;}
						/**< Returns false if user canceled. */
						
			bool		IsReplacing() const						{return mReplacing;}
						/**< Returns true if user is overwriting an existing file. */
			
			XFileSpec 	GetTarget() const;
						/**< Returns a spec identifying where to save the file. */
						
#if MAC
			OSType		GetCreator() const						{return mCreator;}
						/**< Returns the creator the user wants to save the file with. */
						
			OSType		GetType() const							{return mType;}
						/**< Returns the format the user wants to save the file as. */
						
			bool		IsStationery() const					{return mNavReply != nil && mNavReply->isStationery;}
						/**< Returns true if user wants to save the file as stationary. */
			
			bool		NeedsTranslation() const				{return mNavReply != nil && mNavReply->translationNeeded;}
						/**< Returns true if the file needs to be translated (this is
						normally automatic). */
#endif

//-----------------------------------
//	Member Data
//
protected:	
	bool				mIsValid;
	bool				mReplacing;

#if MAC
	OSType				mCreator;
	OSType				mType;

	NavReplyRecord*		mNavReply;
	int32*				mRefCount;
	
	StandardFileReply	mStdReply;
	
#elif WIN
	XFileSpec			mSpec;
#endif
};


// ===================================================================================
//	class XFileDialogOptions
//!		XFileDialog options
// ===================================================================================
class FILES_EXPORT XFileDialogOptions : public OSFileDialogOptions {
		
//-----------------------------------
//	Initialization/Destruction
//
public:
						XFileDialogOptions();
						/**< On the Mac this will call NavGetDefaultDialogOptions.
						dialogOptionFlags defaults to kNavDontAddTranslateItems + 
						kNavAllowMultipleFiles + kNavAllowPreviews + kNavAllowStationery.
						On Windows Flags defaults to OFN_ALLOWMULTISELECT + OFN_EXPLORER + 
						OFN_FILEMUSTEXIST + OFN_PATHMUSTEXIST + OFN_OVERWRITEPROMPT. */
						
private:
						XFileDialogOptions(const XFileDialogOptions& rhs);
			XFileDialogOptions& operator=(const XFileDialogOptions& rhs);
						// $$$ Enable these? Would have to do some work on Windows...
						
//-----------------------------------
//	API
//
public:
			void 		SetAppName(const std::wstring& name);
						/**< Note that not all of these are used on Windows or with Standard File. */
			
			void 		SetDefaultFileName(const std::wstring& name);
			
			void 		SetMessage(const std::wstring& mesg);

			void 		SetActionButtonLabel(const std::wstring& mesg);
			void 		SetCancelButtonLabel(const std::wstring& mesg);
			
			void 		SetPrefKey(uint32 key);
						/**< Some OS's may save state information across calls to the file
						dialogs (eg which file was selected last). Normally this is saved
						on a per app basis, but you can use this function if you want to
						seperately save this information for one or more dialogs. The key
						defaults to zero. */
						
			void 		AddType(const XFileType& type);
						/**< Use this to list the file types the user can open or the types
						the user can save with. The first type added is assumed to be
						the default. */
			
#if WIN			
			void 		SetDefaultExtension(const std::wstring& extension);
			
			void 		SetDefaultFolder(const XFolderSpec& folder);
						/**< The first time XFileDialogOptions is used with a particular
						key the starting folder will default to "My Documents". If the
						user doesn't cancel the next time XFileDialogOptions is used
						with the pref key the starting folder will be the folder the
						user last used. If you don't want to default to "My Documents"
						you can use this function. Note that none of this works in NT 4. */
									
			uint32		GetPrefKey() const					{return mPrefKey;}
#endif
					
//-----------------------------------
//	Internal API
//
public:
	typedef std::vector<XFileType> TypeList;
	
			const TypeList& GetTypes() const				{return mTypes;}

//-----------------------------------
//	Member Data
//
protected:
	TypeList		mTypes;
	
#if WIN
	std::wstring	mTitle;
	wchar_t			mFileName[4096];		// use a C-array since Windows will write to it (this is a large buffer because of the lame way Windows handles opening multiple files)
	std::wstring	mDefaultExtension;
	std::wstring	mExtensionList;

	PlatformString	mTitleStr;
	PlatformString	mExtStr;
	PlatformString	mDefExtStr;

	wchar_t			mInitialFilter[256];
	wchar_t			mInitialDir[MAX_PATH+1];

	uint32			mPrefKey;
	std::wstring	mDefaultPath;
#endif
};


// ===================================================================================
//	class XFileDialog
//!		Wrapper around static functions used to display file related dialogs.
// ===================================================================================
class FILES_EXPORT XFileDialog {

//-----------------------------------
//	API
//
public:
	//! @name Saving
	//@{
	static 	XSaveReply		SaveFile(const XFileDialogOptions& options);
							/**< If Standard File is used (on a Mac) the file will be saved 
							using the first file type. */
	//@}

	//! @name Opening a file
	//@{
	static 	XOpenReply		OpenFile(const XFileDialogOptions& options);	
	static 	XOpenReply		OpenFile(const XFileDialogOptions& options, const XFileFilter& fileFilter);	
							/**< On the Mac this defaults to auto translating files (for Nav Services). */
	//@}

	//! @name Selecting
	//@{
	static 	XOpenReply		ChooseFile(const XFileDialogOptions& options);
	static 	XOpenReply		ChooseFile(const XFileDialogOptions& options, const XFileFilter& fileFilter);
							/**< Simpler open file dialog (eg doesn't do any translation).
							Use this for things like finding data files. (This is the
							same as OpenFile on Windows). */

	static 	XOpenReply		ChooseFolder(const XFileDialogOptions& options);
	static 	XOpenReply		ChooseFolder(const XFileDialogOptions& options, const XFolderFilter& folderFilter);
							
#if MAC
	static 	XOpenReply		ChooseObject(const XFileDialogOptions& options);
	static 	XOpenReply		ChooseObject(const XFileDialogOptions& options, const XFileFilter& fileFilter, const XFolderFilter& folderFilter);
							/**< Use this to pick a file or folder. */
							// $$$ This can be implemented on Windows using SHBrowseForFolder and the
							// $$$ BIF_BROWSEINCLUDEFILES flag, but as far as I can tell this works with
							// $$$ Win98, but not Win95(?) or NT 4. Of course I can't find any additional
							// $$$ info in Microsoft's crappy docs...
#endif
	//@}

	//! @name Changes
	//@{
	static	EAskSaveResult 	AskSaveChanges(const std::wstring& docName, ESaveChanges action);
							/**< Ask's the user if he wants to save any changes before closing the
							document. action can be kClosingDocument, kQuittingApplication, or
							kGenericSave. Returns kSaveChanges, kCancelSave, or kDontSave. */

	static	ERevertResult 	AskRevertChanges(const std::wstring& docName);
							/**< Asks the user if he wants to discard changes to the document
							(eg for Revert). Returns kDiscardChanges or kCancelDiscard. */
	//@}

#if MAC
	//! @name Initialization
	//@{
	typedef void (*NavEventHandler)(const EventRecord& event);

	static 	void 			SetEventCallback(NavEventHandler callback)		{msEventCallback = callback;}
							/**< In order to handle update events correctly you need to 
							install a callback (it will also get null events). */

	static 	void 			SetModalFilter(ModalFilterProcPtr filter)		{msModalFilter = filter;}
							/**< Needed by AskSaveChanges and AskDiscardChanges (these will
							use a movable modal if Nav Services isn't present so we
							need a filter to process update events). */
#endif
	//@}

//-----------------------------------
//	Internal API
//
protected:
#if MAC
	static	void 			DoInit();
	
	static	pascal void 	DoEvents(const NavEventCallbackMessage selector, NavCBRecPtr params, NavCallBackUserData refCon);

	static	pascal Boolean 	DoFilter(AEDesc* item, void* info, NavCallBackUserData refCon, NavFilterModes filterMode);
#elif WIN

	static	int CALLBACK	DoFolderCallback(HWND wind, UINT message, LPARAM param, LPARAM refCon);

	static	int CALLBACK	DoFileFolderCallback(HWND wind, UINT message, LPARAM param, LPARAM refCon);

#endif

//-----------------------------------
//	Member Data
//
protected:
#if MAC
	static const XFileFilter*	msFileFilter;	
	static const XFolderFilter*	mFolderFilter;
	static NavEventHandler		msEventCallback;
	
	static OSType				msSelectedCreator;
	static OSType				msSelectedType;
	
	static NavEventUPP			msEventsUPP;
	static NavObjectFilterUPP	msFilterUPP;
	
	static ModalFilterProcPtr	msModalFilter;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

