/*
 *  File:		CCanonical.h
 *  Summary:	Converts an XXMLDoc into a canonical text string.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCanonical.h,v $
 *		Revision 1.2  2000/11/09 08:53:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 3/22/00	JDJ		Created
 */

#pragma once

#include <XXMLDoc.h>

using namespace Whisper;


//-----------------------------------
//	Forward References 
//
namespace Whisper {
	class XXMLAttribute;
}


// ===================================================================================
//	class CCanonical
// ===================================================================================
class CCanonical {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						~CCanonical();

						CCanonical(const XXMLDoc& xml, bool version1);

//-----------------------------------
//	API
//
public:
			std::wstring GetText() const			{return mText;}

//-----------------------------------
//	Internal API
//
protected:
			std::wstring DoGetCanonical(const XXMLDoc& document, bool version1);
			
			std::wstring DoConvertDTD(const XXMLDoc& document);

			std::wstring DoConvertNotations(const XXMLDoc::NotationMap& notations);
			
			std::wstring DoConvertElement(const XXMLElement* element);
			
			std::wstring DoConvertProcessInstruction(const XXMLProcessInstruction* instruction);
			
			std::wstring DoConvertAttribute(const XXMLAttribute& attribute);
			
			std::wstring DoConvertCharData(const std::wstring& data);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mText;
};


