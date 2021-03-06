/*
 *  File:		XFileIterator.h
 *  Summary:	A class that iterates over all files in a folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileIterator.h,v $
 *		Revision 1.4  2001/03/05 05:41:24  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:11  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:35:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 6/11/99	JDJ		Tweaked the XFolderSpec friend declaration so that it compiles with Pro 5fc3.
 *		 <4>	 6/03/99	JDJ		Uses std::list instead of XQueue
 *		 <3>	 6/02/99	JDJ		File and folder filters are now reference counted.
 *		 <2>	 3/12/99	JDJ		Disabled the post-fix operator++.
 *		 <1>	 7/30/98	JDJ		Created (from Raven)
 */

#pragma once

#include <list>

#include <XConstants.h>
#include <XCountedPtr.h>
#include <XDebug.h>
#include <XFileSpec.h>
#include <XFolderSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileFilter;
class XFolderFilter;


// ===================================================================================
//	class XFileIterator
//!		A class that iterates over all files in a folder.
// ===================================================================================
class FILES_EXPORT XFileIterator : public std::iterator<std::forward_iterator_tag, XFileSpec> {

	friend class XFolderSpec;

//-----------------------------------
//	Initialization/Destruction
//
public:
							~XFileIterator();

protected:
							XFileIterator(const XFolderSpec& folder);
							// Used for end.
		
							XFileIterator(const XFolderSpec& folder, XFileFilter* takeFileFilter, 
					              		  XFolderFilter* takeFolderFilter, bool recursive = kNonRecursive);
					              		  
public:	
							XFileIterator(const XFileIterator& rhs);
							
			XFileIterator& 	operator=(const XFileIterator& rhs);
		
//-----------------------------------
//	API
//
public:	
			XFileSpec		operator*() const;
			XFileSpec*		operator->() const								{ASSERT(mIterating); return const_cast<XFileSpec*>(&mFile);}
	
			XFileIterator& 	operator++();
//			XFileIterator 	operator++(int);
							// Post-fix operator is disabled because the XFileIterator
							// is quite slow on Windows.
						
			bool 			operator==(const XFileIterator& rhs) const;	
			bool 			operator!=(const XFileIterator& rhs) const		{return !this->operator==(rhs);}

//-----------------------------------
//	Internal API
//
protected:
			void 			DoAdvance();
#if MAC
		
			bool 			DoSearchFolder();
#endif
	
//-----------------------------------
//	Internal Types
//
private:
	typedef std::list<XFolderSpec> FolderStack;
	
//-----------------------------------
//	Member Data
//
protected:
#if MAC
	int16				mVolume;
	int32				mDir;
	int32				mStartDir;

	uint32				mCount;
	uint32				mIndex;
	
#elif WIN
	XFolderSpec			mStartFolder;
	XFolderSpec			mParentFolder;
	
	WIN32_FIND_DATAA	mFindInfoA;
	WIN32_FIND_DATAW	mFindInfoW;
	HANDLE				mFindH;
#endif
	
	bool				mIterating;
	XFileSpec			mFile;
	
	XFileFilter*		mFileFilter;
	XFolderFilter*		mFolderFilter;

	bool				mRecursive;
	FolderStack			mSubFolders;			
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

