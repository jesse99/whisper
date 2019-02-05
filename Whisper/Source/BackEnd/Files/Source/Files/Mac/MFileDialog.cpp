/*
 *  File:		MFileDialog.cpp
 *  Summary:	File open and save dialogs.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileDialog.cpp,v $
 *		Revision 1.10  2001/05/06 10:37:25  jesjones
 *		Removed some non-Allegro aliaii work arounds.
 *		
 *		Revision 1.9  2001/04/27 04:20:31  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.8  2001/04/21 03:25:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/14 07:35:11  jesjones
 *		Removed MStandardFile.h include.
 *		
 *		Revision 1.6  2001/04/13 07:50:20  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.5  2001/03/06 07:31:35  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.4  2001/02/13 10:16:21  jesjones
 *		No longer uses MDeactivateDesktop (don't need this because our event loop gets invoked).
 *		
 *		Revision 1.3  2000/12/10 04:03:35  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:13:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	 2/06/00	JDJ		Fixed the XSaveReply copy ctor and assignment operator so that mCreator
 *									is correctly copied.
 *		 <8>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <7>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <6>	 5/07/99	JDJ		Types to open and save are specified in XFileDialogOptions using
 *									XFileType objects.
 *		 <5>	 5/03/99	JDJ		AskSaveChanges and AskRevertChanges make use of an options struct.
 *		 <4>	 4/19/99	JDJ		String with 'Ò' are no longer wide strings.
 *		 <3>	 2/18/99	JDJ		Tweaked XFileDialog::DoFilter to work better with Nav Services.
 *		 <2>	 1/20/99	JDJ		Added ChooseFolder overload taking only an XFileDialogOptions.
 *		 <1>	 8/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFileDialog.h>

#include <MDialogUtils.h>
#include <MFileSystem.h>
#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileSpec.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XFolderSpec.h>
#include <XLocker.h>
#include <XNumbers.h>
#include <XTinyVector.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Types
//
#pragma options align=mac68k
struct SKindRecord {
	OSType			appSignature;
	int16			regionCode;
	int16 			filler;
	int16 			numKinds;
	OSType			fileType;
	unsigned char 	fileStr[1];
};
#pragma options align=reset

struct SKindEntry {
	OSType			type;
	std::wstring	name;
};


//-----------------------------------
//	Constants
//
const ResID kSaveChangesAlert   = 131;
const ResID kConfirmRevertAlert = 133;

const int16 kSaveBtn     = 1;
const int16 kCancelBtn   = 2;
const int16 kDontSaveBtn = 3;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//	
// GetTypeName
//
//---------------------------------------------------------------
static std::wstring GetTypeName(OSType theType)
{
	static XTinyVector<SKindEntry> table;
	
	// Build an array of type/name pairs.
	if (table.empty()) {
		Handle hand = GetIndResource('kind', 1);
		
		if (hand != nil) {
			XLocker lock(hand);
			SKindRecord* data = reinterpret_cast<SKindRecord*>(*hand);
			
			table = XTinyVector<SKindEntry>(data->numKinds);
			
			OSType* type = &data->fileType;
			uint8* name = data->fileStr;
			
			for (uint32 index = 0; index < data->numKinds; index++) {				
				table[index].type = *type;
				table[index].name = FromPascalStr(name);
				
				type = reinterpret_cast<OSType*>(name + name[0] + ((name[0] & 1) ? 1 : 2));	// one for length byte and another for padding
				name = reinterpret_cast<uint8*>(type + 1);
			}
				
			ReleaseResource(hand);
			
		} else
			DEBUGSTR("Missing kind resource!");
	}
	
	// Find theType in the table.
	std::wstring theName;
	for (uint32 index = 0; index < table.size() && theName.size() == 0; index++) {
		if (table[index].type == theType)
			theName = table[index].name;
	}
	
	ASSERT(theName.size() > 0);
	
	return theName;
}


//---------------------------------------------------------------
//	
// BuildPopupHandle
//
//---------------------------------------------------------------
static NavMenuItemSpecArrayHandle BuildPopupHandle(const XFileDialogOptions::TypeList& types)
{
	NavMenuItemSpecArrayHandle hand = nil;
	
	if (types.size() > 1) {
		hand = (NavMenuItemSpecArrayHandle) NewHandle(types.size()*sizeof(NavMenuItemSpec));
		if (hand != nil) {
			XLocker lock((Handle) hand, kMoveHigh);
			
			NavMenuItemSpec* specs = *hand;
			
			uint32 index = 0;
			XFileDialogOptions::TypeList::const_iterator iter = types.begin();
			while (iter != types.end()) {
				const XFileType& type = *iter;
				++iter;
				
				specs[index].version     = kNavMenuItemSpecVersion;
				specs[index].menuCreator = type.GetCreator();
				specs[index].menuType    = type.GetType();
				
				std::wstring name = type.GetName();
				if (name.length() == 0)
					name = GetTypeName(specs[index].menuType);	// use 'kind' resource
					
				uint32 length = Min(name.size(), 255UL);
				BlockMoveData(ToPlatformStr(name).c_str(), specs[index].menuItemName + 1, length);
				specs[index].menuItemName[0] = (uint8) length;
				
				++index;
			}
		
		} else
			DEBUGSTR("Not enough memory to build popup handle");
	}
	
	return hand;
}

#pragma mark -

// ===================================================================================
//	class XOpenReply
// ===================================================================================

//---------------------------------------------------------------
//
// XOpenReply::~XOpenReply
//
//---------------------------------------------------------------
XOpenReply::~XOpenReply()
{
	if (mNavReply != nil) {
		ASSERT(*mRefCount > 0);
		
		--(*mRefCount);
		
		if (*mRefCount == 0) {
			if (mNavReply->validRecord) {
				OSErr err = NavDisposeReply(mNavReply);
				ASSERT(err == noErr || err == errAENotAEDesc);		// don't throw! ($$$ 1.0b1 is returning errAENotAEDesc... $$$)
			}
			
			delete mNavReply;
			delete mRefCount;
		}
	}
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply ()
//
//---------------------------------------------------------------
XOpenReply::XOpenReply()
{
	mNavReply = nil;
	mRefCount = nil;
	
	mStdReply.sfGood = false;
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (NavReplyRecord)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const NavReplyRecord& reply)
{
	mNavReply = new NavReplyRecord(reply);
	mRefCount = new int32(1);
	
	mStdReply.sfGood = false;
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (StandardFileReply)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const StandardFileReply& reply)
{
	mNavReply = nil;
	mRefCount = nil;
	
	mStdReply = reply;
}


//---------------------------------------------------------------
//
// XOpenReply::XOpenReply (XOpenReply)
//
//---------------------------------------------------------------
XOpenReply::XOpenReply(const XOpenReply& rhs)
{
	mNavReply = rhs.mNavReply;
	mRefCount = rhs.mRefCount;
	
	mStdReply = rhs.mStdReply;

	if (mRefCount != nil)
		++(*mRefCount);
}


//---------------------------------------------------------------
//
// XOpenReply::operator=
//
//---------------------------------------------------------------
XOpenReply& XOpenReply::operator=(const XOpenReply& rhs)
{
	if (this != &rhs) {
		if (mRefCount != nil) {
			--(*mRefCount);
		
			if (*mRefCount == 0) {
				if (mNavReply->validRecord) {
					OSErr err = NavDisposeReply(mNavReply);
					ASSERT(err == noErr);					// don't throw
				}
				
				delete mNavReply; 
				delete mRefCount;
			}
		}
				
		mNavReply = rhs.mNavReply;
		mRefCount = rhs.mRefCount;
		
		mStdReply = rhs.mStdReply;

		if (mRefCount != nil)
			++(*mRefCount);		
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XOpenReply::FilteredFiles
//
//---------------------------------------------------------------
bool XOpenReply::FilteredFiles() const
{
	return false;			// real OS's don't allow the user to see or select invalid files!
}


//---------------------------------------------------------------
//
// XOpenReply::GetNumItems
//
//---------------------------------------------------------------
uint32 XOpenReply::GetNumItems() const
{
	int32 count = 0;
	
	if (mNavReply != nil) {
		if (mNavReply->validRecord) {
			OSErr err = AECountItems(&(mNavReply->selection), &count);
			ThrowIfOSErr(err);
		}
		
	} else if (mStdReply.sfGood)
		count = 1;
	
	return (uint32) count;
}


//---------------------------------------------------------------
//
// XOpenReply::GetItem
//
//---------------------------------------------------------------
XFileSystemSpec XOpenReply::GetItem(uint32 index) const
{	
	PRECONDITION(index < this->GetNumItems());
	
	FSSpec spec;	

	if (mNavReply != nil) {
		int32 actualSize;
		DescType keyword, typeCode;
		OSErr err = AEGetNthPtr(&mNavReply->selection, (int32) (index + 1), typeFSS, &keyword, &typeCode, &spec, sizeof(FSSpec), &actualSize);
		ThrowIfOSErr(err);

	} else 
		spec = mStdReply.sfFile;
	
	return spec;
}

#pragma mark -

// ===================================================================================
//	class XSaveReply
// ===================================================================================

//---------------------------------------------------------------
//
// XSaveReply::~XSaveReply
//
//---------------------------------------------------------------
XSaveReply::~XSaveReply()
{
	if (mRefCount != nil) {
		PRECONDITION(*mRefCount > 0);
		
		--(*mRefCount);
		
		if (*mRefCount == 0) {
			if (mNavReply->validRecord) {
				OSErr err = NavDisposeReply(mNavReply);
				ASSERT(err == noErr);					// don't throw!
			}
			
			delete mNavReply;
			delete mRefCount;
		}
	}
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply ()
//
//---------------------------------------------------------------
XSaveReply::XSaveReply()
{
	mNavReply = nil;
	mRefCount = nil;
	
	mIsValid   = false;
	mReplacing = false;
	mCreator   = '????';
	mType      = '????';
	
	mStdReply.sfGood = false;
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply (NavReplyRecord)
//
//---------------------------------------------------------------
XSaveReply::XSaveReply(const NavReplyRecord& reply, OSType creator, OSType type)
{
	mNavReply = new NavReplyRecord(reply);
	mRefCount = new int32(1);
	
	mIsValid   = mNavReply->validRecord;
	mReplacing = mNavReply->replacing;
	mCreator   = creator;
	mType      = type;
	
	mStdReply.sfGood = false;
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply (StandardFileReply)
//
//---------------------------------------------------------------
XSaveReply::XSaveReply(const StandardFileReply& reply, OSType creator, OSType type)
{
	mNavReply = nil;
	mRefCount = nil;
	
	mStdReply = reply;
	
	mIsValid   = mStdReply.sfGood;
	mReplacing = mStdReply.sfReplacing;
	mCreator   = creator;
	mType      = type;
}


//---------------------------------------------------------------
//
// XSaveReply::XSaveReply (XSaveReply)
//
//---------------------------------------------------------------
XSaveReply::XSaveReply(const XSaveReply& rhs)
{
	mNavReply = rhs.mNavReply;
	mRefCount = rhs.mRefCount;
	
	mStdReply = rhs.mStdReply;

	mIsValid   = rhs.mIsValid;
	mReplacing = rhs.mReplacing;
	mCreator   = rhs.mCreator;
	mType      = rhs.mType;
	
	if (mRefCount != nil)
		++(*mRefCount);
}


//---------------------------------------------------------------
//
// XSaveReply::operator=
//
//---------------------------------------------------------------
XSaveReply& XSaveReply::operator=(const XSaveReply& rhs)
{
	if (this != &rhs) {
		if (mRefCount != nil) {
			--(*mRefCount);
			
			if (*mRefCount == 0) {
				if (mNavReply->validRecord) {
					OSErr err = NavDisposeReply(mNavReply);
					ASSERT(err == noErr);					// don't throw
				}
				
				delete mNavReply; 
				delete mRefCount;
			}
		}
				
		mNavReply = rhs.mNavReply;
		mRefCount = rhs.mRefCount;
	
		mStdReply = rhs.mStdReply;

		mIsValid   = rhs.mIsValid;
		mReplacing = rhs.mReplacing;
		mCreator   = rhs.mCreator;
		mType      = rhs.mType;
		
		if (mRefCount != nil)
			++(*mRefCount);		
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XSaveReply::GetTarget
//
//---------------------------------------------------------------
XFileSpec XSaveReply::GetTarget() const
{
	PRECONDITION(this->IsValid());
	
	FSSpec spec;	

	if (mNavReply != nil) {
		int32 actualSize;
		DescType keyword, typeCode;
		OSErr err = AEGetNthPtr(&mNavReply->selection, 1, typeFSS, &keyword, &typeCode, &spec, sizeof(FSSpec), &actualSize);
		ThrowIfOSErr(err);

	} else
		spec = mStdReply.sfFile;
	
	return spec;
}

#pragma mark -

// ===================================================================================
//	struct XFileDialogOptions
// ===================================================================================

//---------------------------------------------------------------
//
// XFileDialogOptions::XFileDialogOptions
//
//---------------------------------------------------------------
XFileDialogOptions::XFileDialogOptions()
{
	if (MSystemInfo::HasNavServices()) {
		OSErr err = NavGetDefaultDialogOptions(this);
		ThrowIfOSErr(err);
	
	} else {
		std::memset(this, 0, sizeof(*this));

		location.h = -1;
		location.v = -1;
	}
	
	dialogOptionFlags = kNavDontAddTranslateItems +
						kNavAllowMultipleFiles + 
						kNavAllowPreviews + 
						kNavAllowStationery;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetPrefKey
//
//---------------------------------------------------------------
void XFileDialogOptions::SetPrefKey(uint32 key)
{
	preferenceKey = key;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetAppName
//
//---------------------------------------------------------------
void XFileDialogOptions::SetAppName(const std::wstring& name)
{
	uint32 length = Min(name.size(), sizeof(clientName) - 1);
	BlockMoveData(ToPlatformStr(name).c_str(), clientName + 1, length);
	clientName[0] = (uint8) length;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetDefaultFileName
//
//---------------------------------------------------------------
void XFileDialogOptions::SetDefaultFileName(const std::wstring& name)
{
	uint32 length = Min(name.size(), sizeof(savedFileName) - 1);
	BlockMoveData(ToPlatformStr(name).c_str(), savedFileName + 1, length);
	savedFileName[0] = (uint8) length;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::AddType
//
//---------------------------------------------------------------
void XFileDialogOptions::AddType(const XFileType& type)
{
	mTypes.push_back(type);
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetMessage
//
//---------------------------------------------------------------
void XFileDialogOptions::SetMessage(const std::wstring& mesg)
{
	uint32 length = Min(mesg.size(), sizeof(message) - 1);
	BlockMoveData(ToPlatformStr(mesg).c_str(), message + 1, length);
	message[0] = (uint8) length;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetActionButtonLabel
//
//---------------------------------------------------------------
void XFileDialogOptions::SetActionButtonLabel(const std::wstring& mesg)
{
	uint32 length = Min(mesg.size(), sizeof(actionButtonLabel) - 1);
	BlockMoveData(ToPlatformStr(mesg).c_str(), actionButtonLabel + 1, length);
	actionButtonLabel[0] = (uint8) length;
}


//---------------------------------------------------------------
//
// XFileDialogOptions::SetCancelButtonLabel
//
//---------------------------------------------------------------
void XFileDialogOptions::SetCancelButtonLabel(const std::wstring& mesg)
{
	uint32 length = Min(mesg.size(), sizeof(cancelButtonLabel) - 1);
	BlockMoveData(ToPlatformStr(mesg).c_str(), cancelButtonLabel + 1, length);
	cancelButtonLabel[0] = (uint8) length;
}

#pragma mark -

// ===================================================================================
//	class XFileDialog
// ===================================================================================

const XFileFilter*				XFileDialog::msFileFilter    = nil;
const XFolderFilter*			XFileDialog::mFolderFilter   = nil;
XFileDialog::NavEventHandler	XFileDialog::msEventCallback = nil;

OSType							XFileDialog::msSelectedCreator = '????';
OSType							XFileDialog::msSelectedType    = '????';

NavEventUPP						XFileDialog::msEventsUPP = nil;
NavObjectFilterUPP				XFileDialog::msFilterUPP = nil;

ModalFilterProcPtr				XFileDialog::msModalFilter = nil;

//---------------------------------------------------------------
//
// XFileDialog::SaveFile 								[static]
//
//---------------------------------------------------------------
XSaveReply XFileDialog::SaveFile(const XFileDialogOptions& options)
{
	XSaveReply reply;
		
	const XFileDialogOptions::TypeList& types = options.GetTypes();
	PRECONDITION(types.size() > 0);
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		const_cast<XFileDialogOptions&>(options).popupExtension = BuildPopupHandle(types);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
		
		msSelectedCreator = types.front().GetCreator();
		msSelectedType    = types.front().GetType();// DoEvents callback will change this if user selects a new type in format popup
				
		SetPort(Whisper::GetWorkPort());			// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavPutFile(nil,					// use system's default location
							   &navReply,
							   const_cast<XFileDialogOptions*>(&options),
							   msEventsUPP,
							   msSelectedType,		// first type will be the default
							   msSelectedCreator,
							   nil);
		
		if (options.popupExtension != nil) {
			DisposeHandle((Handle) options.popupExtension);
			const_cast<XFileDialogOptions&>(options).popupExtension = nil;
		}
			
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);

		reply = XSaveReply(navReply, msSelectedCreator, msSelectedType);

	} else {
		DEBUGSTR("How'd we wind up here?");
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::OpenFile (XFileDialogOptions) 			[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::OpenFile(const XFileDialogOptions& options)
{
	XOpenReply reply;
	
	const XFileDialogOptions::TypeList& types = options.GetTypes();
	PRECONDITION(types.size() > 0);
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = nil;
		mFolderFilter = nil;
		
		XHandle hand(sizeof(NavTypeList) + (types.size() - 1)*sizeof(OSType));
		XLocker lock(hand);							
		
		NavTypeList** typesH = (NavTypeList**) hand.GetHandle();
		(**typesH).componentSignature = XFileSpec(MSystemInfo::GetAppSpec()).GetCreator();
		(**typesH).reserved           = 0;
		(**typesH).osTypeCount        = numeric_cast<int16>(types.size());
		
		uint32 i = 0;
		XFileDialogOptions::TypeList::const_iterator iter = types.begin();
		while (iter != types.end()) {
			const XFileType& type = *iter;
			++iter;
			
			(**typesH).osType[i++] = type.GetType();
		}
		
		SetPort(Whisper::GetWorkPort());			// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavGetFile(nil,					// use system's default location
							   &navReply,
							   const_cast<XFileDialogOptions*>(&options),
							   msEventsUPP,
							   nil,					// previewProc		
							   msFilterUPP,			// filterProc
							   typesH, 				// typeList
							   0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else 
		DEBUGSTR("How'd we wind up here?");

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::OpenFile (XFileDialogOptions, XFileFilter) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::OpenFile(const XFileDialogOptions& options, const XFileFilter& fileFilter)
{
	XOpenReply reply;
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = &fileFilter;
		mFolderFilter = nil;
		
		const XFileDialogOptions::TypeList& types = options.GetTypes();
		PRECONDITION(types.size() > 0);
	
		XHandle hand(sizeof(NavTypeList) + (types.size() - 1)*sizeof(OSType));
		XLocker lock(hand);							
		
		NavTypeList** typesH = (NavTypeList**) hand.GetHandle();
		(**typesH).componentSignature = XFileSpec(MSystemInfo::GetAppSpec()).GetCreator();
		(**typesH).reserved           = 0;
		(**typesH).osTypeCount        = numeric_cast<int16>(types.size());
		
		uint32 i = 0;
		XFileDialogOptions::TypeList::const_iterator iter = types.begin();
		while (iter != types.end()) {
			const XFileType& type = *iter;
			++iter;
			
			(**typesH).osType[i++] = type.GetType();
		}
		
		SetPort(Whisper::GetWorkPort());			// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavGetFile(nil,					// use system's default location
							   &navReply,
							   const_cast<XFileDialogOptions*>(&options),
							   msEventsUPP,
							   nil,					// previewProc		
							   msFilterUPP,			// filterProc
							   typesH,				// typeList
							   0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else 
		DEBUGSTR("How'd we wind up here?");

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFile (XFileDialogOptions) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFile(const XFileDialogOptions& options)
{
	XOpenReply reply;
	
	const XFileDialogOptions::TypeList& types = options.GetTypes();
	PRECONDITION(types.size() > 0);
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = nil;
		mFolderFilter = nil;
		
		XHandle hand(sizeof(NavTypeList) + (types.size() - 1)*sizeof(OSType));
		XLocker lock(hand);							// docs don't say this is neccesary, but sample code locks...
		
		NavTypeList** typesH = (NavTypeList**) hand.GetHandle();
		(**typesH).componentSignature = XFileSpec(MSystemInfo::GetAppSpec()).GetCreator();
		(**typesH).reserved           = 0;
		(**typesH).osTypeCount        = numeric_cast<int16>(types.size());
		
		uint32 i = 0;
		XFileDialogOptions::TypeList::const_iterator iter = types.begin();
		while (iter != types.end()) {
			const XFileType& type = *iter;
			++iter;
			
			(**typesH).osType[i++] = type.GetType();
		}
		
		SetPort(Whisper::GetWorkPort());				// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavChooseFile(nil,				// use system's default location
								  &navReply,
								  const_cast<XFileDialogOptions*>(&options),
								  msEventsUPP,
								  nil,				// previewProc
								  msFilterUPP,		// filterProc
								  typesH,			// typeList
								  0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else 
		DEBUGSTR("How'd we wind up here?");

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFile (XFileDialogOptions, XFileFilter) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFile(const XFileDialogOptions& options, const XFileFilter& fileFilter)
{
	XOpenReply reply;
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = &fileFilter;
		mFolderFilter = nil;
		
		SetPort(Whisper::GetWorkPort());				// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavChooseFile(nil,				// use system's default location
								  &navReply,
								  const_cast<XFileDialogOptions*>(&options),
								  msEventsUPP,
								  nil,				// previewProc
								  msFilterUPP,		// filterProc
								  nil,				// typeList
								  0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else {
		DEBUGSTR("How'd we wind up here?");
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseObject (XFileDialogOptions) 		[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseObject(const XFileDialogOptions& options)
{
	XOpenReply reply;
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
					
		SetPort(Whisper::GetWorkPort());				// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavChooseObject(nil,				// use system's default location
								    &navReply,
								    const_cast<XFileDialogOptions*>(&options),
								    msEventsUPP,
								    nil,				// filterProc
								    0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else {
		DEBUGSTR("How'd we wind up here?");
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseObject (XFileDialogOptions, XFileFilter, XFolderFilter) [static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseObject(const XFileDialogOptions& options, const XFileFilter& fileFilter, const XFolderFilter& folderFilter)
{
	XOpenReply reply;
	
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = &fileFilter;
		mFolderFilter = &folderFilter;
		
		SetPort(Whisper::GetWorkPort());				// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavChooseObject(nil,				// use system's default location
								    &navReply,
								    const_cast<XFileDialogOptions*>(&options),
								    msEventsUPP,
								    msFilterUPP,		// filterProc
								    0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else {
		DEBUGSTR("How'd we wind up here?");
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFolder (XFileDialogOptions) 		[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFolder(const XFileDialogOptions& options)
{
	XFolderFilter filter;

	return XFileDialog::ChooseFolder(options, filter);
}


//---------------------------------------------------------------
//
// XFileDialog::ChooseFolder (XFileDialogOptions, XFolderFilter) 	[static]
//
//---------------------------------------------------------------
XOpenReply XFileDialog::ChooseFolder(const XFileDialogOptions& options, const XFolderFilter& folderFilter)
{
	XOpenReply reply;
	
	if (MSystemInfo::HasNavServices()) {
//		ASSERT(msEventCallback != nil);					// isn't true when in XApplication::OnLocateDataFolders...
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		msFileFilter  = nil;
		mFolderFilter = &folderFilter;
		
		SetPort(Whisper::GetWorkPort());				// NavGetFile 1.0f4c3 will crash if port is bad...

		NavReplyRecord navReply;
		OSErr err = NavChooseFolder(nil,				// use system's default location
									&navReply,
									const_cast<XFileDialogOptions*>(&options),
									msEventsUPP,
								  	msFilterUPP,		// filterProc
								  	0UL);				// callBackUD
		if (err != noErr && err != userCanceledErr)
			ThrowErr(err);
			
		reply = navReply;

	} else {
		DEBUGSTR("How'd we wind up here?");
	}

	return reply;
}


//---------------------------------------------------------------
//
// XFileDialog::AskSaveChanges							[static]
//
//---------------------------------------------------------------
EAskSaveResult XFileDialog::AskSaveChanges(const std::wstring& docName, ESaveChanges action)
{
	PRECONDITION(msModalFilter != nil);
		
	InitCursor();
	
	EAskSaveResult result = kCancelSave;

	OSErr err = noErr;
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		SetPort(Whisper::GetWorkPort());			// NavGetFile 1.0f4c3 will crash if port is bad...

		NavAskSaveChangesResult reply;

		XFileDialogOptions options;
		options.SetDefaultFileName(docName);
		err = NavAskSaveChanges(&options,	
								action,
								&reply,
								msEventsUPP,
								0UL);				// callBackUD
		ASSERT(err == noErr);						// seems kind of pointless to throw

		result = (EAskSaveResult) reply;
		
	} else {
		int16 item = kSaveBtn;
		
		MCreateModalFilterUPP theFilter(msModalFilter);

		if (MSystemInfo::HasAppearanceMgr()) {
			std::wstring defaultText = LoadWhisperString(L"Save (Button)");		// need to keep these in locals so they're not deleted before StandardAlert completes
			std::wstring cancelText  = LoadWhisperString(L"Cancel (Button)");
			std::wstring otherText   = LoadWhisperString(L"Don't Save (Button)");
			
			PascalString defaultStr = ToPascalStr(defaultText);
			PascalString cancelStr = ToPascalStr(cancelText);
			PascalString otherStr = ToPascalStr(otherText);

			AlertStdAlertParamRec params;
			params.movable       = true;
			params.helpButton    = false;
			params.filterProc    = theFilter;
			params.defaultText   = (unsigned char*) defaultStr.c_str();		
			params.cancelText    = (unsigned char*) cancelStr.c_str();
			params.otherText     = (unsigned char*) otherStr.c_str();
			params.defaultButton = kSaveBtn;
			params.cancelButton  = kCancelBtn;
			params.position      = kWindowAlertPositionParentWindowScreen;

			std::wstring errorStr = LoadWhisperString(L"File Ò#1Ó not saved.", docName);
			std::wstring supplementalStr = LoadWhisperString(L"Changes made to the file Ò#1Ó will be discarded if this file is not saved. Do you want to save changes to this file before closing it?", docName);
			
			err = StandardAlert(kAlertStopAlert, (unsigned char*) ToPascalStr(errorStr).c_str(), (unsigned char*) ToPascalStr(supplementalStr).c_str(), &params, &item);
			ASSERT(err == noErr);						// seems kind of pointless to throw

		} else {
			ParamText(ToPascalStr(docName).c_str(), "\p", "\p", "\p");

			item = CautionAlert(kSaveChangesAlert, theFilter);
		}
		
		if (item == kSaveBtn)
			result = kSaveChanges;
		else if (item == kDontSaveBtn)
			result = kDontSave;
	}

	return result;
}


//---------------------------------------------------------------
//
// XFileDialog::AskRevertChanges						[static]
//
//---------------------------------------------------------------
ERevertResult XFileDialog::AskRevertChanges(const std::wstring& docName)
{
	PRECONDITION(msModalFilter != nil);
	
	ERevertResult result = kCancelRevert;
	
	OSErr err = noErr;
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventCallback != nil);
		
		if (msEventsUPP == nil)
			XFileDialog::DoInit();
			
		SetPort(Whisper::GetWorkPort());			// NavGetFile 1.0f4c3 will crash if port is bad...

		NavAskDiscardChangesResult reply;

		XFileDialogOptions options;
		options.SetDefaultFileName(docName);
		err = NavAskDiscardChanges(&options,		// use system's default location
								   &reply,
								   msEventsUPP,
								   0UL);			// callBackUD
		ASSERT(err == noErr);						// seems kind of pointless to throw

		result = (ERevertResult) reply;

	} else {
		int16 item = cancel;
		
		MCreateModalFilterUPP theFilter(msModalFilter);

		if (MSystemInfo::HasAppearanceMgr()) {
			AlertStdAlertParamRec params;
			params.movable       = true;
			params.helpButton    = false; 
			params.filterProc    = theFilter;
			params.defaultText   = (StringPtr) -1L;		// use default (ie "OK")
			params.cancelText    = (StringPtr) -1L;		// use default (ie "Cancel")
			params.otherText     = nil; 
			params.defaultButton = cancel;
			params.cancelButton  = 0;
			params.position      = kWindowAlertPositionParentWindowScreen;

			std::wstring errorStr = LoadWhisperString(L"Revert changes to file Ò#1Ó.", docName);
			std::wstring supplementalStr = LoadWhisperString(L"Reverting file Ò#1Ó will discard all changes made to this file since you last saved it. Do you want to discard all changes to this file?", docName);
			
			err = StandardAlert(kAlertStopAlert, (unsigned char*) ToPascalStr(errorStr).c_str(), (unsigned char*) ToPascalStr(supplementalStr).c_str(), &params, &item);
			ASSERT(err == noErr);						// seems kind of pointless to throw

		} else {
			ParamText(ToPascalStr(docName).c_str(), "\p", "\p", "\p");

			item = CautionAlert(kConfirmRevertAlert, theFilter);
		}
			
		if (item == ok)
			result = kDiscardChanges;
	}

	return result;
}

#pragma mark ~

//---------------------------------------------------------------
//
// XFileDialog::DoInit									[static]
//
//---------------------------------------------------------------
void XFileDialog::DoInit()
{
	if (MSystemInfo::HasNavServices()) {
		ASSERT(msEventsUPP == nil);
		ASSERT(msFilterUPP == nil);
		
		msEventsUPP = NewNavEventUPP(XFileDialog::DoEvents);	
		ThrowIfMemFail(msEventsUPP);
		
		msFilterUPP = NewNavObjectFilterUPP(XFileDialog::DoFilter);	
		ThrowIfMemFail(msFilterUPP);
	}
}


//---------------------------------------------------------------
//
// XFileDialog::DoEvents								[static]
//
// Note that we don't ASSERT that msEventCallback is non-nil here
// to avoid a cascade of ASSERTions.
//
//---------------------------------------------------------------
pascal void XFileDialog::DoEvents(const NavEventCallbackMessage selector, NavCBRecPtr params, NavCallBackUserData refCon)
{
	#pragma unused(refCon)
	
	try {
		if (selector == kNavCBEvent) {		
			const EventRecord& event = *(params->eventData.eventDataParms.event);
			
			if (event.what == nullEvent || event.what == updateEvt || event.what == activateEvt) {
				if (msEventCallback != nil) {
					GrafPtr oldPort;
					GetPort(&oldPort);
			
					msEventCallback(event);
			
					SetPort(oldPort);
				}
			}
			
		} else if (selector == kNavCBPopupMenuSelect) {
			NavMenuItemSpec* spec = reinterpret_cast<NavMenuItemSpec*>(params->eventData.eventDataParms.param);
			ASSERT(spec != nil);
			
			msSelectedCreator = spec->menuCreator;
			msSelectedType    = spec->menuType;
		}
	
	} catch (...) {
	}
}


//---------------------------------------------------------------
//
// XFileDialog::DoFilter								[static]
//
//---------------------------------------------------------------
pascal Boolean XFileDialog::DoFilter(AEDesc* item, void* inInfo, NavCallBackUserData refCon, NavFilterModes filterMode)
{
	#pragma unused(refCon, filterMode)
	
	PRECONDITION(item != nil);
//	PRECONDITION(mFolderFilter != nil || msFileFilter != nil);

	bool display = true;
	
	try {
		if (item->descriptorType == typeFSS) {			// $$$ this code seems to work OK, but it'd probably be better to coerce the desc to an FSSPec	
			NavFileOrFolderInfo* info = reinterpret_cast<NavFileOrFolderInfo*>(inInfo);

			if (info->visible) {
				ASSERT(item->dataHandle != nil);
			
				// Get the spec
				FSSpec spec;						
				VERIFY(::AEGetDescData(item, &spec, sizeof(spec)) == noErr);	// type is typeFSS so there shouldn't be any errors...

				bool isFolder = info->isFolder;
				
				// If it's an alias file, resolve it (note that we resolve the alias ourselves
				// if we're running on System 8. We do this because Nav Services keeps a few
				// hundred aliases in its Recent Items folder and these aliases can easily
				// become broken if the original file is thrown away. When this happens
				// ResolveSpec will throw which doesn't cause any problems, but can be very
				// annoying if the "Break on C++ Exceptions" debugger flag is enabled).
				if ((info->fileAndFolder.fileInfo.finderInfo.fdFlags & kAliasBit) != 0) {
					Boolean wasFolder, wasAliased;		
					OSErr err = ::ResolveAliasFileWithMountFlags(&spec, true, &wasFolder, &wasAliased, kResolveAliasFileNoUI);
					display = err == noErr;
					isFolder = wasFolder;
				}
				
				// Filter out files
				if (display)
					if (isFolder && mFolderFilter != nil)
						display = mFolderFilter->operator()(spec);
						
					else if (!isFolder && msFileFilter != nil)
						display = msFileFilter->operator()(spec);
			}
		}
		
	} catch (...) {
		display = false;
	}
	
	return display;
}


}	// namespace Whisper

