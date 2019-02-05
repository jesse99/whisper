/*
 *  File:		XXMLDocVisitor.cpp
 *  Summary:	Abstract base class used to visit all the nodes of an XXMLDoc.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDocVisitor.cpp,v $
 *		Revision 1.2  2000/11/09 13:01:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 5/04/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLDocVisitor.h>

#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XXMLDocVisitor
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLDocVisitor::~XXMLDocVisitor
//
//---------------------------------------------------------------
XXMLDocVisitor::~XXMLDocVisitor()
{	
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::XXMLDocVisitor
//
//---------------------------------------------------------------
XXMLDocVisitor::XXMLDocVisitor()
{
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::Visit
//
//---------------------------------------------------------------
void XXMLDocVisitor::Visit(const XXMLDoc& doc)
{
	bool visiting = true;
	
	XXMLDoc::ProcessInstructions::const_iterator iter = doc.GetPreProcessInstructions().begin();
	while (iter != doc.GetPreProcessInstructions().end() && visiting) {
		const XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		visiting = this->OnVisit(*instruction);
	}
	
	const XXMLElement* root = doc.GetRootElement();
	if (root != nil) 
		visiting = root->HandleVisit(*this);

	iter = doc.GetPostProcessInstructions().begin();
	while (iter != doc.GetPostProcessInstructions().end() && visiting) {
		const XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		visiting = this->OnVisit(*instruction);
	}
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::HandleVisit (XXMLElement)
//
//---------------------------------------------------------------
bool XXMLDocVisitor::HandleVisit(const XXMLElement& element)
{
	bool visiting = this->OnVisit(element);
	
	XXMLElement::const_item_iterator iter = element.item_begin();
	while (iter != element.item_end() && visiting) {
		const XXMLItem* item = *iter;
		++iter;
		
		visiting = item->HandleVisit(*this);
	}

	return visiting;
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::HandleVisit (XXMLCharData)
//
//---------------------------------------------------------------
bool XXMLDocVisitor::HandleVisit(const XXMLCharData& text)
{
	bool visiting = this->OnVisit(text);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::HandleVisit (XXMLComment)
//
//---------------------------------------------------------------
bool XXMLDocVisitor::HandleVisit(const XXMLComment& comment)
{
	bool visiting = this->OnVisit(comment);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLDocVisitor::HandleVisit (XXMLProcessInstruction)
//
//---------------------------------------------------------------
bool XXMLDocVisitor::HandleVisit(const XXMLProcessInstruction& pi)
{
	bool visiting = this->OnVisit(pi);
	
	return visiting;
}


}	// namespace Whisper

