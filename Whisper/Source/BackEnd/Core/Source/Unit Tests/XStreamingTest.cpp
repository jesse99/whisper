/*
 *  File:       XStreamingTest.cpp
 *  Summary:   	Unit test for binary streams.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStreamingTest.cpp,v $
 *		Revision 1.3  2001/04/17 01:44:56  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:52:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	12/11/99	JDJ		Added XFileSystemSpec streaming methods.
 *		 <2>	 2/19/99	JDJ		Added XDate streaming methods.
 *		 <1>	 1/11/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStreamingTest.h>

#include <climits>

#include <XBinaryPersistent.h>
#include <XDate.h>
#include <XExceptions.h>
#include <XHandle.h>
#include <XHandleStream.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XObjectFactory.h>
#include <XPointer.h>
#include <XRect.h>
#include <XStreaming.h>
#include <XStringUtils.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class ZTestObject
// ===================================================================================
class ZTestObject : public XBinaryPersistentMixin {

	typedef XBinaryPersistentMixin Inherited;

public:
	virtual 				~ZTestObject();
	
							ZTestObject();

	static XCreateByNameMixin* Create(XCreateByNameMixin* parent);
	static ZTestObject* 	   Create(XInStream& stream);

	const ZTestObject*		GetNext() const						{return mNext;}
	void					SetNext(const ZTestObject* next)	{mNext = next;}

protected:
	virtual	void 			OnStreamOut(XOutStream& stream) const;

	virtual void 			OnStreamIn(XInStream& stream);

private:
	const ZTestObject*	mNext;
};


//---------------------------------------------------------------
//
// ZTestObject::~ZTestObject
//
//---------------------------------------------------------------
ZTestObject::~ZTestObject()
{
}


//---------------------------------------------------------------
//
// ZTestObject::ZTestObject
//
//---------------------------------------------------------------
ZTestObject::ZTestObject()
{
	mNext = nil;
	
	this->SetClassName(L"TestObject");
}


//---------------------------------------------------------------
//
// ZTestObject::Create (XCreateByNameMixin*)			[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* ZTestObject::Create(XCreateByNameMixin* parent)
{
	ASSERT(parent == nil);
	
	return new ZTestObject;
}


//---------------------------------------------------------------
//
// ZTestObject::Create (XInStream)						[static]
//
//---------------------------------------------------------------
ZTestObject* ZTestObject::Create(XInStream& stream)
{
	XBinaryPersistentMixin* theObject = XBinaryPersistentMixin::Unflatten(stream, nil);

	ZTestObject* object = dynamic_cast<ZTestObject*>(theObject);
	ASSERT(object != nil);
	
	return object;
}


//---------------------------------------------------------------
//
// ZTestObject::OnStreamOut
//
//---------------------------------------------------------------
void ZTestObject::OnStreamOut(XOutStream& stream) const
{	
	XBinaryPersistentMixin::Flatten(stream, mNext);
}


//---------------------------------------------------------------
//
// ZTestObject::OnStreamIn
//
//---------------------------------------------------------------
void ZTestObject::OnStreamIn(XInStream& stream)
{	
	ZTestObject* temp = ZTestObject::Create(stream);
	
	delete mNext;
	mNext = temp;
}

#if __MWERKS__
#pragma mark Ê
#endif

// ===================================================================================
//	class XStreamUnitTest
// ===================================================================================

//---------------------------------------------------------------
//
// XStreamUnitTest::~XStreamUnitTest
//
//---------------------------------------------------------------
XStreamUnitTest::~XStreamUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XStreamUnitTest::XStreamUnitTest
//
//---------------------------------------------------------------
XStreamUnitTest::XStreamUnitTest() : XUnitTest(L"Backend", L"Streaming")
{
}

						
//---------------------------------------------------------------
//
// XStreamUnitTest::OnTest
//
//---------------------------------------------------------------
void XStreamUnitTest::OnTest()
{
	XHandle data;

	bool testBool = true;
	char testChar = 'a';
	short testShort = 10;
	int testInt = 11;
	long testLong = 12;
	float testFloat = 3.1415f;
	double testDouble = 2.718;
	unsigned char testUChar = 'b';
	unsigned short testUShort = 5;
	unsigned int testUInt = 6;
	unsigned long testULong = 7;
	std::wstring testString = L"hello";
				
	XPoint testPoint = XPoint(2, 3);
	XSize testSize = XSize(4, 5);
	XRect testRect = XRect(6, 7, 8, 9);
				
	XHandle testH(testString.length()*sizeof(wchar_t));
	BlockMoveData(testString.c_str(), testH.GetUnsafePtr(), (uint32) (testString.length()*sizeof(wchar_t)));
	
	std::vector<int32> testVector(4);
	testVector[0] = 100;
	testVector[1] = 101;
	testVector[2] = 102;
	testVector[3] = 103;
	
	std::list<int32> testList;
	testList.push_back(200);
	testList.push_back(201);
	testList.push_back(202);
	testList.push_back(203);
	
	std::set<int32> testSet;
	testSet.insert(1);
	testSet.insert(2);
	testSet.insert(3);
	testSet.insert(4);
	
	std::map<int32, std::wstring> testMap;
	testMap.insert(std::map<int32, std::wstring>::value_type(1, L"one"));
	testMap.insert(std::map<int32, std::wstring>::value_type(2, L"two"));
	testMap.insert(std::map<int32, std::wstring>::value_type(3, L"three"));
	testMap.insert(std::map<int32, std::wstring>::value_type(4, L"four"));
	
	// Test basic streaming:
	{
	XOutHandleStream stream(data);
		stream 	<< testBool
				<< testChar
				<< testShort
				<< testInt
				<< testLong
				<< testFloat
				<< testDouble
				<< testUChar
				<< testUShort
				<< testUInt
				<< testULong
				<< testString
				
				<< testPoint
				<< testSize
				<< testRect
				
				<< testH
				
				<< testVector
				<< testList
				<< testSet
				<< testMap;
	}
					
	{
	XInHandleStream stream(data);
		bool tempBool; stream >> tempBool; ASSERT(tempBool == testBool);
		char tempChar; stream >> tempChar; ASSERT(tempChar == testChar);
		short tempShort; stream >> tempShort; ASSERT(tempShort == testShort);
		int tempInt; stream >> tempInt; ASSERT(tempInt == testInt);
		long tempLong; stream >> tempLong; ASSERT(tempLong == testLong);
		float tempFloat; stream >> tempFloat; ASSERT(tempFloat == testFloat);
		double tempDouble; stream >> tempDouble; ASSERT(tempDouble == testDouble);
		unsigned char tempUChar; stream >> tempUChar; ASSERT(tempUChar == testUChar);
		unsigned short tempUShort; stream >> tempUShort; ASSERT(tempUShort == testUShort);
		unsigned int tempUInt; stream >> tempUInt; ASSERT(tempUInt == testUInt);
		unsigned long tempULong; stream >> tempULong; ASSERT(tempULong == testULong);
		std::wstring tempString; stream >> tempString; ASSERT(tempString == testString);
		
		XPoint tempPoint; stream >> tempPoint; ASSERT(tempPoint == testPoint);
		XSize tempSize; stream >> tempSize; ASSERT(tempSize == testSize);
		XRect tempRect; stream >> tempRect; ASSERT(tempRect == testRect);
		
		XHandle tempH; stream >> tempH; ASSERT(tempH == testH);
		
		std::vector<int32> tempVector; stream >> tempVector; ASSERT(tempVector == testVector);
		std::list<int32> tempList; stream >> tempList; ASSERT(tempList == testList);
		std::set<int32> tempSet; stream >> tempSet; ASSERT(tempSet == testSet);
		std::map<int32, std::wstring> tempMap; stream >> tempMap; ASSERT(tempMap == testMap);
	}
		
	// Test circular object streaming:
	XObjectFactory::Instance()->RegisterClass(L"TestObject", ZTestObject::Create);
	
	ZTestObject	a;
	ZTestObject	b;
	a.SetNext(&b);
	b.SetNext(&a);

	XOutHandleStream outStream;
	XBinaryPersistentMixin::Flatten(outStream, &a);
	XBinaryPersistentMixin::Flatten(outStream, &b);

	data = outStream.GetHandle();

	XInHandleStream inStream(data);
	ZTestObject* c = ZTestObject::Create(inStream);

	ZTestObject* d = ZTestObject::Create(inStream);
	
	ASSERT(c != d);
	ASSERT(c != nil);
	ASSERT(d != nil);
	ASSERT(c->GetNext() == d);
	ASSERT(d->GetNext() == c);
	
	delete c;
	delete d;
	
	TRACE("Completed streaming test.\n\n");
}

#endif	// DEBUG

}		// namespace Whisper
