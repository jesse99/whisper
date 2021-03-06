/*
 *  File:		XXMLDocVisitor.h
 *  Summary:	Abstract base class used to visit all the nodes of an XXMLDocVisitor.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDocVisitor.h,v $
 *		Revision 1.2  2000/11/09 13:01:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/04/99	JDJ		Created
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLCharData;
class XXMLComment;
class XXMLDoc;
class XXMLElement;
class XXMLProcessInstruction;


// ===================================================================================
//	class XXMLDocVisitor
// ===================================================================================
class CORE_EXPORT XXMLDocVisitor {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLDocVisitor() = 0;

						XXMLDocVisitor();
						
private:
						XXMLDocVisitor(const XXMLDocVisitor& rhs);
						
			XXMLDocVisitor& operator=(const XXMLDocVisitor& rhs);
						
//-----------------------------------
//	API
//
public:								
	virtual void 		Visit(const XXMLDoc& doc);
	
	virtual bool 		HandleVisit(const XXMLElement& element);
	virtual bool 		HandleVisit(const XXMLCharData& text);
	virtual bool 		HandleVisit(const XXMLComment& comment);
	virtual bool 		HandleVisit(const XXMLProcessInstruction& pi);

protected:
	virtual bool 		OnVisit(const XXMLElement& element)			{UNUSED(element); return true;}
	virtual bool 		OnVisit(const XXMLCharData& text)			{UNUSED(text); return true;}
	virtual bool 		OnVisit(const XXMLComment& comment)			{UNUSED(comment); return true;}
	virtual bool 		OnVisit(const XXMLProcessInstruction& pi)	{UNUSED(pi); return true;}
						// Return false to abort visiting.
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

