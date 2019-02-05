/*
 *  File:		XXMLCallbacks.cpp
 *  Summary:	Classes used when parsing XML content (ie the stuff outside the DTD). Note that
 *				for simple XML documents it's often easier and more efficient to use a custom
 *				XXMLCallback.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLCallbacks.cpp,v $
 *		Revision 1.4  2001/04/27 04:38:19  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/06 07:33:29  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/09 13:00:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLCallbacks.h>

#include <XAutoPtr.h>
#include <XStringUtils.h>
#include <XTranscode.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XXMLNullCallback
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLNullCallback::~XXMLNullCallback
//
//---------------------------------------------------------------
XXMLNullCallback::~XXMLNullCallback()
{
}


//---------------------------------------------------------------
//
// XXMLNullCallback::XXMLNullCallback
//
//---------------------------------------------------------------
XXMLNullCallback::XXMLNullCallback() 
{
}


//---------------------------------------------------------------
//
// XXMLNullCallback::OnBeginElement
//
//---------------------------------------------------------------
void XXMLNullCallback::OnBeginElement(const std::wstring& name)
{	
	UNUSED(name);
}


//---------------------------------------------------------------
//
// XXMLNullCallback::OnEndElement
//
//---------------------------------------------------------------
void XXMLNullCallback::OnEndElement(const std::wstring& name)
{
	UNUSED(name);
}


//---------------------------------------------------------------
//
// XXMLNullCallback::OnAttribute
//
//---------------------------------------------------------------
void XXMLNullCallback::OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID)
{
	UNUSED(element);
	UNUSED(name);
	UNUSED(value);
	UNUSED(implied);
	UNUSED(isID);
}


//---------------------------------------------------------------
//
// XXMLNullCallback::OnText
//
//---------------------------------------------------------------
void XXMLNullCallback::OnText(const std::wstring& element, const std::wstring& text, bool inElementContent)
{
	UNUSED(element);
	UNUSED(text);
	UNUSED(inElementContent);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLDocumentCallback
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLDocumentCallback::~XXMLDocumentCallback
//
//---------------------------------------------------------------
XXMLDocumentCallback::~XXMLDocumentCallback()
{
	std::vector<XXMLElement*>::iterator iter = mElements.begin();	// should only be non-empty if an exception was thrown
	while (iter != mElements.end()) {
		XXMLElement* element = *iter;
		++iter;
		
		delete element;
	}
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::XXMLDocumentCallback
//
//---------------------------------------------------------------
XXMLDocumentCallback::XXMLDocumentCallback(XXMLDoc& doc) : mDoc(doc)
{
	mFoundRoot = false;
	mStripWhiteSpace = true;
	mStripComments = true;
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnBeginDocument
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnBeginDocument()
{
	mDoc.Reset();
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnEndDocument
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnEndDocument(const std::wstring& docName) 
{
	if (docName.length() > 0)
		if (mDoc.GetRootElement()->GetName() != docName)		// VC: Root Element Type
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Root element name doesn't match DOCTYPE name.")));
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnProcessInstruction
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnProcessInstruction(const std::wstring& target, const std::wstring& data)
{
	if (!mElements.empty()) {
		XXMLElement* element = mElements.back();
		element->AppendItem(new XXMLProcessInstruction(target, data)); 
	
	} else if (mFoundRoot)
		mDoc.AddPostProcessInstruction(new XXMLProcessInstruction(target, data));
	
	else
		mDoc.AddPreProcessInstruction(new XXMLProcessInstruction(target, data));
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnUnparsedEntity
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnUnparsedEntity(const std::wstring& name, const SExternalID& id)
{
	(void) mDoc.AddUnparsedEntity(name, id);
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnNotation
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnNotation(const std::wstring& name, const SExternalID& id)
{
	(void) mDoc.AddNotation(name, id);
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnBeginElement
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnBeginElement(const std::wstring& name)
{	
	XXMLElement* element = new XXMLElement(name);
	
	mElements.push_back(element);
	
	mFoundRoot = true;
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnEndElement
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnEndElement(const std::wstring& name)
{
	PRECONDITION(!mElements.empty());
	
	XXMLElement* element = mElements.back();
	mElements.pop_back();		
	
	ASSERT(element->GetName() == name);			// the parser classes will throw if the elements are nested incorrectly, but we'll double check things here

	if (!mElements.empty()) {
		XXMLElement* parent = mElements.back();
		parent->AppendItem(element); 
		
	} else
		mDoc.SetRootElement(element);
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnAttribute
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnAttribute(const std::wstring& elementName, const std::wstring& name, const std::wstring& value, bool implied, bool isID)
{
	PRECONDITION(!mElements.empty());
	
	XXMLElement* element = mElements.back();
	ASSERT(element->GetName() == elementName);
	
	element->AppendAttribute(XXMLAttribute(name, value, implied)); 
	
	if (isID && !implied)
		mDoc.AddElement(element, value);
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnText
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnText(const std::wstring& elementName, const std::wstring& text, bool inElementContent)
{
	PRECONDITION(!mElements.empty());
	
	if (!inElementContent || !mStripWhiteSpace) {
		XXMLElement* element = mElements.back();
		ASSERT(element->GetName() == elementName);

		element->AppendItem(new XXMLCharData(text)); 
	}
}


//---------------------------------------------------------------
//
// XXMLDocumentCallback::OnComment
//
//---------------------------------------------------------------
void XXMLDocumentCallback::OnComment(const std::wstring& contents)
{
	PRECONDITION(!mElements.empty());
	
	if (!mStripComments) {
		XXMLElement* element = mElements.back();
		element->AppendItem(new XXMLComment(contents)); 
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLElementCallback
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLElementCallback::~XXMLElementCallback
//
//---------------------------------------------------------------
XXMLElementCallback::~XXMLElementCallback()
{
	std::vector<XXMLElement*>::iterator iter = mElements.begin();	// should only be non-empty if an exception was thrown
	while (iter != mElements.end()) {
		XXMLElement* element = *iter;
		++iter;
		
		delete element;
	}
}


//---------------------------------------------------------------
//
// XXMLElementCallback::XXMLElementCallback
//
//---------------------------------------------------------------
XXMLElementCallback::XXMLElementCallback(const std::wstring& rootElement) : mRootName(rootElement)
{
	mStripWhiteSpace = true;
}


//---------------------------------------------------------------
//
// XXMLElementCallback::OnBeginElement
//
//---------------------------------------------------------------
void XXMLElementCallback::OnBeginElement(const std::wstring& name)
{		
	if (name != mRootName) {
		XXMLElement* element = new XXMLElement(name);
	
		mElements.push_back(element);
	}
}


//---------------------------------------------------------------
//
// XXMLElementCallback::OnEndElement
//
//---------------------------------------------------------------
void XXMLElementCallback::OnEndElement(const std::wstring& name)
{
	if (name != mRootName) {
		ASSERT(!mElements.empty());
		
		XAutoPtr<XXMLElement> element(mElements.back());
		mElements.pop_back();		
		
		ASSERT(element->GetName() == name);			// the parser classes will throw if the elements are nested incorrectly, but we'll double check things here
	
		if (!mElements.empty()) {
			XXMLElement* parent = mElements.back();
			parent->AppendItem(element.Release()); 
			
		} else 
			this->OnElement(element.Get());
	}
}


//---------------------------------------------------------------
//
// XXMLElementCallback::OnAttribute
//
//---------------------------------------------------------------
void XXMLElementCallback::OnAttribute(const std::wstring& elementName, const std::wstring& name, const std::wstring& value, bool implied, bool isID)
{	
	PRECONDITION(!mElements.empty());
	UNUSED(isID);
	
	XXMLElement* element = mElements.back();
	ASSERT(element->GetName() == elementName);
	
	element->AppendAttribute(XXMLAttribute(name, value, implied)); 
}


//---------------------------------------------------------------
//
// XXMLElementCallback::OnText
//
//---------------------------------------------------------------
void XXMLElementCallback::OnText(const std::wstring& elementName, const std::wstring& text, bool inElementContent)
{	
	if (!inElementContent || !mStripWhiteSpace) {
		ASSERT(!mElements.empty());

		XXMLElement* element = mElements.back();
		ASSERT(element->GetName() == elementName);

		element->AppendItem(new XXMLCharData(text)); 
	}
}


}	// namespace Whisper

