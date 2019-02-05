/*
 *  File:		XDFA.cpp
 *  Summary:	Deterministic finite automaton.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	ZTransitionTable
 *
 *		$Log: XDFA.cpp,v $
 *		Revision 1.5  2001/04/27 04:32:03  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/06 07:32:22  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:31:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 3/07/00	JDJ		Rewrote ZTransitionTable: got rid of maxSymbol, made the
 *									code a lot simpler, and improved the efficiency of the most
 *									commonly used methods.
 *		 <3>	 6/11/99	JDJ		Tweaked friend declaration for CW Pro 5fc3.
 *		 <2>	 6/01/99	JDJ		Tweaked and commented out overloaded ZTransitionTable::iterator 
 *									comparison operators.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XDFA.h>

#include <cctype>
#include <map>
#include <vector>

#include <XAutoPtr.h>
#include <XDebug.h>
#include <XNumbers.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <XTranscode.h>

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

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZTransitionTable
// ===================================================================================
class ZTransitionTable {
	
//-----------------------------------
//	Types
//
public:
	typedef std::map<int32, int32> 		 Transitions;	// maps symbols to newStates
	typedef std::map<int32, Transitions> States;		// maps oldStates to transition list
	
	typedef States::iterator 			 iterator;
	typedef States::const_iterator 		 const_iterator;
		
//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~ZTransitionTable();

						ZTransitionTable();
						
private:
						ZTransitionTable(const ZTransitionTable& rhs);
						
			ZTransitionTable& operator=(const ZTransitionTable& rhs);
						
//-----------------------------------
//	API
//
public:
	// ----- Common Operations -----
			bool 		HasTransition(int32 oldState, int32 symbol) const;

			int32 		GetTransition(int32 oldState, int32 symbol) const;
						// Returns LONG_MIN if there isn't a transition (it's important
						// that this be fast so we allow clients to call this even if
						// HasTransition returns false).

			void 		AddTransition(int32 oldState, int32 symbol, int32 newState);
						
			void 		Clear();
			
	// ----- Advanced Operations -----
			const_iterator 	begin() const	{if (mStates == nil) this->DoBuildStates(); return mStates->begin();}	
			const_iterator 	end() const		{if (mStates == nil) this->DoBuildStates(); return mStates->end();}

			iterator 	begin()				{if (mStates == nil) this->DoBuildStates(); return mStates->begin();}	
			iterator 	end()				{if (mStates == nil) this->DoBuildStates(); return mStates->end();}

			const_iterator Find(int32 oldState) const;
			iterator 	Find(int32 oldState);
			
			void 		Erase(const iterator& iter);
			
	// ----- Streaming -----
	friend 	XInStream& 	operator>>(XInStream& stream, ZTransitionTable& c);
	friend	XOutStream& operator<<(XOutStream& stream, const ZTransitionTable& c);
	
//-----------------------------------
//	Internal API
//
protected:
			void 		DoBuildStates() const;
			
//-----------------------------------
//	Internal Types
//
protected:
	struct STransition {
		int32 			oldState;
		int32 			symbol;
		mutable int32 	newState;
		
			STransition()										{}
			STransition(int32 os, int32 sy, int32 ns = -1)		{oldState = os; symbol = sy; newState = ns;}
		
			bool 	operator==(const STransition& rhs) const	{return oldState == rhs.oldState && symbol == rhs.symbol;}
			bool 	operator<(const STransition& rhs) const		{return oldState < rhs.oldState || (oldState == rhs.oldState && symbol < rhs.symbol);}

			friend 	XInStream& 	operator>>(XInStream& stream, STransition& c);
			friend	XOutStream& operator<<(XOutStream& stream, const STransition& c);
	};
		
//-----------------------------------
//	Member Data
//
private:
	mutable std::vector<STransition>	mElements;		// use a vector instead of a set since we'll rarely be changing the table after it's built
	mutable States*						mStates;	
		
	mutable bool						mDirty;		
};


//---------------------------------------------------------------
//
// operator>> (STransition)
//
//---------------------------------------------------------------
XInStream& 	operator>>(XInStream& stream, ZTransitionTable::STransition& c)
{
	stream >> c.oldState >> c.symbol >> c.newState;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (STransition)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const ZTransitionTable::STransition& c)
{
	stream << c.oldState << c.symbol << c.newState;
	
	return stream;
}


//---------------------------------------------------------------
//
// ZTransitionTable::~ZTransitionTable
//
//---------------------------------------------------------------
ZTransitionTable::~ZTransitionTable()
{
	delete mStates;
}


//---------------------------------------------------------------
//
// ZTransitionTable::ZTransitionTable
//
//---------------------------------------------------------------
ZTransitionTable::ZTransitionTable()
{
	mStates = nil;
	mDirty = false;
}


//---------------------------------------------------------------
//
// ZTransitionTable::HasTransition
//
//---------------------------------------------------------------
bool ZTransitionTable::HasTransition(int32 oldState, int32 symbol) const
{	
	if (mDirty) {
		std::sort(mElements.begin(), mElements.end());
		mDirty = false;
	}
	
	bool has = std::binary_search(mElements.begin(), mElements.end(), STransition(oldState, symbol));
			
	return has;
}


//---------------------------------------------------------------
//
// ZTransitionTable::GetTransition
//
//---------------------------------------------------------------
int32 ZTransitionTable::GetTransition(int32 oldState, int32 symbol) const
{
	int32 newState = LONG_MIN;
	
	if (mDirty) {
		std::sort(mElements.begin(), mElements.end());
		mDirty = false;
	}
	
	std::vector<STransition>::const_iterator iter = std::lower_bound(mElements.begin(), mElements.end(), STransition(oldState, symbol));
	if (iter != mElements.end() && iter->oldState == oldState && iter->symbol == symbol)
		newState = iter->newState;
		
	return newState;
}


//---------------------------------------------------------------
//
// ZTransitionTable::AddTransition
//
//---------------------------------------------------------------
void ZTransitionTable::AddTransition(int32 oldState, int32 symbol, int32 newState)
{
	PRECONDITION(newState != LONG_MIN);
#if DEBUG
	std::vector<STransition>::const_iterator iter = std::find(mElements.begin(), mElements.end(), STransition(oldState, symbol));
	if (iter != mElements.end())
		PRECONDITION(iter->newState == newState);			// deterministic automata can only have one transition out of a node
#endif
	
	mElements.push_back(STransition(oldState, symbol, newState));	// $$ may add the same transition multiple times
	mDirty = true;

	delete mStates;
	mStates = nil;
}


//---------------------------------------------------------------
//
// ZTransitionTable::Clear
//
//---------------------------------------------------------------
void ZTransitionTable::Clear()											
{
	mElements.clear();
	mDirty = false;

	delete mStates;
	mStates = nil;
}


//---------------------------------------------------------------
//
// ZTransitionTable::Find const
//
//---------------------------------------------------------------
ZTransitionTable::const_iterator ZTransitionTable::Find(int32 oldState) const
{
	if (mStates == nil)
		this->DoBuildStates();
		
	std::map<int32, Transitions>::const_iterator iter = mStates->find(oldState);
	
	return iter;
}


//---------------------------------------------------------------
//
// ZTransitionTable::Find
//
//---------------------------------------------------------------
ZTransitionTable::iterator ZTransitionTable::Find(int32 oldState)
{
	if (mStates == nil)
		this->DoBuildStates();
		
	std::map<int32, Transitions>::iterator iter = mStates->find(oldState);
	
	return iter;
}


//---------------------------------------------------------------
//
// ZTransitionTable::Erase
//
//---------------------------------------------------------------
void ZTransitionTable::Erase(const iterator& iter)
{
	PRECONDITION(mStates != nil);
	
	mStates->erase(iter);
}
			

//---------------------------------------------------------------
//
// ZTransitionTable::DoBuildStates
//
// mStates is only used by CreateMinimalDFA, so we don't build
// it until we need to.
//
//---------------------------------------------------------------
void ZTransitionTable::DoBuildStates() const
{
	PRECONDITION(mStates == nil);
	
	mStates = new States;

	std::vector<STransition>::const_iterator iter = mElements.begin();
	while (iter != mElements.end()) {
		const STransition& transition = *iter;
		++iter;
		
		std::map<int32, Transitions>::iterator iter2 = mStates->find(transition.oldState);
		if (iter2 == mStates->end()) {
			std::map<int32, Transitions>::value_type value(transition.oldState, Transitions());
			iter2 = mStates->insert(value).first;
		}

		std::map<int32, int32>::value_type value2(transition.symbol, transition.newState);
		iter2->second.insert(value2);
	}
}


//---------------------------------------------------------------
//
// operator<< (ZTransitionTable)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const ZTransitionTable& c)	
{
	stream << 1L << c.mElements << c.mDirty;
		
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (ZTransitionTable)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, ZTransitionTable& c)			
{
	int32 version;
	stream >> version;
	
	if (version != 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Can't stream in the ZTransitionTable (bad version number).")));

	stream >> c.mElements >> c.mDirty;
	
	delete c.mStates;
	c.mStates = nil;
	
	return stream;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct SubGroup
// ===================================================================================
struct SubGroup {
	XDFA::StateSet			group;		// for every state in group,
	std::map<int32, int32>	mask;		// mask maps a symbol to a group number
		
				SubGroup()									{}
				
				SubGroup(const ZTransitionTable& table, const XDFA::PartitionList& partitions, int32 state);
				
		bool 	operator==(const SubGroup& rhs) const		{return mask == rhs.mask;}
		bool 	operator!=(const SubGroup& rhs) const		{return mask != rhs.mask;}
};
		

//---------------------------------------------------------------
//
// SubGroup::SubGroup
//
//---------------------------------------------------------------
SubGroup::SubGroup(const ZTransitionTable& table, const XDFA::PartitionList& partitions, int32 oldState) : group(oldState)
{		
	ZTransitionTable::const_iterator iter = table.Find(oldState);
	if (iter != table.end()) {
		const std::map<int32, int32>& transitions = iter->second;
		
		std::map<int32, int32>::const_iterator iter2 = transitions.begin();
		while (iter2 != transitions.end()) {
			int32 symbol   = iter2->first;
			int32 newState = iter2->second;
			++iter2;

			int32 groupNum;
			bool found = false;

			XDFA::PartitionList::const_iterator iter = partitions.begin();
			while (iter != partitions.end() && !found) {
				const XDFA::StateSet& states = *iter++;
				
				if (states.contains(newState)) {
					groupNum = states.min();				// this is OK since we don't muck with partitions while we're creating subgroups
					found = true;
				}
			}

			ASSERT(found);
			
			mask[symbol] = groupNum;
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XDFA
// ===================================================================================

//---------------------------------------------------------------
//
// XDFA::~XDFA
//
//---------------------------------------------------------------
XDFA::~XDFA()
{
	CALL_INVARIANT;

	delete mTransitions;
}


//---------------------------------------------------------------
//
// XDFA::XDFA
//
//---------------------------------------------------------------
XDFA::XDFA() 
{
	mTransitions = new ZTransitionTable;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XDFA::HasTransition
//
//---------------------------------------------------------------
bool XDFA::HasTransition(int32 symbol) const
{
	bool has = false;
	
	if (!mBlocked)
		has = mTransitions->HasTransition(mCurrentState, symbol);
		
	return has;
}


//---------------------------------------------------------------
//
// XDFA::CreateMinimalDFA
// 
// Algorithm is from _Compilers: Principles, Techniques, and Tools_
// by Aho, Sethi, and Ullman (aka the Dragon book) pages 141-143.
//
//---------------------------------------------------------------
XDFA* XDFA::CreateMinimalDFA() const
{
	CHECK_INVARIANT;
	
	ZTransitionTable::iterator transitions;

	XAutoPtr<XDFA> newDFA(new XDFA);
	int32 state, symbol, newState;

	// Divide the states into two groups: those that are halt
	// states and those that are not.
	PartitionList partitions;
	this->DoInitialPartition(partitions);

	// Continue partitioning until the states in each group transition
	// to the same group when given the same input.
	PartitionList newPartition;
	bool partitioned = true;
	
	while (partitioned) {
		this->DoPartition(partitions, newPartition);

		partitioned = newPartition != partitions;
		if (partitioned)
			partitions = newPartition;
	}
	
	StateSet newStates;

	// Loop through each group.
	PartitionList::const_iterator iter = partitions.begin();
	while (iter != partitions.end()) {
		const StateSet& group = *iter;
		++iter;
		
		int32 oldState = group.min();

		// All states in the group will be represented in the new DFA
		// by oldState.
		transitions = mTransitions->Find(oldState);
		if (transitions != mTransitions->end()) {
			const std::map<int32, int32>& trans = transitions->second;

			bool added = false;
			std::map<int32, int32>::const_iterator iter2 = trans.begin();
			while (iter2 != trans.end()) {
				symbol   = iter2->first;
				newState = iter2->second;
				++iter2;
			
				int32 represententive = this->DoGetRepresentitive(partitions, newState);
				
				newDFA->AddTransition(oldState, symbol, represententive);
				added = true;
			}
			
			if (added)
				newStates += oldState;
		}
				
		if (group.contains(mStartState))
			newDFA->SetStartState(oldState);
			
		if (intersects(group, mHaltStates))
			newDFA->AddHaltState(oldState);
	}
	
	StateSet deadStates;

	// Mark for deletion any states that aren't halt states and 
	// don't transition to a different state.
	transitions = newDFA->mTransitions->begin();
	while (transitions != newDFA->mTransitions->end()) {
		state = transitions->first;
		if (newStates.count(state) > 0 && !newDFA->mHaltStates.contains(state)) {
			bool dead = true;
			
			const std::map<int32, int32>& trans = transitions->second;
			std::map<int32, int32>::const_iterator iter3 = trans.begin();
			while (iter3 != trans.end() && dead) {
				newState = iter3->second;
				++iter3;
				
				dead = newState == state;
			}
				
			if (dead) {
				deadStates += state;
				newStates  -= state;
			}
		}
		++transitions;
	}
	
	// Add the states that aren't reachable from the start state to
	// deadStates.
	StateSet reachableStates;
	newDFA->DoGetReachableStates(reachableStates);
	
	deadStates += newStates - reachableStates;

	// Delete transitions from or to a dead state.
	if (!deadStates.empty()) {
		transitions = newDFA->mTransitions->begin();
		while (transitions != newDFA->mTransitions->end()) {
			ZTransitionTable::iterator temp = transitions;
			++transitions;
			
			if (deadStates.contains(temp->first))
				newDFA->mTransitions->Erase(temp);
			else {
				std::map<int32, int32>& trans = temp->second;
				std::map<int32, int32>::iterator iter4 = trans.begin();
				while (iter4 != trans.end()) {
					symbol = iter4->first;
					newState = iter4->second;
					
					if (deadStates.contains(newState))
						trans.erase(iter4);

					++iter4;
				}
			}
		}
	}
				
	return newDFA.Release();
}


//---------------------------------------------------------------
//
// XDFA::Trace
//
//---------------------------------------------------------------
#if DEBUG
void XDFA::Trace() const
{
	ZTransitionTable::const_iterator transitions = mTransitions->begin();
	while (transitions != mTransitions->end()) {
		int32 oldState = transitions->first;			
	
		std::map<int32, int32>::const_iterator iter2 = transitions->second.begin();
		while (iter2 != transitions->second.end()) {
			int32 symbol   = iter2->first;
			int32 newState = iter2->second;
			++iter2;

			TRACE("("); TraceInt(oldState); TRACE(", "); TraceInt(symbol); TRACE(")");
			TRACE(" -> ");
			TraceInt(newState);

			TRACE("\n");
		}

		++transitions;
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
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XDFA::OnNextState
//
//---------------------------------------------------------------
void XDFA::OnNextState(int32 symbol)
{
	mCurrentState = mTransitions->GetTransition(mCurrentState,symbol);
	mBlocked = mCurrentState == LONG_MIN;
}


//---------------------------------------------------------------
//
// XDFA::OnAddTransition
//
//---------------------------------------------------------------
void XDFA::OnAddTransition(int32 oldState, int32 symbol, int32 newState)
{	
	mTransitions->AddTransition(oldState, symbol, newState);
}


//---------------------------------------------------------------
//
// XDFA::OnStreamOut
//
//---------------------------------------------------------------
void XDFA::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);
	       
	stream << *mTransitions;
}


//---------------------------------------------------------------
//
// XDFA::OnStreamIn
//
//---------------------------------------------------------------
void XDFA::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	       
	stream >> *mTransitions;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XDFA::DoInitialPartition
//
// Performs the initial partition: halting states in one group
// and all other states in a second group.
//
//---------------------------------------------------------------
void XDFA::DoInitialPartition(PartitionList& partitions) const
{
	PRECONDITION(partitions.empty());
	
	StateSet halting;
	StateSet notHalting;

	ZTransitionTable::const_iterator iter = mTransitions->begin();
	while (iter != mTransitions->end()) {
		int32 oldState = iter->first;
		if (mHaltStates.contains(oldState))
			halting += oldState;
		else	
			notHalting += oldState;

		std::map<int32, int32>::const_iterator iter2 = iter->second.begin();
		while (iter2 != iter->second.end()) {			// need to check dstStates since some of these may not be source states
			int32 newState = iter2->second;
			++iter2;

			if (mHaltStates.contains(newState))
				halting += newState;
			else	
				notHalting += newState;
		}
						
		++iter;
	}
	
	if (!halting.empty())
		partitions.push_back(halting);
		
	if (!notHalting.empty())
		partitions.push_back(notHalting);
}


//---------------------------------------------------------------
//
// XDFA::DoPartition
//
//---------------------------------------------------------------
void XDFA::DoPartition(const PartitionList& oldPartition, PartitionList& newPartition) const
{
	typedef std::list<SubGroup> SubGroupList;
	
	newPartition.clear();
	
	PartitionList::const_iterator partitions = oldPartition.begin();
	while (partitions != oldPartition.end()) {
		const StateSet& group = *partitions++;
		
		if (group.size() > 1) {
			SubGroupList subGroups;
			
			StateSet::const_iterator states = group.begin();
			int32 state = *states;
			++states;
			
			SubGroup subGroup(*mTransitions, oldPartition, state);
			subGroups.push_back(subGroup);
			
			while (states != group.end()) {
				state = *states++;
				
				subGroup = SubGroup(*mTransitions, oldPartition, state);
				SubGroupList::iterator iter = std::find(subGroups.begin(), subGroups.end(), subGroup);
				if (iter != subGroups.end())
					iter->group += state;
				else
					subGroups.push_back(subGroup);
			}
			
			SubGroupList::const_iterator iter = subGroups.begin();
			while (iter != subGroups.end()) {
				const SubGroup& subGroup = *iter;
				++iter;
				
				newPartition.push_back(subGroup.group);
			}

		} else {
			ASSERT(group.size() == 1);
			
			newPartition.push_back(group);
		}
	}	
}


//---------------------------------------------------------------
//
// XDFA::DoGetRepresentitive
//
// Returns the state that the input state will map to in the
// minimal DFA.
//
//---------------------------------------------------------------
int32 XDFA::DoGetRepresentitive(const PartitionList& partitions, int32 state) const
{
	int32 representitive = 0;
	bool found = false;
	
	PartitionList::const_iterator iter = partitions.begin();
	while (iter != partitions.end() && !found) {
		const StateSet& group = *iter;
		++iter;
		
		if (group.contains(state)) {
			representitive = group.min();
			found = true;
		}
	}
	
	POSTCONDITION(found);
	return representitive;
}


//---------------------------------------------------------------
//
// XDFA::DoGetReachableStates 
//
// Returns all of the states reachable from mStartState.
//
//---------------------------------------------------------------
void XDFA::DoGetReachableStates(StateSet& closure) const
{
	std::list<int32> nodes;
	
	nodes.push_back(mStartState);
	closure.insert(mStartState);

	while (!nodes.empty()) {
		int32 state = nodes.back();
		nodes.pop_back();
		
		ZTransitionTable::const_iterator transitions = mTransitions->begin();
		while (transitions != mTransitions->end()) {
			int32 oldState = transitions->first;			
		
			if (oldState == state) {				
				std::map<int32, int32>::const_iterator iter2 = transitions->second.begin();
				while (iter2 != transitions->second.end()) {
					int32 newState = iter2->second;
					++iter2;
		
					if (!closure.contains(newState)) {
						closure.insert(newState);
						nodes.push_back(newState);
					}
				}
			}
				
			++transitions;
		}
	}
}


}		// namespace Whisper
