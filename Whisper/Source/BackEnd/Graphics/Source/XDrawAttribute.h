/*
 *  File:       XDrawAttribute.h
 *  Summary:	An object used to modify how a draw context draws.
 *  Written by: Jesse Jones
 *
 *	Classes:	XDrawAttribute	- Abstract base class for draw context attributes.
 *					XPen		- Used when drawing lines or framing shapes.
 *					XBrush		- Used when filling shapes.
 *					XFont		- Used when drawing text.
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XDrawAttribute.h,v $
 *		Revision 1.3  2000/12/10 08:51:43  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:33:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XDrawAttribute.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:51:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 12/01/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XDrawAttribute
//!		Abstract base class for draw context attributes.
// ===================================================================================
class GRAPHICS_EXPORT XDrawAttribute {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDrawAttribute()							{}
						
						XDrawAttribute()							{}

protected:						
						XDrawAttribute(const XDrawAttribute&)		{}
						/**< protected to avoid slicing problems */
						
			XDrawAttribute& operator=(const XDrawAttribute&)		{return *this;}
						
//-----------------------------------
//	API
//
public:
	virtual	void 		DoSelect(OSDrawContext context) const = 0;
						/**< context will now draw using 'this'. Note that it's generally
						safer and easier to use XSelectAttribute. */

	virtual void 		DoDeSelect(OSDrawContext context) const = 0;
						/**< Restores the old attribute. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

