/*
 *  File:       CProcessFiles.cpp
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CProcessFiles.cpp,v $
 *		Revision 1.1  2001/01/05 06:12:42  jesjones
 *		Renamed to start with a C.
 *		
 *		Revision 1.2  2000/11/09 12:02:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CProcessFiles.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2001/01/05 06:12:42  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Renamed to start with a C.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/19/00	JDJ		Created
 */

#include "AppHeader.h"
#include "CProcessFiles.h"

#include <cctype>

#include <XFile.h>
#include <XHandle.h>
#include <XLocker.h>
#include <XStringUtils.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ReadFile
//
//---------------------------------------------------------------
static XHandle ReadFile(const XFileSpec& spec)
{
	XHandle buffer(0, kUseTempHeap);
	
	XFile file(spec);	
	file.Open(kReadPermission);

	uint32 bytes = (uint32) file.GetLength();
	buffer.SetSize(bytes);
	
	{
	XLocker lock(buffer);
		file.Read(buffer.GetPtr(), bytes);
	}
	
	file.Close();
	
	return buffer;		
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CProcessFiles
// ===================================================================================

//---------------------------------------------------------------
//
// CProcessFiles::~CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::~CProcessFiles()
{
}


//---------------------------------------------------------------
//
// CProcessFiles::CProcessFiles
//
//---------------------------------------------------------------
CProcessFiles::CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err) : XProcessFiles(fileFilter, folderFilter, specs, err)
{
	mNumFiles = 0;
	mNumBytes = 0;

	// This is used in the progress dialog to generate messages
	// that look like "Processing: Foo.txt". You might want to use
	// a more meaningful string.
	mMesgPrefix = LoadAppString(L"Processing: ");
}

						
//---------------------------------------------------------------
//
// CProcessFiles::OnProcessFile
//
//---------------------------------------------------------------
void CProcessFiles::OnProcessFile(const XFileSpec& file)	
{
	XHandle data = ReadFile(file);
	
	// This is where you would process the file. You should always
	// get a valid file because the filters in App.cpp should handle
	// filtering out files that your app doesn't care about.
	mNumBytes += data.GetSize();
	
	++mNumFiles;
}


