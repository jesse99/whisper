/*
 *  File:       CountFiles.cpp
 *  Summary:	The thread used to count the numbers of files to process (so we can use a progress bar).
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in XTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CountFiles.cpp,v $
 *		Revision 1.2  2001/03/26 10:52:21  jesjones
 *		Removed extra whitespace.
 *		
 *		Revision 1.1.1.1  2000/12/27 03:12:50  jesjones
 *		First Imported.
 *		
 */

#include "AppHeader.h"
#include "CountFiles.h"

#include <XFileSystem.h>
#include <XLocker.h>


//-----------------------------------
//	Globals
//
extern std::wstring gCodeSectionName;		// "Code section,"
extern std::wstring gImportSectionName;		// "Import container"
extern std::wstring gDataSectionName;		// "Data section"

extern vector<std::wstring> gNamespaces;	// "std", "Whisper"


// ===================================================================================
//	class CCountFiles
// ===================================================================================

//---------------------------------------------------------------
//
// CCountFiles::~CCountFiles
//
//---------------------------------------------------------------
CCountFiles::~CCountFiles()
{
}


//---------------------------------------------------------------
//
// CCountFiles::CCountFiles
//
//---------------------------------------------------------------
CCountFiles::CCountFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, const vector<XHandle>& files, ErrorCallback err) : XCountFiles(fileFilter, folderFilter, specs, err), mFiles(files)
{
}

						
//---------------------------------------------------------------
//
// CCountFiles::OnCount
//
// Normally we only process one file so the default progress bar
// won't work too well. In order to get better progress info we
// include the file size (we also include the default file count
// so that CProcessFiles can get by with simply incrementing the
// count maintained by XProcessFiles).
//
//---------------------------------------------------------------
void CCountFiles::OnCount(const XFileSpec& spec)	
{
	Inherited::OnCount(spec);
	
	for (uint32 i = 0; i < mFiles.size(); ++i) {
		const XHandle& data = mFiles[i];
		XLocker lock(data);
		
		const char* begin = (const char*) data.GetPtr();
		const char* end = begin + data.GetSize();
		
		begin = std::search(begin, end, gCodeSectionName.c_str(), gCodeSectionName.c_str() + gCodeSectionName.length());						
		ASSERT(begin != end);

		const char* iter = std::search(begin, end, gImportSectionName.c_str(), gImportSectionName.c_str() + gImportSectionName.length());	
		ASSERT(iter != end);
		
		mCount += iter - begin;
	}
}


