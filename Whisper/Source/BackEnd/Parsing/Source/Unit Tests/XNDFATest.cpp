/*
 *  File:		XNDFATest.cpp
 *  Summary:	XNDFA unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XNDFATest.cpp,v $
 *		Revision 1.2  2000/11/09 12:44:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <2>	 8/12/00	JDJ		Placed in the Parsing suite.
 *		 <1>	 4/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XNDFATest.h>

#include <XAutoPtr.h>
#include <XDFA.h>
#include <XNDFA.h>

namespace Whisper {
#if DEBUG


// ===================================================================================
//	class XNDFAUnitTest
// ===================================================================================	

//---------------------------------------------------------------
//
// XNDFAUnitTest::~XNDFAUnitTest
//
//---------------------------------------------------------------
XNDFAUnitTest::~XNDFAUnitTest()
{
}

	
//---------------------------------------------------------------
//
// XNDFAUnitTest::XNDFAUnitTest
//
//---------------------------------------------------------------
XNDFAUnitTest::XNDFAUnitTest() : XUnitTest(L"Parsing", L"CreateDFA")
{
}

						
//---------------------------------------------------------------
//
// XNDFAUnitTest::OnTest
//
//---------------------------------------------------------------
void XNDFAUnitTest::OnTest()
{
	XNDFA ndfa;

	ndfa.AddTransition('0', '1');
	ndfa.AddTransition('0', '7');

	ndfa.AddTransition('1', '2');
	ndfa.AddTransition('1', '4');

	ndfa.AddTransition('2', 'a', '3');

	ndfa.AddTransition('3', '6');

	ndfa.AddTransition('4', 'b', '5');

	ndfa.AddTransition('5', '6');

	ndfa.AddTransition('6', '1');
	ndfa.AddTransition('6', '7');

	ndfa.AddTransition('7', 'a', '8');

	ndfa.AddTransition('8', 'b', '9');

	ndfa.AddTransition('9', 'b', '10');

	ndfa.SetStartState('0');
	ndfa.AddHaltState('10');

	XAutoPtr<XDFA> dfa(ndfa.CreateDFA());

#if 0
	TRACE("\nShould be:\n");
	TRACE("(A, a) -> B\n");
	TRACE("(A, b) -> C\n");
	TRACE("(B, a) -> B\n");
	TRACE("(B, b) -> D\n");
	TRACE("(C, a) -> B\n");
	TRACE("(C, b) -> C\n");
	TRACE("(D, a) -> B\n");
	TRACE("(D, b) -> E\n");
	TRACE("(E, a) -> B\n");
	TRACE("(E, b) -> C\n");
	TRACE("Start state = A\n");
	TRACE("Halt states = {E}\n\n");
#endif

	int32 state = dfa->GetStartState();
	ASSERT(state == 'A');
	
	this->DoValidateTransitions(dfa.Get(), 'a', 'b');	// 'A' only has transitions on 'a' and 'b'
	ASSERT(!dfa->CanHalt());
	
	dfa->NextState('b');								// ('A', 'b') -> 'C'
	state = dfa->GetState();
	ASSERT(state == 'C');
	ASSERT(!dfa->CanHalt());

	dfa->Reset();
	dfa->NextState('a');								// ('A', 'a') -> 'B'
	state = dfa->GetState();
	ASSERT(state == 'B');
	ASSERT(!dfa->CanHalt());

	this->DoValidateTransitions(dfa.Get(), 'a', 'b');	// 'B' only has transitions on 'a' and 'b'
	
	dfa->NextState('a');								// ('B', 'a') -> 'B'
	state = dfa->GetState();
	ASSERT(state == 'B');
	
	dfa->NextState('b');								// ('B', 'b') -> 'D'
	state = dfa->GetState();
	ASSERT(state == 'D');
	ASSERT(!dfa->CanHalt());
	
	dfa->Reset();
	dfa->NextState('b');							
	dfa->NextState('b');								// ('C', 'b') -> 'C'
	state = dfa->GetState();
	ASSERT(state == 'C');

	this->DoValidateTransitions(dfa.Get(), 'a', 'b');	// 'C' only has transitions on 'a' and 'b'

	dfa->NextState('a');								// ('C', 'a') -> 'B'
	state = dfa->GetState();
	ASSERT(state == 'B');
		
	dfa->NextState('b');							
	this->DoValidateTransitions(dfa.Get(), 'a', 'b');	// 'D' only has transitions on 'a' and 'b'
	
	dfa->NextState('a');								// ('D', 'a') -> 'B'
	state = dfa->GetState();
	ASSERT(state == 'B');
	
	dfa->NextState('b');						
	dfa->NextState('b');								// ('D', 'b') -> 'E'
	state = dfa->GetState();
	ASSERT(state == 'E');
	ASSERT(dfa->CanHalt());

	this->DoValidateTransitions(dfa.Get(), 'a', 'b');	// 'E' only has transitions on 'a' and 'b'

	dfa->NextState('b');								// ('E', 'b') -> 'C'
	state = dfa->GetState();
	ASSERT(state == 'C');
	
	dfa->NextState('a');						
	dfa->NextState('b');							
	dfa->NextState('b');						
	dfa->NextState('a');								// ('E', 'a') -> 'B'			
	state = dfa->GetState();
	ASSERT(state == 'B');

	TRACE("Completed CreateDFA test.\n\n");
}


//---------------------------------------------------------------
//
// XNDFAUnitTest::DoValidateTransitions
//
//---------------------------------------------------------------
void XNDFAUnitTest::DoValidateTransitions(XDFA* dfa, int32 symbol1, int32 symbol2)
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
