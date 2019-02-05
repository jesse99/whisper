/*
 *  File:       XShape.cpp
 *  Summary:	An abstract base class for objects that know how to draw themselves
 *				into an XDrawContext.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XShape.cpp,v $
 *		Revision 1.2  2000/11/09 12:51:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 12/11/97	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XShape.h>

namespace Whisper {


// ===================================================================================
//	class XShape
// ===================================================================================

//---------------------------------------------------------------
//
// XShape::~XShape
//
//---------------------------------------------------------------
XShape::~XShape()
{
}


//---------------------------------------------------------------
//
// XShape::XShape
//
//---------------------------------------------------------------
XShape::XShape()
{
}


}	// namespace Whisper

