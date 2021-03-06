/*
 *  File:       XLogFile.cpp
 *  Summary:    Dumps information out to a file in the app's folder.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLogFile.cpp,v $
 *		Revision 1.4  2001/04/17 01:44:21  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/10 04:10:29  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:42:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XLogFile.h>

#include <cstdarg>
#include <cstdio>

#include <XNumbers.h>

namespace Whisper {

using namespace std;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FSSpecOpen
//
//---------------------------------------------------------------
#if MAC
static FILE* FSSpecOpen(const OSFSSpec& spec, const char* mode)
{
	FILE* file = nil;

	short oldVolume;
	int32 oldDir;
	OSErr err = HGetVol(nil, &oldVolume, &oldDir);
	if (err == noErr) {

		err = HSetVol(0, spec.vRefNum, spec.parID);
		if (err == noErr) {
			char fName[256];

			uint32 len = spec.name[0];
			BlockMoveData(spec.name + 1, fName, len);
			fName[len] = '\0';

			file = fopen(fName, mode);
		}

		HSetVol(nil, oldVolume, oldDir);
	}

	return file;
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLogFile
// ===================================================================================

//---------------------------------------------------------------
//
// XLogFile::~XLogFile
//
//---------------------------------------------------------------
XLogFile::~XLogFile()
{
	if (mFile != nil)
		fclose(mFile);
}


//---------------------------------------------------------------
//
// XLogFile::XLogFile
//
//---------------------------------------------------------------
XLogFile::XLogFile(const char* fileName)
{
	mFile = nil;
	
#if MAC
	ProcessSerialNumber psn;
	OSErr err = GetCurrentProcess(&psn);
	if (err == noErr) {	
		ProcessInfoRec info;		
		info.processInfoLength = sizeof(info);
		info.processName       = 0;
		info.processAppSpec    = &mSpec;

		err = GetProcessInformation(&psn, &info);
		if (err == noErr) {	
			uint32 len = Min(strlen(fileName), sizeof(StrFileName) - 1);
			BlockMoveData(fileName, mSpec.name+1, len);
			mSpec.name[0] = numeric_cast<uint8>(len);
		}
	}

#elif WIN
	// Get a path to the exe.
	uint32 succeeded = ::GetModuleFileNameA(GetModuleHandle(nil), mPath, sizeof(mPath));
	if (succeeded) {

		// Find out where the app name starts.
		uint32 pos = ULONG_MAX;	
		for (uint32 j = strlen(mPath) - 1; pos == ULONG_MAX && j < ULONG_MAX; --j) {
			if (mPath[j] == '\\')
				pos = j;
		}

		// Copy fileName over the app name.
		strcpy(mPath + pos + 1, fileName);
	}
#endif
}


//---------------------------------------------------------------
//
// XLogFile::Print (const char*, ...)
//
//---------------------------------------------------------------
void XLogFile::Print(const char* format, ...)
{
	if (mFile == nil) 
		this->DoOpen();

	if (mFile != nil) {
		char mesg[2048];							
	
		va_list args;
		va_start(args, format);
		vsprintf(mesg, format, args);
		va_end(args);
		
		fprintf(mFile, "%s", mesg);
	}
}


//---------------------------------------------------------------
//
// XLogFile::Print (const wchar_t*, ...)
//
//---------------------------------------------------------------
void XLogFile::Print(const wchar_t* format, ...)
{
	if (mFile == nil) 
		this->DoOpen();

	if (mFile != nil) {
		wchar_t mesg[2048];							
	
		va_list args;
		va_start(args, format);
		vswprintf(mesg, sizeof(mesg)/sizeof(wchar_t), format, args);
		va_end(args);
		
		fprintf(mFile, "%ls", mesg);
	}
}


//---------------------------------------------------------------
//
// XLogFile::Flush
//
//---------------------------------------------------------------
void XLogFile::Flush()
{
	if (mFile != nil)
		fflush(mFile);						
}


//---------------------------------------------------------------
//
// XLogFile::DoOpen
//
//---------------------------------------------------------------
void XLogFile::DoOpen()
{
#if MAC
	if (mSpec.name[0] != 0) {				// don't attempt to re-open the file if we've already failed
		mFile = FSSpecOpen(mSpec, "w");	
		
		if (mFile == nil)
			mSpec.name[0] = 0;
	}

#elif WIN
	if (mPath[0] != 0) {					
		mFile = fopen(mPath, "w");
		
		if (mFile == nil)
			mPath[0] = 0;
	}
#endif

	if (mFile != nil)
		this->DoPrintTimeStamp();
}


//---------------------------------------------------------------
//
// XLogFile::DoPrintTimeStamp		
//
//---------------------------------------------------------------
void XLogFile::DoPrintTimeStamp()
{
#if MAC
	// Get the current time
	LongDateCvt date;
	date.hl.lHigh = 0;
	::GetDateTime(&date.hl.lLow);
	
	// Print the date
	Str255 str;
	::LongDateString(&date.c, abbrevDate, str, nil);
	this->Print("Date: %#s\n", str);
	
	// Print the time
	::LongTimeString(&date.c, true, str, nil);
	this->Print("Time: %#s\n\n", str);

#elif WIN
	// Get the current time
	FILETIME date;
	SYSTEMTIME systemTime;
	::GetLocalTime(&systemTime);
	
	int32 succeeded = ::SystemTimeToFileTime(&systemTime, &date);
	if (!succeeded) goto fail;

	// Print the date
	char buffer[2048];
	
	int32 numChars = ::GetDateFormatA(nil, DATE_LONGDATE, &systemTime, nil, nil, 0);
	if (numChars == 0) goto fail;
	numChars = Min(numChars, (int32) sizeof(buffer));
	
	succeeded = ::GetDateFormatA(nil, DATE_LONGDATE, &systemTime, nil, buffer, numChars);
	if (!succeeded) goto fail;
	
	this->Print("Date: %*s\n", numChars, buffer);

	// Print the time
	numChars = ::GetTimeFormatA(nil, 0UL, &systemTime, nil, nil, 0);
	if (numChars == 0) goto fail;
	numChars = Min(numChars, (int32) sizeof(buffer));
	
	succeeded = ::GetTimeFormatA(nil, 0UL, &systemTime, nil, buffer, numChars);
	if (!succeeded) goto fail;
	
	this->Print("Time: %*s\n\n", numChars, buffer);
	
	return;
	
fail:
	this->Print("Couldn't get the timestamp (error: %d)\n", GetLastError());
#endif
}


}	// namespace Whisper
