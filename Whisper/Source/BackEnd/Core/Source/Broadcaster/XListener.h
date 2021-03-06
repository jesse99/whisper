/*
 *  File:       XListener.h
 *  Summary:    Mixin class that listens to a broadcaster.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).
 *
 *  Change History (most recent first):
 *
 *		$Log: XListener.h,v $
 *		Revision 1.6  2001/03/05 05:42:18  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.5  2000/12/10 08:53:43  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.4  2000/11/09 12:41:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XCriticalSection.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XBaseListenerMixin
//!		Non-template base class for listener classes.
// ===================================================================================
class CORE_EXPORT XBaseListenerMixin {

	friend class XBaseBroadcasterMixin;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBaseListenerMixin();
		
						XBaseListenerMixin();
					
						XBaseListenerMixin(const XBaseListenerMixin& rhs);
						/**< Doesn't copy broadcasters. */
							
			XBaseListenerMixin& operator=(const XBaseListenerMixin& rhs);
						/**< Doesn't copy broadcasters. */
		
//-----------------------------------
//	API
//
public:					
			void 		EnableListening()			{mListenerEnabled = true;}
	
			void 		DisableListening()			{mListenerEnabled = false;}
	
	virtual bool 		IsListening() const;
						/**< Default returns true if listening is enabled. */

//-----------------------------------
//	Internal API
//
protected:
	virtual void		DoAddBroadcaster(XBaseBroadcasterMixin* broadcaster);
		
	virtual void 		DoRemoveBroadcaster(XBaseBroadcasterMixin* broadcaster);
		
			void 		DoRemoveAllBroadcasters();
	
//-----------------------------------
//	Member Data
//
private:
	std::list<XBaseBroadcasterMixin*>*	mBroadcasters;
	bool								mListenerEnabled;
	XCriticalSection					mListenerMutex;
};


// ===================================================================================
//	class XListenerMixin
//!		Template listener class.
// ===================================================================================
template <class MESSAGE> class XListenerMixin : public XBaseListenerMixin {

	typedef XBaseListenerMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XListenerMixin()			{}
		
						XListenerMixin()			{}
		
//-----------------------------------
//	New API
//
public:					
	virtual void 		OnBroadcast(const MESSAGE& mesg) = 0;
						/**< If MESSAGE is a pointer your override needs to look like this:
						OnBroadcast(XTextBoxControl* const& mesg). This is because the 
						pointer is const, not the object being pointed to. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

