/*
 *  File:		XNDFA.h
 *  Summary:	Non-deterministic finite automaton.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNDFA.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/11 01:30:43  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:44:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <list>

#include <XFiniteAutomata.h>
#include <XSparseArray.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XDFA;


// ===================================================================================
//	class XNDFA
//!		Non-deterministic finite automaton.
/*!		This is non-deterministic because it allows multiple transitions from an 
 *		(oldState, symbol) pair and it allows transitions without symbols. Non-deterministic 
 *		automata aren't usually useful by themselves, but it can be useful to build a 
 *		deterministic automaton from a non-deterministic automaton. */
// ===================================================================================
class PARSE_EXPORT XNDFA : public XFiniteAutomata {
	
	typedef XFiniteAutomata Inherited;
	
//-----------------------------------
//	Types
//
public:	
	typedef XSparseArray<StateSet> 		TransitionTable;
	typedef std::map<int32, StateSet>	TransitionMap;
	typedef std::list<int32> 			StateList;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XNDFA();

						XNDFA();
						
//-----------------------------------
//	New API
//
public:
	// ----- Empty Transitions -----
			void 		NextState();
						/**< If the current state has an empty transition goto a new state. */

			void 		AddTransition(int32 oldState, int32 newState);
						/**< Add an empty transition. */
						
	// ----- Misc -----
			uint32 		GetNumStates() const										{return mStates.size();}

			int32 		GetLastState() const										{return mStates.max();}
			
			XDFA* 		CreateDFA(TransitionMap* haltMap = nil) const;
						/**< Converts a non-deterministic automata to a deterministic automata.
						Note that the only state numbers preserved are the halting states. */
						
//-----------------------------------
//	Inherited API
//
public:
			void 		NextState(int32 symbol)										{Inherited::NextState(symbol);}

			void 		AddTransition(int32 oldState, int32 symbol, int32 newState)	{Inherited::AddTransition(oldState, symbol, newState);}

	virtual bool 		HasTransition(int32 symbol) const;

protected:
	virtual void 		OnNextState(int32 symbol);

	virtual void 		OnAddTransition(int32 oldState, int32 symbol, int32 newState);
	
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Internal API
//
public:	
#if DEBUG
			void 		Trace() const;
#endif
	
protected:
			void 		DoGetEmptyTransitions(int32 state, StateSet& closure) const;

			void 		DoGetEmptyTransitions(const StateSet& states, StateSet& closure) const;

			void 		DoGetEmptyTransitions(StateList& nodes, StateSet& closure) const;

//-----------------------------------
//	Member Data
//
protected:	
	TransitionTable		mTransitions;			// maps (state, symbol) to zero or more new states
	TransitionMap		mEmptyTransitions;		// empty transitions from a state to zero or more states
	
	StateSet			mStates;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
