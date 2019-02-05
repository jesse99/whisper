/*
 *  File:       MImageImporters.cpp
 *  Summary:	Classes that can be used to initialize XImageSink's.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: MImageImporters.cpp,v $
 *		Revision 1.3  2001/04/21 03:26:39  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:14:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	  6/11/99	JDJ		Tweaked for CW Pro 5fc3.
 *		 <2>	 12/13/98	JDJ		Now constructed using an XResource.
 *		 <1>	 11/27/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MImageImporters.h>

#include <ImageCompression.h>
#include <PictUtils.h>
#include <Resources.h>

#include <MQuickDrawUtils.h>
#include <XColorTable.h>
#include <XConstants.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XHandle.h>
#include <XBaseImage.h>
#include <XResource.h>

namespace Whisper {


// ===================================================================================
//	class MPictImporter
// ===================================================================================

//---------------------------------------------------------------
//
// MPictImporter::~MPictImporter
//
//---------------------------------------------------------------
MPictImporter::~MPictImporter()
{
	KillPicture(mPictH);
}


//---------------------------------------------------------------
//
// MPictImporter::MPictImporter
//
//---------------------------------------------------------------
MPictImporter::MPictImporter(XResource* data)
{
	PRECONDITION(data != nil);
	PRECONDITION(mColors == nil);
	
	mPictH = nil;

	try {		
		// Move the data into a PICT handle
		uint32 offset = 0;	
		if (data->IsFromFile())
			offset = 512;
	
		uint32 bytes = data->GetSize() - offset;
		Handle hand = NewHandle(bytes);
		ThrowIfMemFail(hand);
		
		BlockMoveData(data->GetUnsafePtr() + offset, *hand, bytes);
	
		mPictH = (PicHandle) hand;
		
		// Get the picture info
		PictInfo pictInfo;
		pictInfo.theColorTable = nil;
	
		::SetPort(GetWorkPort());				// GetPictInfo crashes if port is invalid
		OSErr err = GetPictInfo(mPictH, &pictInfo, returnColorTable, 256, systemMethod, 0);
		ThrowIfOSErr(err);
	
		mWidth  = pictInfo.sourceRect.right - pictInfo.sourceRect.left;
		mHeight = pictInfo.sourceRect.bottom - pictInfo.sourceRect.top;
		
		mDepth = pictInfo.depth;	
		if (mDepth == 0)
			mDepth = 1;
	
		if (mDepth < 16)
			mColors = new XColorTable(pictInfo.theColorTable);
		else
			mColors = nil;
		
		if (pictInfo.theColorTable != nil)
			DisposeCTable(pictInfo.theColorTable);
	
	} catch (...) {
		if (mPictH != nil)
			KillPicture(mPictH);

		data->RemoveReference();
		throw;
	}		
		
	data->RemoveReference();
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// MPictImporter::Create								[static]
//
//---------------------------------------------------------------
XImageImporter* MPictImporter::Create(XResource* takeData)
{
	XImageImporter* image = new MPictImporter(takeData);
	
	return image;
}


//---------------------------------------------------------------
//
// MPictImporter::Unpack (XBaseImage*, XPoint)
//
//---------------------------------------------------------------
void MPictImporter::Unpack(XBaseImage* sink, const XPoint& where) const
{
	PRECONDITION(sink != nil);
	CHECK_INVARIANT;
	
	{
	XLocker lock(sink);
	MSetPort setPort(sink->GetOSContext());
	
		Rect bounds = (this->GetExtent() + where).GetOSRect();

		OSErr err = ::DrawTrimmedPicture(mPictH, &bounds, nil, suppressDither, nil);
		ASSERT(err == noErr);			// don't want to throw here?
	}
}


//---------------------------------------------------------------
//
// MPictImporter::Unpack (XBaseImage*, XRect, XRect)
//
//---------------------------------------------------------------
void MPictImporter::Unpack(XBaseImage* sink, const XRect& srcRect, const XRect& dstRect) const
{
	PRECONDITION(sink != nil);
	PRECONDITION(sink->GetExtent().Contains(dstRect));
	PRECONDITION(srcRect.GetSize() == dstRect.GetSize());
	CHECK_INVARIANT;
	
	{
	XLocker lock(sink);
	MSetPort setPort(sink->GetOSContext());
	
		Rect bounds = (this->GetExtent() - srcRect[topLeft] + dstRect[topLeft]).GetOSRect();

		OSErr err = ::DrawTrimmedPicture(mPictH, &bounds, nil, suppressDither, nil);
		ASSERT(err == noErr);			// don't want to throw here?
	}
}

#pragma mark Ê

//---------------------------------------------------------------
//
// MPictImporter::Invariant
//
//---------------------------------------------------------------
void MPictImporter::Invariant() const
{
	Inherited::Invariant();
	
	ASSERT(mPictH != nil);
}


}	// namespace Whisper

