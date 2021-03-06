/*
 *  File:		XXMLDocVisitors.cpp
 *  Summary:	Classes used to visit all the nodes of an XXMLDocVisitor.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDocVisitors.cpp,v $
 *		Revision 1.3  2001/04/27 04:38:49  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 13:01:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 7/01/99	JDJ		No longer adds extra blank lines to character data.
 *		 <2>	 6/27/99	JDJ		Attributes are now printed on seperate lines. Added
 *									blank lines between elements.
 *		 <1>	 5/04/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLDocVisitors.h>

#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XXMLPrintDoc
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLPrintDoc::~XXMLPrintDoc
//
//---------------------------------------------------------------
XXMLPrintDoc::~XXMLPrintDoc()
{	
}


//---------------------------------------------------------------
//
// XXMLPrintDoc::XXMLPrintDoc
//
//---------------------------------------------------------------
XXMLPrintDoc::XXMLPrintDoc(std::wstring& text, const std::wstring& indent) : mText(text), mIndent(indent)
{
	mLevel = 0;
}


//---------------------------------------------------------------
//
// XXMLPrintDoc::HandleVisit (XXMLElement)
//
// [39] element ::= EmptyElemTag | STag content ETag
//
// [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
//
// [40] STag ::= '<' Name (S Attribute)* S? '>'
//
// [42] ETag ::= '</' Name S? '>'
//
//---------------------------------------------------------------
bool XXMLPrintDoc::HandleVisit(const XXMLElement& element)
{
	PRECONDITION(mLevel >= 0);
	
	bool visiting = true;
	
	mText += L"\n";
	for (int32 i = 0; i < mLevel; ++i)
		mText += mIndent;
	
	// '<' Name
	mText += L"<" + element.GetName();
	
	// (S Attribute)* S?
	XXMLElement::const_attr_iterator iter = element.attr_begin();
	while (iter != element.attr_end() && visiting) {
		XXMLAttribute attr = *iter;
		++iter;
		
		mText += L"\n";
		for (int32 i = 0; i < mLevel+1; ++i)
			mText += mIndent;
		
		visiting = this->OnVisit(attr);
	}
		
	if (visiting) {	
		if (element.item_begin() == element.item_end()) {
		
			// '/>'
			mText += L"/>\n";
	
		} else {
		
			// '>'
			mText += L">";
			
			// content
			++mLevel;
			XXMLElement::const_item_iterator iter2 = element.item_begin();
			while (iter2 != element.item_end() && visiting) {
				const XXMLItem* item = *iter2;
				++iter2;
				
				visiting = item->HandleVisit(*this);
			}
			--mLevel;

			// '</' Name S? '>'
			mText += L"</" + element.GetName() + L">\n";
		}

		for (int32 i = 0; i < mLevel-1; ++i)
			mText += mIndent;
	}
		
	return visiting;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XXMLPrintDoc::OnVisit (XXMLElement)
// 
//---------------------------------------------------------------
bool XXMLPrintDoc::OnVisit(const XXMLElement& element)
{
	UNUSED(element);
	
	// we take care of all of this in HandleVisit
			
	return true;
}


//---------------------------------------------------------------
//
// XXMLPrintDoc::OnVisit (XXMLCharData)
// 
// [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
//
//---------------------------------------------------------------
bool XXMLPrintDoc::OnVisit(const XXMLCharData& data)
{
	const std::wstring& text = data.GetText();
	
	for (uint32 i = 0; i < text.length(); ++i) {
		wchar_t ch = text[i];
		
		switch (ch) {
			case '<':
				mText += L"&lt;";
				break;
				
			case '>':
				mText += L"&gt;";
				break;
				
			case '&':
				mText += L"&amp;";
				break;
				
			case '\'':
				mText += L"&apos;";
				break;
				
			case '"':
				mText += L"&quot;";
				break;
				
			default:
				mText += ch;
		}
	}
	
	return true;
}


//---------------------------------------------------------------
//
// XXMLPrintDoc::OnVisit (XXMLComment)
//
// [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
//
//---------------------------------------------------------------
bool XXMLPrintDoc::OnVisit(const XXMLComment& comment)
{
	mText += L"\n";
	for (int32 i = 0; i < mLevel; ++i)
		mText += mIndent;
		
	mText += L"<!--" + comment.GetText() + L"-->\n";
	
	return true;
}


//---------------------------------------------------------------
//
// XXMLPrintDoc::OnVisit (XXMLProcessInstruction)
//
// [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
//
//---------------------------------------------------------------
bool XXMLPrintDoc::OnVisit(const XXMLProcessInstruction& pi)
{
	mText += L"\n";
	for (int32 i = 0; i < mLevel; ++i)
		mText += mIndent;
		
	mText += L"<?" + pi.GetTarget() + L" " + pi.GetData() + L"?>\n";
	
	return true;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XXMLPrintDoc::OnVisit (XXMLAttribute)
// 
// 41] Attribute ::= Name Eq AttValue
//
//---------------------------------------------------------------
bool XXMLPrintDoc::OnVisit(const XXMLAttribute& attr)
{
	mText += attr.GetName() + L" = \"" + attr.GetValue() + L"\"";
	
	return true;
}


}	// namespace Whisper

