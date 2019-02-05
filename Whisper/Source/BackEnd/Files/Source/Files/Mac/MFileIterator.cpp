/*
 *  File:		MFileIterator.cpp
 *  Summary:	A class that iterates over all files in a folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MFileIterator.cpp,v $
 *		Revision 1.4  2001/04/21 03:26:07  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:03:45  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:13:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 7/21/00	JDJ		DoAdvance no longer uses recursion.
 *		 <4>	 6/03/99	JDJ		DoAdvance calls CountFiles only once per sub-folder.
 *		 <3>	 6/02/99	JDJ		File and folder filters are now reference counted.
 *		 <2>	 3/12/99	JDJ		Disabled the post-fix operator++.
 *		 <1>	 7/30/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileIterator.h>

#include <Errors.h>
#include <Folders.h>
#include <TextUtils.h>

#include <XConstants.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CountFiles
//
// Returns the number of files and folders within a directory.
//
//---------------------------------------------------------------
static uint32 CountFiles(const XFolderSpec& folder)
{
	Str255 fileName = "\p";
	
	CInfoPBRec block;
	block.dirInfo.ioCompletion = nil;
	block.dirInfo.ioNamePtr    = fileName;
	block.dirInfo.ioVRefNum    = folder.GetVolume();
	block.dirInfo.ioFDirIndex  = -1;
	block.dirInfo.ioDrDirID    = folder.GetDirID();
	
	OSErr err = PBGetCatInfoSync(&block);
	ThrowIfOSErr(err);
		
	return block.dirInfo.ioDrNmFls;
}

#pragma mark -

// ===================================================================================
//	class XFileIterator
// ===================================================================================

//---------------------------------------------------------------
//
// XFileIterator::~XFileIterator
//
//---------------------------------------------------------------
XFileIterator::~XFileIterator() 
{
	if (mFileFilter != nil)
		mFileFilter->RemoveReference();
	if (mFolderFilter != nil)
		mFolderFilter->RemoveReference();
}


//---------------------------------------------------------------
//
// XFileIterator::XFileIterator (XFolderSpec)
//
//---------------------------------------------------------------
XFileIterator::XFileIterator(const XFolderSpec& folder)
{
	mIterating = false;
	mVolume = folder.GetVolume();
	mDir = folder.GetDirID();
	mStartDir = mDir;
	mRecursive = false;
	
	mCount = 0;							// this ctor is used for end()...
	mIndex = 0;

	mFileFilter = nil;
	mFolderFilter = nil; 
}


//---------------------------------------------------------------
//
// XFileIterator::XFileIterator (XFolderSpec, XFileFilter*, XFolderFilter*, bool)
//
//---------------------------------------------------------------
XFileIterator::XFileIterator(const XFolderSpec& folder, XFileFilter* fileFilter, XFolderFilter* folderFilter, bool recursive)
{
	mIterating = false;
	mVolume = folder.GetVolume();
	mDir = folder.GetDirID();
	mStartDir = mDir;
	mRecursive = recursive;
	
	mFileFilter = fileFilter;
	mFolderFilter = folderFilter; 
	
	if (mFolderFilter == nil || mFolderFilter->operator()(folder)) {
		mCount = CountFiles(folder);
		
		mIndex = 0;
		mIterating = true;
		this->DoAdvance();
	}
}


//---------------------------------------------------------------
//
// XFileIterator::XFileIterator (XFileIterator)
//
//---------------------------------------------------------------
XFileIterator::XFileIterator(const XFileIterator& rhs) : mFile(rhs.mFile), mSubFolders(rhs.mSubFolders)
{
	mVolume = rhs.mVolume;
	mDir = rhs.mDir;
	mStartDir = rhs.mStartDir;

	mCount = rhs.mCount;
	mIndex = rhs.mIndex;
	
	mIterating = rhs.mIterating;
	mRecursive = rhs.mRecursive;
		
	mFileFilter = rhs.mFileFilter;
	mFolderFilter = rhs.mFolderFilter; 
	
	if (mFileFilter != nil)
		mFileFilter->AddReference();
	if (mFolderFilter != nil)
		mFolderFilter->AddReference();
}


//---------------------------------------------------------------
//
// XFileIterator::operator=
//
//---------------------------------------------------------------
XFileIterator& XFileIterator::operator=(const XFileIterator& rhs)
{
	if (this != &rhs) {
		mFile = rhs.mFile;
		mSubFolders = rhs.mSubFolders;
	
		mVolume = rhs.mVolume;
		mDir = rhs.mDir;
		mStartDir = rhs.mStartDir;
	
		mCount = rhs.mCount;
		mIndex = rhs.mIndex;
		
		mIterating = rhs.mIterating;
		mRecursive = rhs.mRecursive;
			
		if (mFileFilter != nil)
			mFileFilter->RemoveReference();
		if (mFolderFilter != nil)
			mFolderFilter->RemoveReference();
			
		mFileFilter = rhs.mFileFilter;
		mFolderFilter = rhs.mFolderFilter; 
		
		if (mFileFilter != nil)
			mFileFilter->AddReference();
		if (mFolderFilter != nil)
			mFolderFilter->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XFileIterator::operator++ ()
//
//---------------------------------------------------------------
XFileIterator& XFileIterator::operator++()
{
	if (mIterating)
		this->DoAdvance();
		
	return *this;
}


//---------------------------------------------------------------
//
// XFileIterator::operator++ (int)
//
//---------------------------------------------------------------
#if 0
XFileIterator XFileIterator::operator++(int)
{
	XFileIterator temp = *this;
	
	if (mIterating)
		this->DoAdvance();
		
	return temp;
}
#endif


//---------------------------------------------------------------
//
// XFileIterator::operator*
//
// CW 5 won't inline this...
//
//---------------------------------------------------------------
XFileSpec XFileIterator::operator*() const								
{
	PRECONDITION(mIterating); 
	
	return mFile;
}


//---------------------------------------------------------------
//
// XFileIterator::operator==
//
//---------------------------------------------------------------
bool XFileIterator::operator==(const XFileIterator& rhs) const		
{
	PRECONDITION(mStartDir == rhs.mStartDir); 
	
	bool equal = false;
	
	if (mDir == rhs.mDir)
		if (mCount == rhs.mCount)
			if (mIndex == rhs.mIndex)
				if (mSubFolders.size() == rhs.mSubFolders.size())
					equal = true;
		
	return equal;
}	

#pragma mark Ê

//---------------------------------------------------------------
//
// XFileIterator::DoAdvance
//
//---------------------------------------------------------------
void XFileIterator::DoAdvance()
{
	PRECONDITION(mIterating);
		
	mIterating = this->DoSearchFolder();
	
	while (!mIterating && mRecursive && !mSubFolders.empty()) {
		XFolderSpec folder = mSubFolders.back();
		mSubFolders.pop_back();
		
		mIndex  = 0;
		mCount  = CountFiles(folder);
		mDir    = folder.GetDirID();
		mVolume = folder.GetVolume();
				
		mIterating = this->DoSearchFolder();
	}
	
	if (!mIterating) {				// so 'this' compares equal to end()
		mIndex = 0;
		mCount = 0;
		mDir = mStartDir;
	}
}


//---------------------------------------------------------------
//
// XFileIterator::DoSearchFolder
//
//---------------------------------------------------------------
bool XFileIterator::DoSearchFolder()
{	
	bool found = false;
	XFolderSpec folder;
	while (!found && mIndex < mCount) {
		++mIndex;

		Str255 fileName = "\p";
		
		CInfoPBRec block;				
		block.hFileInfo.ioCompletion = nil;
		block.hFileInfo.ioNamePtr    = fileName;
		block.hFileInfo.ioVRefNum    = mVolume;
		block.hFileInfo.ioFDirIndex  = numeric_cast<int16>(mIndex);		// 1 based
		block.hFileInfo.ioDirID      = mDir;
		
		OSErr err = PBGetCatInfoSync(&block);
		ThrowIfOSErr(err);
		
		FSSpec spec;
		err = FSMakeFSSpec(mVolume, mDir, fileName, &spec);
		ThrowIfOSErr(err);
		
		Boolean isFolder, wasAliased;
		err = ResolveAliasFile(&spec, true, &isFolder, &wasAliased);
		if (err == noErr) {
			if (!isFolder) {
				mFile = XFileSpec(spec);
				
				found = mFileFilter == nil || mFileFilter->operator()(mFile);
				
			} else if (mRecursive) {
				folder = spec;
				
				if (mFolderFilter == nil || mFolderFilter->operator()(folder))
//					if (CountFiles(folder) > 0)				// most folders will contain files so it's more efficient to skip this check
						mSubFolders.push_back(folder);		// note that the File System caches info about the current directory so it's faster to defer sub-folders till later
			}
		}
	}
	
	return found;
}


}	// namespace Whisper
