/*
 *  File:       ProcessFiles.h
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.h,v $
 *		Revision 1.3  2001/01/21 00:37:43  jesjones
 *		Added GetNumFiles.
 *		
 *		Revision 1.2  2000/11/09 12:02:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
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

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnRun();

	virtual void 		OnProcessFile(const XFileSpec& file);

	virtual void 		OnException(const std::exception* e);

//-----------------------------------
//	Member Data
//
protected:
	uint32	mNumFiles;
};

