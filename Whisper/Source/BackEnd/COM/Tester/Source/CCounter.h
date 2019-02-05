/*
 *  File:       CCounter.h
 *  Summary:   	Counts the number of non-comment lines using plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCounter.h,v $
 *		Revision 1.2  2000/11/09 08:53:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/31/00	JDJ		Created
 */

#pragma once

#include <XTypes.h>

using namespace Whisper;
using namespace std;

//-----------------------------------
//	Forward References
//
namespace Whisper {
	class XFileSpec;
	class XFolderSpec;
}


// ===================================================================================
//	class CCounter
// ===================================================================================
class CCounter {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~CCounter();
	
						CCounter(const XFolderSpec& folder, const map<std::wstring, std::wstring>& extMap);
						
private:
						CCounter(const CCounter& rhs);
						
			CCounter& 	operator=(const CCounter& rhs);

//-----------------------------------
//	API
//
public:
			uint32 		GetNumFiles() const				{return mNumFiles;}
			
			uint32 		GetNumLines() const				{return mNumLines;}

			uint32 		GetNumSemiColons() const		{return mNumSemiColons;}

//-----------------------------------
//	Internal API
//
private:
			std::wstring DoReadFile(const XFileSpec& spec);

//-----------------------------------
//	Member Data
//
private:
	uint32	mNumFiles;
	uint32	mNumLines;
	uint32 	mNumSemiColons;
};
