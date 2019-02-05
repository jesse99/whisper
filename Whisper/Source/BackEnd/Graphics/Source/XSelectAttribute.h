/*
 *  File:       XSelectAttribute.h
 *  Summary:	Stack based class used to change a draw context's attributes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XSelectAttribute.h,v $
 *		Revision 1.4  2001/03/05 05:43:15  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:23  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:51:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDrawAttribute.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDrawContext;


// ===================================================================================
//	class XSelectAttribute
//!		Stack based class used to change a draw context's attributes.
// ===================================================================================
class GRAPHICS_EXPORT XSelectAttribute {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XSelectAttribute();
						
						XSelectAttribute(XDrawContext& context, const XDrawAttribute& attr);
						/**< Use this to select a new pen, brush, or font into the context. */

private:
						XSelectAttribute(const XSelectAttribute& rhs);

			XSelectAttribute& operator=(const XSelectAttribute& rhs);

//-----------------------------------
//	Member Data
//
protected:
	const XDrawAttribute&	mAttr;
	XDrawContext&			mContext;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

