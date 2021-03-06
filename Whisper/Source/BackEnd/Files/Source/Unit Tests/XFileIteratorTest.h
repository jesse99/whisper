/*
 *  File:		XFileIteratorTest.h
 *  Summary:	Unit test for XFileIterator.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileIteratorTest.h,v $
 *		Revision 1.2  2000/11/09 12:35:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/06/00	JDJ		Created (from Raven)
 */

#pragma once

#include <XFolderSpec.h>
#include <XUnitTest.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XFileIteratorTest
// ===================================================================================
class XFileIteratorTest : public XUnitTest {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFileIteratorTest();
	
						XFileIteratorTest();
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnTest();

//-----------------------------------
//	Internal API
//
protected:
			void 		DoEmptyFolder();
			void 		DoOneItem();
			void 		DoTwoItems();
			void 		DoNestedFolders();
			void 		DoFiltered();
			
private:
	XFolderSpec		mTestFolder;
};


}		// namespace Whisper
#endif	// DEBUG
