/*
 *  File:		XNDFA.cpp
 *  Summary:	Non-deterministic finite automaton.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNDFA.cpp,v $
 *		Revision 1.5  2001/04/27 04:34:46  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/06 07:33:02  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2001/03/02 11:24:41  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.2  2000/11/09 12:44:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XNDFA.h>

#include <cctype>

#include <XDebug.h>
#include <XDFA.h>
#include <XExceptions.h>
#include <XNumbers.h>
#include <XStreaming.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// TraceInt
//
//---------------------------------------------------------------
#if DEBUG
static void TraceInt(int32 state)
{
	if (std::isprint(state))
		TRACE(state);
	else
		TRACE("\\", state);
}
#endif


//---------------------------------------------------------------
//
// Dump
//
//---------------------------------------------------------------
#if DEBUG
static void Dump(const XNDFA::StateSet& states)
{
	XNDFA::StateSet::const_iterator iter = states.begin();
	while (iter != states.end()) {
		int32 state = *iter++;
		
		TraceInt(state);
		if (iter != states.end())
			TRACE(", ");
	}
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XNDFA
// ===================================================================================

//---------------------------------------------------------------
//
// XNDFA::~XNDFA
//
//---------------------------------------------------------------
XNDFA::~XNDFA()
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XNDFA::XNDFA
//
//---------------------------------------------------------------
XNDFA::XNDFA()
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XNDFA::NextState
//
//---------------------------------------------------------------
void XNDFA::NextState()
{
	PRECONDITION(!mBlocked);
	CHECK_INVARIANT;
	
	TransitionMap::iterator transitions = mEmptyTransitions.find(mCurrentState);
	if (transitions != mEmptyTransitions.end()) {
	
		StateSet& states = (*transitions).second;
		if (!states.empty()) {				
			StateSet::iterator dstStates = states.begin();

			uint32 count = states.size();
			int32 index = Random((int32) count);
			ASSERT(count > 0);
			ASSERT(index >= 0 && index < count);
			
			while (index--)
				++dstStates;

			ASSERT(dstStates != states.end());
			if (*dstStates != mCurrentState) {
				mCurrentState = *dstStates;

				mCanHalt = mHaltStates.contains(mCurrentState);
			}

			if (mBlocked)
				mCanHalt = false;
		}
	}
}


//---------------------------------------------------------------
//
// XNDFA::AddTransition
//
//---------------------------------------------------------------
void XNDFA::AddTransition(int32 oldState, int32 newState)
{
	CHECK_INVARIANT;
	
	TransitionMap::iterator transitions = mEmptyTransitions.find(oldState);
	if (transitions != mEmptyTransitions.end())
		(*transitions).second.insert(newState);

	else {	
		TransitionMap::value_type value(oldState, newState);
		mEmptyTransitions.insert(value);
	}
	
	mStates += oldState;
	mStates += newState;
}


//---------------------------------------------------------------
//
// XNDFA::HasTransition
//
//---------------------------------------------------------------
bool XNDFA::HasTransition(int32 symbol) const
{
	bool has = false;
	
	if (!mBlocked) {
		TransitionTable::const_iterator transitions = mTransitions.Find(mCurrentState, symbol);
		if (transitions != mTransitions.end()) {
			const StateSet& states = *transitions;
			has = !states.empty();
		} 
	}
		
	return has;
}


//---------------------------------------------------------------
//
// XNDFA::CreateDFA
//
// Algorithm is from _Compilers: Principles, Techniques, and Tools_
// by Aho, Sethi, and Ullman (aka the Dragon book) pages 117-119.
//
//---------------------------------------------------------------
XDFA* XNDFA::CreateDFA(TransitionMap* haltMap) const
{
	CHECK_INVARIANT;
	
	XDFA* dfa = new XDFA;

	typedef std::list<StateSet> 		StateSetList;
	typedef std::map<StateSet, int32>	StateSetMap;
	typedef std::map<int32, StateSet>	IntMap;
	
	StateSetList unprocessed;			// list of states the NDFA can transition to from one state/symbol
	StateSetMap  dStates;				// mapping of the above states to a state in the DFA
	
	int32 startState = 'A';				// any value will do, but this looks nicer when dumping the DFA
	int32 newState = startState;				
	
	// The first state set we need to process are the states
	// reachable via empty transitions from the start state.
	StateSet states;
	this->DoGetEmptyTransitions(mStartState, states);
	unprocessed.push_back(states);
	
	// This state set will be represented in the DFA with a single 
	// state.
	dfa->SetStartState(newState);
	
	StateSet haltStates = mHaltStates & states;
	if (!haltStates.empty()) {
		dfa->AddHaltState(newState);
		
		if (haltMap != nil) {
			ASSERT(haltStates.size() == 1);
			int32 haltState = haltStates.min();
			haltMap->operator[](haltState) += newState;
		}
	}

	StateSetMap::value_type value(states, newState++);
	dStates.insert(value);
		
	while (!unprocessed.empty()) {
	
		// Pop off the next state set we need to process.
		StateSet t = unprocessed.back();
		unprocessed.pop_back();
		
		// Build a list containing the states reachable from states 
		// within t for every symbol.
		IntMap u;
		
		TransitionTable::const_iterator transitions = mTransitions.begin();
		while (transitions != mTransitions.end()) {			
			int32 state  = transitions.GetCol();
			int32 symbol = transitions.GetRow();
			
			if (t.count(state) > 0) {
				IntMap::iterator us = u.find(symbol);
				if (us != u.end())
					(*us).second += *transitions;
				else
					u.insert(IntMap::value_type(symbol, *transitions));
			}			
				
			++transitions;
		}
		
		// Loop through each state set reachable from t.
		IntMap::iterator us = u.begin();
		while (us != u.end()) {	
		
			// State set includes any states reachable via empty transitions
			// from states in the set.
			states.clear();
			this->DoGetEmptyTransitions((*us).second, states);
						
			// If this is a new state set add it to the list of sets to be
			// processed and give it a DFA state number.
			StateSetMap::iterator dstIter = dStates.find(states);
			if (dstIter == dStates.end()) {
				unprocessed.push_back(states);
				
				haltStates = mHaltStates & states;
				if (!haltStates.empty()) {
					dfa->AddHaltState(newState);
					
					if (haltMap != nil) {
						ASSERT(haltStates.size() == 1);
						int32 haltState = haltStates.min();
						haltMap->operator[](haltState) += newState;
					}
				}
				
				StateSetMap::value_type value2(states, newState++);
				dstIter = dStates.insert(value2).first;
				
				// There are ndfa's with n states that result in dfa's with 2^n
				// states, eg the ndfa resulting from the regular expression 
				// "(a|b)*a(a|b)(a|b)�(a|b)". To avoid consuming vast amounts
				// of time and memory we'll abort if we wind up with more than
				// n^2 states.
				uint32 numStates = mStates.size();
				if (newState - startState > numStates*numStates)
					throw std::domain_error("Can't construct the dfa (probable exponential growth).");
			}
			
			// Find the DFA state for t.
			StateSetMap::iterator srcIter = dStates.find(t);
			ASSERT(srcIter != dStates.end());

			int32 symbol = (*us).first;

			// Add the transition to the DFA.
			dfa->AddTransition((*srcIter).second, symbol, (*dstIter).second);
							
			++us;
		}	
	}

	return dfa;
}


//---------------------------------------------------------------
//
// XNDFA::Trace
//
//---------------------------------------------------------------
#if DEBUG
void XNDFA::Trace() const
{
	TransitionTable::const_iterator transitions = mTransitions.begin();
	while (transitions != mTransitions.end()) {
		int32 state  = transitions.GetCol();
		int32 symbol = transitions.GetRow();
		
		const StateSet& states = *transitions;
		
		TRACE("("); TraceInt(state); TRACE(", "); TraceInt(symbol); TRACE(")");
		TRACE(" -> ");
		
		StateSet::const_iterator iter = states.begin();
		while (iter != states.end()) {
			int32 dstState = *iter++;
			
			TraceInt(dstState);
			if (iter != states.end())
				TRACE(", ");
		}
		TRACE("\n");
		
		++transitions;
	}

	TransitionMap::const_iterator empties = mEmptyTransitions.begin();
	while (empties != mEmptyTransitions.end()) {
		int32 state  = (*empties).first;
		
		const StateSet& states = (*empties).second;

		const wchar_t kEmptySet = 0x2205;
		TRACE("("); TraceInt(state); TRACE(", ", kEmptySet, ")");
		TRACE(" -> ");
		
		StateSet::const_iterator iter = states.begin();
		while (iter != states.end()) {
			int32 dstState = *iter++;
			
			TraceInt(dstState);
			if (iter != states.end())
				TRACE(", ");
		}
		TRACE("\n");
		++empties;
	}
	
	TRACE("Start state = "); TraceInt(mStartState); TRACE("\n");

	TRACE("Halt states = {"); 
	StateSet::const_iterator iter = mHaltStates.begin();
	while (iter != mHaltStates.end()) {
		int32 state = *iter++;
		
		TraceInt(state);
		
		if (iter != mHaltStates.end())
			TRACE(", ");
	}
	TRACE("}\n");

	TRACE("\n");
}
#endif

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XNDFA::OnNextState
//
//---------------------------------------------------------------
void XNDFA::OnNextState(int32 symbol)
{
	TransitionTable::iterator transitions = mTransitions.Find(mCurrentState, symbol);
	if (transitions != mTransitions.end()) {
		
		const StateSet& states = *transitions;
		if (!states.empty()) {				
			StateSet::iterator dstStates = states.begin();

			uint32 count = states.size();
			int32 index = Random((int32) count);
			ASSERT(count > 0);
			ASSERT(index >= 0 && index < count);
			
			while (index--)
				++dstStates;

			ASSERT(dstStates != states.end());
			mCurrentState = *dstStates;
		
		} else	
			mBlocked = true;

	} else
		mBlocked = true;
}


//---------------------------------------------------------------
//
// XNDFA::OnAddTransition
//
//---------------------------------------------------------------
void XNDFA::OnAddTransition(int32 oldState, int32 symbol, int32 newState)
{
	TransitionTable::iterator transitions = mTransitions.Find(oldState, symbol);
	if (transitions != mTransitions.end()) {
		StateSet& states = *transitions;
		states.insert(newState);

	} else
		mTransitions.Force(oldState, symbol, newState);

	mStates += oldState;
	mStates += newState;
}


//---------------------------------------------------------------
//
// XNDFA::OnStreamOut
//
//---------------------------------------------------------------
void XNDFA::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);

	stream << mTransitions;
	stream << mEmptyTransitions;
	stream << mStates;
}


//---------------------------------------------------------------
//
// XNDFA::OnStreamIn
//
//---------------------------------------------------------------
void XNDFA::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	       
	stream >> mTransitions;
	stream >> mEmptyTransitions;
	stream >> mStates;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XNDFA::DoGetEmptyTransitions (int32, StateSet)
//
// Returns the set of states that are reachable from state via
// empty transitions.
//
//---------------------------------------------------------------
void XNDFA::DoGetEmptyTransitions(int32 state, StateSet& closure) const
{	
	StateList nodes;
	
	nodes.push_back(state);
	closure.insert(state);

	this->DoGetEmptyTransitions(nodes, closure);
}


//---------------------------------------------------------------
//
// XNDFA::DoGetEmptyTransitions (StateSet, StateSet)
//
//---------------------------------------------------------------
void XNDFA::DoGetEmptyTransitions(const StateSet& states, StateSet& closure) const
{
	StateList nodes;

	StateSet::const_iterator iter = states.begin();
	while (iter != states.end()) {
		int32 state = *iter++;
		
		nodes.push_back(state);
		closure.insert(state);
	}

	this->DoGetEmptyTransitions(nodes, closure);
}


//---------------------------------------------------------------
//
// XNDFA::DoGetEmptyTransitions (StateList, StateSet)
//
//---------------------------------------------------------------
void XNDFA::DoGetEmptyTransitions(StateList& nodes, StateSet& closure) const
{	
	while (!nodes.empty()) {
		int32 state = nodes.back();
		nodes.pop_back();
		
		TransitionMap::const_iterator transitions = mEmptyTransitions.begin();
		while (transitions != mEmptyTransitions.end()) {
			if ((*transitions).first == state) {
				const StateSet& states = (*transitions).second;
				StateSet::const_iterator dstStates = states.begin();
				while (dstStates != states.end()) {
					int32 dstState = *dstStates++;
					
					if (!closure.contains(dstState)) {
						closure.insert(dstState);
						nodes.push_back(dstState);
					}
				}
			}
			++transitions;
		}
	}
}
	

}		// namespace Whisper
