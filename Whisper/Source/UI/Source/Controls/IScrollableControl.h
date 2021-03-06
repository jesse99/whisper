/*
 *  File:       IScrollableControl.h
 *  Summary:   	A user control that is capable of being scrolled.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IScrollableControl.h,v $
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:24:46  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:06:44  jesjones
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
//	class IScrollableControl
//!		A user control that is capable of being scrolled.
/*!		IScrollableControl's are normally placed inside of IScrollerControl's (see 
 *		IScrollerControl.h for more details). When used in this way all of the scrolling 
 *		happens automatically. All that you have to do is set the image size (see below) 
 *		and draw the control's contents.
 *
 *		IScrollableControl uses two rectangles to do its work. The larger rectangle is 
 *		called the image rectangle. The image rectangle's topLeft is always kZeroPt and 
 *		the size should be equal to the size of whatever the control is scrolling about 
 *		in. The second rectangle is called the revealed rectangle and can be thought of 
 *		as the part of the image the control is currently displaying. As IScrollableControl 
 *		scrolls it offsets the revealed rectangle to show different parts of the image. 
 *		By default the revealed rectangle is the same size as the control.
 *
 *		IScrollableControl has a few options that you can tweak. The first is the reconcile 
 *		overhang flag. When this flag is set the control will attempt to keep the revealed 
 *		rectangle completely within the image. This comes into play when doing things like 
 *		enlarging the control when it has already been scrolled to show the bottom right of 
 *		the image. By default the reconcile overhang flag is on.
 *
 *		The second option is the adjust origin flag. When this flag is set the draw context's 
 *		origin is adjusted by the amount the control has scrolled. This means that your 
 *		control's OnDraw method can do its drawing in image coordinates. For example, you can 
 *		draw objects that are located at various places within the image rectangle and only 
 *		the objects within the revealed rectangle will show up. By default the adjust origin 
 *		flag is set. */
// ===================================================================================
class IScrollableControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init() = 0;
						/**< Call this after IUserControl::Init. */
	//@}

	//! @name Scrolling
	//@{
	virtual void 		ScrollTo(const XPoint& loc, bool redraw = kRedraw) = 0;
						/**< Sets mRevealedRect[topLeft] to loc. */
	
	virtual void 		ScrollBy(const XPoint& delta, bool redraw = kRedraw) = 0;
						/**< Offsets mRevealedRect by delta. */
	
	virtual void 		ScrollIntoView(const XPoint& loc, bool redraw = kRedraw) = 0;
						/**< Shifts the revealed rect so that loc is visible. */

	virtual void 		AutoScroll(const XPoint& localPt) = 0;
						/**< If localPt is outside the control the control is scrolled in that
						direction. Note that this is called for you by the default track 
						constrain mechanism. */
						
	virtual XPoint 		GetScrollOffset() const = 0;
						/**< Returns an offset by which to offset a draw context (or kZeroPt
						if adjust origin is false). */
	//@}

	//! @name Image Size
	//@{
	virtual XSize		GetImageSize() const = 0;
			
	virtual void 		SetImageSize(const XSize& newSize, bool redraw = kRedraw) = 0;
						/**< Call this to let the control know how large the image is. Note that you
						can only use images larger than MAX_SHRT with the mAdjustOrigin off. */
	//@}
	
	//! @name Revealed Rect
	//@{
	virtual XRect		GetRevealedRect() const = 0;
						/**< Returns the area within the image the control is currently displaying. */
	//@}
						
	//! @name Options
	//@{
	virtual void 		SetAdjustOrigin(bool value = true) = 0;
						/**< Defaults to true. */

	virtual void 		SetReconcileOverhang(bool value = true) = 0;
						/**< Defaults to true. */
	//@}
};


typedef XInterfacePtr<IScrollableControl> IScrollableControlPtr;
typedef XInterfacePtr<const IScrollableControl> ConstScrollableControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
