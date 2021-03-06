/*
 *  File:       XXMLArchitecture.cpp
 *  Summary:   	Functions uses to stream the architecture classes into and out of XML.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLArchitecture.cpp,v $
 *		Revision 1.5  2001/04/27 04:38:14  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/01 11:40:12  jesjones
 *		Tweaked the TRACEFLOW statements so less code is generated.
 *		
 *		Revision 1.3  2000/12/19 07:59:38  jesjones
 *		Added an IValidateText loader and some range loaders.
 *		
 *		Revision 1.2  2000/11/09 12:59:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLArchitecture.h>

#include <IBinding.h>
#include <IBrushDrawer.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IRange.h>
#include <IReadXML.h>
#include <ISetCheckedHelp.h>
#include <ISetDisabledHelp.h>
#include <ISetHelp.h>
#include <IValidateText.h>
#include <XRGBColor.h>
#include <XStringUtils.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// LoadBinding
//
//---------------------------------------------------------------
void LoadBinding(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	if (pass == kLoadedBoss) {
		IBindingPtr ptr(boss, interface);						// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		SBinding binding;
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			bool value = attr.Get<bool>();
		
			if (name == L"bind_left") {			
				binding.left = value;	
							  												  					
			} else if (name == L"bind_right") {			
				binding.right = value;	
							  												  					
			} else if (name == L"bind_top") {			
				binding.top = value;	
				
			} else if (name == L"bind_bottom") {			
				binding.bottom = value;	
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IBinding!");
		}		
		
		ptr->SetBinding(binding);
	}
}


//---------------------------------------------------------------
//
// LoadBrushDrawer
//
//---------------------------------------------------------------
void LoadBrushDrawer(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	if (pass == kLoadedBoss) {
		IBrushDrawerPtr ptr(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"color") {			
				XRGBColor color = attr.Get<XRGBColor>();	
				ptr->SetColors(color, color);
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IBrushDrawer!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadDoubleRange
//
//---------------------------------------------------------------
void LoadDoubleRange(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IDoubleRangePtr range(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"min") {			
				range->SetMin(attr.Get<double>());
							  												  											  															  					
			} else if (name == L"max") {			
				range->SetMax(attr.Get<double>());
							  												  											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IDoubleRange!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadGeometry
//
//---------------------------------------------------------------
void LoadGeometry(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IGeometryPtr geometry(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have required attributes
				
		XSize minSize, maxSize, size, winSize, macSize;
		size = winSize = macSize = XSize(-1, 1);
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"location") {			
				geometry->SetLocation(attr.Get<XPoint>());	
							  												  					
			} else if (name == L"size") {			
				size = attr.Get<XSize>();	
							  												  					
			} else if (name == L"mac_size") {		
				if (!attr.IsImplied())	
					macSize = attr.Get<XSize>();	
							  												  					
			} else if (name == L"win_size") {		
				if (!attr.IsImplied())	
					winSize = attr.Get<XSize>();	
							  												  					
			} else if (name == L"min_size") {			
				minSize = attr.Get<XSize>();
							  					
			} else if (name == L"max_size") {			
				maxSize = attr.Get<XSize>();
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IGeometry!");
		}		
		
#if MAC
		if (macSize.GetArea() > 0)
			size = macSize;
#elif WIN
		if (winSize.GetArea() > 0)
			size = winSize;
#endif
		ASSERT(size.GetArea() > 0);
		
		geometry->SetMinMaxSize(minSize, maxSize, kDontRedraw);	// do this down here since size attributes may arrive in any order
		geometry->SetSize(size, kDontRedraw);	
	}
}


//---------------------------------------------------------------
//
// LoadInt32Range
//
//---------------------------------------------------------------
void LoadInt32Range(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IInt32RangePtr range(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"min") {			
				range->SetMin(attr.Get<int32>());
							  												  											  															  					
			} else if (name == L"max") {			
				range->SetMax(attr.Get<int32>());
							  												  											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IInt32Range!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadSetHelp
//
//---------------------------------------------------------------
void LoadSetHelp(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		ISetHelpPtr help(boss, interface);						// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"message") {			
				help->SetDefaultHelp(value);
							  												  											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ISetHelp!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadSetDisabledHelp
//
//---------------------------------------------------------------
void LoadSetDisabledHelp(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		ISetDisabledHelpPtr help(boss, interface);				// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"message") {			
				help->SetDisabledHelp(value);
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ISetDisabledHelp!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadSetCheckedHelp
//
//---------------------------------------------------------------
void LoadSetCheckedHelp(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		ISetCheckedHelpPtr help(boss, interface);				// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"message") {			
				help->SetCheckedHelp(value);
							  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of ISetCheckedHelp!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadHierarchy
//
//---------------------------------------------------------------
void LoadHierarchy(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IHierarchyPtr super(boss, interface);		// this should always be valid because IPersistXML checks to make sure the names match
		IReadXMLPtr persist(L"XML Persist");
		
		XXMLElement::const_item_iterator iter = element.item_begin();	
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			const XXMLElement* child = dynamic_cast<const XXMLElement*>(item);
			if (child != nil) {
				IHierarchyPtr node = persist->CreateBoss(*child, super);
				ASSERT(node->GetParent() == super);
			}
		}		
	}
}


//---------------------------------------------------------------
//
// LoadUInt32Range
//
//---------------------------------------------------------------
void LoadUInt32Range(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IUInt32RangePtr range(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"min") {			
				range->SetMin(attr.Get<uint32>());
							  												  											  															  					
			} else if (name == L"max") {			
				range->SetMax(attr.Get<uint32>());
							  												  											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IUInt32Range!");
		}		
	}
}


//---------------------------------------------------------------
//
// LoadValidateText
//
//---------------------------------------------------------------
void LoadValidateText(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);

	if (pass == kLoadedBoss) {
		IValidateTextPtr range(boss, interface);				// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have no child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have attributes
				
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"min_chars") {			
				range->SetMinChars(attr.Get<uint32>());
							  												  											  															  					
			} else if (name == L"max_chars") {			
				range->SetMaxChars(attr.Get<uint32>());
							  												  											  															  					
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IValidateText!");
		}		
	}
}


}	// namespace Whisper
