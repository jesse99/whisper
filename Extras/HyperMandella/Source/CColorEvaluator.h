/*
 *  File:		CColorEvaluator.h
 *  Summary:	A class designed to quickly evaluate a color expression tree.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CColorEvaluator.h,v $
 *		Revision 1.1  2000/12/28 02:48:25  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:53:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 2/15/98	JDJ		Created
 */

#include <stack>

#include <X3DPrimitives.h>
#include <XLexerGrammar.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class CColorEvaluator
// ===================================================================================
class CColorEvaluator {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~CColorEvaluator();

						CColorEvaluator(const std::wstring& formula);
						
private:
						CColorEvaluator(const CColorEvaluator& rhs);
						
			CColorEvaluator& operator=(const CColorEvaluator& rhs);
						
//-----------------------------------
//	API
//
public:
			double 		Evaluate(const X3DPoint& pt);
	
//-----------------------------------
//	Internal API
//
public:
			void 		PushToken(TokenNum token);

			void 		PushLiteral(double number);
	
//-----------------------------------
//	Member Data
//
protected:
	vector<TokenNum>				mTokens;
	vector<double>					mLiterals;
	stack<double, vector<double> >	mStack;
};



