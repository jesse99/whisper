/*
 *  File:		XXMLDocVisitors.h
 *  Summary:	Classes used to visit all the nodes of an XXMLDocVisitor.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Classes:	XXMLPrintDoc	- Converts an XXMLDoc to a string.
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDocVisitors.h,v $
 *		Revision 1.2  2000/11/09 13:01:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/04/99	JDJ		Created
 */

#pragma once

#include <XXMLDocVisitor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLAttribute;


// ===================================================================================
//	class XXMLPrintDoc
// ===================================================================================
class CORE_EXPORT XXMLPrintDoc : public XXMLDocVisitor {

	typedef XXMLDocVisitor Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLPrintDoc();

						XXMLPrintDoc(std::wstring& text, const std::wstring& indent = L"\t");
						// Appends the doc to text.
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		HandleVisit(const XXMLElement& element);

protected:
	virtual bool 		OnVisit(const XXMLElement& element);
	virtual bool 		OnVisit(const XXMLCharData& text);
	virtual bool 		OnVisit(const XXMLComment& comment);
	virtual bool 		OnVisit(const XXMLProcessInstruction& pi);

//-----------------------------------
//	New API
//
protected:
	virtual bool 		OnVisit(const XXMLAttribute& attr);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring&	mText;
	std::wstring	mIndent;
	
	int32			mLevel;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

