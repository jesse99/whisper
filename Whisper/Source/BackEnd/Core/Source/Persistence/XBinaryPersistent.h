/*
 *  File:		XBinaryPersistent.h
 *  Summary:	Mixin for objects that can be flattened onto a binary stream.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBinaryPersistent.h,v $
 *		Revision 1.4  2001/03/05 05:40:31  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:50:24  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:26:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XPersistent.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;


// ===================================================================================
//	class XBinaryPersistentMixin
//!		Mixin for objects that can be flattened onto a binary stream.
// ===================================================================================
class CORE_EXPORT XBinaryPersistentMixin : public XPersistentMixin {
	
	typedef XPersistentMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBinaryPersistentMixin();

						XBinaryPersistentMixin(bool persistant = kPersistant);
						
//-----------------------------------
//	New API
//
public:
	static	void 		Flatten(XOutStream& stream, const XBinaryPersistentMixin* object);
						/**< Streams out the class name and then the object. Note that 
						this correctly handles circular object graphs. */
	
	static	XBinaryPersistentMixin* Unflatten(XInStream& stream, XCreateByNameMixin* parent);
						/**< Creates the object using the object factory and the saved
						class name and then calls HandleStreamIn. */

	virtual	void 		HandleStreamOut(XOutStream& stream) const;
	virtual void 		HandleStreamIn(XInStream& stream);

	friend 	XInStream& 	operator>>(XInStream& stream, XBinaryPersistentMixin& value)			{value.HandleStreamIn(stream); return stream;}
	friend	XOutStream& operator<<(XOutStream& stream, const XBinaryPersistentMixin& value)		{value.HandleStreamOut(stream); return stream;}

	friend 	XInStream& 	operator>>(XInStream& stream, XBinaryPersistentMixin* value)			{value->HandleStreamIn(stream); return stream;}
	friend	XOutStream& operator<<(XOutStream& stream, const XBinaryPersistentMixin* value)		{value->HandleStreamOut(stream); return stream;}

protected:
	virtual	void 		OnStreamOut(XOutStream& stream) const = 0;

	virtual void 		OnStreamIn(XInStream& stream) = 0;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

