/*
 *  File:       XSelectAttribute.cpp
 *  Summary:	Stack based class used to change a draw context's attributes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XSelectAttribute.cpp,v $
 *		Revision 1.2  2000/11/09 12:50:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 12/01/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XSelectAttribute.h>

#include <XDrawContext.h>

namespace Whisper {


// ===================================================================================
//	class XSelectAttribute
// ===================================================================================

//---------------------------------------------------------------
//
// XSelectAttribute::~XSelectAttribute
//
//---------------------------------------------------------------
XSelectAttribute::~XSelectAttribute()
{
	mAttr.DoDeSelect(mContext.GetOSContext());
}
						
						
//---------------------------------------------------------------
//
// XSelectAttribute::XSelectAttribute
//
//---------------------------------------------------------------
XSelectAttribute::XSelectAttribute(XDrawContext& context, const XDrawAttribute& attr) : mAttr(attr), mContext(context)	
{
	mAttr.DoSelect(mContext.GetOSContext());
	
#if MAC && DEBUG
	GrafPtr port;
	::GetPort(&port);
	ASSERT(port == mContext.GetOSContext());	// MShapes.cpp code relies on this
#endif
}


}	// namespace Whisper

