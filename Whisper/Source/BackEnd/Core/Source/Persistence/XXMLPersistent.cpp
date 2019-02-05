/*
 *  File:		XXMLPersistent.cpp
 *  Summary:	Mixin for objects that can be flattened onto XML objects.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLPersistent.cpp,v $
 *		Revision 1.4  2001/04/27 04:39:39  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:45:42  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/10 04:47:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLPersistent.h>

#include <XAutoPtr.h>
#include <XObjectFactory.h>
#include <XStringUtils.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XXMLPersistentMixin
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLPersistentMixin::~XXMLPersistentMixin
//
//---------------------------------------------------------------
XXMLPersistentMixin::~XXMLPersistentMixin()
{
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::XXMLPersistentMixin
//
//---------------------------------------------------------------
XXMLPersistentMixin::XXMLPersistentMixin(bool persistant) : XPersistentMixin(persistant)
{
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::Flatten							[static]
//
//---------------------------------------------------------------
void XXMLPersistentMixin::Flatten(XXMLDoc& doc, XXMLElement& parent, const XXMLPersistentMixin* object)
{
	PRECONDITION(object != nil);
	
	std::wstring name = object->GetClassName();
	
	XAutoPtr<XXMLElement> element(new XXMLElement(name));
	object->HandleStreamOut(doc, *element);
	
	parent.AppendItem(element.Release());
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::Unflatten 						 [static]
//
//---------------------------------------------------------------
XXMLPersistentMixin* XXMLPersistentMixin::Unflatten(const XXMLDoc& doc, const XXMLElement& element, XCreateByNameMixin* parent)
{
	std::wstring name = element.GetName();
	
	XCreateByNameMixin* theObject = XObjectFactory::Instance()->CreateObject(name, parent);

	XAutoPtr<XXMLPersistentMixin> object(dynamic_cast<XXMLPersistentMixin*>(theObject));
	ASSERT(object.Get() != nil);			
	
	object->HandleStreamIn(doc, element);
	
	return object.Release();
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::HandleStreamOut
//
//---------------------------------------------------------------
void XXMLPersistentMixin::HandleStreamOut(XXMLDoc& doc, XXMLElement& parent) const
{
	PRECONDITION(this->IsPersistant());

	this->OnStreamOut(doc, parent);
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::HandleStreamIn
//
//---------------------------------------------------------------
void XXMLPersistentMixin::HandleStreamIn(const XXMLDoc& doc, const XXMLElement& parent)
{
	PRECONDITION(this->IsPersistant());
	
	this->OnStreamIn(doc, parent, parent);

	XXMLElement::const_item_iterator iter = parent.item_begin();
	while (iter != parent.item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		XXMLItem::EType type = item->GetType();
		switch (type) {
			case XXMLItem::kElement:
				this->OnStreamIn(doc, parent, dynamic_cast<const XXMLElement&>(*item));
				break;
				
			case XXMLItem::kCharData:
				this->OnStreamIn(doc, parent, dynamic_cast<const XXMLCharData&>(*item));
				break;
				
			case XXMLItem::kProcessInstruction:
				this->OnStreamIn(doc, parent, dynamic_cast<const XXMLProcessInstruction&>(*item));
				break;
				
			case XXMLItem::kComment:
				break;
				
			default:
				DEBUGSTR("Bad item type in XXMLPersistentMixin::HandleStreamIn");
		}
	}	
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XXMLPersistentMixin::OnStreamIn (XXMLDoc, XXMLElement, XXMLCharData)
//
//---------------------------------------------------------------
void XXMLPersistentMixin::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLCharData& data)
{
	UNUSED(doc);
	
#if DEBUG
	if (!IsWhiteSpace(data.GetText()))
		TRACEFLOW("XWarning", "Ignoring character data in element ", parent.GetName(), "\n");
#endif
}


//---------------------------------------------------------------
//
// XXMLPersistentMixin::OnStreamIn (XXMLDoc, XXMLElement, XXMLProcessInstruction)
//
//---------------------------------------------------------------
void XXMLPersistentMixin::OnStreamIn(const XXMLDoc& doc, const XXMLElement& parent, const XXMLProcessInstruction& pi)
{
	UNUSED(doc);
	UNUSED(pi);

	TRACEFLOW("XWarning", "Ignoring process instruction in element ", parent.GetName(), "\n");
}


}	// namespace Whisper
