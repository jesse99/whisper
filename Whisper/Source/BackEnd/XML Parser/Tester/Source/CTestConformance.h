/*
 *  File:		CTestConformance.h
 *  Summary:	Run the oasis XML conformance tests.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CTestConformance.h,v $
 *		Revision 1.2  2000/11/09 08:57:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/21/00	JDJ		Created
 */

#pragma once

#include <XLogFile.h>
#include <XURI.h>

using namespace Whisper;


//-----------------------------------
//	Forward References 
//
namespace Whisper {
	class XFileSpec;
	class XXMLDoc;
	class XXMLElement;
}


// ===================================================================================
//	class CTestConformance
// ===================================================================================
class CTestConformance {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						~CTestConformance();

						CTestConformance(const XFileSpec& spec);
						// Spec should point to the xmlconf.xml file.

//-----------------------------------
//	Internal API
//
protected:
			void 		DoTestCase(const XXMLElement* testCase);

			void 		DoTest(const XXMLElement* test);

			XXMLDoc* 	DoParseTest(const std::wstring& uri, std::vector<std::wstring>& warnings);
			
			std::wstring DoGetText(const XXMLElement* test);
			
			void 		DoCheckOutput(const XXMLDoc& doc, const XXMLElement* test, bool version1);
			
			void 		DoLogFailure(const XXMLElement* test, const std::exception* e);
			
			void 		DoLogWarnings(const XXMLElement* test, const std::vector<std::wstring>& warnings);

//-----------------------------------
//	Member Data
//
protected:
	int32		mNumTests;
	int32		mNumFailed;
	XLogFile	mLog;
	
	XURI		mRootURI;
	XURI		mBaseURI;
};


