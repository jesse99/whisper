/*
 *  File:       MPicture.h
 *  Summary:	Encapsulates a PicHandle.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1996-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 * 
 *		$Log: MPicture.h,v $
 *		Revision 1.3  2000/12/10 08:48:48  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:17:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: MPicture.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:48:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  5/31/97	JDJ		Added a Write method. Added a ctor to create a
 *									picture from a TGWorld.
 *		 <1>	  4/07/96	JDJ		Created.
 */

#pragma once

#include <ImageCompression.h>
#include <QuickDraw.h>
#include <Resources.h>

#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class MPicture;
class XColorTable;
class XPixMap;
class ZPictureRef;


// ===================================================================================
//	class MOpenPicture
//!		Drawing operations are routed to the picture while this class is alive.
// ===================================================================================
class MOpenPicture {

public:
						~MOpenPicture();
						
						MOpenPicture(MPicture& picture);

						MOpenPicture(MPicture& picture, const XRect& frame);
						
private:	
			void 		DoInit(const XRect& frame);
			
private:
	MPicture&	mPicture;
};


// ===================================================================================
//	class MPicture
//!		Encapsulates a PicHandle.
// ===================================================================================
class MPicture {

	friend class MOpenPicture;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~MPicture();
						
						MPicture();
						
	explicit			MPicture(ResID id);
						
						MPicture(const XPixMap& offscreen, int16 mode = srcCopy);

						MPicture(const FSSpec& file);
						
						MPicture(PicHandle takePictH);
						
						MPicture(const MPicture& rhs);

						MPicture& operator=(const MPicture& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
						operator PicHandle() const;
	//@}

	//! @name Drawing
	//@{
			void 		Draw(const XPoint& where = kZeroPt, int16 doDither = defaultDither) const;
						/**< doDither can be defaultDither, forceDither, or suppressDither.
						See DrawTrimmedPicture for details. */

			void 		Draw(const XRect& dstRect, int16 doDither = defaultDither) const;

			void 		Draw(const XRect& srcRect, const XRect& dstRect, int16 doDither = defaultDither) const;
			 			/**< Note that the rectangles must be the same size. */
	//@}

	//! @name Accessors
	//@{
			XSize 		GetSize() const;
		
			XRect 		GetExtent() const;
		
			int16 		GetDepth() const;
			
			int32 		GetBytes() const;
			
			XColorTable* GetColors() const;
	//@}
		
	//! @name Writing
	//@{
			void 		Write(int16 refNum);
						/**< Write the picture out to a file. (You can write to a resource
						using the PicHandle). */
	//@}

//-----------------------------------
//	Member Data
//
protected:
	ZPictureRef*	mPictureRef;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
