/*
 *  File:		XScanners.h
 *  Summary:	XScanner subclasses.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScanners.h,v $
 *		Revision 1.3  2000/12/11 01:31:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:50:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XScanners.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/11 01:31:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XScanner.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XCScanner
//!		A scanner that skips over C/C++ comments.
// ===================================================================================
class PARSE_EXPORT XCScanner : public XScanner {

	typedef XScanner Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XCScanner();
	
						XCScanner();

						XCScanner(const std::wstring& text);

//-----------------------------------
//	Inherited API
//
public:
	virtual	void 		SkipWhiteSpace();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoSkipCComment();

			void 		DoSkipCPlusComment();
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
