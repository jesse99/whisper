/*
 *  File:       CProcessFiles.h
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CProcessFiles.h,v $
 *		Revision 1.1  2001/01/05 06:12:48  jesjones
 *		Renamed to start with a C.
 *		
 *		Revision 1.2  2000/11/09 12:02:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: CProcessFiles.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2001/01/05 06:12:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Renamed to start with a C.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/19/00	JDJ		Created
 */

#pragma once

#include <XProcessFiles.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class CProcessFiles
// ===================================================================================
class CProcessFiles : public XProcessFiles {

	typedef XProcessFiles Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CProcessFiles();
	
  						CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err);
  						
//-----------------------------------
//	New API
//
public:
			uint32 		GetNumFiles() const			{ASSERT(!mRunning); return mNumFiles;}

			uint64 		GetNumBytes() const			{ASSERT(!mRunning); return mNumBytes;}
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnProcessFile(const XFileSpec& file);

//-----------------------------------
//	Member Data
//
protected:
	uint32	mNumFiles;
	uint64	mNumBytes;
};

