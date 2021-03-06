/*
 *  File:       XPen.h
 *  Summary:	An object used when drawing lines or framing shapes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPen.h,v $
 *		Revision 1.5  2001/04/17 01:44:34  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:42:50  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:12  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:46:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	  8/09/99	JDJ		Added streaming support. mWidth is now Win only.
 *		 <4>	  1/15/99	JDJ		Replaced kSelectionPens array with kSelectionBrushes.
 *		 <3>	  1/15/99	JDJ		Disabled XPen ctor taking an XPixMap.
 *		 <2>	 12/24/98	JDJ		Added kSelectionPens and some platform specific ctors.
 *									Added SetDrawMode.
 *		 <1>	 11/30/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XBrush.h>
#include <XDrawContext.h>
#include <XXMLPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XPen;


//-----------------------------------
//	Constants
//
extern GRAPHICS_EXPORT const XPen kBlackPen;
extern GRAPHICS_EXPORT const XPen kWhitePen;
extern GRAPHICS_EXPORT const XPen kRedPen;
extern GRAPHICS_EXPORT const XPen kYellowPen;
extern GRAPHICS_EXPORT const XPen kBluePen;
extern GRAPHICS_EXPORT const XPen kGreenPen;
extern GRAPHICS_EXPORT const XPen kBrownPen;
extern GRAPHICS_EXPORT const XPen kPurplePen;
extern GRAPHICS_EXPORT const XPen kLightBluePen;
extern GRAPHICS_EXPORT const XPen kGrayPen;
extern GRAPHICS_EXPORT const XPen kLightGrayPen;
extern GRAPHICS_EXPORT const XPen kDarkGrayPen;


// ===================================================================================
//	class XPen
//!		An object used when drawing lines or framing shapes.
// ===================================================================================
class GRAPHICS_EXPORT XPen : public XDrawAttribute, public XXMLPersistentMixin {

	typedef XDrawAttribute Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPen();		
						
						XPen(const XRGBColor& color = kRGBBlack, int32 width = 1, EDrawMode mode = kCopyMode);
						
						XPen(EHatchBrush brush, const XRGBColor& color, int32 width = 1, EDrawMode mode = kCopyMode);

//						XPen(const XPixMap& pixMap, EDrawMode mode = kCopyMode);
						// This is unsupported on Win98 (see the cpp for more details).
						// $$ Should this be enabled on Macs? NT?
						
#if MAC
						XPen(const Pattern& pattern, const XRGBColor& color, int32 width = 1, EDrawMode mode = kCopyMode);
#elif WIN
						XPen(HPEN takePen, EDrawMode mode = kCopyMode);
#endif
						
						XPen(const XPen& rhs);

			XPen& 		operator=(const XPen& rhs);
						
//-----------------------------------
//	API
//
public:
			void 		SetDrawMode(EDrawMode mode);
			
			int32 		GetWidth() const;

			bool 		operator==(const XPen& rhs) const;
			bool 		operator!=(const XPen& rhs) const		{return !this->operator==(rhs);}
			
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
//	Internal API
//
public:
#if WIN
						operator HPEN() const;
						
			int32 		GetMode() const							{return (int32) mDrawMode;}
#endif

//-----------------------------------
//	Member Data
//
protected:
	class ZPen*		mPen;

#if MAC
	mutable int16	mOldWidth;

#elif WIN
	int32			mWidth;
	EDrawMode		mDrawMode;
	mutable int32	mOldMode;
	mutable HPEN	mOldPen;
#endif
};


// ===================================================================================
//	Inlines
// ===================================================================================
#if WIN
	inline int32 XPen::GetWidth() const						
	{	
		return mWidth;
	}
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

