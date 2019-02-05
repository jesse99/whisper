/*
 *  File:		XXMLItems.cpp
 *  Summary:	Concrete classes for things that can be nested within an XML element..
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XXMLItems.cpp,v $
 *		Revision 1.4  2001/04/27 04:39:16  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/06 07:33:37  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/10 04:46:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 3/18/00	JDJ		No longer calls SetClassName to use an abbreviated name.
 *		 <1>	 3/02/00	JDJ		Split off from XXMLDoc.cpp
 */

#include <XWhisperHeader.h>
#include <XXMLItems.h>

#include <XStreaming.h>
#include <XStringUtils.h>
#include <XTranscode.h>
#include <XXMLDocVisitor.h>

namespace Whisper {


// ===================================================================================
//	class XXMLComment
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLComment::~XXMLComment
//
//---------------------------------------------------------------
XXMLComment::~XXMLComment()
{
}


//---------------------------------------------------------------
//
// XXMLComment::XXMLComment
//
//---------------------------------------------------------------
XXMLComment::XXMLComment(const std::wstring& contents) : XXMLItem(kComment), mContents(contents)
{	
}


//---------------------------------------------------------------
//
// XXMLComment::Create 									[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XXMLComment::Create(XCreateByNameMixin* parent)
{
	UNUSED(parent);
	
	XXMLItem* item = new XXMLComment(L"");
	
	return item;
}


//---------------------------------------------------------------
//
// XXMLComment::HandleVisit
//
//---------------------------------------------------------------
bool XXMLComment::HandleVisit(XXMLDocVisitor& visitor) const
{
	bool visiting = visitor.HandleVisit(*this);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLComment::OnStreamOut
//
//---------------------------------------------------------------
void XXMLComment::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);
	
	stream << mContents;
}


//---------------------------------------------------------------
//
// XXMLComment::OnStreamIn
//
//---------------------------------------------------------------
void XXMLComment::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	
	stream >> mContents;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLCharData
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLCharData::~XXMLCharData
//
//---------------------------------------------------------------
XXMLCharData::~XXMLCharData()
{
}


//---------------------------------------------------------------
//
// XXMLCharData::XXMLCharData
//
//---------------------------------------------------------------
XXMLCharData::XXMLCharData(const std::wstring& text) : XXMLItem(kCharData), mText(text)
{	
}


//---------------------------------------------------------------
//
// XXMLCharData::Create 								[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XXMLCharData::Create(XCreateByNameMixin* parent)
{
	UNUSED(parent);
	
	XXMLItem* item = new XXMLCharData(L"");
	
	return item;
}


//---------------------------------------------------------------
//
// XXMLCharData::HandleVisit
//
//---------------------------------------------------------------
bool XXMLCharData::HandleVisit(XXMLDocVisitor& visitor) const
{
	bool visiting = visitor.HandleVisit(*this);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLCharData::OnStreamOut
//
//---------------------------------------------------------------
void XXMLCharData::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);
	
	stream << mText;
}


//---------------------------------------------------------------
//
// XXMLCharData::OnStreamIn
//
//---------------------------------------------------------------
void XXMLCharData::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	
	stream >> mText;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLProcessInstruction
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLProcessInstruction::~XXMLProcessInstruction
//
//---------------------------------------------------------------
XXMLProcessInstruction::~XXMLProcessInstruction()
{
}


//---------------------------------------------------------------
//
// XXMLProcessInstruction::XXMLProcessInstruction
//
//---------------------------------------------------------------
XXMLProcessInstruction::XXMLProcessInstruction(const std::wstring& target, const std::wstring& data) : XXMLItem(kProcessInstruction), mTarget(target), mData(data)
{	
	PRECONDITION(mTarget.length() > 0);
}


//---------------------------------------------------------------
//
// XXMLProcessInstruction::Create 						[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XXMLProcessInstruction::Create(XCreateByNameMixin* parent)
{
	UNUSED(parent);
	
	XXMLItem* item = new XXMLProcessInstruction(L"dummy", L"");
	
	return item;
}


//---------------------------------------------------------------
//
// XXMLProcessInstruction::HandleVisit
//
//---------------------------------------------------------------
bool XXMLProcessInstruction::HandleVisit(XXMLDocVisitor& visitor) const
{
	bool visiting = visitor.HandleVisit(*this);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLProcessInstruction::OnStreamOut
//
//---------------------------------------------------------------
void XXMLProcessInstruction::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);
	
	stream << 1L << mTarget << mData;
}


//---------------------------------------------------------------
//
// XXMLProcessInstruction::OnStreamIn
//
//---------------------------------------------------------------
void XXMLProcessInstruction::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	
	int32 version;
	stream >> version;

	if (version != 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Invalid XXMLProcessInstruction version.")));

	stream >> mTarget >> mData;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLElement
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLElement::~XXMLElement
//
//---------------------------------------------------------------
XXMLElement::~XXMLElement()
{
	item_iterator iter = mChildren.begin();
	while (iter != mChildren.end()) {
		XXMLItem* item = *iter;
		++iter;
		
		delete item;	
	}
}


//---------------------------------------------------------------
//
// XXMLElement::XXMLElement
//
//---------------------------------------------------------------
XXMLElement::XXMLElement(const std::wstring& name) : XXMLItem(kElement), mName(name)
{	
}


//---------------------------------------------------------------
//
// XXMLElement::Create 								[static]
//
//---------------------------------------------------------------
XCreateByNameMixin* XXMLElement::Create(XCreateByNameMixin* parent)
{
	UNUSED(parent);
	
	XXMLItem* item = new XXMLElement(L"");
	
	return item;
}


//---------------------------------------------------------------
//
// XXMLElement::AppendAttribute
//
//---------------------------------------------------------------
void XXMLElement::AppendAttribute(const XXMLAttribute& attribute)
{
	PRECONDITION(attribute.GetName().length() > 0);
	
	mAttributes.push_back(attribute);
}


//---------------------------------------------------------------
//
// XXMLElement::AppendItem
//
//---------------------------------------------------------------
void XXMLElement::AppendItem(XXMLItem* takeItem)
{
	PRECONDITION(takeItem != nil);
		
	mChildren.push_back(takeItem);
}


//---------------------------------------------------------------
//
// XXMLElement::FindAttribute
//
//---------------------------------------------------------------
const XXMLAttribute& XXMLElement::FindAttribute(const std::wstring& name) const
{
	XXMLElement::const_attr_iterator iter = this->attr_begin();	
	while (iter != this->attr_end() && iter->GetName() != name) 
		++iter;
	
	if (iter == this->attr_end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the '#1' attribute of element '#2'.", name, mName)));
	
	return *iter;
}


//---------------------------------------------------------------
//
// XXMLElement::FindElement
//
//---------------------------------------------------------------
const XXMLElement* XXMLElement::FindElement(const std::wstring& name, bool recursive) const
{
	const XXMLElement* element = nil;
	
	std::list<const XXMLElement*> elements;
	elements.push_back(this);
	
	while (!elements.empty() && element == nil) {
		const XXMLElement* temp = elements.back();
		elements.pop_back();
		
		XXMLElement::const_item_iterator iter = temp->item_begin();	
		while (iter != temp->item_end() && element == nil) {
			const XXMLItem* item = *iter;
			++iter;
			
			const XXMLElement* candidate = dynamic_cast<const XXMLElement*>(item);
			if (candidate != nil) {
				if (candidate->GetName() == name)
					element = candidate;
				else if (recursive)
					elements.push_front(candidate);
			}
		}
	}
			
	return element;
}


//---------------------------------------------------------------
//
// XXMLElement::HandleVisit
//
//---------------------------------------------------------------
bool XXMLElement::HandleVisit(XXMLDocVisitor& visitor) const
{
	bool visiting = visitor.HandleVisit(*this);
	
	return visiting;
}


//---------------------------------------------------------------
//
// XXMLElement::OnStreamOut
//
//---------------------------------------------------------------
void XXMLElement::OnStreamOut(XOutStream& stream) const
{
	Inherited::OnStreamOut(stream);
	
	stream << 1UL << mName << mAttributes << (uint32) mChildren.size();
	
	const_item_iterator iter = this->item_begin();
	while (iter != this->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		XBinaryPersistentMixin::Flatten(stream, item);
	}
}


//---------------------------------------------------------------
//
// XXMLElement::OnStreamIn
//
//---------------------------------------------------------------
void XXMLElement::OnStreamIn(XInStream& stream)
{
	Inherited::OnStreamIn(stream);
	
	uint32 version, count;
	stream >> version;

	if (version != 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Invalid XXMLElement version.")));
	
	stream >> mName >> mAttributes >> count;

	while (!mChildren.empty()) {
		delete mChildren.back();
		mChildren.pop_back();
	}

	for (uint32 i = 0; i < count; ++i) {
		XBinaryPersistentMixin* object = XBinaryPersistentMixin::Unflatten(stream, this);
		XXMLItem* item = dynamic_cast<XXMLItem*>(object);
		if (item == nil) {
			delete object;
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XXMLElement::OnStreamIn created something that wasn't an XXMLItem.")));
		}
		
		this->AppendItem(item);
	}
}


}	// namespace Whisper

