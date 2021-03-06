/*
 *  File:       ProcessFiles.h
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.h,v $
 *		Revision 1.2  2000/11/09 12:02:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 7/19/00	JDJ		Created
 */

#pragma once

#include <XFileSpec.h>
#include <XLocale.h>
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
	
  						CProcessFiles(const XFileSpec& tableSpec, XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, ErrorCallback err);
  						
//-----------------------------------
//	New API
//
public:
			uint32 		GetNumInserted() const			{ASSERT(!mRunning); return mNumInserted;}

			uint32 		GetNumRemoved() const			{ASSERT(!mRunning); return mNumRemoved;}
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnRun();
	
	virtual void 		OnProcessFile(const XFileSpec& file);
	
//-----------------------------------
//	Internal API
//
protected:			
			void 		DoProcessQuote(const std::wstring& fileName, const std::wstring& text, uint32 index);

			void 		DoWriteTable();
			
			void 		DoAppendText(std::wstring& contents, const std::wstring& text);
	
//-----------------------------------
//	Internal Types
//
public:
	struct STranslation {
		std::wstring	text;
		XLocale			locale;
	};
	
	typedef vector<STranslation> Translations;
	typedef vector<std::wstring> 	 Comments;
	typedef set<std::wstring> 		 Files;

	struct SEntry {
		Translations	translations;
		Files			files;
		Comments		comments;
		bool			used;
		bool			synthetic;
	};
	
	typedef map<std::wstring, SEntry> Entries;
	
//-----------------------------------
//	Member Data
//
protected:
	Entries				mEntries;
	std::wstring		mFnName;

	XFileSpec			mTableSpec;
	volatile uint32		mNumInserted;
	volatile uint32		mNumRemoved;
};

