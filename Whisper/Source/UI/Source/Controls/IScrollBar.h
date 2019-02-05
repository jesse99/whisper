/*
 *  File:       IScrollBar.h
 *  Summary:   	A control that displays an OS scroll bar.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IScrollBar.h,v $
 *		Revision 1.4  2001/03/05 05:38:22  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:24:51  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:06:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultScrollBarProcID = kControlScrollBarLiveProc; 
const uint32 kDefaultScrollBarStyle  = WS_CHILD + WS_VISIBLE;		// SBS_HORZ and SBS_VERT will be added depending on frame orientation


// ===================================================================================
//	class IScrollBar
//!		A control that displays an OS scroll bar.
// ===================================================================================
class IScrollBar : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, int32 minValue, int32 maxValue,
							 uint32 style = kDefaultScrollBarStyle,
							 uint32 procID = kDefaultScrollBarProcID) = 0;
	//@}

	//! @name Page Delta
	//@{
	virtual int32		GetPageDelta() const = 0;
						/**< Amount to scroll when user scrolls by a page. Defaults
						to one. */
	
	virtual void		SetPageDelta(int32 newDelta) = 0;
	//@}

	//! @name Arrow Delta
	//@{
	virtual int32		GetArrowDelta() const = 0;
						/**< Amount to scroll when user scrolls using arrows. Defaults
						to one. */
						
	virtual void		SetArrowDelta(int32 newDelta) = 0;
	//@}

	//! @name Misc
	//@{
	virtual void 		UpdateThumb(int32 imageSize, int32 viewSize, bool redraw = kRedraw) = 0;
						/**< Used for proportional scrollbars: if viewSize is half imageSize
						the thumb will fill half the scrollbar. */
						
#if WIN
	virtual void 		HandleTrack(int32 code) = 0;
#endif
	//@}
};


typedef XInterfacePtr<IScrollBar> IScrollBarPtr;
typedef XInterfacePtr<const IScrollBar> IConstScrollBarPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
