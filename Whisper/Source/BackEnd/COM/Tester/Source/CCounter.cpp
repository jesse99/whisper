/*
 *  File:       CCounter.cpp
 *  Summary:   	Counts the number of non-comment lines using plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCounter.cpp,v $
 *		Revision 1.2  2000/11/09 08:53:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/31/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include "CCounter.h"

#include <XFile.h>
#include <XFileFilter.h>
#include <XFileIterator.h>
#include <XFolderFilter.h>
#include <XFolderSpec.h>
#include <XLocker.h>
#include <XTranscode.h>

#include "ICountLines.h"
#include "ICountSemiColons.h"


// ===================================================================================
//	class CCounter
// ===================================================================================

//---------------------------------------------------------------
//
// CCounter::~CCounter
//
//---------------------------------------------------------------
CCounter::~CCounter()
{
}


//---------------------------------------------------------------
//
// CCounter::CCounter
//
//---------------------------------------------------------------
CCounter::CCounter(const XFolderSpec& folder, const map<std::wstring, std::wstring>& extMap)
{
	mNumFiles = 0;
	mNumLines = 0;
	mNumSemiColons = 0;
	
	XFileFilter* fileFilter = new XFileFilter;
#if MAC
	fileFilter->AddType(XFileType('****', 'TEXT'));
#endif

	XFolderSpec::iterator iter = folder.begin(fileFilter, new XFolderFilter, kRecursive);
	while (iter != folder.end()) {
		XFileSpec spec = *iter;
		++iter;
		
		std::wstring ext = spec.GetExtension();
		map<std::wstring, std::wstring>::const_iterator iter2 = extMap.find(ext);
		if (iter2 != extMap.end()) {
			std::wstring text = this->DoReadFile(spec);
			
			IConstCountLinesPtr counter(iter2->second);
			mNumLines += counter->CountLines(text);
			++mNumFiles;
			
			IConstCountSemiColonsPtr counter2(iter2->second);	// query for the optional interface
			if (counter2)
				mNumSemiColons += counter2->CountSemiColons(text);
		}
	}
}


//---------------------------------------------------------------
//
// CCounter::DoReadFile
//
//---------------------------------------------------------------
std::wstring CCounter::DoReadFile(const XFileSpec& spec)
{
	std::string text;
	XHandle buffer(0, kUseTempHeap);
	
	XFile file(spec);	
	file.Open(kReadPermission);

	uint32 bytes = (uint32) file.GetLength();
	buffer.SetSize(bytes);
	
	{
	XLocker lock(buffer);
		file.Read(buffer.GetPtr(), bytes);
		text.assign((const char*) buffer.GetPtr(), buffer.GetSize());
	}
	
	file.Close();
		
	return FromPlatformStr(text.c_str(), text.length());		
}


