/*
 *  File:		XRegularExprTest.h
 *  Summary:	XRegularExpression unit test.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegularExprTest.h,v $
 *		Revision 1.2  2000/11/09 12:49:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/06/00	JDJ		Created
 */

#pragma once

#include <XUnitTest.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRegularExpression;
	

// ===================================================================================
//	class XRegExprUnitTest
// ===================================================================================	
class XRegExprUnitTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XRegExprUnitTest();
	
						XRegExprUnitTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();
	
//-----------------------------------
//	Internal API
//
protected:
			bool 		DoMatch(const std::wstring& expr, const std::wstring& str);

			bool 		DoMatch(const XRegularExpression& expr, const std::wstring& str);
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

