/*
 *  File:		MFile.cpp
 *  Summary:	A class that encapsulates reading and writing to a file.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFile.cpp,v $
 *		Revision 1.3  2001/04/21 03:25:52  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:13:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 7/20/00	JDJ		ThrowFileErr converts the err code to a string in debug builds.
 *		 <3>	 2/26/99	JDJ		GetLength now returns a uint32. Added GetLength64 which returns a uint64.
 *									Ditto for GetPosition.
 *		 <2>	12/08/98	JDJ		Exception messages include the file name.
 *		 <1>	 1/10/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XFile.h>

#include <climits>

#include <Errors.h>
#include <Resources.h>
#include <Script.h>

#include <MoreFilesExtras.h>
#include <XConstants.h>
#include <XDebug.h>
#include <XError.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XIntConversions.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ThrowFileErr
//
//---------------------------------------------------------------
static void ThrowFileErr(const XFileSpec& spec, OSStatus err)
{	
	switch (err) {				
		case dirFulErr:
		case dskFulErr:
		case nsvErr:
		case ioErr:
		case fnOpnErr:
		case eofErr:
		case posErr:
		case mFulErr:
		case tmfoErr:
		case fnfErr:
		case wPrErr:
		case fLckdErr:

		case vLckdErr:
		case fBsyErr:
		case dupFNErr:
		case opWrErr:
		case rfNumErr:
		case gfpErr:
		case volOffLinErr:
		case permErr:
		case volOnLinErr:
		case nsDrvErr:
		case noMacDskErr:
		case wrPermErr:
		case dirNFErr:
		case tmwdoErr:
		case volGoneErr:
			{
#if DEBUG
			std::wstring mesg = XError::Instance()->GetText(err) + L" with file " + spec.GetName();
#else
			std::wstring mesg = LoadWhisperString(L"File error ##1 (with file '#2')", Int32ToStr(err), spec.GetName());
#endif
			throw std::runtime_error(ToUTF8Str(mesg));				
			}
			break;
			
		default:
			ThrowErr(err);
	}
}		 


//---------------------------------------------------------------
//
// ThrowIfFileErr
//
//---------------------------------------------------------------
inline void ThrowIfFileErr(const XFileSpec& spec, OSStatus err)		
{
	if (err != noErr) 
		ThrowFileErr(spec, err);
}	

#pragma mark -

// ===================================================================================
//	class XFile
// ===================================================================================

//---------------------------------------------------------------
//
// XFile::~XFile
//
//---------------------------------------------------------------
XFile::~XFile()
{
	PRECONDITION(!this->IsOpened());		// in the normal course of events you should explicitly close the file
									// $$$ skip this ASSERT if uncaught_exception() returns true
									
	if (this->IsOpened())			// but if an exception was thrown the file may still be open
		(void) FSClose(mRefNum);	// if so, we need to close the file
}


//---------------------------------------------------------------
//
// XFile::XFile (XFileSpec)
//
//---------------------------------------------------------------
XFile::XFile(const XFileSpec& file) : mSpec(file)
{		
	mRefNum = kNoFileRefNum;
	
	mOpenForWrite = false;
}


//---------------------------------------------------------------
//
// XFile::XFile (XFolderSpec, string)
//
//---------------------------------------------------------------
XFile::XFile(const XFolderSpec& folder, const std::wstring& name) : mSpec(folder, name)
{
	mRefNum = kNoFileRefNum;
	
	mOpenForWrite = false;
}


//---------------------------------------------------------------
//
// XFile::GetLength
//
//---------------------------------------------------------------
uint32 XFile::GetLength() const
{
	PRECONDITION(this->IsOpened());
	
	int32 size;
	
	OSErr err = GetEOF(mRefNum, &size);
	ThrowIfFileErr(mSpec, err);

	if (size > ULONG_MAX) {
		std::wstring mesg = LoadWhisperString(L"File #1's length is over 32 bits.", mSpec.GetName());
		throw std::range_error(ToUTF8Str(mesg));				
	}
	
	return (uint32) size;	
}


//---------------------------------------------------------------
//
// XFile::GetLength64
//
//---------------------------------------------------------------
uint64 XFile::GetLength64() const
{
	PRECONDITION(this->IsOpened());
	
	int32 size;
	
	OSErr err = GetEOF(mRefNum, &size);
	ThrowIfFileErr(mSpec, err);

	return (uint64) size;	
}


//---------------------------------------------------------------
//
// XFile::SetLength
//
//---------------------------------------------------------------
void XFile::SetLength(uint64 newSize)
{
	PRECONDITION(this->IsOpened());
	
	if (newSize > LONG_MAX) {
		std::wstring mesg = LoadWhisperString(L"Mac file lengths cannot be over 32 bits! (file '#1')", mSpec.GetName());
		throw std::domain_error(ToUTF8Str(mesg));				
	}
	
	OSErr err = SetEOF(mRefNum, (int32) newSize);
	ThrowIfFileErr(mSpec, err);
}


//---------------------------------------------------------------
//
// XFile::Open (OSType, OSType, EFilePermission, bool)
//
//---------------------------------------------------------------
void XFile::Open(OSType creator, OSType fileType, EFilePermission perm, bool fork)
{
	PRECONDITION(!this->IsOpened());
	
	OSErr err = noErr;
	int16 tempRef = kNoFileRefNum;
	
	if (fork == kDataFork) {
		err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
															
		if (err == fnfErr) {
			ASSERT(perm != kReadPermission);
			
			err = FSpCreate(&(mSpec.GetOSSpec()), creator, fileType, smSystemScript);
			ThrowIfFileErr(mSpec, err);
			
			err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
		}	
	
	} else {
		err = FSpOpenRF(&(mSpec.GetOSSpec()), perm, &tempRef);				
															
		if (err == fnfErr) {
			ASSERT(perm != kReadPermission);
			
			FSpCreateResFile(&(mSpec.GetOSSpec()), creator, fileType, smSystemScript);
			ThrowIfResError();
			
			err = FSpOpenRF(&(mSpec.GetOSSpec()), perm, &tempRef);				
		}	
	}
	ThrowIfFileErr(mSpec, err);

	mRefNum = tempRef;						// assign only if we succeeded
	mOpenForWrite = perm == kWritePermission;
}


//---------------------------------------------------------------
//
// XFile::Open (EFilePermission, bool)
//
//---------------------------------------------------------------
void XFile::Open(EFilePermission perm, bool fork)
{
	PRECONDITION(!this->IsOpened());
	
	int16 tempRef = kNoFileRefNum;
	
	OSErr err;				
	if (fork == kDataFork)
		err = FSpOpenDF(&(mSpec.GetOSSpec()), perm, &tempRef);				
	else
		err = FSpOpenRF(&(mSpec.GetOSSpec()), perm, &tempRef);				
	ThrowIfFileErr(mSpec, err);
	
	mRefNum = tempRef;						// assign only if we succeeded
	mOpenForWrite = perm == kWritePermission;
}


//---------------------------------------------------------------
//
// XFile::Close
//
//---------------------------------------------------------------
void XFile::Close()
{
	PRECONDITION(this->IsOpened());

	if (mRefNum != kNoFileRefNum) {			// TN 1089 says that closing a file twice can corrupt the file system so we'll be extra careful
		OSErr err = FSClose(mRefNum);

		mRefNum = kNoFileRefNum;			// make sure this is reset!
		mOpenForWrite = false;

		ThrowIfFileErr(mSpec, err);			// File Manager buffers data and flushes on close so an error is quite possible
	}
}


//---------------------------------------------------------------
//
// XFile::IsOpened
//
//---------------------------------------------------------------
bool XFile::IsOpened() const
{
	bool open = mRefNum != kNoFileRefNum;
	
	return open;
}


//---------------------------------------------------------------
//
// XFile::Seek
//
//---------------------------------------------------------------
void XFile::Seek(ESeekMode mode, int64 offset)
{
	OSErr err = SetFPos(mRefNum, mode, (int32) offset);
	ThrowIfFileErr(mSpec, err);
}


//---------------------------------------------------------------
//
// XFile::GetPosition
//
//---------------------------------------------------------------
uint32 XFile::GetPosition() const
{
	int32 position;
	
	OSErr err = GetFPos(mRefNum, &position);
	ThrowIfFileErr(mSpec, err);
	
	if (position > ULONG_MAX) {
		std::wstring mesg = LoadWhisperString(L"File #1's file pointer is over 32 bits.", mSpec.GetName());
		throw std::range_error(ToUTF8Str(mesg));				
	}
	
	return (uint32) position;
}


//---------------------------------------------------------------
//
// XFile::GetPosition64
//
//---------------------------------------------------------------
uint64 XFile::GetPosition64() const
{
	int32 position;
	
	OSErr err = GetFPos(mRefNum, &position);
	ThrowIfFileErr(mSpec, err);
	
	return (uint64) position;
}


//---------------------------------------------------------------
//
// XFile::Read
//
//---------------------------------------------------------------
void XFile::Read(void* buffer, uint32 inBytes)
{
	PRECONDITION(this->IsOpened());
	PRECONDITION(buffer != nil);
	PRECONDITION(!mOpenForWrite);
		
	if (inBytes > 0) {
		int32 bytes = (int32) inBytes;
		
		OSErr err = FSRead(mRefNum, &bytes, buffer);
		ThrowIfFileErr(mSpec, err);
	}
}


//---------------------------------------------------------------
//
// XFile::Write
//
//---------------------------------------------------------------
void XFile::Write(const void* buffer, uint32 inBytes)
{
	PRECONDITION(this->IsOpened());
	PRECONDITION(buffer != nil);
	
	if (inBytes > 0) {
		int32 bytes = (int32) inBytes;
		
		OSErr err = FSWrite(mRefNum, &bytes, buffer);
		ThrowIfFileErr(mSpec, err);
	}
}


//---------------------------------------------------------------
//
// XFile::Flush
//
//---------------------------------------------------------------
void XFile::Flush()
{
	OSErr err = FlushVol(nil, mSpec.GetVolume());
	ThrowIfFileErr(mSpec, err);
}


}	// namespace Whisper
