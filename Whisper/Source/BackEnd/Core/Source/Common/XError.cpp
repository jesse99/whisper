/*
 *  File:       XError.cpp
 *  Summary:   	Singleton used to manage errors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XError.cpp,v $
 *		Revision 1.5  2001/04/27 09:20:39  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/27 04:25:37  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2000/11/20 05:31:35  jesjones
 *		XCallback uses IsValid again.
 *		
 *		Revision 1.2  2000/11/09 12:34:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XError.h>

#include <XExceptions.h>
#include <XStringUtils.h>

#if WIN
	#include <WSystemInfo.h>
#elif MAC
	#include <Dialogs.h>
	#include <NumberFormatting.h>
	#include <MSystemInfo.h>
	#include <XLocker.h>
#endif

namespace Whisper {


//-----------------------------------
//	Types
//
#if DEBUG && MAC
	#pragma options align=mac68k
	
	struct SXEntry {
		int32	code;
		Str255	str;
	};
	
	struct SXTable {					// 'STRX' resource format
		int32	numEntries;
		SXEntry	entries[1];
	};
		
	#pragma options align=reset
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// NextEntry
//
// We have to do this grody pointer arithmetic because ResEdit 
// only saves the characters that are actually used by the string.
//
//---------------------------------------------------------------
#if DEBUG && MAC
static const SXEntry* NextEntry(const SXEntry* entry)
{
	const uint8* ptr = reinterpret_cast<const uint8*>(entry);
	
	ptr += sizeof(int32) + sizeof(char) + entry->str[0];
	int32 delta = (int32) ptr & 3;				// // ResEdit will add pad bytes to keep token number aligned.
	if (delta > 0)
		ptr += 4 - delta;
	
	return reinterpret_cast<const SXEntry*>(ptr);
}
#endif


//---------------------------------------------------------------
//
// LookUpString
//
//---------------------------------------------------------------
#if DEBUG && MAC
static std::wstring LookUpString(int32 code)
{
	std::wstring str;
	
	SXTable** table = (SXTable **) GetResource('STRX', 203);	
	if (table != nil) {
		HNoPurge((Handle) table);
		ThrowIfMemError();
		
		XLocker lock((Handle) table);
		
		int32 count = (**table).numEntries;
		const SXEntry* entry = &((**table).entries[0]);
		bool found = false;
		while (count > 0 && !found) {
			found = entry->code == code;
	
			if (!found) {
				entry = NextEntry(entry);
				count--;
			}
		}
		
		if (found) 		
			str = FromPascalStr(entry->str);	
		
		HPurge((Handle) table);
	}
	
	return str;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XError
// ===================================================================================

XAutoPtr<XError>	XError::msInstance;		
XCriticalSection 	XError::msCreateMutex;

//---------------------------------------------------------------
//
// XError::~XError
//
//---------------------------------------------------------------
XError::~XError()
{
}


//---------------------------------------------------------------
//
// XError::XError
//
//---------------------------------------------------------------
XError::XError()
{
	mErrHandler = ErrorHandler(this, &XError::DefaultErrorHandler);
	mErrToStrHandler = ErrToStrHandler(this, &XError::DefaultErrToStrHandler);
}


//---------------------------------------------------------------
//
// XError::Instance										[static]
//
//---------------------------------------------------------------
XError* XError::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XError);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XError::ReportError (wstring, wstring)
//
//---------------------------------------------------------------
void XError::ReportError(const std::wstring& summary, const std::wstring& narrative) const
{
	XEnterCriticalSection enter(mMutex);

	mErrHandler(summary, narrative);
}


//---------------------------------------------------------------
//
// XError::ReportError (wstring, OSError)
//
//---------------------------------------------------------------
void XError::ReportError(const std::wstring& summary, OSError err) const
{
	std::wstring narrative = this->GetText(err);

	this->ReportError(summary, narrative);
}


//---------------------------------------------------------------
//
// XError::ReportError (wstring, std::exception)
//
//---------------------------------------------------------------
void XError::ReportError(const std::wstring& summary, const std::exception& e) const
{
	std::wstring narrative = this->GetText(&e);

	this->ReportError(summary, narrative);
}


//---------------------------------------------------------------
//
// XError::DeferError
//
//---------------------------------------------------------------
void XError::DeferError(const std::wstring& summary, const std::wstring& narrative) const
{
	XEnterCriticalSection enter(mMutex);

	if (mDeferredErrHandler.IsValid())
		mDeferredErrHandler(summary, narrative);
	else
		DEBUGSTR("XError::DeferError was called but no handler is installed!");
}


//---------------------------------------------------------------
//
// XError::GetText (OSError)
//
//---------------------------------------------------------------
std::wstring XError::GetText(OSError err) const
{
	std::wstring description;

	{
	XEnterCriticalSection enter(mMutex);
		description = mErrToStrHandler(err);
	}
	
	return description;
}


//---------------------------------------------------------------
//
// XError::GetText (const std::exception*)
//
//---------------------------------------------------------------
std::wstring XError::GetText(const std::exception* e) const
{
	std::wstring description;

	if (e == nil)
		description = LoadWhisperString(L"Unknown error.");
	
	else if (const XSystemException* sysErr = dynamic_cast<const XSystemException*>(e))
		description = this->GetText(sysErr->mError);
		
	else if (const std::bad_alloc* allocErr = dynamic_cast<const std::bad_alloc*>(e))
#if MAC
		description = this->GetText(memFullErr);		// don't use bad_alloc's lame message
#elif WIN
		description = this->GetText(ERROR_NOT_ENOUGH_MEMORY);
#endif

	else
		description = FromUTF8Str(e->what());
	
	return description;
}


//---------------------------------------------------------------
//
// XError::SetErrorHandler
//
//---------------------------------------------------------------
void XError::SetErrorHandler(ErrorHandler handler)
{
	XEnterCriticalSection enter(mMutex);
	
	mErrHandler = handler;
}


//---------------------------------------------------------------
//
// XError::SetDeferredErrorHandler
//
//---------------------------------------------------------------
void XError::SetDeferredErrorHandler(ErrorHandler handler)
{
	XEnterCriticalSection enter(mMutex);
	
	mDeferredErrHandler = handler;
}


//---------------------------------------------------------------
//
// XError::SetErrToStringHandler
//
//---------------------------------------------------------------
void XError::SetErrToStringHandler(ErrToStrHandler handler)
{
	XEnterCriticalSection enter(mMutex);
	
	mErrToStrHandler = handler;
}


//---------------------------------------------------------------
//
// XError::DefaultErrorHandler
//
//---------------------------------------------------------------
void XError::DefaultErrorHandler(const std::wstring& summary, const std::wstring& narrative) const
{
#if MAC
	InitCursor();

	if (MSystemInfo::HasAppearanceMgr()) {
		AlertStdAlertParamRec params;
		params.movable       = false;
		params.helpButton    = false; 
		params.filterProc    = nil;
		params.defaultText   = (StringPtr) -1L;		// use default (ie "OK")
		params.cancelText    = nil;		
		params.otherText     = nil; 
		params.defaultButton = ok;
		params.cancelButton  = 0;
		params.position      = kWindowDefaultPosition;

		short item;
		OSErr err = StandardAlert(kAlertStopAlert, (unsigned char*) ToPascalStr(summary).c_str(), (unsigned char*) ToPascalStr(narrative).c_str(), &params, &item);
		ASSERT(err == noErr);						// don't throw

	} else {
		ParamText(ToPascalStr(summary).c_str(), ToPascalStr(narrative).c_str(), "\p", "\p");

		(void) Alert(135, nil);
	}

#elif WIN
	std::wstring text = summary + L'\n' + narrative;

	uint32 flags = MB_OK +					// just an OK button
				   MB_ICONERROR +			// display the icon for errors
				   MB_DEFBUTTON1 +			// select the OK button
				   MB_TASKMODAL +			// don't let the user do anything else in the app
				   MB_SETFOREGROUND;		// bring the app to the front
	if (WSystemInfo::HasUnicode())
		(void) MessageBoxW(nil, text.c_str(), nil, flags);
	else
		(void) MessageBoxA(nil, ToPlatformStr(text).c_str(), nil, flags);
#endif
}


//---------------------------------------------------------------
//
// XError::DefaultErrToStrHandler
//
//---------------------------------------------------------------
std::wstring XError::DefaultErrToStrHandler(OSError err) const
{
	PRECONDITION(err != noErr);
	
	std::wstring narrative;

#if MAC
	Str255 numberStr;
	NumToString(err, numberStr);
	
	std::wstring key = L"Error " + FromPascalStr(numberStr);
	narrative = LoadWhisperString(key);
	if (narrative == key) {
#if DEBUG
		narrative = LookUpString(err);
		if (narrative.length() == 0)
#endif
			narrative = LoadWhisperString(L"There was an unknown system error (#1).", FromPascalStr(numberStr));
	}
	
#elif WIN
	// $$$ may want to special case the most common errors.	
	if (WSystemInfo::HasUnicode()) {
		wchar_t mesg[512];
		uint32 count = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts
		ThrowIf(count == 0);							
		narrative.assign(mesg, count);
	
	} else {
		char mesg[512];
		uint32 count = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,					// source and processing options
									  nil,											// message source
									  err,											// message indentifier
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),// language identifier
									  mesg,											// message buffer
									  sizeof(mesg),									// maximum size of message buffer
									  nil);											// message inserts
		ThrowIf(count == 0);							
		narrative = FromPlatformStr(mesg, count);
	}
#endif

	return narrative;
}


}	// namespace Whisper

