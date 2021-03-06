/*
 *  File:       CDocInfo.cpp
 *  Summary:   	Interface that maintains the fractal parameters.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDocInfo.cpp,v $
 *		Revision 1.6  2001/04/21 02:24:01  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/05 03:58:44  jesjones
 *		Lights default to being further away from the origin.
 *		
 *		Revision 1.4  2001/04/04 04:49:41  jesjones
 *		Added a Reset method.
 *		
 *		Revision 1.3  2001/03/28 09:37:33  jesjones
 *		Added streaming methods to SFractalInfo.
 *		
 *		Revision 1.2  2001/03/26 04:32:49  jesjones
 *		SetResolution broadcasts kResizedImage.
 *		
 *		Revision 1.1  2000/12/28 02:49:05  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.5  2000/12/10 04:02:13  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.4  2000/11/26 03:23:34  jesjones
 *		mFunction is no longer a const pointer.
 *		
 *		Revision 1.3  2000/11/25 01:46:49  jesjones
 *		SetFractalFunction no longer directly compares const and non-const interface ptrs.
 *		
 *		Revision 1.2  2000/11/09 08:54:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "IDocInfo.h"

#include <IDocument.h>
#include <IHierarchy.h>
#include <ILoaded.h>
#include <XObjectModel.h>
#include <XPreference.h>
#include <XStreaming.h>
#include <XXMLItems.h>

#include "ICamera.h"
#include "IColorFormulas.h"
#include "IFractalFunction.h"
#include "ILight.h"
#include "IOrthographicCamera.h"
#include "IPalettes.h"
#include "IPointLight.h"
#include "IRenderer.h"
#include "IShader.h"


// ===================================================================================
//	struct SFractalInfo		 
//		$$ Outlined methods will cause problems if we want to support plugins...
// ===================================================================================

//---------------------------------------------------------------
//
// SFractalInfo::SFractalInfo ()
//
//---------------------------------------------------------------
SFractalInfo::SFractalInfo()
{
	w        = -0.36f;	
	constant = CHyperComplex(-0.578f, 0.2728f, 0.1356f, 0.221f);
	lambda   = kZeroHyperComplex;
	bailout  = 16.0f;
	maxDwell = 20;
}


//---------------------------------------------------------------
//
// SFractalInfo::SFractalInfo (XXMLElement)
//
//---------------------------------------------------------------
SFractalInfo::SFractalInfo(const XXMLElement& element)
{
	PRECONDITION(element.GetName() == L"FractalInfo");					// should be caught by the parser
		
	XXMLElement::const_attr_iterator iter = element.attr_begin();
	while (iter != element.attr_end()) {
		const XXMLAttribute& attr = *iter;
		++iter;
		
		const std::wstring& name = attr.GetName();
		
		if (name == L"w")
			attr.Get(w);
		
		else if (name == L"constant")
			attr.Get(constant);
		
		else if (name == L"lambda")
			attr.Get(lambda);
		
		else if (name == L"bailout")
			attr.Get(bailout);
		
		else if (name == L"max_dwell")
			attr.Get(maxDwell);
				
		else
			DEBUGSTR(name, " isn't a valid FractalInfo attribute");	// should be caught by the parser
	}
}


//---------------------------------------------------------------
//
// SFractalInfo::CreateElement
//
//---------------------------------------------------------------
XXMLElement* SFractalInfo::CreateElement() const
{
	XXMLElement* element = new XXMLElement(L"FractalInfo");
	
	element->AppendAttribute(XXMLAttribute(L"w", w));
	element->AppendAttribute(XXMLAttribute(L"constant", constant));
	element->AppendAttribute(XXMLAttribute(L"lambda", lambda));
	element->AppendAttribute(XXMLAttribute(L"bailout", bailout));
	element->AppendAttribute(XXMLAttribute(L"max_dwell", maxDwell));

	return element;
}


//---------------------------------------------------------------
//
// SFractalInfo::operator==
//
//---------------------------------------------------------------
bool SFractalInfo::operator==(const SFractalInfo& rhs) const
{
	bool equal = false;
	
	if (Equal(w, rhs.w))
		if (constant == rhs.constant)
			if (lambda == rhs.lambda)
				if (Equal(bailout, rhs.bailout))
					if (maxDwell == rhs.maxDwell)
						equal = true;
						
	return equal;
}


//---------------------------------------------------------------
//
// operator>> (SFractalInfo)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, SFractalInfo& value)
{
	int32 version;
	stream 	>> version
			>> value.w		
			>> value.constant		
			>> value.lambda
			>> value.bailout
			>> value.maxDwell;
	ASSERT(version == 1);
		
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (SFractalInfo)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const SFractalInfo& value)
{
	int32 version = 1;
	stream 	<< version
			<< value.w		
			<< value.constant		
			<< value.lambda
			<< value.bailout
			<< value.maxDwell;
				
	return stream;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct SShaderInfo
// ===================================================================================

//---------------------------------------------------------------
//
// SShaderInfo::SShaderInfo ()
//
//---------------------------------------------------------------
SShaderInfo::SShaderInfo()
{	
	ambientCoeff  = 0.9f;
	diffuseCoeff  = 0.8f;
	specularCoeff = 0.3f;
	specularExp   = 500.0f;

	IColorFormulasPtr formulas(L"Application");
	colorFormula = formulas->GetDefault();
}


//---------------------------------------------------------------
//
// SShaderInfo::SShaderInfo (XXMLElement)
//
//---------------------------------------------------------------
SShaderInfo::SShaderInfo(const XXMLElement& element)
{
	PRECONDITION(element.GetName() == L"ShaderInfo");						// should be caught by the parser
		
	XXMLElement::const_attr_iterator iter = element.attr_begin();
	while (iter != element.attr_end()) {
		const XXMLAttribute& attr = *iter;
		++iter;
		
		const std::wstring& name = attr.GetName();
		
		if (name == L"ambient")
			attr.Get(ambientCoeff);
		
		else if (name == L"diffuse")
			attr.Get(diffuseCoeff);
		
		else if (name == L"specular")
			attr.Get(specularCoeff);
		
		else if (name == L"specular_exp")
			attr.Get(specularExp);
		
		else if (name == L"formula")
			attr.Get(colorFormula);
				
		else
			DEBUGSTR(name, " isn't a valid ShaderInfo attribute");	// should be caught by the parser
	}
}


//---------------------------------------------------------------
//
// SShaderInfo::CreateElement
//
//---------------------------------------------------------------
XXMLElement* SShaderInfo::CreateElement() const
{
	XXMLElement* element = new XXMLElement(L"ShaderInfo");
	
	element->AppendAttribute(XXMLAttribute(L"ambient", ambientCoeff));
	element->AppendAttribute(XXMLAttribute(L"diffuse", diffuseCoeff));
	element->AppendAttribute(XXMLAttribute(L"specular", specularCoeff));
	element->AppendAttribute(XXMLAttribute(L"specular_exp", specularExp));
	element->AppendAttribute(XXMLAttribute(L"formula", colorFormula));

	return element;
}


//---------------------------------------------------------------
//
// SShaderInfo::operator==
//
//---------------------------------------------------------------
bool SShaderInfo::operator==(const SShaderInfo& rhs) const
{
	bool equal = false;
	
	if (Equal(ambientCoeff, rhs.ambientCoeff))
		if (Equal(diffuseCoeff, rhs.diffuseCoeff))
			if (Equal(specularCoeff, rhs.specularCoeff))
				if (Equal(specularExp, rhs.specularExp))
 					if (colorFormula == rhs.colorFormula)
						equal = true;
						
	return equal;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct SResolution		 
// ===================================================================================

//---------------------------------------------------------------
//
// SResolution::SResolution ()
//
//---------------------------------------------------------------
SResolution::SResolution()
{	
	XPreference<int32> defaultHeight(L"Default Height", 256);
	XPreference<int32> defaultWidth(L"Default Width", 256);

	width  = *defaultWidth;
	height = *defaultHeight;
	depth  = 32;
}


//---------------------------------------------------------------
//
// SResolution::SResolution (XXMLElement)
//
//---------------------------------------------------------------
SResolution::SResolution(const XXMLElement& element)
{
	PRECONDITION(element.GetName() == L"Resolution");						// should be caught by the parser
		
	XXMLElement::const_attr_iterator iter = element.attr_begin();
	while (iter != element.attr_end()) {
		const XXMLAttribute& attr = *iter;
		++iter;
		
		const std::wstring& name = attr.GetName();
		
		if (name == L"width")
			attr.Get(width);
		
		else if (name == L"height")
			attr.Get(height);
		
		else if (name == L"depth")
			attr.Get(depth);
				
		else
			DEBUGSTR(name, " isn't a valid Resolution attribute");	// should be caught by the parser
	}
}


//---------------------------------------------------------------
//
// SResolution::CreateElement
//
//---------------------------------------------------------------
XXMLElement* SResolution::CreateElement() const
{
	XXMLElement* element = new XXMLElement(L"Resolution");
	
	element->AppendAttribute(XXMLAttribute(L"width", width));
	element->AppendAttribute(XXMLAttribute(L"height", height));
	element->AppendAttribute(XXMLAttribute(L"depth", depth));

	return element;
}


//---------------------------------------------------------------
//
// SResolution::operator==
//
//---------------------------------------------------------------
bool SResolution::operator==(const SResolution& rhs) const
{
	bool equal = false;
	
	if (width == rhs.width)
		if (height == rhs.height)
			if (depth == rhs.depth)
					equal = true;
						
	return equal;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CDocInfo
//!		Interface that maintains the fractal parameters.
// ===================================================================================
class CDocInfo : public IDocInfo {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDocInfo();
	
						CDocInfo(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual const SFractalInfo& GetFractalInfo() const					{return mFractalInfo;}		
	virtual void 		SetFractalInfo(const SFractalInfo& info, bool dirtyDoc = false);
						
	virtual const SShaderInfo& GetShaderInfo() const					{return mShaderInfo;}	
	virtual void 		SetShaderInfo(const SShaderInfo& info, bool dirtyDoc = false);
						
	virtual const SResolution& GetResolution() const					{return mResolution;}		
	virtual void 		SetResolution(const SResolution& resolution, bool dirtyDoc = false);
						
	virtual IConstFractalFunctionPtr GetFractalFunction() const			{return mFunction;}		
	virtual void 		SetFractalFunction(const IFractalFunctionPtr& function, bool dirtyDoc = false);
	virtual void 		SetFractalFunction(const std::wstring& name, bool dirtyDoc = false);

	virtual IConstShaderPtr GetShader() const							{return mShader;}	
	virtual void 		SetShader(const IShaderPtr& shader, bool dirtyDoc = false);

	virtual const XColorTable& GetPalette() const						{return mPalette;}
	virtual const std::wstring& GetPaletteName() const					{return mPaletteName;}		
	virtual void 		SetPalette(const XColorTable& palette, const std::wstring& name, bool dirtyDoc = false);

	virtual IConstLightPtr GetAmbientLight() const						{return mAmbientLight;}
	virtual void 		SetAmbientLight(const ILightPtr& light, bool dirtyDoc = false);

	virtual uint32	 	GetNumLights() const							{return mLights.size();}
	virtual uint32	 	GetNumPointLights() const 						{return mLights.size() - 1;}
	virtual uint32	 	GetNumDirectionalLights() const					{return 1;}

	virtual IConstLightPtr GetLight(uint32 i) const						{PRECONDITION(i < mLights.size()); return mLights[i];}
	virtual void 		SetLight(uint32 index, const ILightPtr& light, bool dirtyDoc = false);

	virtual XPixMapPtr 	CreateThumbnail(const XSize& size) const;
	virtual void 		Reset(const XInterfacePtr<const IDocInfo>& rhs);
	
//-----------------------------------
//	Internal Types
//
private:
	typedef std::vector<ILightPtr> Lights;
	
//-----------------------------------
//	Member Data
//
private:
	SFractalInfo		mFractalInfo;
	SShaderInfo			mShaderInfo;
	SResolution			mResolution;
	IFractalFunctionPtr mFunction;
	IShaderPtr			mShader;
	
	XColorTable			mPalette;
	std::wstring		mPaletteName;
	
	ILightPtr			mAmbientLight;
	Lights				mLights;
};

DEFINE_INTERFACE_FACTORY(CDocInfo)

//---------------------------------------------------------------
//
// CDocInfo::~CDocInfo
//
//---------------------------------------------------------------
CDocInfo::~CDocInfo()
{
}


//---------------------------------------------------------------
//
// CDocInfo::CDocInfo
//
//---------------------------------------------------------------
CDocInfo::CDocInfo(XBoss* boss)
{	
	IDocInfo::DoSetBoss(boss);
		
	IPalettesPtr palettes(L"Application");
	mPalette = palettes->GetDefault();
	mPaletteName = palettes->GetDefaultName();
	
	this->SetFractalFunction(L"q^2 + c");
						
	mShader = IShaderPtr(L"Phong Shader");

	mAmbientLight = ILightPtr(L"Ambient Light");
	
	IPointLightPtr light(L"Point Light");
	light->SetLocation(X3DPoint(10.0f, -20.0f, 20.0f));
	mLights.push_back(ILightPtr(light));
			
	light = IPointLightPtr(L"Point Light");
	light->SetLocation(X3DPoint(10.0f, 20.0f, 20.0f));
	mLights.push_back(ILightPtr(light));
	mLights.back()->TurnOn(false);
			
	light = IPointLightPtr(L"Point Light");
	light->SetLocation(X3DPoint(-10.0f, 20.0f, 20.0f));
	mLights.push_back(ILightPtr(light));
	mLights.back()->TurnOn(false);
			
	light = IPointLightPtr(L"Point Light");
	light->SetLocation(X3DPoint(-10.0f, -20.0f, 20.0f));
	mLights.push_back(ILightPtr(light));
	mLights.back()->TurnOn(false);
			
	mLights.push_back(ILightPtr(L"Directional Light"));
	mLights.back()->TurnOn(false);
}


//---------------------------------------------------------------
//
// CDocInfo::Reset
//
//---------------------------------------------------------------
void CDocInfo::Reset(const XInterfacePtr<const IDocInfo>& rhs)
{
	mFractalInfo = rhs->GetFractalInfo();
	mShaderInfo = rhs->GetShaderInfo();
	mResolution = rhs->GetResolution();
	mShader = IShaderPtr(rhs->GetShader()->GetBoss()->GetName());

	this->SetFractalFunction(rhs->GetFractalFunction()->GetFormula(), false);
			
	mPalette = rhs->GetPalette();
	mPaletteName = rhs->GetPaletteName();
			
	mAmbientLight = rhs->GetAmbientLight()->Clone();	
	for (uint32 i = 0; i < rhs->GetNumLights(); ++i)
		mLights[i] = rhs->GetLight(i)->Clone();
	
	ICameraPtr lhsCamera(this);
	IConstCameraPtr rhsCamera(rhs);
	lhsCamera->SetPlacement(rhsCamera->GetPlacement(), true, false);
	lhsCamera->SetRange(rhsCamera->GetRange(), false);
	
	IOrthographicCameraPtr lhsOrtho(this);
	IConstOrthographicCameraPtr rhsOrtho(rhs);
	lhsOrtho->SetViewSize(rhsOrtho->GetViewSize(), false);
}


//---------------------------------------------------------------
//
// CDocInfo::SetFractalInfo
//
//---------------------------------------------------------------
void CDocInfo::SetFractalInfo(const SFractalInfo& info, bool dirtyDoc)		
{
	if (info != mFractalInfo) {
		mFractalInfo = info;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}
						

//---------------------------------------------------------------
//
// CDocInfo::SetShaderInfo
//
//---------------------------------------------------------------
void CDocInfo::SetShaderInfo(const SShaderInfo& info, bool dirtyDoc)			
{
	if (info != mShaderInfo) {
		mShaderInfo = info;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}
						

//---------------------------------------------------------------
//
// CDocInfo::SetResolution
//
//---------------------------------------------------------------
void CDocInfo::SetResolution(const SResolution& resolution, bool dirtyDoc)	
{
	if (resolution != mResolution) {
		mResolution = resolution;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty(true, kResizedImage);
		}
	}
}
						

//---------------------------------------------------------------
//
// CDocInfo::SetFractalFunction (IFractalFunctionPtr, bool)
//
//---------------------------------------------------------------
void CDocInfo::SetFractalFunction(const IFractalFunctionPtr& function, bool dirtyDoc)	
{	
	PRECONDITION(function);
	
	if (mFunction != function) {
		mFunction = function;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}


//---------------------------------------------------------------
//
// CDocInfo::SetFractalFunction (wstring, bool)
//
//---------------------------------------------------------------
void CDocInfo::SetFractalFunction(const std::wstring& name, bool dirtyDoc)
{
	IFractalFunctionPtr function;

	XBoss* boss = XObjectModel::Instance()->CreateBoss(L"Fractal Functions");	// singleton boss
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !function) {
		IFractalFunctionPtr candidate(boss, iter);
		++iter;

		if (candidate && candidate->GetFormula() == name)
			function = candidate;
	}
			
	this->SetFractalFunction(function, dirtyDoc);
}


//---------------------------------------------------------------
//
// CDocInfo::SetLight
//
//---------------------------------------------------------------
void CDocInfo::SetLight(uint32 index, const ILightPtr& light, bool dirtyDoc)
{
	PRECONDITION(index < mLights.size());
	
	if (light != mLights[index]) {
		mLights[index] = light;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}


//---------------------------------------------------------------
//
// CDocInfo::SetShader
//
//---------------------------------------------------------------
void CDocInfo::SetShader(const IShaderPtr& shader, bool dirtyDoc)				
{
	if (shader != mShader) {
		mShader = shader;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}


//---------------------------------------------------------------
//
// CDocInfo::SetPalette
//
//---------------------------------------------------------------
void CDocInfo::SetPalette(const XColorTable& palette, const std::wstring& name, bool dirtyDoc)	
{
	if (palette != mPalette || name != mPaletteName) {
		mPalette = palette; 
		mPaletteName = name;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}


//---------------------------------------------------------------
//
// CDocInfo::SetAmbientLight
//
//---------------------------------------------------------------
void CDocInfo::SetAmbientLight(const ILightPtr& light, bool dirtyDoc)			
{
	if (light != mAmbientLight) {
		mAmbientLight = light;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}


//---------------------------------------------------------------
//
// CDocInfo::CreateThumbnail
//
//---------------------------------------------------------------
XPixMapPtr CDocInfo::CreateThumbnail(const XSize& thumbnailSize) const
{
	IHierarchyPtr parent(const_cast<CDocInfo*>(this));

	// Map the image rect to the thumbnail size (so we preserve the
	// image's aspect ratio).
	XRect imageArea(0, 0, mResolution.width, mResolution.height);
	XRect thumbnailArea(0, 0, thumbnailSize.width, thumbnailSize.height);
	imageArea.MapTo(thumbnailArea);
	
	// Create a render object
	IRendererPtr renderer(L"Renderer");
	parent->AddChild(IHierarchyPtr(renderer));
	
	ILoadedPtr loaded(renderer);
	loaded->OnLoaded();
	
	renderer->SetResolution(imageArea.GetSize(), 16);
	
	// Render the fractal
	while (!renderer->IsDone())
		(void) renderer->Render(500);
	
	// Clone the pixmap
	XPixMapPtr image(renderer->GetImage()->Clone());
	
	// Cleanup
	parent->RemoveChild(IHierarchyPtr(renderer));
	
	return image;
}

