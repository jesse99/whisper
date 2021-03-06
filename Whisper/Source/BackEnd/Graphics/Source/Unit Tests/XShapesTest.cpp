/*
 *  File:       XShapesTest.cpp
 *  Summary:	XShapes unit test.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XShapesTest.cpp,v $
 *		Revision 1.3  2001/02/25 08:23:06  jesjones
 *		Tweaked the line test.
 *		
 *		Revision 1.2  2000/11/09 12:51:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XShapesTest.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2001/02/25 08:23:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Tweaked the line test.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	  4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XShapesTest.h>

#include <XDrawContexts.h>
#include <XIntConversions.h>
#include <XPixMap.h>
#include <XShapes.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XShapesUnitTest
// ===================================================================================	


//---------------------------------------------------------------
//
// XShapesUnitTest::~XShapesUnitTest
//
//---------------------------------------------------------------
XShapesUnitTest::~XShapesUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XShapesUnitTest::XShapesUnitTest
//
//---------------------------------------------------------------
XShapesUnitTest::XShapesUnitTest() : XUnitTest(L"Graphics", L"XShapes")
{
}

						
//---------------------------------------------------------------
//
// XShapesUnitTest::OnTest
//
//---------------------------------------------------------------
void XShapesUnitTest::OnTest()
{
	this->DoTestLine();
	this->DoTestRect();
	this->DoTestRegion();

	TRACE("Completed XShapes test.\n\n");
}


//---------------------------------------------------------------
//
// XShapesUnitTest::DoTestLine
//
//---------------------------------------------------------------
void XShapesUnitTest::DoTestLine()
{
	XPixMap pixels(XSize(8, 6), nil, 1);
	XLocker lock(pixels);
	
	XPixMapContext dc(&pixels);
	
	XRectShape::Fill(dc, pixels.GetExtent(), kWhiteBrush);

	XLineShape::Draw(dc, XPoint(1, 0), XPoint(5, 0), kBlackPen);
	XLineShape::Draw(dc, XPoint(1, 1), XSize(1, 1), kBlackPen);
	XLineShape::Draw(dc, XPoint(0, 1), XPoint(0, 4), kBlackPen);
	
//	XPen pen(kRGBBlack, 2);
	XLineShape::Draw(dc, XPoint(1, 4), XPoint(4, 4), kBlackPen);
	
	const uint8 kPixels[6] = {
		0x7C,		// -11111--
		0xC0,		// 11------
		0xA0,		// 1-1-----
		0x80,		// 1-------
		0xF8,		// 11111---
		0x00};		// --------

	this->DoCompare(pixels, kPixels);
}


//---------------------------------------------------------------
//
// XShapesUnitTest::DoTestRect
//
//---------------------------------------------------------------
void XShapesUnitTest::DoTestRect()
{
	XPixMap pixels(XSize(8, 6), nil, 1);
	XLocker lock(pixels);
	
	XPixMapContext dc(&pixels);
	
	// Test framing a rect with a one pixel pen
	XRectShape::Fill(dc, pixels.GetExtent(), kWhiteBrush);
	XRectShape::Frame(dc, XRect(1, 1, 5, 5), kBlackPen);

	const uint8 kPixels1[6] = {
		0x00,		// --------
		0x78,		// -1111---
		0x48,		// -1--1---
		0x48,		// -1--1---
		0x78,		// -1111---
		0x00};		// --------

	this->DoCompare(pixels, kPixels1);
	
	// Test framing a rect with a two pixel pen
	XRectShape::Fill(dc, pixels.GetExtent(), kWhiteBrush);

	XPen pen(kRGBBlack, 2);
	XRectShape::Frame(dc, XRect(0, 0, 6, 6), pen);

	const uint8 kPixels2[6] = {
		0xFC,		// 111111--
		0xFC,		// 111111--
		0xCC,		// 11--11--
		0xCC,		// 11--11--
		0xFC,		// 111111--
		0xFC};		// 111111--

	this->DoCompare(pixels, kPixels2);

	// Test filling a rect
	XRectShape::Fill(dc, pixels.GetExtent(), kWhiteBrush);
	XRectShape::Fill(dc, XRect(1, 1, 5, 5), kBlackBrush);

	const uint8 kPixels3[6] = {
		0x00,		// --------
		0x78,		// -1111---
		0x78,		// -1111---
		0x78,		// -1111---
		0x78,		// -1111---
		0x00};		// --------

	this->DoCompare(pixels, kPixels3);	
}


//---------------------------------------------------------------
//
// XShapesUnitTest::DoTestRegion
//
//---------------------------------------------------------------
void XShapesUnitTest::DoTestRegion()
{
	// regions will be a pixel different on Mac and Win...
}


//---------------------------------------------------------------
//
// XShapesUnitTest::DoCompare
//
//---------------------------------------------------------------
void XShapesUnitTest::DoCompare(const XPixMap& pixels, const uint8* bits)
{
	ASSERT(pixels.GetWidth() == 8);
	ASSERT(pixels.GetHeight() == 6);
	ASSERT(pixels.GetDepth() == 1);
	ASSERT(bits != nil);
	
	for (int32 v = 0; v < 6; ++v) {
		const uint8* row = pixels.GetUnsafeBufferAt(0, v);
#if WIN
		uint8 pixel = (uint8) (~(*row));	// white and black have different pixel values on Mac and Win
#else
		uint8 pixel = *row;
#endif
		
//		TRACE("%ls\n", UInt8ToBinary(pixel).c_str());
		ASSERT(pixel == bits[v]);
	}
	
//	TRACE("\n");
}


#endif	// DEBUG
}		// namespace Whisper

