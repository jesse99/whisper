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
 *		Revision 1.2  2001/01/01 03:50:49  jesjones
 *		Misc changes for an 8x speed increase.
 *		
 *		Revision 1.1.1.1  2000/12/27 03:12:53  jesjones
 *		First Imported.
 */

#pragma once

#include <XHandle.h>
#include <XProcessFiles.h>
#include <XSet.h>

#include "FastString.h"


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
	
  						CProcessFiles(XFileFilter* fileFilter, XFolderFilter* folderFilter, const std::vector<XFileSystemSpec>& specs, const vector<XHandle>& files, ErrorCallback err);
  						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnProcessFile(const XFileSpec& file);

//-----------------------------------
//	Internal API
//
protected:
			void 		DoWriteInfo(const XFileSpec& spec);
			
			void 		DoProcessLine();
			
			void 		DoSkipWhiteSpace();
			
			void 		DoSkipPastEOL();
			
			FastString 	DoParseName();
			
			uint32 		DoParseOffset();
				
//-----------------------------------
//	Internal Types
//
public:
	struct SEntry {
		std::string	name;
		uint32		bytes;

		friend	void swap(SEntry& lhs, SEntry& rhs)		{swap(lhs.name, rhs.name); swap(lhs.bytes, rhs.bytes);}	// provides a big speedup to sort (provided that iter_swap calls swap)
	};
	
	struct CompareName : binary_function<SEntry, SEntry, bool> {
	    bool operator()(const SEntry& lhs, const SEntry& rhs) const		{return lhs.name < rhs.name;}
	};

	struct CompareSize : binary_function<SEntry, SEntry, bool> {
	    bool operator()(const SEntry& lhs, const SEntry& rhs) const		{return lhs.bytes > rhs.bytes;}
	};

	typedef XSet<SEntry, CompareName> Entries;

//-----------------------------------
//	Member Data
//
protected:
	vector<XHandle>	mFiles;
	const char*		mIter; 
	const char*		mEnd; 
	uint32			mLine;
	
	Entries			mEntries;
	uint32			mOffset;
	
	std::string		mName;			// variables used by DoProcessLine (it's much faster to create these once instead of each time DoProcessLine is called)
	std::string		mTemp;
	SEntry			mEntry;
};

