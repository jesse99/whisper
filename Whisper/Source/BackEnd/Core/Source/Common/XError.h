/*
 *  File:       XError.h
 *  Summary:   	Singleton used to manage errors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XError.h,v $
 *		Revision 1.4  2000/12/10 08:51:56  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:31:16  jesjones
 *		XCallback uses IsValid again.
 *		
 *		Revision 1.2  2000/11/09 12:34:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 3/19/00	JDJ		Created (from the old code in XMiscUtils)
 */

#pragma once

#include <XAutoPtr.h>
#include <XCallbacks.h>
#include <XCriticalSection.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XError
//!		Singleton used to manage errors.
// ===================================================================================
class CORE_EXPORT XError {

//-----------------------------------
//	Types
//
public:
	typedef XCallback2<void, std::wstring, std::wstring> ErrorHandler;		
	typedef XCallback1<std::wstring, OSError> 	   ErrToStrHandler;		

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XError();

						XError();

	static XError* 		Instance();

private:
						XError(const XError& rhs);
						
			XError& 	operator=(const XError& rhs);

//-----------------------------------
//	New API
//
public:
	//! @name Error Reporting
	//@{
			void 		ReportError(const std::wstring& summary, const std::wstring& narrative = L"") const;
						/**< Calls the error handler. */

			void 		ReportError(const std::wstring& summary, OSError err) const;
						/**< Calls an ErrToStringHandler to convert the error into a narrative string.
						For the most common errors DefaultErrToStringHandler produces something like:
								"Couldn't save the document.", "The disk is full. Delete some files and try again."
						If DefaultErrToStringHandler can't find the error you'll get a message like:
								"Couldn't save the document.", "There was an unknown system error (#-50)." */
		
			void 		ReportError(const std::wstring& summary, const std::exception& e) const;
						/**< If the exception is an XSystemException the function above will be called 	
						with the error code. Otherwise the exception's string will be used
						resulting in something like:
							"Couldn't complete the operation.", "Internal Error: SDialogBoxInfo can't be streamed in (bad version)." */
			
			void 		DeferError(const std::wstring& summary, const std::wstring& narrative = L"") const;
						/**< Typically calls ReportError the next time the main event loop executes.
						(This is handy for reporting errors within threads). */

			std::wstring GetText(OSError err) const;
			std::wstring GetText(const std::exception* e) const;
						/**< Uses mErrToStrHandler to return a human readable description of the error.
						Note that e may be nil. */
	//@}

	//! @name Callbacks
	//@{
			void 		SetErrorHandler(ErrorHandler handler);
						/**< mErrHandler defaults to DefaultErrorHandler. */

			void 		SetErrToStringHandler(ErrToStrHandler handler);
						/**< mErrToStrHandler defaults to DefaultErrToStringHandler. */
						
			void 		SetDeferredErrorHandler(ErrorHandler handler);
						/**< mDeferredErrHandler defaults to nil: if DeferError is called it will
						assert and do nothing. */

			bool 		HasDeferredErrorHandler() const						{return mDeferredErrHandler.IsValid();}
	//@}

	//! @name Default Callbacks
	//@{
			void 		DefaultErrorHandler(const std::wstring& summary, const std::wstring& narrative) const;
						/**< Pops up a simple alert. */

			std::wstring DefaultErrToStrHandler(OSError err) const;
						/**< On Windows this will use FormatMessage to convert the error into a
						string. On the Mac this will simply convert the error number into a
						string (except in debug where it will look for a 'STRX' resource and
						attempt to convert the error number into the sort of strings found in
						Errors.h). */
	//@}

//-----------------------------------
//	Member Data
//
protected:
	ErrorHandler				mErrHandler;
	ErrorHandler				mDeferredErrHandler;
	ErrToStrHandler				mErrToStrHandler;

	mutable XCriticalSection	mMutex;

private:
	static XAutoPtr<XError>		msInstance;			
	static XCriticalSection		msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

