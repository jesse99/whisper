/*
 *  File:		XFSM.h
 *  Summary:	A finite state machine that calls a member function upon state changes.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFSM.h,v $
 *		Revision 1.3  2000/12/10 08:52:56  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:37:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XFSM.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:52:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 5/16/99	JDJ		Created 
 */

#pragma once

#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XFSM
//!		A finite state machine that calls a member function upon state changes.
// ===================================================================================
template <class OBJECT, class STATE, class STIMULUS>
class XFSM {

//-----------------------------------
//	Public Types
//
public:
	typedef void (OBJECT::*Callback)();

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XFSM();

						XFSM(OBJECT* object);
												
private:
						XFSM(const XFSM& rhs);
						
			XFSM& 		operator=(const XFSM& rhs);
						
//-----------------------------------
//	API
//
public:
	// ----- Operation -----
			STATE 		GetState() const							{return mState;}
			void 		SetState(STATE state)						{mState = state;}
						
			void 		NextState(STIMULUS stimulus);
						/**< Calls the callback associated with prevState/stimulus and then
						sets the state to nextState. Throws an exception if there isn't
						a valid transition. */
				
	// ----- Construction -----
			void 		AddTransition(STATE prevState, STIMULUS stimulus, STATE nextState, Callback callback);

			void 		RemoveTransition(STATE prevState, STIMULUS stimulus);
	
//-----------------------------------
//	Internal Types
//
protected:
	struct STransition {
		STATE	 prevState;
		STIMULUS stimulus;
		
					STransition(STATE p, STIMULUS s) : prevState(p), stimulus(s)	{}
		
			bool 	operator==(const STransition& rhs) const	{return prevState == rhs.prevState && stimulus == rhs.stimulus;}
			bool 	operator<(const STransition& rhs) const		{return prevState < rhs.prevState || (prevState == rhs.prevState && stimulus < rhs.stimulus);}
	};
	
	struct SState {
		STATE	 nextState;
		Callback callback;

					SState(STATE n, Callback c) : nextState(n), callback(c)	{}
	};

	typedef std::map<STransition, SState> TransitionTable;

//-----------------------------------
//	Member Data
//
protected:	
	OBJECT*			mObject;
	STATE			mState;
	TransitionTable	mTransitions;
};


// ===================================================================================
//	Outlined Methods
// ===================================================================================

template <class OBJECT, class STATE, class STIMULUS>
XFSM<OBJECT, STATE, STIMULUS>::~XFSM()
{
}


template <class OBJECT, class STATE, class STIMULUS>
XFSM<OBJECT, STATE, STIMULUS>::XFSM(OBJECT* object)
{
	ASSERT(object != nil);
	
	mObject = object;
}

						
template <class OBJECT, class STATE, class STIMULUS>
void XFSM<OBJECT, STATE, STIMULUS>::AddTransition(STATE prevState, STIMULUS stimulus, STATE nextState, Callback callback)
{
	ASSERT(callback != nil);
	
	TransitionTable::value_type value(STransition(prevState, stimulus), SState(nextState, callback));
	
	TransitionTable::pair_iterator_bool iter = mTransitions.insert(value);
	ASSERT(iter.second);
}


template <class OBJECT, class STATE, class STIMULUS>
void XFSM<OBJECT, STATE, STIMULUS>::RemoveTransition(STATE prevState, STIMULUS stimulus)
{	
	STransition transition(prevState, stimulus);
		
	TransitionTable::iterator iter = mTransitions.find(transition);
	ASSERT(iter != mTransitions.end());
	
	mTransitions.erase(iter);
}


template <class OBJECT, class STATE, class STIMULUS>
void XFSM<OBJECT, STATE, STIMULUS>::NextState(STIMULUS stimulus)
{
	STransition transition(mState, stimulus);
		
	TransitionTable::iterator iter = mTransitions.find(transition);
	if (iter == mTransitions.end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal error: invalid FSM stimulus")));
		
	Callback callback = iter->second.callback;
	(mObject->*callback)();
	
	mState = iter->second.nextState;
}


}	// namespace Whisper
