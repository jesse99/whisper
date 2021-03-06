/*
 *  File:       CPersistXML.cpp
 *  Summary:   	Functions used to stream the fractal out and in as XML. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPersistXML.cpp,v $
 *		Revision 1.5  2001/04/21 02:24:37  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/02 11:22:10  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.3  2001/02/09 04:29:48  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2000/12/31 04:18:23  jesjones
 *		Fixed some comments.
 *		
 *		Revision 1.1  2000/12/28 02:50:13  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/12/17 03:43:39  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.2  2000/11/09 08:56:07  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CPersistXML.h"

#include <XColorTable.h>
#include <XFloatConversions.h>
#include <XStringUtils.h>
#include <XXMLItems.h>

#include "ICamera.h"
#include "IDirectionalLight.h"
#include "IDocInfo.h"
#include "IFractalFunction.h"
#include "ILight.h"
#include "IOrthographicCamera.h"
#include "IPointLight.h"
#include "IShader.h"


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// FromStr (wstring, ELightAttenuation)
//
//---------------------------------------------------------------
static void FromStr(const std::wstring& str, ELightAttenuation& value)
{	
	if (str == L"none")
		value = kNoAttenuation;
	
	else if (str == L"inverse")
		value = kInverseDistanceAttenuation;
	
	else if (str == L"inverse-squared")
		value = kInverseDistanceSqrAttenuation;
		
	else
		DEBUGSTR("Bad ELightAttenuation!");
}


//---------------------------------------------------------------
//
// ToStr (ELightAttenuation)
//
//---------------------------------------------------------------
static std::wstring ToStr(const ELightAttenuation& value)
{
	std::wstring str;
	
	switch (value) {
		case kNoAttenuation:
			str = L"none";
			break;
				  
		case kInverseDistanceAttenuation:
			str = L"inverse";
			break;
				  
		case kInverseDistanceSqrAttenuation:
			str = L"inverse-squared";
			break;
			
		default:
			DEBUGSTR("Bad ELightAttenuation!");
	}
	
	return str;
}


//---------------------------------------------------------------
//
// CreateElement (IConstLightPtr)
//
//---------------------------------------------------------------
static XXMLElement* CreateElement(const IConstLightPtr& light)
{
	XXMLElement* element = new XXMLElement(L"Light");
	
	element->AppendAttribute(XXMLAttribute(L"on", light->IsOn()));
	element->AppendAttribute(XXMLAttribute(L"brightness", light->GetBrightness()));
	element->AppendAttribute(XXMLAttribute(L"color", light->GetColor()));

	return element;
}


//---------------------------------------------------------------
//
// CreateElement (IConstPointLightPtr)
//
//---------------------------------------------------------------
static XXMLElement* CreateElement(const IConstPointLightPtr& light)
{
	XXMLElement* element = new XXMLElement(L"PointLight");
	
	element->AppendAttribute(XXMLAttribute(L"loc", light->GetLocation()));
	element->AppendAttribute(XXMLAttribute(L"attenuation", light->GetAttenuation()));

	element->AppendItem(CreateElement(IConstLightPtr(light)));

	return element;
}


//---------------------------------------------------------------
//
// CreateElement (IConstDirectionalLightPtr)
//
//---------------------------------------------------------------
static XXMLElement* CreateElement(const IConstDirectionalLightPtr& light)
{
	XXMLElement* element = new XXMLElement(L"DirectionalLight");
	
	element->AppendAttribute(XXMLAttribute(L"direction", X3DPoint(light->GetDirection())));

	element->AppendItem(CreateElement(IConstLightPtr(light)));

	return element;
}


//---------------------------------------------------------------
//
// InitLight
//
//---------------------------------------------------------------
static void InitLight(ILightPtr light, const XXMLElement* element)
{
	const XXMLAttribute& o = element->FindAttribute(L"on");
	const XXMLAttribute& b = element->FindAttribute(L"brightness");
	const XXMLAttribute& c = element->FindAttribute(L"color");
	
	light->TurnOn(o.Get<bool>());
	light->SetBrightness(b.Get<float>());
	light->SetColor(c.Get<XRGBColor>());
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
//---------------------------------------------------------------
XXMLElement* WriteDocInfo(const XBoss* boss, XBoss::const_iterator iter)
{
	PRECONDITION(boss != nil);
	
	IConstDocInfoPtr info(boss, iter);
	
	XXMLElement* element = new XXMLElement(L"IDocInfo");
	
	element->AppendItem(info->GetFractalInfo().CreateElement());
	element->AppendItem(info->GetShaderInfo().CreateElement());
	element->AppendItem(info->GetResolution().CreateElement());
	
	XXMLElement* e = info->GetPalette().CreateElement();
	e->AppendAttribute(XXMLAttribute(L"name", FormatAsXML(info->GetPaletteName())));
	element->AppendItem(e);
						
	IConstFractalFunctionPtr function = info->GetFractalFunction();
	element->AppendAttribute(XXMLAttribute(L"function", function->GetFormula()));
			
	IConstShaderPtr shader = info->GetShader();
	element->AppendAttribute(XXMLAttribute(L"shader", shader->GetBoss()->GetName()));
			
	element->AppendItem(CreateElement(info->GetAmbientLight()));
	
	uint32 count = info->GetNumPointLights();
	for (uint32 index = 0; index < count; ++index) {
		IConstPointLightPtr light = info->GetLight(index);
		element->AppendItem(CreateElement(light));
	}
	
	for (uint32 index = 0; index < info->GetNumDirectionalLights(); ++index) {
		IConstDirectionalLightPtr light = info->GetLight(index + count);
		element->AppendItem(CreateElement(light));
	}

	return element;
}


//---------------------------------------------------------------
//
// WriteCamera
//
//---------------------------------------------------------------
XXMLElement* WriteCamera(const XBoss* boss, XBoss::const_iterator iter)
{
	PRECONDITION(boss != nil);
	
	IConstCameraPtr camera(boss, iter);
	
	XXMLElement* element = new XXMLElement(L"ICamera");
	
	element->AppendItem(camera->GetPlacement().CreateElement());
	element->AppendItem(camera->GetRange().CreateElement());

	return element;
}


//---------------------------------------------------------------
//
// WriteOrthoCamera
//
//---------------------------------------------------------------
XXMLElement* WriteOrthoCamera(const XBoss* boss, XBoss::const_iterator iter)
{
	PRECONDITION(boss != nil);
	
	IConstOrthographicCameraPtr camera(boss, iter);
	
	XXMLElement* element = new XXMLElement(L"IOrthographicCamera");
	
	element->AppendAttribute(XXMLAttribute(L"size", camera->GetViewSize()));

	return element;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// ReadDocInfo
//
//---------------------------------------------------------------
void ReadDocInfo(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(element.GetName() == L"IDocInfo");	// should be caught by the parser
	UNUSED(parent);
	
	if (pass == kLoadingBoss) {
		IDocInfoPtr info(boss, iter);
				
		const XXMLAttribute& function = element.FindAttribute(L"function");
		info->SetFractalFunction(function.GetValue());

		const XXMLAttribute& name = element.FindAttribute(L"shader");
		IShaderPtr shader(name.GetValue());
		info->SetShader(shader);
		
		uint32 numPtLights = 0;
		uint32 numDirLights = 0;

		XXMLElement::const_item_iterator iter = element.item_begin();
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {
				const std::wstring& name = child->GetName();
				
				if (name == L"FractalInfo") {
					SFractalInfo fInfo(*child);
					info->SetFractalInfo(fInfo);
				
				} else if (name == L"ShaderInfo") {
					SShaderInfo sInfo(*child);
					info->SetShaderInfo(sInfo);
				
				} else if (name == L"Resolution") {
					SResolution rInfo(*child);
					info->SetResolution(rInfo);
					
				} else if (name == L"Palette") {
					const XXMLAttribute& pn = child->FindAttribute(L"name");
					
					XColorTable colors(*child);
					info->SetPalette(colors, pn.GetValue());
					
				} else if (name == L"Light") {
					ILightPtr light(L"Ambient Light");
					InitLight(light, child);
					
					info->SetAmbientLight(light);

				} else if (name == L"PointLight") {
					ILightPtr light(L"Point Light");

					const XXMLElement* c = child->FindElement(L"Light");
					InitLight(light, c);
					
					IPointLightPtr pl(light);

					const XXMLAttribute& l = child->FindAttribute(L"loc");
					const XXMLAttribute& a = child->FindAttribute(L"attenuation");
					
					pl->SetLocation(l.Get<X3DPoint>());
					pl->SeAttenuation(a.Get<ELightAttenuation>());

					if (numPtLights+1 < info->GetNumPointLights())
						info->SetLight(numPtLights, light);
					++numPtLights;
					
				} else if (name == L"DirectionalLight") {
					ILightPtr light(L"Directional Light");

					const XXMLElement* c = child->FindElement(L"Light");
					InitLight(light, c);
					
					IDirectionalLightPtr dl(light);

					const XXMLAttribute& d = child->FindAttribute(L"direction");
					
					dl->SetDirection(d.Get<X3DVector>());

					if (numDirLights+1 < info->GetNumDirectionalLights())
						info->SetLight(info->GetNumPointLights() + numDirLights, light);
					++numDirLights;
					
				} else
					DEBUGSTR(name, " isn't a legal IDocInfo element!");	// should be caught by the parser
			}
		}
		
		ASSERT(numPtLights == info->GetNumPointLights());			// $$ really should handle this better
		ASSERT(numDirLights == info->GetNumDirectionalLights());	
	}
}


//---------------------------------------------------------------
//
// ReadCamera
//
//---------------------------------------------------------------
void ReadCamera(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(element.GetName() == L"ICamera");	// should be caught by the parser
	UNUSED(parent);
	
	if (pass == kLoadingBoss) {
		ICameraPtr camera(boss, iter);
				
		XXMLElement::const_item_iterator iter = element.item_begin();
		while (iter != element.item_end()) {
			const XXMLItem* item = *iter;
			++iter;
			
			if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {
				const std::wstring& name = child->GetName();
				
				if (name == L"Placement") {
					SCameraPlacement p(*child);
					camera->SetPlacement(p);
										
				} else if (name == L"Range") {
					SCameraRange r(*child);
					camera->SetRange(r);
					
				} else
					DEBUGSTR(name, " isn't a legal ICamera element!");	// should be caught by the parser
			}
		}
	}
}


//---------------------------------------------------------------
//
// ReadOrthoCamera
//
//---------------------------------------------------------------
void ReadOrthoCamera(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(element.GetName() == L"IOrthographicCamera");	// should be caught by the parser
	UNUSED(parent);
	
	if (pass == kLoadingBoss) {
		IOrthographicCameraPtr camera(boss, iter);
				
		const XXMLAttribute& size = element.FindAttribute(L"size");
		camera->SetViewSize(size.Get<X2DSize>());
	}
}

