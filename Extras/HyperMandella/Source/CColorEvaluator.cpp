/*
 *  File:		CColorEvaluator.cpp
 *  Summary:	A class designed to quickly evaluate a color expression tree.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CColorEvaluator.cpp,v $
 *		Revision 1.2  2001/04/21 02:23:24  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/28 02:48:22  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CColorEvaluator.h"

#include <X3DPrimitives.h>
#include <XAutoPtr.h>
#include <XNumbers.h>

#include "CColorParser.h"


// ===================================================================================
//	class CColorEvaluator
// ===================================================================================

//---------------------------------------------------------------
//
// CColorEvaluator::~CColorEvaluator
//
//---------------------------------------------------------------
CColorEvaluator::~CColorEvaluator()
{
}


//---------------------------------------------------------------
//
// CColorEvaluator::CColorEvaluator
//
//---------------------------------------------------------------
CColorEvaluator::CColorEvaluator(const std::wstring& formula)
{		
	XAutoPtr<CColorTree> tree(CColorTree::Create(formula));
	
	tree->Compile(*this);
}


//---------------------------------------------------------------
//
// CColorEvaluator::Evaluate
//
//---------------------------------------------------------------
double CColorEvaluator::Evaluate(const X3DPoint& pt)
{
	PRECONDITION(mStack.empty());
	
	double lhs, rhs, result;
	
	uint32 count = mTokens.size();
	for (uint32 index = 0; index < count; ++index) {
		TokenNum token = mTokens[index];
		
		switch (token) {
			case kPowToken:
				rhs = mStack.top();
				mStack.pop();

				lhs = mStack.top();
				mStack.pop();
				
				result = pow(lhs, rhs);
				break;
				
			case kAddToken:
				rhs = mStack.top();
				mStack.pop();

				lhs = mStack.top();
				mStack.pop();
				
				result = lhs + rhs;
				break;

			case kMinusToken:
				rhs = mStack.top();
				mStack.pop();

				lhs = mStack.top();
				mStack.pop();
				
				result = lhs - rhs;
				break;

			case kProductToken:
				rhs = mStack.top();
				mStack.pop();

				lhs = mStack.top();
				mStack.pop();
				
				result = lhs*rhs;
				break;

			case kDivideToken:
				rhs = mStack.top();
				mStack.pop();

				lhs = mStack.top();
				mStack.pop();
				
				result = lhs/rhs;
				break;

			case kXVariableToken:
				result = pt.x;
				break;

			case kYVariableToken:
				result = pt.y;
				break;

			case kZVariableToken:
				result = pt.z;
				break;

			case kSinFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = sin(rhs);
				break;
				
			case kCosFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = cos(rhs);
				break;
				
			case kTanFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = tan(rhs);
				break;
				
			case kSqrFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = rhs*rhs;
				break;
				
			case kSqrtFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = sqrt(rhs);
				break;
				
			case kExpFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = exp(rhs);
				break;
				
			case kLnFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = log(rhs);
				break;
				
			case kLogFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = log10(rhs);
				break;
				
			case kRoundFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = round(rhs);
				break;
				
			case kTruncFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = trunc(rhs);
				break;
				
			case kAbsFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = Abs(rhs);
				break;
				
			case kRandomFunctionToken:
				rhs = mStack.top();
				mStack.pop();
				
				result = rhs*Random(1.0);
				break;
				
			default:
				if (token < 0)
					result = mLiterals[Abs(token) - 1UL];
				else
					DEBUGSTR("Bad token in CColorEvaluator::Evaluate:", token);
		}
		
		mStack.push(result);
	}
	
	result = mStack.top();
	mStack.pop();
	POSTCONDITION(mStack.empty());
	
	return result;
}

#pragma mark ~

//---------------------------------------------------------------
//
// CColorEvaluator::PushToken							
//
//---------------------------------------------------------------
void CColorEvaluator::PushToken(TokenNum token)
{
	mTokens.push_back(token);
}


//---------------------------------------------------------------
//
// CColorEvaluator::PushLiteral							
//
//---------------------------------------------------------------
void CColorEvaluator::PushLiteral(double number)
{
	mLiterals.push_back(number);
	mTokens.push_back(-((int32) mLiterals.size()));
}


