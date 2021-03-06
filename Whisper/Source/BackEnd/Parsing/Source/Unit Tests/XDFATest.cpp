/*
 *  File:		XDFATest.cpp
 *  Summary:	XDFA unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDFATest.cpp,v $
 *		Revision 1.2  2000/11/09 12:31:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 8/12/00	JDJ		Placed in the Parsing suite.
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XDFATest.h>

#include <XDFA.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XDFAUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XDFAUnitTest::~XDFAUnitTest
//
//---------------------------------------------------------------
XDFAUnitTest::~XDFAUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XDFAUnitTest::XDFAUnitTest
//
//---------------------------------------------------------------
XDFAUnitTest::XDFAUnitTest() : XUnitTest(L"Parsing", L"CreateMinimalDFA")
{
}

						
//---------------------------------------------------------------
//
// XDFAUnitTest::OnTest
//
//---------------------------------------------------------------
void XDFAUnitTest::OnTest()
{
	XDFA dfa;
	
	dfa.AddTransition('A', 'a', 'B');
	dfa.AddTransition('A', 'b', 'C');
	
	dfa.AddTransition('B', 'a', 'B');
	dfa.AddTransition('B', 'b', 'D');

	dfa.AddTransition('C', 'a', 'B');
	dfa.AddTransition('C', 'b', 'C');

	dfa.AddTransition('D', 'a', 'B');
	dfa.AddTransition('D', 'b', 'E');

	dfa.AddTransition('E', 'a', 'B');
	dfa.AddTransition('E', 'b', 'C');
	
	dfa.SetStartState('A');
	dfa.AddHaltState('E');
		
	XDFA* newDFA = dfa.CreateMinimalDFA();
	
#if 0
	TRACE("\nShould be:\n");
	TRACE("(A, a) -> B\n");
	TRACE("(A, b) -> A\n");
	TRACE("(B, a) -> B\n");
	TRACE("(B, b) -> D\n");
	TRACE("(D, a) -> B\n");
	TRACE("(D, b) -> E\n");
	TRACE("(E, a) -> B\n");
	TRACE("(E, b) -> A\n");
	TRACE("Start state = A\n");
	TRACE("Halt states = {E}\n\n");
#endif
	
	int32 state = newDFA->GetStartState();
	ASSERT(state == 'A');
	
	this->DoValidateTransitions(newDFA, 'a', 'b');	// 'A' only has transitions on 'a' and 'b'
	ASSERT(!newDFA->CanHalt());
	
	newDFA->NextState('b');							// ('A', 'b') -> 'A'
	state = newDFA->GetState();
	ASSERT(state == 'A');
	ASSERT(!newDFA->CanHalt());
	
	newDFA->NextState('a');							// ('A', 'a') -> 'B'
	state = newDFA->GetState();
	ASSERT(state == 'B');
	ASSERT(!newDFA->CanHalt());

	this->DoValidateTransitions(newDFA, 'a', 'b');	// 'B' only has transitions on 'a' and 'b'
	
	newDFA->NextState('a');							// ('B', 'a') -> 'B'
	state = newDFA->GetState();
	ASSERT(state == 'B');
	
	newDFA->NextState('b');							// ('B', 'b') -> 'D'
	state = newDFA->GetState();
	ASSERT(state == 'D');
	ASSERT(!newDFA->CanHalt());

	this->DoValidateTransitions(newDFA, 'a', 'b');	// 'D' only has transitions on 'a' and 'b'
	
	newDFA->NextState('a');							// ('D', 'a') -> 'B'
	state = newDFA->GetState();
	ASSERT(state == 'B');
	newDFA->NextState('b');		
	
	newDFA->NextState('b');							// ('D', 'b') -> 'E'
	state = newDFA->GetState();
	ASSERT(state == 'E');
	ASSERT(newDFA->CanHalt());

	this->DoValidateTransitions(newDFA, 'a', 'b');	// 'E' only has transitions on 'a' and 'b'
	
	newDFA->NextState('a');							// ('E', 'a') -> 'B'
	state = newDFA->GetState();
	ASSERT(state == 'B');
	newDFA->NextState('b');		
	newDFA->NextState('b');		
	
	newDFA->NextState('b');							// ('E', 'b') -> 'A'
	state = newDFA->GetState();
	ASSERT(state == 'A');
	
	delete newDFA;
	
	TRACE("Completed CreateMinimalDFA test.\n\n");
}


//---------------------------------------------------------------
//
// XDFAUnitTest::DoValidateTransitions
//
//---------------------------------------------------------------
void XDFAUnitTest::DoValidateTransitions(XDFA* dfa, int32 symbol1, int32 symbol2)
{
	ASSERT(dfa != nil);
	
	for (int32 symbol = 0; symbol < 256; ++symbol) {
		if (symbol == symbol1 || symbol == symbol2) 
			ASSERT(dfa->HasTransition(symbol));
		else
			ASSERT(!dfa->HasTransition(symbol));
	}
}


#endif	// DEBUG
}		// namespace Whisper
