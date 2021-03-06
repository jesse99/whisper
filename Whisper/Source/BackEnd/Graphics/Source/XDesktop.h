/*
 *	File:		XDesktop.h
 *	Function:	Encapsulates the notion of a desktop (ie an object containing one or more displays).
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDesktop.h,v $
 *		Revision 1.4  2000/12/10 08:51:33  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/09 09:24:01  jesjones
 *		Fixed the log comment.
 *		
 *		Revision 1.2  2000/11/09 08:49:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added Log keyword.
 *		
 *		 <2>	 7/09/00	JDJ		Added GetClientArea.
 *		 <1>	 1/16/98	JDJ		Created
 */

#pragma once

#include <XDisplay.h>
#include <XRegion.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XDesktop
//!		Encapsulates the notion of a desktop (ie an object containing one or more displays).
// ===================================================================================
class GRAPHICS_EXPORT XDesktop {

//-----------------------------------
//	Types
//
public:
	class iterator {

	public:						
					iterator();				
					iterator(OSDisplay display);	

		XDisplay 	operator*() const							{return mDisplay;}
		iterator& 	operator++();
		
		bool 		operator==(const iterator& rhs) const;	
		bool 		operator!=(const iterator& rhs) const		{return !this->operator==(rhs);}
	
	private:
		OSDisplay	mDisplay;
	};

//-----------------------------------
//	API
//
public:
	//! @name Main Display
	//@{
	static	XDisplay 	GetMainDisplay();
						/**< $$$ Does this make sense on Windows? */
	//@}

	//! @name Display Intersecting
	//@{
	static	XDisplay 	GetMaxDisplay(const XRect& globalRect);
						/**< Returns the display that intersects the most of rect. */

	static	XDisplay 	GetShallowDisplay(const XRect& globalRect);
						/**< Returns the shallowest (pixel depth) display intersecting rect. */

	static	XDisplay 	GetDeepestDisplay(const XRect& globalRect);
						/**< Returns the deepest display intersecting rect. */
	//@}

	//! @name Misc
	//@{
	static	iterator 	begin();
	static	iterator	end();
	
	static 	XRegion 	GetClientArea();
						/**< Returns the region in which app windows will be visible (eg on
						the Mac this will remove the menubar, on Windows it may remove
						the taskbar). */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
