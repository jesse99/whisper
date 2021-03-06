/*
 *  File:		XPersistent.cpp
 *  Summary:	Base class for objects that can be flattened.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPersistent.cpp,v $
 *		Revision 1.2  2000/11/09 12:46:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/20/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XPersistent.h>

namespace Whisper {

 
// ===================================================================================
//	class XPersistentMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XPersistentMixin::~XPersistentMixin
//
//---------------------------------------------------------------
XPersistentMixin::~XPersistentMixin()
{
}


//---------------------------------------------------------------
//
// XPersistentMixin::XPersistentMixin
//
//---------------------------------------------------------------
XPersistentMixin::XPersistentMixin(bool persistant)
{
	mPersistant = persistant;
}


//---------------------------------------------------------------
//
// XPersistentMixin::IsPersistant
//
//---------------------------------------------------------------
bool XPersistentMixin::IsPersistant() const
{
	return mPersistant;
}


}	// namespace Whisper
