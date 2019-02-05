/*
 *  File:       CPersistXML.cpp
 *  Summary:   	Functions used to stream the document out and in as XML. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPersistXML.cpp,v $
 *		Revision 1.2  2001/04/21 02:24:41  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/31 04:23:41  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "CPersistXML.h"

#include <XStringUtils.h>
#include <XXMLItems.h>

#include "IDocInfo.h"


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateElement
//
//---------------------------------------------------------------
static XXMLElement* CreateElement(const XRect& rect)
{
	XXMLElement* element = new XXMLElement(L"Rect");
	
	element->AppendAttribute(XXMLAttribute(L"left", rect.left));
	element->AppendAttribute(XXMLAttribute(L"right", rect.right));
	element->AppendAttribute(XXMLAttribute(L"top", rect.top));
	element->AppendAttribute(XXMLAttribute(L"bottom", rect.bottom));

	return element;
}
					

//---------------------------------------------------------------
//
// GetRect
//
//---------------------------------------------------------------
static XRect GetRect(const XXMLElement* element)
{
	XRect rect;
	
	const XXMLAttribute& left = element->FindAttribute(L"left");
	rect.left = left.Get<int32>();

	const XXMLAttribute& right = element->FindAttribute(L"right");
	rect.right = right.Get<int32>();

	const XXMLAttribute& top = element->FindAttribute(L"top");
	rect.top = top.Get<int32>();

	const XXMLAttribute& bottom = element->FindAttribute(L"bottom");
	rect.bottom = bottom.Get<int32>();

	return rect;
}
					
#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// WriteDocInfo
//
// IWriteXML is used to convert bosses to XML. It does this by
// iterating over each interface and checking to see if there is
// a writer function registered for that interface. This is the
// writer function for the IDocInfo interface.
//
//---------------------------------------------------------------
XXMLElement* WriteDocInfo(const XBoss* boss, XBoss::const_iterator iter)
{
	PRECONDITION(boss != nil);
	
	IConstDocInfoPtr info(boss, iter);
	
	XXMLElement* element = new XXMLElement(L"IDocInfo");
	
	element->AppendAttribute(XXMLAttribute(L"pen_size", info->GetPenWidth()));
	element->AppendAttribute(XXMLAttribute(L"color", info->GetColor()));

	uint32 count = info->GetNumRects();
	for (uint32 index = 0; index < count; ++index) {
		XRect rect = info->GetRect(index);
		element->AppendItem(CreateElement(rect));
	}

	return element;
}


//---------------------------------------------------------------
//
// ReadDocInfo
//
// Streaming bosses in from XML works just like writing except 
// IReadXML is used instead of IWriteXML and you can choose to
// read the data in the first pass or after each interface has 
// had a chance to read the XML.
//
//---------------------------------------------------------------
void ReadDocInfo(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(element.GetName() == L"IDocInfo");	// should be caught by the parser
	UNUSED(parent);
	
	if (pass == kLoadingBoss) {
		IDocInfoPtr info(boss, iter);
				
		const XXMLAttribute& width = element.FindAttribute(L"pen_size");
		info->SetPenWidth(width.Get<int32>(), false);

		const XXMLAttribute& color = element.FindAttribute(L"color");
		info->SetColor(color.Get<XRGBColor>(), false);
		
		info->RemoveRects(false);

		XXMLElement::const_item_iterator iter = element.item_begin();
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {
				const std::wstring& name = child->GetName();
				
				if (name == L"Rect") {
					XRect rect = GetRect(child);
					info->AddRect(rect, false);
				
				} else
					DEBUGSTR("Bad element");		// should be caught by the parser
			}
		}
	}
}


