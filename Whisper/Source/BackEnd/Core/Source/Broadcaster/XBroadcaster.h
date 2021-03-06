/*
 *  File:       XBroadcaster.h
 *  Summary:    Mixin class allowing an object to broadcast to one or more listeners.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *	
 *		$Log: XBroadcaster.h,v $
 *		Revision 1.5  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/12/10 08:50:28  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/09 12:26:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/11/99	JDJ		Tweaked the XBaseListenerMixin friend declaration so that it compiles with Pro 5fc3.
 *		 <3>	 5/28/99	JDJ		Made thread safe.
 *		 <2>	 1/02/99	JDJ		Made Broadcast virtual.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XListener.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XBaseBroadcasterMixin
//!		Non-template base class for broadcasters.
/*!		This class allows an object to broadcast messages to one or more listeners. This 
 *		is a good idea because it reduces coupling between classes: the  broadcaster 
 *		doesn't need to know anything about its listeners. The base classes use templates 
 *		so an arbitrary amount of information can be broadcast to the listeners. */
// ===================================================================================
class CORE_EXPORT XBaseBroadcasterMixin {

	friend class XBaseListenerMixin;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBaseBroadcasterMixin();
						/**< Removes itself from its listeners. */
		
						XBaseBroadcasterMixin();

						XBaseBroadcasterMixin(const XBaseBroadcasterMixin& rhs);
						/**< Doesn't copy listeners. */
		
			XBaseBroadcasterMixin& operator=(const XBaseBroadcasterMixin& rhs);
						/**< Doesn't copy listeners. */
		
//-----------------------------------
//	API
//
public:					
	// ----- Broadcasting -----
			void 		EnableBroadcasting()			{mBroadcasterEnabled = true;}

			void 		DisableBroadcasting()			{mBroadcasterEnabled = false;}

	virtual bool 		IsBroadcasting() const;
						/**< Default returns true if broadcasting is enabled. */
		
	// ----- Listeners -----
			void 		RemoveAllListeners();
			
			bool		HasListeners() const;

//-----------------------------------
//	Internal API
//
protected:		
			void 		DoAddListener(XBaseListenerMixin* listener);
		
			void 		DoRemoveListener(XBaseListenerMixin* listener);

	virtual void 		DoBroadcasting(const void* message) const;
			
	virtual void 		DoBroadcast(XBaseListenerMixin* listener, const void* message) const = 0;

//-----------------------------------
//	Member Data
//
private:	
	std::list<XBaseListenerMixin*>*	mListeners;
	bool							mBroadcasterEnabled;
	mutable XCriticalSection		mBroadcasterMutex;
};


// ===================================================================================
//	class XBroadcasterMixin
//!		Template broadcaster class.
// ===================================================================================
template <class MESSAGE> class XBroadcasterMixin : public XBaseBroadcasterMixin {

	typedef XBaseBroadcasterMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XBroadcasterMixin()								{}
						/**< Removes itself from its listeners. */
		
						XBroadcasterMixin()									{}
		
//-----------------------------------
//	New API
//
public:					
	// ----- Broadcasting -----
	virtual void 		Broadcast(const MESSAGE& message) const				{this->DoBroadcasting(&message);}
						/**< Listeners may query the object containing the broadcaster 
						so it must be in a valid state! */
		
	// ----- Listeners -----
			void 		AddListener(XListenerMixin<MESSAGE>* listener)		{this->DoAddListener(listener);}
		
			void 		RemoveListener(XListenerMixin<MESSAGE>* listener)	{this->DoRemoveListener(listener);}
						/**< Note that the XBaseListenerMixin calls this automatically. */

//-----------------------------------
//	Inherited API
//
protected:		
	virtual void 		DoBroadcast(XBaseListenerMixin* listener, const void* message) const;
};


// ===================================================================================
//	Outlined Methods
// ===================================================================================

// All of this casting is kind of gross, but it allows us to put
// all the list code in a non-template base class. This can really
// add up since broadcasters are often used with weird types.
template <class MESSAGE>
void XBroadcasterMixin<MESSAGE>::DoBroadcast(XBaseListenerMixin* theListener, const void* theMessage) const
{	
	XListenerMixin<MESSAGE>* listener = dynamic_cast<XListenerMixin<MESSAGE>*>(theListener);
	ASSERT(theListener != nil);
	ASSERT(listener != nil);
	
	const MESSAGE* message = reinterpret_cast<const MESSAGE*>(theMessage);
	ASSERT(message != nil);

	listener->OnBroadcast(*message);
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper




