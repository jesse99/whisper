/*
 *  File:       MPicture.h
 *  Summary:	Encapsulates a PicHandle.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1996-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MPicture.cpp,v $
 *		Revision 1.6  2001/04/21 03:27:29  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:41:19  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/04/13 07:53:03  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2000/12/10 04:04:48  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:17:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	  2/14/98	JDJ		Uses TSetPort instead of SetPort (PowerPlant freaks out
 *									if the port is changed without calling LView::OutOfFocus).
 *		 <8>	 12/15/97	JDJ		ZPictureRef::DoInit only calls DisposeCTable if color
 *									table isn't nil.
 *		 <7>	 12/14/97	JDJ		ZPictureRef::DoInit sets current port to the work port.
 *		 <6>	  5/31/97	JDJ		Added a Write method. Added a ctor to create a
 *									picture from a XPixMap. Uses KillPicture instead
 *									of DisposeHandle.
 *		 <5>	  5/02/97	JDJ		Throws if FSClose fails.
 *		 <4>	  4/12/97	JDJ		Doesn't call PRECONDITION in dtor.
 *		 <3>	  4/12/97	JDJ		ZPictureRef descends from TInvariantMixin.
 *		 <2>	 12/05/96	JDJ		GetPictInfo is no longer called right away.
 *		 <1>	  4/07/96	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <MPicture.h>

#include <Files.h>
#include <ImageCompression.h>
#include <PictUtils.h>
#include <Resources.h>

#include <MQuickDrawUtils.h>
#include <MSystemInfo.h>
#include <XColorTable.h>
#include <XConstants.h>
#include <XExceptions.h>
#include <XMemUtils.h>
#include <XNumbers.h>
#include <XPixMap.h>
#include <XRegion.h>
#include <XReferenceCounted.h>

namespace Whisper {


// ===================================================================================
//	class ZPictureRef
// ===================================================================================
class ZPictureRef : public XReferenceCountedMixin {

	typedef XReferenceCountedMixin Inherited;
	
	friend class MOpenPicture;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZPictureRef();
						
						ZPictureRef();

	explicit			ZPictureRef(ResID id);
						
						ZPictureRef(PicHandle takePictH);

						ZPictureRef(const FSSpec& file);

private:						
						ZPictureRef(const ZPictureRef& rhs);

						ZPictureRef& operator=(const ZPictureRef& rhs);
						
//-----------------------------------
//	Drawing
//
public:
			void 		Draw(const XPoint& where, int16 doDither) const;

			void 		Draw(const XRect& dstRect, int16 doDither) const;

			void 		Draw(const XRect& srcRect, const XRect& dstRect, int16 doDither) const;

//-----------------------------------
//	Accessors
//
public:
			XSize 		GetSize() const			{return XSize(mWidth, mHeight);}
		
			XRect 		GetExtent() const		{return XRect(kZeroPt, XSize(mWidth, mHeight));}
		
			int16 		GetDepth() const;
			
			XColorTable* GetColors() const;
			
			PicHandle	GetPicture() const		{return mPictH;}
			
//-----------------------------------
//	Internal API
//
public:
			void		Invariant() const;
	
			void		DoInit();
			
//-----------------------------------
//	Member Data
//
protected:
	PicHandle		mPictH;
	int16			mWidth;
	int16			mHeight;
	int16			mDepth;
	XColorTable*	mColors;
	
	bool			mInited;
};


//---------------------------------------------------------------
//
// ZPictureRef::~ZPictureRef
//
//---------------------------------------------------------------
ZPictureRef::~ZPictureRef()
{
	if (mPictH != nil)
		KillPicture(mPictH);
	
	delete mColors;
}


//---------------------------------------------------------------
//
// ZPictureRef::ZPictureRef ()
//
//---------------------------------------------------------------
ZPictureRef::ZPictureRef()
{
	MSetPort setter(GetWorkPort());

	XRect extent(0, 0, 1, 1);
	Rect temp = extent.GetOSRect();
	mPictH = ::OpenPicture(&temp);
	ThrowIfNil(mPictH);
	
	::EraseRect(&temp);
	
	::ClosePicture();
	
	mColors = nil;
	mDepth  = 0;
	mWidth  = 0;
	mHeight = 0;
	mInited = false;
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZPictureRef::ZPictureRef (ResID)
//
//---------------------------------------------------------------
ZPictureRef::ZPictureRef(ResID id)
{
	mPictH = ::GetPicture(id);
	ThrowIfResFail(mPictH);
	
	try {
		HNoPurge((Handle) mPictH);
		ThrowIfMemError();
		
		DetachResource((Handle) mPictH);
		ThrowIfResError();

	} catch (...) {
		ReleaseResource((Handle) mPictH);
		throw;
	}
		
	mColors = nil;
	mDepth  = 0;
	mWidth  = numeric_cast<int16>((**mPictH).picFrame.right - (**mPictH).picFrame.left);
	mHeight = numeric_cast<int16>((**mPictH).picFrame.bottom - (**mPictH).picFrame.top);
	mInited = false;
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZPictureRef::ZPictureRef (PicHandle)
//
//---------------------------------------------------------------
ZPictureRef::ZPictureRef(PicHandle takePictH)
{
	PRECONDITION(takePictH != nil);
	PRECONDITION(*takePictH != nil);
	PRECONDITION(!IsResource((Handle) takePictH));
	
	mPictH = takePictH;
	
	mColors = nil;
	mDepth  = 0;
	mWidth  = numeric_cast<int16>((**mPictH).picFrame.right - (**mPictH).picFrame.left);
	mHeight = numeric_cast<int16>((**mPictH).picFrame.bottom - (**mPictH).picFrame.top);
	mInited = false;
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZPictureRef::ZPictureRef (FSSpec)
//
//---------------------------------------------------------------
ZPictureRef::ZPictureRef(const FSSpec& file)
{
	mPictH = nil;
	
	OSErr err;
	
	int16 refNum = kNoFileRefNum;
	err = FSpOpenDF(&file, fsRdPerm, &refNum);	
	ThrowIfOSErr(err);
	
	try {
		int32 size;
		err = GetEOF(refNum, &size);
		ThrowIfOSErr(err);
		
		size -= 512;
		err = SetFPos(refNum, fsFromMark, 512);
		ThrowIfOSErr(err);
		
		mPictH = (PicHandle) ::NewHandle(size);
		ThrowIfMemFail(mPictH);
		
		HLock((Handle) mPictH);
		
		int32 bytes = size;
		err = FSRead(refNum, &bytes, *mPictH);
		HUnlock((Handle) mPictH);
		ThrowIfOSErr(err);
		
		err = FSClose(refNum);
		refNum = kNoFileRefNum;
		ThrowIfOSErr(err);
		
	} catch (...) {
		if (refNum != kNoFileRefNum)
			(void) FSClose(refNum);

		if (mPictH != nil)
			KillPicture(mPictH);

		throw;
	}
	
	mColors = nil;
	mDepth  = 0;
	mWidth  = numeric_cast<int16>((**mPictH).picFrame.right - (**mPictH).picFrame.left);
	mHeight = numeric_cast<int16>((**mPictH).picFrame.bottom - (**mPictH).picFrame.top);
	mInited = false;

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZPictureRef::Draw (XPoint)
//
//---------------------------------------------------------------
void ZPictureRef::Draw(const XPoint& where, int16 doDither) const
{
	XRect dstRect = this->GetExtent() + where;
	this->Draw(dstRect, doDither);
}


//---------------------------------------------------------------
//
// ZPictureRef::Draw (XRect)
//
//---------------------------------------------------------------
void ZPictureRef::Draw(const XRect& dstRect, int16 doDither) const
{
	CHECK_INVARIANT;
	
	Rect temp = dstRect.GetOSRect();
	if (MSystemInfo::HasQuickTime()) {
		OSErr err = ::DrawTrimmedPicture(mPictH, &temp, nil, doDither, nil);
		ASSERT(err == noErr);			// don't want to throw here?
		
	} else {
		::DrawPicture(mPictH, &temp);
		
		QDErr err = QDError();
		ASSERT(err == noErr);			// don't throw for drawing errors?
	}
}


//---------------------------------------------------------------
//
// ZPictureRef::Draw (XRect, XRect)
//
//---------------------------------------------------------------
void ZPictureRef::Draw(const XRect& srcRect, const XRect& dstRect, int16 doDither) const
{
	PRECONDITION(srcRect.GetSize() == dstRect.GetSize());
	CHECK_INVARIANT;
	
	XRegion clip(dstRect);

	XRect bounds = this->GetExtent() - srcRect[topLeft] + dstRect[topLeft];
	Rect temp = bounds.GetOSRect();

	if (MSystemInfo::HasQuickTime()) {
		OSErr err = ::DrawTrimmedPicture(mPictH, &temp, clip, doDither, nil);
		ASSERT(err == noErr);			// don't want to throw here?
		
	} else {
		MIntersectClip setClip(clip);
		
		::DrawPicture(mPictH, &temp);
		
		QDErr err = QDError();
		ASSERT(err == noErr);			// don't throw for drawing errors?
	}
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// ZPictureRef::GetDepth
//
//---------------------------------------------------------------
int16 ZPictureRef::GetDepth() const		
{
	if (!mInited) {
		ZPictureRef* thisPtr = const_cast<ZPictureRef*>(this);
		
		thisPtr->DoInit();
	}
	
	return mDepth;
}


//---------------------------------------------------------------
//
// ZPictureRef::GetColors
//
//---------------------------------------------------------------
XColorTable* ZPictureRef::GetColors() const		
{
	if (!mInited) {
		ZPictureRef* thisPtr = const_cast<ZPictureRef*>(this);
		
		thisPtr->DoInit();
	}
	
	return mColors;
}

#pragma mark ~

//---------------------------------------------------------------
//
// ZPictureRef::Invariant
//
//---------------------------------------------------------------
void ZPictureRef::Invariant() const
{	
	ASSERT(mPictH != nil);
	
	ASSERT(mWidth >= 0);
	ASSERT(mHeight >= 0);

	if (mInited) {
		ASSERT(mDepth == 1 || mDepth == 2 || mDepth == 4 || mDepth == 8 || mDepth == 16 || mDepth == 32);
		ASSERT(mColors != nil);
		
	} else
		ASSERT(mColors == nil);
}


//---------------------------------------------------------------
//
// ZPictureRef::DoInit
//
// According to the profiler GetPictInfo is a rather expensive
// operation. Because of this we'll defer calling it until we
// really need to.
//
//---------------------------------------------------------------
void ZPictureRef::DoInit()
{
	PRECONDITION(!mInited);
	PRECONDITION(mPictH != nil);
	PRECONDITION(mColors == nil);
		
	PictInfo pictInfo;
	pictInfo.theColorTable = nil;

	try {
		MSetPort setter(GetWorkPort());
		OSErr err = GetPictInfo(mPictH, &pictInfo, returnColorTable, 256, systemMethod, 0);
		ThrowIfOSErr(err);

		if (mWidth == 0 && mHeight == 0) {
			mWidth  = numeric_cast<int16>(pictInfo.sourceRect.right - pictInfo.sourceRect.left);
			mHeight = numeric_cast<int16>(pictInfo.sourceRect.bottom - pictInfo.sourceRect.top);

		} else {
			ASSERT(mWidth == pictInfo.sourceRect.right - pictInfo.sourceRect.left);
			ASSERT(mHeight == pictInfo.sourceRect.bottom - pictInfo.sourceRect.top);
		}
		
		mDepth = pictInfo.depth;	
		if (mDepth == 0)
			mDepth = 1;

		mColors = new XColorTable(pictInfo.theColorTable);
		
		if (pictInfo.theColorTable != nil)
			DisposeCTable(pictInfo.theColorTable);
		
	} catch (...) {
		delete mColors;
		mColors = nil;
		
		if (pictInfo.theColorTable != nil)
			DisposeCTable(pictInfo.theColorTable);

		throw;
	}
	
	mInited = true;
}

#pragma mark -

// ===================================================================================
//	class MOpenPicture
// ===================================================================================

//---------------------------------------------------------------
//
// MOpenPicture::~MOpenPicture
//
//---------------------------------------------------------------
MOpenPicture::~MOpenPicture()
{
	::ClosePicture();

	delete mPicture.mPictureRef->mColors;		
	mPicture.mPictureRef->mColors = nil;

	mPicture.mPictureRef->DoInit();
}

						
//---------------------------------------------------------------
//
// MOpenPicture::MOpenPicture (MPicture)
//
//---------------------------------------------------------------
MOpenPicture::MOpenPicture(MPicture& picture) : mPicture(picture)
{
	this->DoInit(picture.GetExtent());
}


//---------------------------------------------------------------
//
// MOpenPicture::MOpenPicture (MPicture, XRect)
//
//---------------------------------------------------------------
MOpenPicture::MOpenPicture(MPicture& picture, const XRect& frame)  : mPicture(picture)
{
	this->DoInit(frame);
}


//---------------------------------------------------------------
//
// MOpenPicture::DoInit
//
//---------------------------------------------------------------
void MOpenPicture::DoInit(const XRect& frame)
{
	MSetPort setter(GetWorkPort());
	::PenNormal();
	::SetOrigin(0, 0);
	Rect temp = frame.GetOSRect();
	::ClipRect(&temp);
#ifdef __PowerPlant__
	LView::OutOfFocus(nil);
#endif
	
	PicHandle pictH = ::OpenPicture(&temp);
	ThrowIfNil(pictH);
	
	if (mPicture.mPictureRef->mPictH != nil)
		KillPicture(mPicture.mPictureRef->mPictH);

	mPicture.mPictureRef->mPictH = pictH;
}

#pragma mark -

// ===================================================================================
//	class MPicture
// ===================================================================================

//---------------------------------------------------------------
//
// MPicture::~MPicture
//
//---------------------------------------------------------------
MPicture::~MPicture()
{
	mPictureRef->RemoveReference();
}


//---------------------------------------------------------------
//
// MPicture::MPicture ()
//
//---------------------------------------------------------------
MPicture::MPicture()
{
	mPictureRef = new ZPictureRef;
}


//---------------------------------------------------------------
//
// MPicture::MPicture (ResID)
//
//---------------------------------------------------------------
MPicture::MPicture(ResID id)
{
	mPictureRef = new ZPictureRef(id);		// $$�use a cache?
}


//---------------------------------------------------------------
//
// MPicture::MPicture (XPixMap)
//
//---------------------------------------------------------------
MPicture::MPicture(const XPixMap& offscreen, int16 mode)
{
	PicHandle pictH = nil;
	
	{
	MSetPort setter(GetWorkPort());
	XLocker lock(offscreen);

		// Create the pict
		XRect bounds = offscreen.GetExtent();
		Rect temp = bounds.GetOSRect();
		ClipRect(&temp);
		
		pictH = ::OpenPicture(&temp);
		ThrowIfNil(pictH);
		
		// Draw the pixmap
		CGrafPtr srcPort = offscreen.GetOSContext();
		CGrafPtr dstPort = GetQDGlobalsThePort();

		PrepCopyBits();
		OSErr err = noErr;
			
		{
		XLocker lock(offscreen);
			Rect extent = offscreen.GetExtent().GetOSRect();
			CopyColorBits(srcPort, dstPort, &extent, &extent, mode, nil);
			
			err = QDError();
		}

		// Close the pict
		::ClosePicture();
		if (err == noErr)
			err = QDError();
		
		if (err != noErr) {
			KillPicture(pictH);
			ThrowErr(err);
		}
	}
	
	try {
		mPictureRef = new ZPictureRef(pictH);
		
	} catch (...) {
		KillPicture(pictH);
		throw;
	}
}


//---------------------------------------------------------------
//
// MPicture::MPicture (PicHandle)
//
//---------------------------------------------------------------
MPicture::MPicture(PicHandle takePictH)
{
	try {
		mPictureRef = new ZPictureRef(takePictH);
		
	} catch (...) {
		KillPicture(takePictH);
		throw;
	}
}


//---------------------------------------------------------------
//
// MPicture::MPicture (FSSpec)
//
//---------------------------------------------------------------
MPicture::MPicture(const FSSpec& file)
{
	mPictureRef = new ZPictureRef(file);
}


//---------------------------------------------------------------
//
// MPicture::MPicture (MPicture)
//
//---------------------------------------------------------------
MPicture::MPicture(const MPicture& rhs)
{
	mPictureRef = rhs.mPictureRef;
	mPictureRef->AddReference();
}


//---------------------------------------------------------------
//
// MPicture::operator=
//
//---------------------------------------------------------------
MPicture& MPicture::operator=(const MPicture& rhs)
{
	if (mPictureRef != rhs.mPictureRef) {
		mPictureRef->RemoveReference();

		mPictureRef = rhs.mPictureRef;
		mPictureRef->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// MPicture::operator PicHandle
//
//---------------------------------------------------------------
MPicture::operator PicHandle() const
{
	return mPictureRef->GetPicture();
}


//---------------------------------------------------------------
//
// MPicture::Draw (XPoint)
//
//---------------------------------------------------------------
void MPicture::Draw(const XPoint& where, int16 doDither) const
{
	mPictureRef->Draw(where, doDither);
}


//---------------------------------------------------------------
//
// MPicture::Draw (XRect)
//
//---------------------------------------------------------------
void MPicture::Draw(const XRect& dstRect, int16 doDither) const
{
	mPictureRef->Draw(dstRect, doDither);
}


//---------------------------------------------------------------
//
// MPicture::Draw (XRect, XRect)
//
//---------------------------------------------------------------
void MPicture::Draw(const XRect& srcRect, const XRect& dstRect, int16 doDither) const
{
	mPictureRef->Draw(srcRect, dstRect, doDither);
}


//---------------------------------------------------------------
//
// MPicture::GetSize
//
//---------------------------------------------------------------
XSize MPicture::GetSize() const
{
	return mPictureRef->GetSize();
}


//---------------------------------------------------------------
//
// MPicture::GetExtent
//
//---------------------------------------------------------------
XRect MPicture::GetExtent() const
{
	return mPictureRef->GetExtent();
}


//---------------------------------------------------------------
//
// MPicture::GetDepth
//
//---------------------------------------------------------------
int16 MPicture::GetDepth() const
{
	return mPictureRef->GetDepth();
}


//---------------------------------------------------------------
//
// MPicture::GetBytes
//
//---------------------------------------------------------------
int32 MPicture::GetBytes() const
{
	PicHandle pict = mPictureRef->GetPicture();

	return GetHandleSize((Handle) pict);
}


//---------------------------------------------------------------
//
// MPicture::GetColors
//
//---------------------------------------------------------------
XColorTable* MPicture::GetColors() const
{
	return mPictureRef->GetColors();
}


//---------------------------------------------------------------
//
// MPicture::Write
//
//---------------------------------------------------------------
void MPicture::Write(int16 refNum)
{	
	// Write a 512 byte header full of zeros.
	Byte header[512];
	ClearMemory(header, 512);

	int32 bytes = 512;
	OSErr err = FSWrite(refNum, &bytes, header);
	ThrowIfOSErr(err);

	// Write the picture data.
	PicHandle pictH = *this;
	
	SignedByte oldState = HGetState((Handle) pictH);		// file system will move memory if it has to pop up a dialog (eg disk needs to be inserted)
	HLock((Handle) pictH);
	
	bytes = GetHandleSize((Handle) pictH);
	err = FSWrite(refNum, &bytes, *pictH);
	HSetState((Handle) pictH, oldState);
	ThrowIfOSErr(err);
}


}	// namespace Whisper
