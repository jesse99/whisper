/*
 *  File:       XMemUtilsTest.cpp
 *  Summary:   	Unit test for the memory utils functions.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemUtilsTest.cpp,v $
 *		Revision 1.3  2001/04/27 04:15:44  jesjones
 *		Added a test for the new Base64 encoder/decoder.
 *		
 *		Revision 1.2  2000/11/09 12:42:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XMemUtilsTest.h>

#include <cctype>

#include <XBase64.h>
#include <XDebug.h>
#include <XLocker.h>
#include <XMemUtils.h>
#include <XNumbers.h>

#if MAC
	#include <Gestalt.h>
#endif

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XMemUtilsUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XMemUtilsUnitTest::~XMemUtilsUnitTest
//
//---------------------------------------------------------------
XMemUtilsUnitTest::~XMemUtilsUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XMemUtilsUnitTest::XMemUtilsUnitTest
//
//---------------------------------------------------------------
XMemUtilsUnitTest::XMemUtilsUnitTest() : XUnitTest(L"Backend", L"Mem Utils")
{
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMemUtilsUnitTest::OnTest
//
//---------------------------------------------------------------
void XMemUtilsUnitTest::OnTest()
{
	this->DoTestMemoryOperations();
	this->DoTestBitOperations();
	this->DoTestBase64();

	TRACE("Completed mem utils test.\n\n");
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XMemUtilsUnitTest::DoTestMemoryOperations
//
//---------------------------------------------------------------
void XMemUtilsUnitTest::DoTestMemoryOperations()
{
	uint8 block1[100];
	uint8 block2[205];
	
	ClearMemory(block1, 100);
	this->DoVerifyBlock(block1, 100, 0);
	
	SetMemory(block2, 205, 34);
	this->DoVerifyBlock(block2, 34, 205);
	
	InvertMemory(block1, 100);
	this->DoVerifyBlock(block1, 100, 0xFF);
	
	ASSERT(!EqualMemory(block1, block2, 100));	

	BlockMoveData(block1, block2, 100L);
	ASSERT(EqualMemory(block1, block2, 100));
}


//---------------------------------------------------------------
//
// XMemUtilsUnitTest::DoTestBitOperations
//
//---------------------------------------------------------------
void XMemUtilsUnitTest::DoTestBitOperations()
{
#if MAC
	uint32 temp = 0;
	SetBit(&temp, 0);
	ASSERT(temp == 0x80000000);

	SetBit(&temp, 15);
	ASSERT(temp == 0x80010000);

	ClearBit(&temp, 0);
	ASSERT(temp == 0x00010000);

	InvertBit(&temp, 15);
	ASSERT(temp == 0);
	InvertBit(&temp, 1);
	ASSERT(temp == 0x40000000);

#elif WIN
	uint32 temp = 0;
	SetBit(&temp, 0);
	ASSERT(temp == 0x00000080);

	SetBit(&temp, 15);
	ASSERT(temp == 0x00000180);

	ClearBit(&temp, 0);
	ASSERT(temp == 0x00000100);

	InvertBit(&temp, 15);
	ASSERT(temp == 0);
	InvertBit(&temp, 1);
	ASSERT(temp == 0x00000040);
#endif

	ASSERT(TestBit(&temp, 0) == false);
	ASSERT(TestBit(&temp, 1) == true);
	ASSERT(TestBit(&temp, 2) == false);
}


//---------------------------------------------------------------
//
// XMemUtilsUnitTest::DoTestBase64
//
//---------------------------------------------------------------
void XMemUtilsUnitTest::DoTestBase64()
{
	// try an empty buffer
	std::string text = EncodeBase64(nil, 0);
	XHandle data = DecodeBase64(text);
	ASSERT(data.GetSize() == 0);
	
	// try a randomized buffer
	data.SetSize(1024);
	XLocker lock(data);
	
	for (uint32 i = 0; i < 1024; ++i)
		data.GetPtr()[i] = (uint8) Random(255L);

	text = EncodeBase64(data.GetPtr(), data.GetSize());
	for (uint32 j = 0; j < text.length(); ++j) {
		ASSERT(text[j] >= 0x20 || std::isspace(text[j]));	// text should be 7-bit ASCII
		ASSERT(text[j] < 0x80);
	}
	
	XHandle newData = DecodeBase64(text);
	ASSERT(data == newData);
}


//---------------------------------------------------------------
//
// XMemUtilsUnitTest::DoVerifyBlock
//
//---------------------------------------------------------------
void XMemUtilsUnitTest::DoVerifyBlock(uint8* block, uint32 size, uint8 value)
{
	uint8* ptr = block;
	for (uint32 i = 0; i < size; i++)
		ASSERT(*ptr++ == value);
}


#endif	// DEBUG
}		// namespace Whisper

