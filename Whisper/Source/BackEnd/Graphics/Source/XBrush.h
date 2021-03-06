/*
 *  File:       XBrush.h
 *  Summary:	An object used when filling shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBrush.h,v $
 *		Revision 1.5  2001/04/17 01:43:16  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:40:40  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:50:34  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:27:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	  8/06/99	JDJ		Added support for streaming and kSolidBrush.
 *									Corrected EHatchBrush (for WPen.cpp).
 *		 <3>	  1/15/99	JDJ		Added kSelectionBrushes.
 *		 <2>	 12/24/98	JDJ		Added SetDrawMode and some platform specific ctors.
 *		 <1>	 11/30/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XDrawAttribute.h>
#include <XDrawContext.h>
#include <XInvariant.h>
#include <XRGBColor.h>
#include <XXMLPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XBrush;
class XPixMap;
class ZBrush;


//-----------------------------------
//	Constants
//
enum EHatchBrush {kHorzBrush = 0, kVertBrush = 1, kDiagnalBrush = 2, kCrossBrush = 4, kSolidBrush = 256};	// values are for Windows (eg HS_HORIZONTAL)

extern GRAPHICS_EXPORT const XBrush kBlackBrush;
extern GRAPHICS_EXPORT const XBrush kWhiteBrush;
extern GRAPHICS_EXPORT const XBrush kRedBrush;
extern GRAPHICS_EXPORT const XBrush kYellowBrush;
extern GRAPHICS_EXPORT const XBrush kBlueBrush;
extern GRAPHICS_EXPORT const XBrush kGreenBrush;
extern GRAPHICS_EXPORT const XBrush kBrownBrush;
extern GRAPHICS_EXPORT const XBrush kPurpleBrush;
extern GRAPHICS_EXPORT const XBrush kLightBlueBrush;
extern GRAPHICS_EXPORT const XBrush kGrayBrush;
extern GRAPHICS_EXPORT const XBrush kLightGrayBrush;
extern GRAPHICS_EXPORT const XBrush kDarkGrayBrush;

const uint32 kNumSelectionBrushes = 8;
extern GRAPHICS_EXPORT const XBrush* kSelectionBrushes;		// these are used when drawing selection rectangles (see XTrackers.cpp for an example)


// ===================================================================================
//	class XBrush
//!		An object used when filling shapes.
// ===================================================================================
class GRAPHICS_EXPORT XBrush : public XDrawAttribute, public XXMLPersistentMixin {

	typedef XDrawAttribute Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XBrush();		
						
						XBrush(const XRGBColor& color = kRGBBlack, EDrawMode mode = kCopyMode);
						
						XBrush(EHatchBrush brush, const XRGBColor& color, EDrawMode mode = kCopyMode);
						
						XBrush(const XPixMap& pixMap, EDrawMode mode = kCopyMode); 
						/**< Width and height must be eight or less on Windows and a power 
						of two on the Mac. */
						
#if MAC
						XBrush(const Pattern& pattern, const XRGBColor& color, EDrawMode mode = kCopyMode);
#elif WIN
						XBrush(HBRUSH takeBrush, EDrawMode mode = kCopyMode);
#endif
						
						XBrush(const XBrush& rhs);

			XBrush& 	operator=(const XBrush& rhs);
						
//-----------------------------------
//	New API
//
public:
#if WIN
						operator HBRUSH() const;
#endif

			void 		SetDrawMode(EDrawMode mode)				{mDrawMode = mode;}
			
			bool 		operator==(const XBrush& rhs) const;
			bool 		operator!=(const XBrush& rhs) const		{return !this->operator==(rhs);}
			
//-----------------------------------
//	Inherited API
//
public:
	virtual	void 		DoSelect(OSDrawContext context) const;

	virtual	void 		DoDeSelect(OSDrawContext context) const;

protected:
			void 		Invariant() const;

	virtual	void 		OnStreamOut(XXMLDoc& doc, XXMLElement& root) const;

	virtual void 		OnStreamIn(const XXMLDoc& doc, const XXMLElement& root, const XXMLElement& element);

//-----------------------------------
//	Member Data
//
protected:
	ZBrush*					mBrush;
	EDrawMode				mDrawMode;

#if MAC
	mutable Pattern			mOldPattern;
	mutable int16			mOldMode;
	mutable XRGBColor		mOldColor;
	mutable PixPatHandle	mOldPixPat;		

#elif WIN	
	mutable int32			mOldMode;
	mutable HBRUSH			mOldBrush;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

