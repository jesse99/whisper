/*
 *  File:		XFiniteAutomata.h
 *  Summary:	Abstract base class for objects that bounce from state to state.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFiniteAutomata.h,v $
 *		Revision 1.4  2001/04/17 01:43:44  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2000/12/11 01:30:13  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:36:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XBinaryPersistent.h>
#include <XInvariant.h>
#include <XSet.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XFiniteAutomata
//!		Abstract base class for objects that bounce from state to state.
// ===================================================================================
class PARSE_EXPORT XFiniteAutomata : public XBinaryPersistentMixin {

	typedef XBinaryPersistentMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XFiniteAutomata();

						XFiniteAutomata();

private:
						XFiniteAutomata(const XFiniteAutomata& rhs);
						
			XFiniteAutomata& operator=(const XFiniteAutomata& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Operation
	//@{
	virtual void 		Reset();
						/**< Set the current state to the start state. */
						
			void 		NextState(int32 symbol);
						/**< Goto a new state using the current state, the symbol, and
						a transition rule. If a transition rule cannot be found the
						automata blocks. */
	//@}
			
	//! @name Inquiry
	//@{
			int32 		GetStartState() const 										{return mStartState;}
			
			int32 		GetState() const											{return mCurrentState;}
						/**< Returns the current state. */
						
	virtual bool 		HasTransition(int32 symbol) const = 0;
						/**< Returns true if symbol is a valid transition out of the
						current state. */

			bool 		IsBlocked() const											{return mBlocked;}
						/**< Returns true if the automata has fallen into an invalid state.
						If the automata is blocked NextState cannot be used. */
			
			bool 		CanHalt() const												{return mCanHalt;}
						/**< Returns true if the automata is in one of the halt states. */
	//@}

	//! @name Construction
	//@{
			void 		SetStartState(int32 state)									{mStartState = state; this->Reset();}
			
			void 		AddHaltState(int32 state)									{mHaltStates.insert(state);}

			void 		AddTransition(int32 oldState, int32 symbol, int32 newState)	{this->OnAddTransition(oldState, symbol, newState);}
	//@}
	
protected:
	virtual void 		OnNextState(int32 symbol) = 0;
						/**< Sets mBlocked if there isn't a legit transition. */

	virtual void 		OnAddTransition(int32 oldState, int32 symbol, int32 newState) = 0;

//-----------------------------------
//	Inherited API
//
protected:
			void 		Invariant() const;

	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Internal Types
//
public:
	typedef XSet<int32> StateSet;
	
//-----------------------------------
//	Member Data
//
protected:	
	int32		mStartState;
	int32		mCurrentState;
	StateSet	mHaltStates;
	
	bool		mBlocked;
	bool		mCanHalt;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
