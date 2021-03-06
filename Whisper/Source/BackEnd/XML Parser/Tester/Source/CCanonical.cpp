/*
 *  File:		CCanonical.cpp
 *  Summary:	Converts an XXMLDoc into a canonical text string.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CCanonical.cpp,v $
 *		Revision 1.3  2001/04/21 02:23:20  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 08:52:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include "CCanonical.h"

#include <XXMLDoc.h>
#include <XXMLItems.h>


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// operator== (XXMLAttribute, XXMLAttribute)
//
//---------------------------------------------------------------
namespace Whisper {
static bool operator==(const XXMLAttribute& lhs, const XXMLAttribute& rhs)
{	
	bool equal = lhs.GetName() == rhs.GetName();
	
	return equal;
}
}


//---------------------------------------------------------------
//
// operator< (XXMLAttribute, XXMLAttribute)
//
//---------------------------------------------------------------
namespace Whisper {
static bool operator<(const XXMLAttribute& lhs, const XXMLAttribute& rhs)
{	
	bool equal = lhs.GetName() < rhs.GetName();
	
	return equal;
}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CCanonical
// ===================================================================================

//----------------------------------------------------------------
//
// CCanonical::~CCanonical
//
//----------------------------------------------------------------
CCanonical::~CCanonical()
{
}


//----------------------------------------------------------------
//
// CCanonical::CCanonical
//
//----------------------------------------------------------------
CCanonical::CCanonical(const XXMLDoc& xml, bool version1)
{
	mText = this->DoGetCanonical(xml, version1);
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// CCanonical::DoGetCanonical
//
// CanonXML2 ::= DTD2? CanonXML
//
// CanonXML ::= Pi* element Pi*
//
//---------------------------------------------------------------
std::wstring CCanonical::DoGetCanonical(const XXMLDoc& document, bool version1)
{	
	std::wstring text;	
	
	if (!version1)
		text += this->DoConvertDTD(document);

	const XXMLDoc::ProcessInstructions& pre = document.GetPreProcessInstructions();
	XXMLDoc::ProcessInstructions::const_iterator iter = pre.begin();
	while (iter != pre.end()) {
		const XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		text += this->DoConvertProcessInstruction(instruction);
	}
	
	const XXMLElement* root = document.GetRootElement();
	text += this->DoConvertElement(root);
	
	const XXMLDoc::ProcessInstructions& post = document.GetPostProcessInstructions();
	iter = post.begin();
	while (iter != post.end()) {
		const XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		text += this->DoConvertProcessInstruction(instruction);
	}
	
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertDTD
//
// DTD2 ::= '<!DOCTYPE ' name ' [' #xA Notations? ']>' #xA
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertDTD(const XXMLDoc& document)
{	
	std::wstring text;	
	
	const XXMLDoc::NotationMap& notations = document.GetNotations();
	if (notations.size() > 0) {
		text += L"<!DOCTYPE ";
		text += document.GetRootElement()->GetName();
		text += L" [\r";
	
		text += this->DoConvertNotations(notations);
		text += L"]>\r";
	}
		
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertNotations
//
// Notations ::= ( '<!NOTATION ' Name '
//                 (('PUBLIC ' PubidLiteral ' ' SystemLiteral)
//                 |('PUBLIC ' PubidLiteral)
//                 |('SYSTEM ' SystemLiteral))
//                 '>' #xA )*
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertNotations(const XXMLDoc::NotationMap& notations)
{	
	std::wstring text;	
	
	XXMLDoc::NotationMap::const_iterator iter = notations.begin();
	while (iter != notations.end()) {
		const std::wstring& name = iter->first;
		const SExternalID& id = iter->second;
		++iter;
		
		text += L"<!NOTATION ";
		text += name;
		text += L" ";

		if (id.publicID.length() > 0) {
			text += L"PUBLIC '";
			text += id.publicID;
			text += L"'";

			if (id.systemID.length() > 0)
				text += L" '" + id.systemID + L"'";

		} else if (id.systemID.length() > 0) {
			text += L"SYSTEM '";
			text += id.systemID;
			text += L"'";
			
		} else
			DEBUGSTR("Found an unparsed entity?");

		text += L">\r";
	}
			
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertElement
//
// element ::= Stag (Datachar | Pi | element)* Etag
//
// Stag ::= '<'  Name Atrribute* '>'
//
// Etag ::= '</' Name '>'
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertElement(const XXMLElement* element)
{
	ASSERT(element != nil);
	
	// '<'  Name
	std::wstring text = L"<" + element->GetName();
	
	// Atrribute*
	std::vector<XXMLAttribute> attributes;
	XXMLElement::const_attr_iterator iter = element->attr_begin();
	while (iter != element->attr_end()) {
		const XXMLAttribute& attribute = *iter;
		++iter;
	
		attributes.push_back(attribute);
	}
	std::sort(attributes.begin(), attributes.end());
	
	std::vector<XXMLAttribute>::const_iterator iter3 = attributes.begin();
	while (iter3 != attributes.end()) {
		const XXMLAttribute& attribute = *iter3;
		++iter3;
	
		text += this->DoConvertAttribute(attribute);
	}
	
	// '>'
	text += L">";
	
	// (Datachar | Pi | element)*
	XXMLElement::const_item_iterator iter2 = element->item_begin();
	while (iter2 != element->item_end()) {
		const XXMLItem* item = *iter2;
		++iter2;
		
		XXMLItem::EType type = item->GetType();
		if (type == XXMLItem::kElement)
			text += this->DoConvertElement((const XXMLElement*) item);
		
		else if (type == XXMLItem::kCharData)
			text += this->DoConvertCharData(((const XXMLCharData*) item)->GetText());
		
		else if (type == XXMLItem::kProcessInstruction)
			text += this->DoConvertProcessInstruction((const XXMLProcessInstruction*) item);
			
		else
			DEBUGSTR("Item has a bad type!");
	}
	
	// '</' Name '>'
	text += L"</" + element->GetName() + L">";
	
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertProcessInstruction
//
// Pi ::= '<?' Name ' ' (((Char - S) Char*)? - (Char* '?>' Char*)) '?>'
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertProcessInstruction(const XXMLProcessInstruction* instruction)
{
	ASSERT(instruction != nil);
	
	std::wstring text = L"<?" + instruction->GetTarget() + L" " + instruction->GetData() + L"?>";
	
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertAttribute
//
// Atrribute ::= ' ' Name '=' '"' Datachar* '"'
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertAttribute(const XXMLAttribute& attribute)
{
	std::wstring text;
	
	if (!attribute.IsImplied()) {
		text = L" " + attribute.GetName() + L"=\"";
		
		text += this->DoConvertCharData(attribute.GetValue());
		
		text += L"\"";
	}
		
	return text;
}


//---------------------------------------------------------------
//
// CCanonical::DoConvertCharData
//
// Datachar ::= '&amp;' | '&lt;' | '&gt;' | '&quot;' | 
//              '&#9;'| '&#10;'| '&#13;'| 
//              (Char - ('&' | '<' | '>' | '"' | #x9 | #xA | #xD))
//
//---------------------------------------------------------------
std::wstring CCanonical::DoConvertCharData(const std::wstring& data)
{		
	std::wstring text;
	
	for (uint32 index = 0; index < data.length(); ++index) {
		wchar_t ch = data[index];
		
		switch (ch) {
			case '&':
				text += L"&amp;";
				break;
			
			case '<':
				text += L"&lt;";
				break;
			
			case '>':
				text += L"&gt;";
				break;
			
			case '"':
				text += L"&quot;";
				break;
			
			case '\t':
				text += L"&#9;";
				break;
			
			case '\n':
				text += L"&#10;";
				break;
			
			case '\r':
				text += L"&#13;";
				break;
			
			default:
				text += ch;
		}
	}
	
	return text;
}


