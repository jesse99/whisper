/*
 *  File:       CProcessFiles.h
 *  Summary:   	The thread used to process files.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ProcessFiles.h,v $
 *		Revision 1.1  2001/03/05 04:51:54  jesjones
 *		Checked in
 *		
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
	virtual void 		OnProcessFile(const XFileSpec& file);

//-----------------------------------
//	Internal API
//
private:
			void 		DoReadFile(const XFileSpec& spec);
			void 		DoWriteFile(const XFileSpec& spec, const std::string& text) const;
			
			bool 		DoProcessRevision(const std::string& fileName);
			void 		DoGetComment();
			
			int32 		DoParseNumber();
			std::string DoParseLine();
			
			void 		DoSkipToNextLine();
			void 		DoSkipWhiteSpace();
			void 		DoSkipDigits();
			void 		DoSkip(char ch);
			void 		DoSkipCommentStart();
			
			void 		DoWriteListing();

//-----------------------------------
//	Internal Types
//
private:
	struct SDate {
		int32 	year;
		int32	month;
		
		std::string ToStr() const;
		
		bool 		operator==(const SDate& rhs) const	{return year == rhs.year && month == rhs.month;}
		bool 		operator<(const SDate& rhs) const	{return year > rhs.year || (year == rhs.year && month > rhs.month);}
	};
	
	typedef std::vector<std::string> 		History;		// list of changes for one month
	typedef std::map<std::string, History> 	FileHistories;	// maps file names to change lists
	typedef std::map<SDate, FileHistories> 	Listing;		// maps dates to file histories

//-----------------------------------
//	Member Data
//
private:
	uint32		mNumFiles;
	std::string	mText;
	uint32		mIndex;
	uint32		mLength;
	
	Listing		mListing;
};

