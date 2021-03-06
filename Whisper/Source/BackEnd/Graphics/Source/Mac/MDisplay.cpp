/*
 *	File:		MDisplay.cpp
 *	Function:	Encapsulates a display device (typically a monitor).
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDisplay.cpp,v $
 *		Revision 1.5  2001/04/21 03:25:14  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/04/13 07:49:43  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/14 11:06:32  jesjones
 *		Allow for 0 Hz refresh rate (LCDs).
 *		
 *		Revision 1.2  2000/11/09 09:12:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XDisplay.h>

#include <Displays.h>
#include <FixMath.h>

#include <MSystemInfo.h>
#include <XExceptions.h>
#include <XTinyVector.h>

namespace Whisper {


//-----------------------------------
//	Types
//
struct ZDepthInfo {							
	VDSwitchInfoRec		switchInfo;				// Cookie used to choose this timing/depth
	VPBlock				vpBlock;				// Resolution, depth, and pixel format of this timing/depth.
};

struct ZModeInfo {
	VDTimingInfoRec			timingInfo;			// Contains timing flags and such
	VDResolutionInfoRec		resolutionInfo;		// Max resolution and refresh rate of this timing mode
	XTinyVector<ZDepthInfo>	depths;				// Depths for this timing mode.
};


// ===================================================================================
//	class ZDisplayIterator
// ===================================================================================
class ZDisplayIterator {

public:
							~ZDisplayIterator();				
							
							ZDisplayIterator();				
							ZDisplayIterator(const XDisplay* display);	
							ZDisplayIterator(const ZDisplayIterator& rhs);	
			
			ZDisplayIterator& operator=(const ZDisplayIterator& rhs);

			XDisplayState 	GetState() const			{PRECONDITION(mIterating); return mState;}
			void 			Advance();
			
			bool 			operator==(const ZDisplayIterator& rhs) const;	

protected:
			bool 			GotDisplayMode();

			void 			DoInitTables();

	static 	pascal void 	DoModeListIterator(void* userData, DMListIndexType, DMDisplayModeListEntryRec* displaymodeInfo);

private:
	GDHandle						mDisplay;
	
	bool							mIterating;

	DMListType						mDisplayList;
	ZModeInfo						mModeInfo;			
	DisplayIDType 					mDisplayID;				

	DMListIndexType					mModeCount;			// timing modes
	DMListIndexType					mModeIndex;
	
	uint32							mDepthCount;		// depths for a timing mode
	uint32							mDepthIndex;
	
	XDisplayState					mState;

	DMDisplayModeListIteratorUPP	mIterProc;			
};


//---------------------------------------------------------------
//
// ZDisplayIterator::~ZDisplayIterator
//
//---------------------------------------------------------------
ZDisplayIterator::~ZDisplayIterator()
{
	if (mDisplayList != nil)
		(void) DMDisposeList(mDisplayList);
	
	if (mIterProc != nil)
#if TARGET_CARBON
		DisposeDMDisplayModeListIteratorUPP(mIterProc);
#else
		DisposeRoutineDescriptor(mIterProc);
#endif
}

				
//---------------------------------------------------------------
//
// ZDisplayIterator::ZDisplayIterator ()
//
//---------------------------------------------------------------
ZDisplayIterator::ZDisplayIterator()
{
	mIterating = false;
	mDisplay = nil;
	
	mDisplayList = nil;
	mIterProc = nil;
}

				
//---------------------------------------------------------------
//
// ZDisplayIterator::ZDisplayIterator (XDisplay*)
//
//---------------------------------------------------------------
ZDisplayIterator::ZDisplayIterator(const XDisplay* display)
{
	PRECONDITION(display != nil);
	PRECONDITION(MSystemInfo::HasDisplayMgr());
	PRECONDITION(MSystemInfo::GetDisplayMgrVersion() >= 0x0200);
	
	mIterating = true;
	mDisplayList = nil;
	mIterProc = nil;
	mModeIndex = 0;
	mDepthIndex = 0;
	
	mDisplay = *display;

	OSErr err = DMGetDisplayIDByGDevice(mDisplay, &mDisplayID, false);
	ThrowIfOSErr(err);

	mModeCount = 0;	
	err = DMNewDisplayModeList(mDisplayID, 0, 0, &mModeCount, &mDisplayList);
	ThrowIfOSErr(err);
	
	mIterProc = NewDMDisplayModeListIteratorUPP(DoModeListIterator);	
	ThrowIfNil(mIterProc);

	mModeIndex = (uint32) -1;
	mDepthCount = 0;
	mDepthIndex = mDepthCount;
	
	this->Advance();
}

	
//---------------------------------------------------------------
//
// ZDisplayIterator::ZDisplayIterator (ZDisplayIterator)
//
//---------------------------------------------------------------
ZDisplayIterator::ZDisplayIterator(const ZDisplayIterator& rhs)
{
	mIterating = false;
	mDisplayList = nil;
	mIterProc = nil;
	
	mDisplay = rhs.mDisplay;

	if (rhs.mDisplay != nil) {
		mModeCount = 0;
		OSErr err = DMNewDisplayModeList(rhs.mDisplayID, 0, 0, &mModeCount, &mDisplayList);
		ThrowIfOSErr(err);
		ASSERT(mModeCount == rhs.mModeCount);
	
		mIterating = rhs.mIterating;
		mState = rhs.mState;
		
		mModeIndex = rhs.mModeIndex;
		mDepthIndex = rhs.mDepthIndex;
		mDepthCount = rhs.mDepthCount;
		mDisplayID = rhs.mDisplayID;
	
		mIterProc = NewDMDisplayModeListIteratorUPP(DoModeListIterator);	
		ThrowIfNil(mIterProc);
		
		if (mModeIndex < mModeCount)
			this->DoInitTables();
	}
}
	

//---------------------------------------------------------------
//
// ZDisplayIterator::operator=
//
//---------------------------------------------------------------
ZDisplayIterator& ZDisplayIterator::operator=(const ZDisplayIterator& rhs)
{
	if (this != &rhs) {
		if (rhs.mDisplay != nil) {
			DMListType			tempDisplayList;
			DMListIndexType		tempCount = 0;
	
			OSErr err = DMNewDisplayModeList(rhs.mDisplayID, 0, 0, &tempCount, &tempDisplayList);
			ThrowIfOSErr(err);
			ASSERT(tempCount == rhs.mModeCount);
	
			mDisplay = rhs.mDisplay;
			mIterating = rhs.mIterating;
			mState = rhs.mState;
			
			mModeCount = tempCount;
			mDepthCount = rhs.mDepthCount;
			mModeIndex = rhs.mModeIndex;
			mDepthIndex = rhs.mDepthIndex;
			mDisplayID = rhs.mDisplayID;
	
			if (mDisplayList != nil) {
				OSErr err = DMDisposeList(mDisplayList);
				ASSERT(err == noErr);
			}	
			
			mDisplayList = tempDisplayList;
	
			if (mModeIndex < mModeCount)
				this->DoInitTables();
				
		} else {
			if (mDisplayList != nil) {
				OSErr err = DMDisposeList(mDisplayList);
				ASSERT(err == noErr);
			}	

			mDisplay = nil;
			mIterating = false;
			mDisplayList = nil;
			mModeInfo.depths = XTinyVector<ZDepthInfo>(0);
			mIterProc = nil;
		}
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// ZDisplayIterator::Advance
//
//---------------------------------------------------------------
void ZDisplayIterator::Advance()
{	
	while (mIterating) {
		mDepthIndex++;
		if (mDepthIndex < mDepthCount) {
			if (this->GotDisplayMode())
				break;
				
		} else {
			mDepthIndex = 0;
			mModeIndex++;
			
			if (mModeIndex < mModeCount) {
				this->DoInitTables();

				if (mDepthCount > 0 && this->GotDisplayMode())
					break;
				
			} else
				mIterating = false;
		}
	}
}


//---------------------------------------------------------------
//
// ZDisplayIterator::operator==
//
//---------------------------------------------------------------
bool ZDisplayIterator::operator==(const ZDisplayIterator& rhs) const		
{
	bool equal = false;
	
	if (mDisplay == nil || rhs.mDisplay == nil) {
		equal = mIterating == rhs.mIterating;
	
	} else {
		ASSERT(mDisplay == rhs.mDisplay); 
	
		equal = mModeIndex == rhs.mModeIndex && mDepthIndex == rhs.mDepthIndex;
	}
	
	return equal;
}	


//---------------------------------------------------------------
//
// ZDisplayIterator::GotDisplayMode
//
//---------------------------------------------------------------
bool ZDisplayIterator::GotDisplayMode()
{
	PRECONDITION(mIterating);
	PRECONDITION(mDepthCount > 0);
	PRECONDITION(mModeCount > 0);
	PRECONDITION(mDepthIndex < mDepthCount);
	PRECONDITION(mModeIndex < mModeCount);
	
	bool found = false;

	// only if the mode is valid and is safe 
	if	(mModeInfo.timingInfo.csTimingFlags & 1<<kModeValid) {
		if (mModeInfo.timingInfo.csTimingFlags & 1<<kModeSafe) {
			found = true;
						
			mState = XDisplayState(mDisplay, mModeInfo, mDepthIndex);
		}
	}
	
	return found;
}


//---------------------------------------------------------------
//
// ZDisplayIterator::DoInitTables
//
//---------------------------------------------------------------
void ZDisplayIterator::DoInitTables()
{
	PRECONDITION(mModeCount > 0);
	PRECONDITION(mModeIndex < mModeCount);
	
	// get the mode lists for this GDevice
	OSErr err = DMGetIndexedDisplayModeFromList(mDisplayList, mModeIndex, 0, mIterProc, &mModeInfo);
	ThrowIfOSErr(err);

	mDepthCount = mModeInfo.depths.size();
}


//---------------------------------------------------------------
//
// ZDisplayIterator::DoModeListIterator
//
// Lifted from the Display Manager sample code.
//
//---------------------------------------------------------------
pascal void ZDisplayIterator::DoModeListIterator(void* userData, DMListIndexType, DMDisplayModeListEntryRec* displayModeInfo)
{
	ZModeInfo* searchData = reinterpret_cast<ZModeInfo*>(userData);	

	// set user data in a round about way
	searchData->timingInfo     = *displayModeInfo->displayModeTimingInfo;
	searchData->resolutionInfo = *displayModeInfo->displayModeResolutionInfo;
	
	// now get the DMDepthInfo info into memory we own
	uint32 depthCount = displayModeInfo->displayModeDepthBlockInfo->depthBlockCount;
	searchData->depths = XTinyVector<ZDepthInfo>(depthCount);				

	// and fill out all the entries
	for (uint32 index = 0; index < depthCount; index++) {
		searchData->depths[index].switchInfo = *displayModeInfo->displayModeDepthBlockInfo->depthVPBlock[index].depthSwitchInfo;
		searchData->depths[index].vpBlock    = *displayModeInfo->displayModeDepthBlockInfo->depthVPBlock[index].depthVPBlock;
	}
}

#pragma mark -

// ===================================================================================
//	class XDisplayState
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplayState::XDisplayState ()
//
//---------------------------------------------------------------
XDisplayState::XDisplayState()
{
	mDisplay = nil;
}


//---------------------------------------------------------------
//
// XDisplayState::XDisplayState (GDHandle, ZModeInfo, uint32)
//
//---------------------------------------------------------------
XDisplayState::XDisplayState(GDHandle display, const ZModeInfo& info, uint32 depthIndex)
{
	PRECONDITION(display != nil);
	
	mDisplay = display;
	mState = info.depths[depthIndex].switchInfo;
	
	mResolution.width  = info.depths[depthIndex].vpBlock.vpBounds.right - info.depths[depthIndex].vpBlock.vpBounds.left;
	mResolution.height = info.depths[depthIndex].vpBlock.vpBounds.bottom - info.depths[depthIndex].vpBlock.vpBounds.top;

	mDepth = info.depths[depthIndex].vpBlock.vpPixelSize;
	
	mRefreshRate = Fix2X(info.resolutionInfo.csRefreshRate);
	ASSERT(mRefreshRate >= 0.0);				// LCDs have a 0 refresh rate...
	ASSERT(mRefreshRate < 256.0);
}

#pragma mark -

// ===================================================================================
//	class XDisplay::iterator
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplay::iterator::~iterator
//
//---------------------------------------------------------------
XDisplay::iterator::~iterator()
{
	delete mIter;
}

				
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator ()
//
//---------------------------------------------------------------
XDisplay::iterator::iterator()
{
	mIter = new ZDisplayIterator;
}

				
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator (XDisplay*)
//
//---------------------------------------------------------------
XDisplay::iterator::iterator(const XDisplay* display)
{
	mIter = new ZDisplayIterator(display);
}

	
//---------------------------------------------------------------
//
// XDisplay::iterator::iterator (iterator)
//
//---------------------------------------------------------------
XDisplay::iterator::iterator(const iterator& rhs)
{
	mIter = new ZDisplayIterator(*rhs.mIter);
}
	

//---------------------------------------------------------------
//
// XDisplay::iterator::operator=
//
//---------------------------------------------------------------
XDisplay::iterator& XDisplay::iterator::operator=(const iterator& rhs)
{
	if (this != &rhs) {
		ZDisplayIterator* temp = new ZDisplayIterator(*rhs.mIter);

		delete mIter;
		mIter = temp;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator*
//
//---------------------------------------------------------------
XDisplayState XDisplay::iterator::operator*() const
{
	return mIter->GetState();
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator++
//
//---------------------------------------------------------------
XDisplay::iterator& XDisplay::iterator::operator++()
{	
	mIter->Advance();
	
	return *this;
}


//---------------------------------------------------------------
//
// XDisplay::iterator::operator==
//
//---------------------------------------------------------------
bool XDisplay::iterator::operator==(const iterator& rhs) const		
{
	bool equal = *mIter == *rhs.mIter;
	
	return equal;
}	

#pragma mark -

// ===================================================================================
//	class XDisplay
// ===================================================================================

//---------------------------------------------------------------
//
// XDisplay::XDisplay
//
//---------------------------------------------------------------
XDisplay::XDisplay(const OSDisplay& display)
{
	PRECONDITION(display != nil);
	
	mDisplay = display;
}


//---------------------------------------------------------------
//
// XDisplay::GetLocation
//
//---------------------------------------------------------------
XPoint XDisplay::GetLocation() const
{
	XRect frame = (**mDisplay).gdRect;
	
	XPoint loc = frame[topLeft];
	
	return loc;
}


//---------------------------------------------------------------
//
// XDisplay::GetResolution
//
//---------------------------------------------------------------
XSize XDisplay::GetResolution() const
{
	XRect frame = (**mDisplay).gdRect;
	
	XSize size = frame.GetSize();
	
	return size;
}


//---------------------------------------------------------------
//
// XDisplay::GetDepth
//
//---------------------------------------------------------------
int32 XDisplay::GetDepth() const
{
	int32 depth = (**(**mDisplay).gdPMap).pixelSize;
	
	return depth;
}


//---------------------------------------------------------------
//
// XDisplay::IsColor
//
//---------------------------------------------------------------
bool XDisplay::IsColor() const
{
	bool isColor = TestDeviceAttribute(mDisplay, gdDevType);
	
	return isColor;
}


//---------------------------------------------------------------
//
// XDisplay::GetClientArea
//
//---------------------------------------------------------------
XRect XDisplay::GetClientArea() const
{
	XRect area = this->GetFrame();
	
	if (mDisplay == ::GetMainDevice())
		area.top += ::GetMBarHeight();	// $$$ should also remove app-wide floaters (eg the control strip)
	
	return area;		// $$$ probably should return a region
}


//---------------------------------------------------------------
//
// XDisplay::GetState
//
// From RequestVideo sample code.
//
//---------------------------------------------------------------
OSDisplayState XDisplay::GetState() const
{
	PRECONDITION(MSystemInfo::HasDisplayMgr());
	PRECONDITION(MSystemInfo::GetDisplayMgrVersion() >= 0x0200);

	VDSwitchInfoRec switchInfo;
	
	OSErr err = DMGetDisplayMode(mDisplay, &switchInfo);	
	ThrowIfOSErr(err);
	
	return switchInfo;
}


//---------------------------------------------------------------
//
// XDisplay::SetState
//
//---------------------------------------------------------------
bool XDisplay::SetState(const OSDisplayState& state)
{
	PRECONDITION(MSystemInfo::HasDisplayMgr());
	PRECONDITION(MSystemInfo::GetDisplayMgrVersion() >= 0x0200);

	OSErr err = DMSetDisplayMode(mDisplay,
			0,								// DM1.0 uses this
			nil,							// DM1.0 uses this
			(uint32) &state,				// DM2.0 uses this rather than displayMode/depthMode combo
			nil);
	
	return err == noErr;
}


//---------------------------------------------------------------
//
// XDisplay::begin
//
//---------------------------------------------------------------
XDisplay::iterator XDisplay::begin() const
{
	PRECONDITION(MSystemInfo::HasDisplayMgr());
	PRECONDITION(MSystemInfo::GetDisplayMgrVersion() >= 0x0200);

	return iterator(this);
}


//---------------------------------------------------------------
//
// XDisplay::end
//
//---------------------------------------------------------------
XDisplay::iterator XDisplay::end() const
{
	PRECONDITION(MSystemInfo::HasDisplayMgr());
	PRECONDITION(MSystemInfo::GetDisplayMgrVersion() >= 0x0200);

	return iterator();
}


}		// namespace Whisper
