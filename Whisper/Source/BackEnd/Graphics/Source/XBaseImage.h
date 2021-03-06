/*
 *  File:       XBaseImage.h
 *  Summary:   	Abstract base class for objects that can be initialized using an XImageSource.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBaseImage.h,v $
 *		Revision 1.6  2001/05/30 01:17:32  jesjones
 *		Pixel values are now uint32's (instead of int32's).
 *		
 *		Revision 1.5  2001/04/17 01:42:58  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:40:28  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:50:19  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:26:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>
#include <XInvariant.h>
#include <XLockable.h>
#include <XLocker.h>
#include <XRect.h>
#include <XRGBColor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XColorTable;
class XImageData;


// ===================================================================================
//	class XBaseImage
//!		Abstract base class for objects that can be initialized using an XImageSource.
// ===================================================================================
class GRAPHICS_EXPORT XBaseImage : public XLockableMixin {

	typedef XLockableMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBaseImage();

						XBaseImage();

	virtual XBaseImage* Clone() const = 0;

private:
						XBaseImage(const XBaseImage& rhs);
	
			XBaseImage& operator=(const XBaseImage& rhs);

//-----------------------------------
//	New API
//
public:
	//! @name Size
	//@{
			XRect 		GetExtent() const								{return XRect(kZeroPt, mSize);}
			
			XSize 		GetSize() const									{return mSize;}
			int32 		GetWidth() const								{return mSize.width;}
			int32 		GetHeight() const								{return mSize.height;}
						
			uint32 		GetRowBytes() const								{return mRowBytes;}
	//@}

	//! @name Colors
	//@{
			const XColorTable* GetColors() const						{ASSERT(mColors != nil); return mColors;}
	virtual	void 		SetColors(const XColorTable* newColors) = 0;
			
			OSColor 	PixelToColor(uint32 pixel) const;
			uint32 		ColorToPixel(const OSColor& color) const;
	//@}

	//! @name Direct Pixel Access
	//@{
			uint32 		GetBufferSize() const							{return (uint32) mSize.height*mRowBytes;}
		
			const uint8* GetBuffer() const								{ASSERT(this->IsLocked()); return this->GetUnsafeBuffer();}
			uint8* 		GetBuffer()										{ASSERT(this->IsLocked()); return this->GetUnsafeBuffer();}
		
			const uint8* GetBufferAt(int32 h, int32 v) const			{ASSERT(this->IsLocked()); return this->GetUnsafeBufferAt(h, v);}
			uint8* 		GetBufferAt(int32 h, int32 v)					{ASSERT(this->IsLocked()); return this->GetUnsafeBufferAt(h, v);}

	virtual	uint8* 		GetUnsafeBuffer() = 0;	
			const uint8* GetUnsafeBuffer() const						{return const_cast<XBaseImage*>(this)->GetUnsafeBuffer();}
						/**< Unlike GetBuffer these can be called when the pixmap is unlocked. */
		
			uint8* 		GetUnsafeBufferAt(int32 h, int32 v);
			const uint8* GetUnsafeBufferAt(int32 h, int32 v) const;

			uint32 		GetPixelAt(int32 h, int32 v) const;
			void 		SetPixelAt(int32 h, int32 v, uint32 value);
	//@}

	//! @name Color Channels
	//@{
	virtual	uint32		GetAlphaMask() const = 0;
	virtual	uint32		GetRedMask() const = 0;
	virtual	uint32		GetGreenMask() const = 0;
	virtual	uint32		GetBlueMask() const = 0;
						/**< These return the size and position of the color channels 
						of images with depth > 8. For example, a 32-bit GWorld 
						will return 0x00FF0000 for the red mask whereas a 32-bit 
						DIB will return 0x000000FF. */
	//@}

	//! @name Misc
	//@{
	virtual OSDrawContext GetOSContext() const = 0;

			int32 		GetDepth() const								{return mDepth;}
					
			void 		Erase(uint8 value = 0);
						/**< Sets all the pixels to value. Note that this works only with indexed offscreens. */
	//@}
						
//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;

//-----------------------------------
//	Member Data
//
protected:
	int32			mDepth;
	XSize			mSize;
	uint32			mRowBytes;

	XColorTable*	mColors;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

