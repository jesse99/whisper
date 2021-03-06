/*
 *  File:		XDFA.h
 *  Summary:	Deterministic finite automaton.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDFA.h,v $
 *		Revision 1.4  2000/12/15 09:07:48  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/11 01:30:08  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:31:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 3/05/00	JDJ		Got rid of maxSymbol.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <list>

#include <XFiniteAutomata.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XDFA
//!		Deterministic finite automaton.
/*!		Finite automata allowing one one transition for a (state, symbol) pair. */
// ===================================================================================
class PARSE_EXPORT XDFA : public XFiniteAutomata {
	
	typedef XFiniteAutomata Inherited;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDFA();

						XDFA();
						
//-----------------------------------
//	New API
//
public:
			XDFA* 		CreateMinimalDFA() const;
						/**< Creates a new DFA with a minimal number of states. Note that
						this will renumber the state values. Note also that this is 
						rather slow. */

//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		HasTransition(int32 symbol) const;

protected:
	virtual void 		OnNextState(int32 symbol);

	virtual void 		OnAddTransition(int32 oldState, int32 symbol, int32 newState);

	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Internal Types
//
public:	
	typedef std::list<StateSet>	PartitionList;

//-----------------------------------
//	Internal API
//
public:	
#if DEBUG
			void 		Trace() const;
#endif

protected:			
			void 		DoInitialPartition(PartitionList& partitions) const;

			void 		DoPartition(const PartitionList& oldPartition, PartitionList& newPartition) const;
	
			int32 		DoGetRepresentitive(const PartitionList& partitions, int32 state) const;

			void 		DoGetReachableStates(StateSet& closure) const;

//-----------------------------------
//	Member Data
//
protected:	
	class ZTransitionTable*	mTransitions;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
