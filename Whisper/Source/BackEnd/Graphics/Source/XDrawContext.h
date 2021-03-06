/*
 *  File:       XDrawContext.h
 *  Summary:	Abstract base class for objects that can be drawn into.
 *  Written by: Jesse Jones
 *
 *	Classes:	XDrawContext	- A context used to draw into a graf port/DC.
 *				XSaveContext	- Stack based class used to save the origin and clip region of a draw context.
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDrawContext.h,v $
 *		Revision 1.4  2001/03/05 05:41:13  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:47  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:33:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XRect.h>
#include <XRegion.h>
#include <XRGBColor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDrawContext;


//-----------------------------------
//	Constants
//
#if MAC
	enum EDrawMode {kCopyMode 		 = patCopy,				//!< dest = source 
					kNotCopyMode	 = notPatCopy,			//!< dest = (NOT source) 
					kOrMode			 = patOr,				//!< dest = source OR dest
					kNotOrMode		 = notPatOr,			//!< dest = (NOT source) OR dest
					kXOrMode		 = patXor,				//!< dest = source XOR dest 
					kClearMode		 = patBic,				//!< dest = source AND dest
					kNotClearMode	 = notPatBic,			//!< dest = (NOT source) AND dest
					kHiliteMode		 = hilitetransfermode,	//!< system hiliting
					kTransparentMode = transparent			//!< like kCopyMode except pixels with index 0 (or white if true color) aren't copied
					};

#elif WIN
	enum EDrawMode {kCopyMode 		 = R2_COPYPEN,
					kNotCopyMode	 = R2_NOTCOPYPEN,
					kOrMode			 = R2_MERGEPEN,
					kNotOrMode		 = R2_MERGEPENNOT,
					kXOrMode		 = R2_XORPEN,
					kClearMode		 = R2_MASKPEN,
					kNotClearMode	 = R2_MASKNOTPEN,
					kHiliteMode		 = R2_NOTCOPYPEN,
					kTransparentMode = -1};	
#endif		

GRAPHICS_EXPORT std::wstring 	DrawModeToStr(EDrawMode mode);
GRAPHICS_EXPORT EDrawMode 		StrToDrawMode(const std::wstring& str);


// ===================================================================================
//	class XSaveContext
//!		Stack based class used to save the origin and clip region of a draw context.
// ===================================================================================
class GRAPHICS_EXPORT XSaveContext {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XSaveContext();
						
						XSaveContext(OSDrawContext context);
						
//-----------------------------------
//	Member Data
//
protected:
	OSDrawContext	mContext;

#if MAC
	XPoint			mOldOrigin;		
	XRegion			mOldClip;
	
#elif WIN
	XPoint			mOldWindowOrigin;		
	XPoint			mOldViewOrigin;		
	XRegion			mOldClip;
	bool			mHadClip;
#endif
};


// ===================================================================================
//	class XDrawContext
//!		A context used to draw into a graf port/DC.
// ===================================================================================
class GRAPHICS_EXPORT XDrawContext  {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDrawContext();
						
						XDrawContext(OSDrawContext context);
						
#if MAC && !TARGET_CARBON
						XDrawContext(CGrafPtr context);
#endif
						
private:
						XDrawContext(const XDrawContext& rhs);
						
			XDrawContext& operator=(const XDrawContext& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Origin
	//@{
			XPoint 		GetOrigin() const;

	virtual void 		SetOrigin(const XPoint& newOrigin);
	//@}
		
	//! @name Clip Region
	//@{
			XRegion 	GetClip() const;
	
	virtual void 		SetClip(const XRegion& newClip);
	//@}

	//! @name Inquiry
	//@{
			bool 		IsDynamic() const							{return mDynamicCanvas;}
						/**< Returns true if the context is interactive (eg a printer context
						would return false). */
						
			bool 		IsOffscreen() const							{return mOffsceenCanvas;}
						/**< Returns true if we're drawing into a pixmap or printer context. */
	
			OSDrawContext GetOSContext() const						{return mContext;}

			XRect 		GetExtent() const;
	//@}

	//! @name Misc
	//@{
#if MAC
			void 		MakeCurrent();
						/**< Sets the global grafport to 'this'. */
#endif
	//@}

//-----------------------------------
//	Member Data
//
protected:
	OSDrawContext	mContext;
	
	bool			mDynamicCanvas;
	bool			mOffsceenCanvas;

	XSaveContext	mSaveContext;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

