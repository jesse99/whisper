/*
 *  File:       IScrollerControl.h
 *  Summary:   	A user control used to scroll an IScrollableControl.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IScrollerControl.h,v $
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:24:55  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:06:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 8/02/00	JDJ		Created
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XRect.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IScrollerControl
//!		A user control used to scroll an IScrollableControl.
/*!		To scroll a control you need to create a subclass of XScrollableControl and 
 *		place it in a XScrollerControl. To do this you can stream the control hierachy 
 *		in or use IScrollerControl::InstallView. IScrollerControl will build horizontal 
 *		and/or vertical scroll bars and handle all of the details involved in scrolling 
 *		the IScrollableView so you should not have to create a subclass of IScrollerControl. 
 *		See XScrollableView.h for information on how to use IScrollableView. */
// ===================================================================================
class IScrollerControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(bool hasVertBar = true, bool hasHorzBar = true, 
							 const XRect& indent = kZeroRect, bool fillsWindow = true) = 0;
						/**< Call this after IUserControl::Init. Use indent if you want to
						put widgets next to the scroll bars. If the scroller doesn't 
						fill the entire window set fillsWindow to false. */

	// ----- Scroll Units -----
	virtual XSize		GetScrollUnits() const = 0;
						/**< Scroll bar arrows will scroll by this amount. Defaults
						to one. */
						
	virtual void		SetScrollUnits(const XSize& newUnits) = 0;

	// ----- Misc -----
	virtual XRect		GetContentExtent() const = 0;
						/**< Returns the scroller's extent minus the area occupied by the scroll bars. */
						
	virtual void 		ScrollAreaHasChanged(bool redraw = kRedraw) = 0;
						/**< Called by XScrollableControl when the image size or revealed
						rect changes. */
};


typedef XInterfacePtr<IScrollerControl> IScrollerControlPtr;
typedef XInterfacePtr<const IScrollerControl> IConstScrollerControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
