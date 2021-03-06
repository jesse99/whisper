/*
 *  File:		XBinaryPersistent.cpp
 *  Summary:	Mixin for objects that can be flattened onto a binary stream.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBinaryPersistent.cpp,v $
 *		Revision 1.4  2001/04/21 03:39:36  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/17 01:43:02  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:26:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XBinaryPersistent.h>

#include <XDebug.h>
#include <XInvariant.h>
#include <XStream.h>

namespace Whisper {


// ===================================================================================
//	class XBinaryPersistentMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XBinaryPersistentMixin::~XBinaryPersistentMixin
//
//---------------------------------------------------------------
XBinaryPersistentMixin::~XBinaryPersistentMixin()
{
}


//---------------------------------------------------------------
//
// XBinaryPersistentMixin::XBinaryPersistentMixin
//
//---------------------------------------------------------------
XBinaryPersistentMixin::XBinaryPersistentMixin(bool persistant) : XPersistentMixin(persistant)
{
}


//---------------------------------------------------------------
//
// XBinaryPersistentMixin::Flatten						[static]
//
//---------------------------------------------------------------
void XBinaryPersistentMixin::Flatten(XOutStream& stream, const XBinaryPersistentMixin* object)
{
	PRECONDITION(object != nil);
	
	stream.Flatten(object);
}


//---------------------------------------------------------------
//
// XBinaryPersistentMixin::Unflatten					[static]
//
//---------------------------------------------------------------
XBinaryPersistentMixin* XBinaryPersistentMixin::Unflatten(XInStream& stream, XCreateByNameMixin* parent)
{
	XBinaryPersistentMixin* object = stream.Unflatten(parent);

	return object;
}


//---------------------------------------------------------------
//
// XBinaryPersistentMixin::HandleStreamOut
//
//---------------------------------------------------------------
void XBinaryPersistentMixin::HandleStreamOut(XOutStream& stream) const
{	
	PRECONDITION(this->IsPersistant());

	this->OnStreamOut(stream);
}


//---------------------------------------------------------------
//
// XBinaryPersistentMixin::HandleStreamIn
//
//---------------------------------------------------------------
void XBinaryPersistentMixin::HandleStreamIn(XInStream& stream)
{
	PRECONDITION(this->IsPersistant());

	this->OnStreamIn(stream);
}


}	// namespace Whisper
