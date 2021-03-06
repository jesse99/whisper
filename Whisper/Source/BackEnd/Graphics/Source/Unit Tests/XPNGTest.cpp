/*
 *  File:       XPNGTest.cpp
 *  Summary:	PNG image import/export tests.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPNGTest.cpp,v $
 *		Revision 1.4  2001/05/30 01:18:58  jesjones
 *		Pixel value is now a uint32.
 *		
 *		Revision 1.3  2001/04/27 04:35:11  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:46:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XPNGTest.h>

#include <XColorTable.h>
#include <XDrawContexts.h>
#include <XFileSystem.h>
#include <XFolderSpec.h>
#include <XImageExporters.h>
#include <XImageImporters.h>
#include <XIntConversions.h>
#include <XMemUtils.h>
#include <XPixMap.h>
#include <XResource.h>
#include <XShapes.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XPNGTest
// ===================================================================================	


//---------------------------------------------------------------
//
// XPNGTest::~XPNGTest
//
//---------------------------------------------------------------
XPNGTest::~XPNGTest()
{
}

	
//---------------------------------------------------------------
//
// XPNGTest::XPNGTest
//
//---------------------------------------------------------------
XPNGTest::XPNGTest() : XUnitTest(L"Graphics", L"XPNG IO")
{
}

						
//---------------------------------------------------------------
//
// XPNGTest::OnTest
//
//---------------------------------------------------------------
void XPNGTest::OnTest()
{
	XPixMapPtr pixels = this->DoCreate32BitImage();
	this->DoTest(*pixels.Get());

	pixels = this->DoCreate8BitImage();
	this->DoTest(*pixels.Get());

	pixels = this->DoCreate1BitImage();
	this->DoTest(*pixels.Get());

	TRACE("Completed PNG export/import test.\n\n");
}


//---------------------------------------------------------------
//
// XPNGTest::DoCreate32BitImage
//
//   01234567
// 0 -111111-
// 1 -2------
// 2 --2-----
// 3 ---2--3-
// 4 ----2-3-
// 5 ------3-
//
//---------------------------------------------------------------
XPixMapPtr XPNGTest::DoCreate32BitImage()
{
	XPixMapPtr pixels(new XPixMap(XSize(8, 6), nil, 32));
	XLocker lock(*pixels);
	
	XPixMapContext dc(pixels.Get());
	
	XRectShape::Fill(dc, pixels->GetExtent(), kWhiteBrush);

	XLineShape::Draw(dc, XPoint(1, 0), XSize(5, 0), XRGBColor(0.9, 0.3, 0.1));
	XLineShape::Draw(dc, XPoint(1, 1), XSize(3, 3), XRGBColor(0.3, 0.9, 0.1));
	XLineShape::Draw(dc, XPoint(6, 3), XSize(0, 3), XRGBColor(0.1, 0.3, 0.9));
	
	return pixels;
}


//---------------------------------------------------------------
//
// XPNGTest::DoCreate8BitImage
//
//---------------------------------------------------------------
XPixMapPtr XPNGTest::DoCreate8BitImage()
{
	XPixMapPtr pixels(new XPixMap(XSize(8, 6), nil, 8));
	XLocker lock(*pixels);
	
	XPixMapContext dc(pixels.Get());
	
	XRectShape::Fill(dc, pixels->GetExtent(), kWhiteBrush);

	XLineShape::Draw(dc, XPoint(1, 0), XSize(6, 0), XRGBColor(0.9, 0.3, 0.1));
	XLineShape::Draw(dc, XPoint(1, 1), XSize(3, 3), XRGBColor(0.3, 0.9, 0.1));
	XLineShape::Draw(dc, XPoint(7, 3), XSize(0, 3), XRGBColor(0.1, 0.3, 0.9));
	
	return pixels;
}


//---------------------------------------------------------------
//
// XPNGTest::DoCreate1BitImage
//
//---------------------------------------------------------------
XPixMapPtr XPNGTest::DoCreate1BitImage()
{
	XPixMapPtr pixels(new XPixMap(XSize(8, 6), nil, 1));
	XLocker lock(*pixels);
	
	XPixMapContext dc(pixels.Get());
	
	XRectShape::Fill(dc, pixels->GetExtent(), kWhiteBrush);

	XLineShape::Draw(dc, XPoint(1, 0), XSize(6, 0), kBlackPen);
	XLineShape::Draw(dc, XPoint(1, 1), XSize(3, 3), kBlackPen);
	XLineShape::Draw(dc, XPoint(7, 3), XSize(0, 3), kBlackPen);
	
	return pixels;
}


//---------------------------------------------------------------
//
// XPNGTest::DoTest
//
//---------------------------------------------------------------
void XPNGTest::DoTest(const XPixMap& lhs)
{
	XFileSpec spec(XFolderSpec::GetAppFolder(), L"Import Test.png");
	
	XPNGExporter exporter(1.0);
	exporter.Export(spec, &lhs, '????', 'PNGf');
	
	try {
		XResource* data = new XResource(spec);
		XPNGImporter importer(data, 1.0);
		
		XPixMap rhs(importer); 
		this->DoCompare(lhs, rhs);
	
		XFileSystem::DeleteFile(spec);

	} catch (...) {
//		XFileSystem::DeleteFile(spec);
		throw;
	}
}
	

//---------------------------------------------------------------
//
// XPNGTest::DoDumpPixels
//
//---------------------------------------------------------------
void XPNGTest::DoDumpPixels(const XPixMap& pixels)
{
	for (int32 v = 0; v < pixels.GetHeight(); ++v) {
		for (int32 h = 0; h < pixels.GetWidth(); ++h) {
			uint32 pixel = pixels.GetPixelAt(h, v);
			XRGBColor color = pixels.PixelToColor(pixel);
			TRACE(color, "\t");
		}
		TRACE("\n");
	}
	TRACE("\n");
}


//---------------------------------------------------------------
//
// XPNGTest::DoCompare
//
//---------------------------------------------------------------
void XPNGTest::DoCompare(const XPixMap& lhs, const XPixMap& rhs)
{
	ASSERT(lhs.GetSize() == rhs.GetSize());
	ASSERT(lhs.GetDepth() == rhs.GetDepth());

	if (lhs.GetDepth() <= 8)
		ASSERT(*lhs.GetColors() == *rhs.GetColors());
	
	bool matches = true;
	uint32 bytes = ((uint32) lhs.GetDepth() * lhs.GetWidth() + 7) >> 3;	// can't use rowBytes because it may have cruft at the end of rows
	for (int32 v = 0; v < lhs.GetHeight() && matches; ++v) {
		const uint8* left  = lhs.GetUnsafeBufferAt(0, v);
		const uint8* right = rhs.GetUnsafeBufferAt(0, v);
		matches = EqualMemory(left, right, bytes);
	}
	
	if (!matches) {
		TRACE("lhs:\n");
		this->DoDumpPixels(lhs);

		TRACE("rhs:\n");
		this->DoDumpPixels(rhs);
		
		DEBUGSTR("pixels differ");
	}
}


#endif	// DEBUG
}		// namespace Whisper

