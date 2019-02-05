/*
 *  File:		XFileIteratorUnitTest.cpp
 *  Summary:	Unit test for XFileIterator.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileIteratorUnitTest.cpp,v $
 *		Revision 1.2  2000/11/09 12:35:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/06/00	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XFileIterator.h>

#include <XFileFilter.h>
#include <XFileSystem.h>
#include <XFolderFilter.h>
#include <XUnitTest.h>

namespace Whisper {


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
//	class ZFileIteratorTest
// ===================================================================================
#if DEBUG
class ZFileIteratorTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZFileIteratorTest();
	
						ZFileIteratorTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoEmptyFolder();
			void 		DoOneItem();
			void 		DoTwoItems();
			void 		DoNestedFolders();
			void 		DoFiltered();
			
private:
	XFolderSpec		mTestFolder;
};

static ZFileIteratorTest sFileIteratorTest;

//---------------------------------------------------------------
//
// ZFileIteratorTest::~ZFileIteratorTest
//
//---------------------------------------------------------------
ZFileIteratorTest::~ZFileIteratorTest()
{
}

	
//---------------------------------------------------------------
//
// ZFileIteratorTest::ZFileIteratorTest
//
//---------------------------------------------------------------
ZFileIteratorTest::ZFileIteratorTest() : XUnitTest(L"Backend", L"XFileIterator")
{
}
						

//---------------------------------------------------------------
//
// ZFileIteratorTest::OnTest
//
//---------------------------------------------------------------
void ZFileIteratorTest::OnTest()
{
	XFolderSpec appFolder = XFolderSpec::GetAppFolder();
	mTestFolder = XFolderSpec(appFolder, L"FilesTest");
	
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
// ZFileIteratorTest::DoEmptyFolder
//										
//---------------------------------------------------------------
void ZFileIteratorTest::DoEmptyFolder()
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
// ZFileIteratorTest::DoOneItem
//										
//---------------------------------------------------------------
void ZFileIteratorTest::DoOneItem()
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
// ZFileIteratorTest::DoTwoItems
//										
//---------------------------------------------------------------
void ZFileIteratorTest::DoTwoItems()
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
// ZFileIteratorTest::DoNestedFolders
//										
//---------------------------------------------------------------
void ZFileIteratorTest::DoNestedFolders()
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
// ZFileIteratorTest::DoFiltered
//										
//---------------------------------------------------------------
void ZFileIteratorTest::DoFiltered()
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
