/*
 *  File:       XPixMap.h
 *  Summary:   	Wrapper around an offscreen whose pixels are accessible.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPixMap.h,v $
 *		Revision 1.4  2001/03/05 05:42:56  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:22  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:46:32  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/08/99	JDJ		Added CreateCompatibleBitmap.
 *		 <3>	 1/13/99	JDJ		Added GetAlphaMask, GetRedMask, etc.
 *		 <2>	 1/06/99	JDJ		Implemented on Windows.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#if MAC
	#include <QDOffscreen.h>
#endif

#include <XCountedPtr.h>
#include <XBaseImage.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XImageImporter;


// ===================================================================================
//	class XPixMap
//!		Wrapper around an offscreen whose pixels are accessible.
// ===================================================================================
class GRAPHICS_EXPORT XPixMap : public XBaseImage {

	typedef XBaseImage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPixMap();

						XPixMap(const XSize& size, const XColorTable* colors, int32 depth = 8);
						/**< If colors is nil the default colors are used. */
						
						XPixMap(const XImageImporter& image);

						XPixMap(const XImageImporter& image, const XColorTable* colors, int32 depth = 8);

	virtual XPixMap* 	Clone() const;

//-----------------------------------
//	New API
//
public:
	virtual void 		SetSize(const XSize& size);
						/**< If size is different from current size the new pixmap contents
						are undefined. */

#if WIN
	virtual HBITMAP		CreateCompatibleBitmap(HDC dc) const;
#endif
	
//-----------------------------------
//	Inherited API
//
public:
	virtual	void 		SetColors(const XColorTable* newColors);

			const uint8* GetUnsafeBuffer() const					{return Inherited::GetUnsafeBuffer();}	
	virtual	uint8* 		GetUnsafeBuffer();	
						
	virtual OSDrawContext GetOSContext() const;
		
	virtual	uint32		GetAlphaMask() const;
	virtual	uint32		GetRedMask() const;
	virtual	uint32		GetGreenMask() const;
	virtual	uint32		GetBlueMask() const;

protected:
	virtual void 		Invariant() const;
	
	virtual void 		OnLock(bool moveHigh);
		
	virtual void 		OnUnlock();

//-----------------------------------
//	Internal API
//
public:
			OSPixMap 	GetOSPixMap() const;
			
#if MAC
			GWorldPtr 	GetWorld() const								{return mWorld;}
		
	virtual void 		SetPort()										{ASSERT(this->IsLocked()); SetGWorld(mWorld, nil);}
						/**< Set current port to the offscreen's GWorld. Note that this also 
						changes the device! */
#elif WIN
			const BITMAPINFO* GetInfo() const;
#endif

private:
			void		DoInit(const XSize& size, const XColorTable* colors, int32 depth);

//-----------------------------------
//	Internal Types
//
public:
#if WIN
	struct SBitmapInfo : public BITMAPINFO {	

						SBitmapInfo(const XSize& size, int32 depth, const XColorTable* colors);

		static	void* 	operator new(size_t size);
		static	void 	operator delete(void* ptr);
};
#endif

//-----------------------------------
//	Member Data
//
protected:
#if MAC	
	GWorldPtr		mWorld;
	uint8* 			mCachedBuffer;		// valid while GWorld is locked

#elif WIN
	HBITMAP			mDIB;
	SBitmapInfo*	mInfo;
	uint8*			mBits;
	
	HDC				mDC;
#endif
};


//-----------------------------------
//	Types
//
typedef XCountedPtr<XPixMap> XPixMapPtr;


// ===================================================================================
//	Inlines
// ===================================================================================
#if MAC
	inline OSDrawContext XPixMap::GetOSContext() const								
	{
		ASSERT(this->IsLocked()); return (GrafPtr) mWorld;
	}	

	inline OSPixMap XPixMap::GetOSPixMap() const								
	{	
		return GetGWorldPixMap(mWorld);
	}
	
#elif WIN
	inline OSDrawContext XPixMap::GetOSContext() const								
	{
		return mDC;
	}	

	inline OSPixMap XPixMap::GetOSPixMap() const								
	{	
		return mDIB;
	}

	inline const BITMAPINFO* XPixMap::GetInfo() const
	{
		return mInfo;
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

