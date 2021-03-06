/*
 *  File:		XFiniteAutomata.cpp
 *  Summary:	Abstract base class for objects that bounce from state to state.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFiniteAutomata.cpp,v $
 *		Revision 1.4  2001/04/27 04:32:42  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:43:40  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:36:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XFiniteAutomata.h>

#include <XDebug.h>
#include <XStreaming.h>

namespace Whisper {


// ===================================================================================
//	class XFiniteAutomata
// ===================================================================================

//---------------------------------------------------------------
//
// XFiniteAutomata::~XFiniteAutomata
//
//---------------------------------------------------------------
XFiniteAutomata::~XFiniteAutomata()
{
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XFiniteAutomata::XFiniteAutomata
//
//---------------------------------------------------------------
XFiniteAutomata::XFiniteAutomata()
{	
	mStartState = 1;
	mCurrentState = 1;
	
	mBlocked = true;
	mCanHalt = false;

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XFiniteAutomata::Reset
//
//---------------------------------------------------------------
void XFiniteAutomata::Reset()
{
	CHECK_INVARIANT;
	
	mCurrentState = mStartState;

	mBlocked = false;
	mCanHalt = mHaltStates.contains(mCurrentState);
}


//---------------------------------------------------------------
//
// XFiniteAutomata::NextState
//
//---------------------------------------------------------------
void XFiniteAutomata::NextState(int32 symbol)
{
	PRECONDITION(!mBlocked);
	PRECONDITION(symbol >= 0);
	CHECK_INVARIANT;
	
	int32 oldState = mCurrentState;
	
	this->OnNextState(symbol);

	if (mCurrentState != oldState)
		mCanHalt = mHaltStates.contains(mCurrentState);
		
	if (mBlocked)
		mCanHalt = false;

	POSTCONDITION(true);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XFiniteAutomata::Invariant
//
//---------------------------------------------------------------
void XFiniteAutomata::Invariant() const
{
}


//---------------------------------------------------------------
//
// XFiniteAutomata::OnStreamOut
//
//---------------------------------------------------------------
void XFiniteAutomata::OnStreamOut(XOutStream& stream) const
{
	int32 dummy = 0;
	
	stream << mStartState
	       << mCurrentState
	       << mHaltStates 
	       << dummy
	       << mBlocked
	       << mCanHalt;
}


//---------------------------------------------------------------
//
// XFiniteAutomata::OnStreamIn
//
//---------------------------------------------------------------
void XFiniteAutomata::OnStreamIn(XInStream& stream)
{	
	int32 dummy = 0;

	stream >> mStartState
	       >> mCurrentState
	       >> mHaltStates 
	       >> dummy
	       >> mBlocked
	       >> mCanHalt;
}


}	// namespace Whisper
