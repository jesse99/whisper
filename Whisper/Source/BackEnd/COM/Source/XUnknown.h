/*
 *  File:       XUnknown.h
 *  Summary:    Base class for all interface classes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XUnknown.h,v $
 *		Revision 1.5  2001/04/17 01:45:18  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/03/05 05:44:22  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:31:25  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:58:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XAtomicCounter.h>
#include <XInvariant.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

#define GARBAGE_COLLECT_COM		0


//-----------------------------------
//	Forward References
//
class XBoss;


// ===================================================================================
//	class XUnknown
//!		Base class for all interface classes.
//		Note that the vtable and the member data of this class cannot be changed without 
//		breaking old plugins. 
// ===================================================================================
class COM_EXPORT XUnknown {

	friend class XImplementation;
	friend class ZBaseInterfacePtr;

//-----------------------------------
//	Initialization/Destruction
//
protected:
	virtual				~XUnknown() = 0;

public:
						XUnknown();

private:
						XUnknown(const XUnknown& rhs);
			
			XUnknown& 	operator=(const XUnknown& rhs);
			
//-----------------------------------
//	API
//
public:
			XBoss* 		GetBoss() const								{return mBoss;}

#if DEBUG && !GARBAGE_COLLECT_COM
			int32 		GetRefCount() const							{return mRefCount;}
#endif

protected:
	virtual void 		Invariant() const;

//-----------------------------------
//	Internal API
//
protected:
			void 		DoSetBoss(XBoss* boss)						{ASSERT(mBoss == nil); ASSERT(boss != nil); mBoss = boss;}

#if !GARBAGE_COLLECT_COM
	virtual void 		DoAddReference();	
	virtual void 		DoRemoveReference();
						// Tracking down mismatched adds and removes is often a serious
						// pain in the ass. To avoid these problems we, for now, require
						// people to use XInterfacePtr so that all the reference counting
						// is done automatically.
#endif

//-----------------------------------
//	Member Data
//
private:
	XBoss*			mBoss;
#if DEBUG && !GARBAGE_COLLECT_COM
	XAtomicCounter	mRefCount;	
	bool			mBreakOnRefs;		// if true DoAddReference and DoRemoveReference break into the debugger
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
