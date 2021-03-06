/*
 *  File:		XPersistent.h
 *  Summary:	Base class for objects that can be flattened.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPersistent.h,v $
 *		Revision 1.4  2001/03/05 05:42:53  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:18  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:46:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XCreateByName.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XPersistentMixin
//!		Base class for objects that can be flattened.
// ===================================================================================
class CORE_EXPORT XPersistentMixin : public XCreateByNameMixin {

	typedef XCreateByNameMixin Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XPersistentMixin();

						XPersistentMixin(bool persistant = kPersistant);
						/**< The persistant flag can be set to false if you don't want
						to stream the object out (eg a draw extension used to hilite
						selections would only be streamed if the document saved the
						current selection). */
						
//-----------------------------------
//	New API
//
public:
	virtual bool 		IsPersistant() const;
	
	virtual void 		SetPersistent(bool enable = true)			{mPersistant = enable;}
	
//-----------------------------------
//	Member Data
//
protected:
	bool	mPersistant;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

