/*
 *  File:		XFileIteratorTest.cpp
 *  Summary:	Unit test for XFileIterator.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileIteratorTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:35:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 8/12/00	JDJ		Placed into the Files suite.
 *		 <1>	 2/06/00	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileIteratorTest.h>

#include <XFileFilter.h>
#include <XFileIterator.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class ZParensFilter
// ===================================================================================
class ZParensFilter : public XFolderFilter {

public:
	virtual bool		operator()(const XFolderSpec& folder) const;
};


//---------------------------------------------------------------
//
// ZParensFilter::operator()
//
//---------------------------------------------------------------
bool ZParensFilter::operator()(const XFolderSpec& folder) const
{
	std::wstring name = folder.GetName();
	
	uint32 len = name.length();
	bool shielded = name[0] == '(' && name[len-1] == ')';
	
	return !shielded;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XFileIteratorTest
// ===================================================================================

//---------------------------------------------------------------
//
// XFileIteratorTest::~XFileIteratorTest
//
//---------------------------------------------------------------
XFileIteratorTest::~XFileIteratorTest()
{
}

	
//---------------------------------------------------------------
//
// XFileIteratorTest::XFileIteratorTest
//
//---------------------------------------------------------------
XFileIteratorTest::XFileIteratorTest() : XUnitTest(L"Files", L"XFileIterator")
{
}
						

//---------------------------------------------------------------
//
// XFileIteratorTest::OnTest
//
//---------------------------------------------------------------
void XFileIteratorTest::OnTest()
{
	XFolderSpec appFolder = XFolderSpec::GetAppFolder();
	mTestFolder = XFolderSpec(appFolder, L"FilesTest3");
	
	if (XFileSystem::FolderExists(mTestFolder))
		XFileSystem::DeleteFolder(mTestFolder);

	this->DoEmptyFolder();
	this->DoOneItem();
	this->DoTwoItems();
	this->DoNestedFolders();
	this->DoFiltered();
	
	TRACE("Completed XFileIterator test.\n\n");
}

			
//---------------------------------------------------------------
//
// XFileIteratorTest::DoEmptyFolder
//										
//---------------------------------------------------------------
void XFileIteratorTest::DoEmptyFolder()
{
	XFileSystem::CreateFolder(mTestFolder);

	uint32 count = 0;
	XFileIterator iter = mTestFolder.begin();
	while (iter != mTestFolder.end()) {
		++count;
		++iter;
	}
	
	ASSERT(count == 0);

	XFileSystem::DeleteFolder(mTestFolder);
}


//---------------------------------------------------------------
//
// XFileIteratorTest::DoOneItem
//										
//---------------------------------------------------------------
void XFileIteratorTest::DoOneItem()
{
	XFileSystem::CreateFolder(mTestFolder);

	XFileSpec file(mTestFolder, L"A");
	XFileSystem::CreateFile(file, 'CWIE', 'TEXT');

	uint32 count = 0;
	XFileIterator iter = mTestFolder.begin();
	while (iter != mTestFolder.end()) {
		XFileSpec spec = *iter;
		++iter;
		
		ASSERT(spec.GetName() == L"A");
		
#if MAC			
		ASSERT(spec.GetCreator() == 'CWIE');
		ASSERT(spec.GetType() == 'TEXT');
#endif

		++count;
	}
	
	ASSERT(count == 1);

	XFileSystem::DeleteFolder(mTestFolder);
}


//---------------------------------------------------------------
//
// XFileIteratorTest::DoTwoItems
//										
//---------------------------------------------------------------
void XFileIteratorTest::DoTwoItems()
{
	XFileSystem::CreateFolder(mTestFolder);

	XFileSpec file(mTestFolder, L"A");
	XFileSystem::CreateFile(file, 'CWIE', 'TEXT');

	file = XFileSpec(mTestFolder, L"B");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	uint32 count = 0;
	XFileIterator iter = mTestFolder.begin();
	while (iter != mTestFolder.end()) {
		++iter;
		++count;
	}
	
	ASSERT(count == 2);

	XFileSystem::DeleteFolder(mTestFolder);
}


//---------------------------------------------------------------
//
// XFileIteratorTest::DoNestedFolders
//										
//---------------------------------------------------------------
void XFileIteratorTest::DoNestedFolders()
{
	XFileSystem::CreateFolder(mTestFolder);

	XFileSpec file(mTestFolder, L"A");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	file = XFileSpec(mTestFolder, L"B");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	XFolderSpec subFolder(mTestFolder, L"Sub");
	XFileSystem::CreateFolder(subFolder);

	file = XFileSpec(subFolder, L"C");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	file = XFileSpec(subFolder, L"D");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	// Non-recursive
	uint32 count = 0;
	XFileIterator iter = mTestFolder.begin();
	while (iter != mTestFolder.end()) {
		++iter;
		++count;
	}
	
	ASSERT(count == 2);

	// Recursive
	count = 0;
	iter = mTestFolder.begin(kRecursive);
	while (iter != mTestFolder.end()) {
		XFileSpec spec = *iter;
		++iter;
		
		if (spec.GetName() == L"A" || spec.GetName() == L"B") {
			ASSERT(spec.GetParent() == mTestFolder);
		
		} else if (spec.GetName() == L"C" || spec.GetName() == L"D") {
			ASSERT(spec.GetParent() == subFolder);
		
		} else
			DEBUGSTR("Bad file name");

		++count;
	}
	
	ASSERT(count == 4);

	XFileSystem::DeleteFolder(mTestFolder);
}


//---------------------------------------------------------------
//
// XFileIteratorTest::DoFiltered
//										
//---------------------------------------------------------------
void XFileIteratorTest::DoFiltered()
{
	XFileSystem::CreateFolder(mTestFolder);

	XFileSpec file(mTestFolder, L"A.txt");
	XFileSystem::CreateFile(file, 'CWIE', 'TEXT');

	file = XFileSpec(mTestFolder, L"B.pdf");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	XFolderSpec subFolder(mTestFolder, L"Sub");
	XFileSystem::CreateFolder(subFolder);

	file = XFileSpec(subFolder, L"C.pdf");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	file = XFileSpec(subFolder, L"D.txt");
	XFileSystem::CreateFile(file, 'FooB', 'TEXT');

	XFolderSpec hiddenFolder(mTestFolder, L"(Hidden)");
	XFileSystem::CreateFolder(hiddenFolder);

	file = XFileSpec(hiddenFolder, L"D.pdf");
	XFileSystem::CreateFile(file, 'CARO', 'PDF ');

	file = XFileSpec(hiddenFolder, L"E.txt");
	XFileSystem::CreateFile(file, 'CWIE', 'TEXT');
	
	XFileFilter* fileFilter = new XFileFilter;
	fileFilter->AddType(XFileType(L"Text Files", L"txt", '****', 'TEXT'));	// will get a match if extension is "txt" or type is 'TEXT'

	uint32 count = 0;
	XFileIterator iter = mTestFolder.begin(fileFilter, new ZParensFilter, kRecursive);
	while (iter != mTestFolder.end()) {
		XFileSpec spec = *iter;
		++iter;
		
		ASSERT(spec.GetParent() != hiddenFolder);

#if MAC			
		ASSERT(spec.GetType() == 'TEXT');
#endif

		++count;
	}
	
	ASSERT(count == 2);

	XFileSystem::DeleteFolder(mTestFolder);
}

#endif	// DEBUG
}		// namespace Whisper
