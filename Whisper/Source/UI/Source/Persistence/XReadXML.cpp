/*
 *  File:       XReadXML.cpp
 *  Summary:   	Interface used to stream bosses in from XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XReadXML.cpp,v $
 *		Revision 1.5  2001/04/27 04:35:31  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/01/30 07:18:51  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:37:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:48:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IReadXML.h>

#include <functional>

#include <IHierarchy.h>
#include <ILoaded.h>
#include <XAutoPtr.h>
#include <XInterfaceMacros.h>
#include <XLoadXML.h>
#include <XObjectModel.h>
#include <XStringUtils.h>
#include <XTranscode.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kLanguageWeight = 4;
const uint32 kPlatformWeight = 2;	
const uint32 kRegionWeight   = 1;	


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetWeight
//
//---------------------------------------------------------------
static uint32 GetWeight(const XXMLElement* candidate, const XLocale& locale)
{
	PRECONDITION(candidate != nil);
	PRECONDITION(candidate->GetName() == L"LocaleEntry");
		
	uint32 weight = 0;
	
	XXMLElement::const_attr_iterator iter = candidate->attr_begin();
	while (iter != candidate->attr_end()) {
		XXMLAttribute attribute = *iter;
		++iter;
		
		std::wstring value = attribute.GetValue();
		
		if (attribute.GetName() == L"language") {
			if (value == locale.GetLanguage())
				weight += kLanguageWeight;
												
		} else if (attribute.GetName() == L"region") {
			if (value == locale.GetRegion())
				weight += kRegionWeight;
						
		} else if (attribute.GetName() == L"platform") {
			if (value == L"all")
				weight += kPlatformWeight;
#if MAC
			else if (value == L"Mac")
				weight += kPlatformWeight;
#elif WIN
			else if (value == L"Win")
				weight += kPlatformWeight;
#endif
						
		} else if (attribute.GetName() == L"ref") {
			// do nothing
									
		} else
			DEBUGSTR("Bogus LocaleEntry attribute");		// should have been caught by the parser
	}
	
	return weight;
}


//---------------------------------------------------------------
//
// FindLocalID
//
//---------------------------------------------------------------
static std::wstring FindLocalID(const XXMLElement* element, const XLocale& locale)
{	
	PRECONDITION(element != nil);
	PRECONDITION(element->GetName() == L"LocaleTable");
	
	const XXMLElement* entry = nil;
	uint32 weight = 0;

	XXMLElement::const_item_iterator iter = element->item_begin();
	while (iter != element->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLElement* candidate = dynamic_cast<const XXMLElement*>(item);
		ASSERT(candidate != nil);
		ASSERT(candidate->GetName() == L"LocaleEntry");
		
		uint32 w = GetWeight(candidate, locale);
		if (w > weight) {
			entry = candidate;
			weight = w;
		}
	}
	
	std::wstring id;
	if (weight >= kLanguageWeight) {
		const XXMLAttribute& attribute = entry->FindAttribute(L"ref");
		id = attribute.GetValue();
	}
	
	return id;
}


//---------------------------------------------------------------
//
// MatchInterfaces
//
//---------------------------------------------------------------
class MatchInterfaces : public std::binary_function<XBoss::iterator, std::wstring, bool> {
public:
	bool operator()(XBoss::iterator iter, const std::wstring& name) const;
};


bool MatchInterfaces::operator()(XBoss::iterator iter, const std::wstring& name) const
{
	const XInterface& interface = *iter;
	const std::wstring& lhs = interface.GetEncodedName();
	
	std::wstring rhs = XObjectModel::Instance()->GetEncodedName(name);
	
	bool matches = lhs == rhs;
	
	return matches;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XReadXML
//!		Interface used to stream bosses in from XML.
// ===================================================================================
class XReadXML : public IReadXML {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XReadXML();
	
						XReadXML(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual XUnknownPtr	CreateBoss(const XXMLElement& element, const IHierarchyPtr& parent);	
	virtual XUnknownPtr	CreateBoss(const XXMLDoc& doc, const std::wstring& id, const IHierarchyPtr& parent);	
	virtual XUnknownPtr	CreateBoss(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent);
							
	virtual void 		ReadBoss(XUnknownPtr& unknown, const XXMLElement& element);

	virtual void 		AddReader(const std::wstring& interfaceName, ReadProc proc);

//-----------------------------------
//	Internal API
//
private:
			const XXMLElement* DoFindLocalizedElement(const XXMLDoc& doc, const std::wstring& id);

			void		DoLoadBoss(XUnknownPtr& unknown, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
			
			void 		DoLoaded(XBoss* boss);
			
//-----------------------------------
//	Types
//
private:
	typedef std::map<std::wstring, ReadProc>	LoaderTable;

//-----------------------------------
//	Member Data
//
private:
	LoaderTable		mLoaders;
};

DEFINE_INTERFACE_FACTORY(XReadXML)

//---------------------------------------------------------------
//
// XReadXML::~XReadXML
//
//---------------------------------------------------------------
XReadXML::~XReadXML()
{
}


//---------------------------------------------------------------
//
// XReadXML::XReadXML
//
//---------------------------------------------------------------
XReadXML::XReadXML(XBoss* boss)
{
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XReadXML::CreateBoss (XURI, wstring, IHierarchyPtr)
//
//---------------------------------------------------------------
XUnknownPtr	XReadXML::CreateBoss(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent)
{
	XXMLDoc doc;
	Load(doc, uri);
	
	XUnknownPtr unknown = this->CreateBoss(doc, id, parent);
		
	return unknown;
}


//---------------------------------------------------------------
//
// XReadXML::CreateBoss (XXMLDoc, wstring, IHierarchyPtr)
//
//---------------------------------------------------------------
XUnknownPtr	XReadXML::CreateBoss(const XXMLDoc& doc, const std::wstring& id, const IHierarchyPtr& parent)
{
	const XXMLElement* element = this->DoFindLocalizedElement(doc, id);		// will throw if id isn't found
	ASSERT(element != nil);			
	
	XUnknownPtr unknown = this->CreateBoss(*element, parent);
	
	this->DoLoaded(unknown->GetBoss());
	
	return unknown;
}


//---------------------------------------------------------------
//
// XReadXML::CreateBoss (XXMLElement, IHierarchyPtr)
//
//---------------------------------------------------------------
XUnknownPtr	XReadXML::CreateBoss(const XXMLElement& element, const IHierarchyPtr& parent)
{
	// Create the boss
	const XXMLAttribute& name = element.FindAttribute(L"name");
	XUnknownPtr unknown(name.GetValue());
	
	// Give each interface a chance to load
	this->DoLoadBoss(unknown, element, parent, kLoadingBoss);
	
	// Give each interface a second chance to load (interfaces like
	// IGeometry require the signature interface to be loaded before
	// it can be called).
	this->DoLoadBoss(unknown, element, parent, kLoadedBoss);
		
	return unknown;
}


//---------------------------------------------------------------
//
// XReadXML::ReadBoss
//
//---------------------------------------------------------------
void XReadXML::ReadBoss(XUnknownPtr& unknown, const XXMLElement& element)
{
	IHierarchyPtr parent;

	IHierarchyPtr node(unknown);
	if (node)
		parent = node->GetParent();
	
	// Give each interface a chance to load
	this->DoLoadBoss(unknown, element, parent, kLoadingBoss);
	
	// Give each interface a second chance to load (interfaces like
	// IGeometry require the signature interface to be loaded before
	// it can be called).
	this->DoLoadBoss(unknown, element, parent, kLoadedBoss);
}


//---------------------------------------------------------------
//
// XReadXML::AddReader
//
//---------------------------------------------------------------
void XReadXML::AddReader(const std::wstring& interfaceName, ReadProc proc)
{
	PRECONDITION(interfaceName.length() > 0);
	PRECONDITION(proc != nil);
	
	(void) mLoaders.insert(LoaderTable::value_type(interfaceName, proc));	// allow overwriting existing loaders
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XReadXML::DoFindLocalizedElement 					
//
//---------------------------------------------------------------
const XXMLElement* XReadXML::DoFindLocalizedElement(const XXMLDoc& doc, const std::wstring& id)
{
	const XXMLElement* element = doc.FindElement(id);		// will throw if id isn't found
	ASSERT(element != nil);			

	if (element->GetName() == L"LocaleTable") {
		std::wstring newID = Whisper::FindLocalID(element, XLocale());
		
		if (newID.length() == 0)
			newID = Whisper::FindLocalID(element, XLocale(L"English", L""));

		if (newID.length() == 0) 
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Couldn't find a localized (or English) version of '#1'.", id)));
	
		element = doc.FindElement(newID);
		ASSERT(element != nil);
	}
		
	return element;
}


//---------------------------------------------------------------
//
// XReadXML::DoLoaded 					
//
//---------------------------------------------------------------
void XReadXML::DoLoaded(XBoss* boss)
{
	PRECONDITION(boss != nil);
	
	if (boss->begin() != boss->end()) {
	
		// Notify each implementation that cares that it's been loaded.
		XUnknownPtr unknown(boss, boss->begin());
		CallRepeatedMethod(unknown.Get(), &ILoaded::OnLoaded);
		
		// If the boss is part of a hierarchy then notify it's children as well.
		IHierarchyPtr parent(unknown);
		if (parent) {
		
			IHierarchy::iterator iter2 = parent->begin();
			while (iter2 != parent->end()) {
				IHierarchyPtr child = *iter2;
				++iter2;
				
				this->DoLoaded(child->GetBoss());
			}
		}
	}
}


//---------------------------------------------------------------
//
// XReadXML::DoLoadBoss
//
//---------------------------------------------------------------
void XReadXML::DoLoadBoss(XUnknownPtr& unknown, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	// Save off iterators to each interface on the boss
	XBoss* boss = unknown->GetBoss();
	
	std::list<XBoss::iterator> interfaces, interfaces2;	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end()) {
		interfaces.push_back(iter);
		interfaces2.push_back(iter);
		++iter;
	}
	
	// Iterate over each interface in the XML and,
	XXMLElement::const_item_iterator iter2 = element.item_begin();
	while (iter2 != element.item_end()) {
		const XXMLElement* interface = dynamic_cast<const XXMLElement*>(*iter2);
		++iter2;
		
		// get the corresponding interface in the boss and,
		if (interface != nil) {
			const std::wstring& name = interface->GetName();
			std::list<XBoss::iterator>::iterator iter3 = std::find_if(interfaces.begin(), interfaces.end(), std::bind2nd(MatchInterfaces(), name));
			if (iter3 != interfaces.end()) {
			
				// invoke the loader and,
				LoaderTable::const_iterator iter4 = mLoaders.find(name);
				if (iter4 != mLoaders.end()) {
					ReadProc loader = iter4->second;
					loader(boss, *iter3, *interface, parent, pass);				
				
				} else if (pass == kLoadingBoss)
					TRACEFLOW("XReadXML", "Couldn't find a loader for ", name, "\n");	// a warning because we want to be able to read old bosses
				
				// remove the interface from our list of interfaces
				// in the boss (we need to do this to handle duplicate
				// interfaces).
				interfaces.erase(iter3);
					
			} else if (pass == kLoadingBoss)
				TRACEFLOW("XReadXML", "Couldn't find an interface in boss '", boss->GetName(), "' named '", name, "'\n");	// a warning because we want to be able to read old bosses
		}
	}
}


}	// namespace Whisper
