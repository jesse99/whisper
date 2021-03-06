/*
 *  File:		WFileIterator.cpp
 *  Summary:	A class that iterates over all files in a folder.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WFileIterator.cpp,v $
 *		Revision 1.4  2001/04/27 09:18:25  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/21 03:31:37  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:07:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 6/02/99	JDJ		File and folder filters are now reference counted.
 *		 <2>	 3/12/99	JDJ		Disabled the post-fix operator++.
 *		 <1>	 7/30/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileIterator.h>

#include <WSystemInfo.h>
#include <XConstants.h>
#include <XExceptions.h>
#include <XFileFilter.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


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
	if (mFindH != nil)
		::FindClose(mFindH);

	if (mFileFilter != nil)
		mFileFilter->RemoveReference();
	if (mFolderFilter != nil)
		mFolderFilter->RemoveReference();

	mFindH = nil;
}


//---------------------------------------------------------------
//
// XFileIterator::XFileIterator (XFolderSpec)
//
//---------------------------------------------------------------
XFileIterator::XFileIterator(const XFolderSpec& folder) 
{
	mIterating = false;
	mRecursive = false;
	
	mStartFolder = folder;
	mParentFolder = folder;
	mFindH = nil;				// this ctor is used for end()...

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
	mRecursive = recursive;
	
	mStartFolder = folder;
	mParentFolder = folder;
	mFindH = nil;		
	
	mFileFilter = fileFilter;
	mFolderFilter = folderFilter; 
	
	if (mFolderFilter == nil || mFolderFilter->operator()(mStartFolder)) {
		mSubFolders.push_back(mStartFolder);

		mIterating = true;
		this->DoAdvance();
	}
}


//---------------------------------------------------------------
//
// XFileIterator::XFileIterator (XFileIterator)
//
// $$$ There doesn't appear to be a way to copy a find file handle
// $$$ so we'll create a brand new one and call FindNextFile until
// $$$ we're synched up. Since iterators are typically copied while
// $$$ they're at the beginning or end of the sequence this is
// $$$ probably better than the alternative of reading an entire
// $$$ directory into memory.
//
//---------------------------------------------------------------
XFileIterator::XFileIterator(const XFileIterator& rhs) : mStartFolder(rhs.mStartFolder),
														 mParentFolder(rhs.mStartFolder)
{
	mIterating = false;
	mRecursive = rhs.mRecursive;
	
	mFindH = nil;		
	
	mFileFilter = rhs.mFileFilter;
	mFolderFilter = rhs.mFolderFilter; 
	
	if (mFileFilter != nil)
		mFileFilter->AddReference();
	if (mFolderFilter != nil)
		mFolderFilter->AddReference();

	if (rhs.mFindH != nil) {
		mSubFolders.push_back(mStartFolder);

		mIterating = true;
		while (mIterating && mFile != rhs.mFile)
			this->DoAdvance();

		ASSERT(mFile == rhs.mFile);
	}
}


//---------------------------------------------------------------
//
// XFileIterator::operator=
//
//---------------------------------------------------------------
XFileIterator& XFileIterator::operator=(const XFileIterator& rhs)
{
	if (this != &rhs) {
		mStartFolder = rhs.mStartFolder;
		mParentFolder = rhs.mStartFolder;
	
		mFileFilter = rhs.mFileFilter;
		mFolderFilter = rhs.mFolderFilter;

		mIterating = false;
		mRecursive = rhs.mRecursive;
	
		mFindH = nil;
	
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

		if (rhs.mFindH != nil) {
			mSubFolders.push_back(mStartFolder);

			mIterating = true;
			while (mIterating && mFile != rhs.mFile)
				this->DoAdvance();

			ASSERT(mFile == rhs.mFile);
		}
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
	PRECONDITION(mStartFolder == rhs.mStartFolder); 
	
	bool equal = false;
	
	if (mParentFolder == rhs.mParentFolder)
		if (mSubFolders.size() == rhs.mSubFolders.size())
			if (mFile == rhs.mFile || !mIterating && !rhs.mIterating)
				equal = true;
		
	return equal;
}	

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XFileIterator::DoAdvance
//
//---------------------------------------------------------------
void XFileIterator::DoAdvance()
{
	PRECONDITION(mIterating);
	
	bool found = false;
	int32 succeeded = true;
	while (!found && succeeded) {
		if (WSystemInfo::HasUnicode()) {
			if (mFindH == nil) {
				if (!mSubFolders.empty()) {
					mParentFolder = mSubFolders.back();
					mSubFolders.pop_back();
						
					std::wstring path = mParentFolder.GetPath() + L"\\*.*";
					mFindH = ::FindFirstFileW(path.c_str(), &mFindInfoW);				
					ThrowIfBadHandle(mFindH);
					
				} else
					succeeded = false;
						
			} else 
				succeeded = ::FindNextFileW(mFindH, &mFindInfoW);			// no way to distinguish between an error and end of list...
			
			if (succeeded) {			
				if ((mFindInfoW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					mFile = XFileSpec(mParentFolder, mFindInfoW.cFileName);	// $$$ are short cuts resolved automatically?
					
					found = mFileFilter == nil || mFileFilter->operator()(mFile);
						
				} else if (mRecursive && mFindInfoW.cFileName[0] != '.') {	// first two names returned are '.' and '..'
					XFolderSpec folder(mParentFolder, mFindInfoW.cFileName);
					
					if (mFolderFilter == nil || mFolderFilter->operator()(folder))
						mSubFolders.push_back(folder);
				}	
				
			} else if (mFindH != nil) {
				VERIFY(::FindClose(mFindH));
				mFindH = nil;
				
				succeeded = !mSubFolders.empty();
			}

		} else {
			if (mFindH == nil) {
				if (!mSubFolders.empty()) {
					mParentFolder = mSubFolders.back();
					mSubFolders.pop_back();
						
					std::wstring path = mParentFolder.GetPath() + L"\\*.*";
					mFindH = ::FindFirstFileA(ToPlatformStr(path).c_str(), &mFindInfoA);				
					ThrowIfBadHandle(mFindH);
					
				} else
					succeeded = false;
						
			} else 
				succeeded = ::FindNextFileA(mFindH, &mFindInfoA);			// no way to distinguish between an error and end of list...
			
			if (succeeded) {			
				if ((mFindInfoA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
					mFile = XFileSpec(mParentFolder, FromPlatformStr(mFindInfoA.cFileName));	// $$$ are short cuts resolved automatically?
					
					found = mFileFilter == nil || mFileFilter->operator()(mFile);
						
				} else if (mRecursive && mFindInfoA.cFileName[0] != '.') {	// first two names returned are '.' and '..'
					XFolderSpec folder(mParentFolder, FromPlatformStr(mFindInfoA.cFileName));
					
					if (mFolderFilter == nil || mFolderFilter->operator()(folder))
						mSubFolders.push_back(folder);
				}	
				
			} else if (mFindH != nil) {
				VERIFY(::FindClose(mFindH));
				mFindH = nil;
				
				succeeded = !mSubFolders.empty();
			}
		}
	}	
		
	mIterating = found;
	
	if (!mIterating)							// so we compare equal to end()
		mParentFolder = mStartFolder;
}


}	// namespace Whisper
