/*
 *  File:       IDocInfo.h
 *  Summary:   	Interface that maintains the fractal parameters.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDocInfo.h,v $
 *		Revision 1.4  2001/04/04 04:49:55  jesjones
 *		Added a Reset method.
 *		
 *		Revision 1.3  2001/03/28 09:37:52  jesjones
 *		Added streaming methods to SFractalInfo.
 *		
 *		Revision 1.2  2001/03/25 06:08:10  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:13  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:01:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XPixMap.h>
#include <XUnknown.h>

#include "CHyperComplex.h"


//-----------------------------------
//	Forward References
//
class IFractalFunction;
class ILight;
class IShader;

typedef XInterfacePtr<IFractalFunction> IFractalFunctionPtr;
typedef XInterfacePtr<ILight> ILightPtr;
typedef XInterfacePtr<IShader> IShaderPtr;

typedef XInterfacePtr<const IFractalFunction> IConstFractalFunctionPtr;
typedef XInterfacePtr<const ILight> IConstLightPtr;
typedef XInterfacePtr<const IShader> IConstShaderPtr;

namespace Whisper {
	class XColorTable;
	class XInStream;
	class XOutStream;
}


//-----------------------------------
//	Constants
//
const uint32 kRenderedImage = 'RndI';
const uint32 kResizedImage  = 'RszI';

const XSize kMaxThumbnailSize(128, 128);


// ===================================================================================
//	struct SFractalInfo		 
// ===================================================================================
struct SFractalInfo {
	float			w;					// w is fixed (ie we're plotting a slice through 4D space)
	CHyperComplex	constant;			// the Julia constant
	CHyperComplex	lambda;				// a second constant for the more esoteric fractals	
	float			bailout;
	uint32			maxDwell;
	
				SFractalInfo();
				SFractalInfo(const XXMLElement& element);

	XXMLElement* CreateElement() const;

	bool 		operator==(const SFractalInfo& rhs) const;
	bool 		operator!=(const SFractalInfo& rhs) const		{return !this->operator==(rhs);}

	friend XInStream&  operator>>(XInStream& stream, SFractalInfo& value);
	friend XOutStream& operator<<(XOutStream& stream, const SFractalInfo& value);
};


// ===================================================================================
//	struct SShaderInfo		 
// ===================================================================================
struct SShaderInfo {
	float			ambientCoeff;
	float			diffuseCoeff;
	float			specularCoeff;
	float			specularExp;
	std::wstring	colorFormula;
		
				SShaderInfo();
				SShaderInfo(const XXMLElement& element);

	XXMLElement* CreateElement() const;

	bool 		operator==(const SShaderInfo& rhs) const;
	bool 		operator!=(const SShaderInfo& rhs) const		{return !this->operator==(rhs);}
};


// ===================================================================================
//	struct SResolution		 
// ===================================================================================
struct SResolution {
	int32	width;				// number of horz pixels
	int32	height;				// number of vert pixels
	int32	depth;				// number of points to check between hither and yon planes
		
				SResolution();
				SResolution(const XXMLElement& element);

	XXMLElement* CreateElement() const;

	XSize 		GetSize() const									{return XSize(width, height);}
	bool 		operator==(const SResolution& rhs) const;
	bool 		operator!=(const SResolution& rhs) const		{return !this->operator==(rhs);}
};


// ===================================================================================
//	class IDocInfo
//!		Interface that maintains the fractal parameters.
// ===================================================================================
class IDocInfo : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Fractal Info -----
	virtual const SFractalInfo& GetFractalInfo() const = 0;
			
	virtual void 		SetFractalInfo(const SFractalInfo& info, bool dirtyDoc = false) = 0;
						
	// ----- Shader Info -----
	virtual const SShaderInfo& GetShaderInfo() const = 0;
			
	virtual void 		SetShaderInfo(const SShaderInfo& info, bool dirtyDoc = false) = 0;
						
	// ----- Resolution -----
	virtual const SResolution& GetResolution() const = 0;
			
	virtual void 		SetResolution(const SResolution& resolution, bool dirtyDoc = false) = 0;
						
	// ----- Fractal Function -----
	virtual IConstFractalFunctionPtr GetFractalFunction() const = 0;
			
	virtual void 		SetFractalFunction(const IFractalFunctionPtr& function, bool dirtyDoc = false) = 0;
	virtual void 		SetFractalFunction(const std::wstring& name, bool dirtyDoc = false) = 0;

	// ----- Shader -----
	virtual IConstShaderPtr GetShader() const = 0;
			
	virtual void 		SetShader(const IShaderPtr& shader, bool dirtyDoc = false) = 0;

	// ----- Palette -----
	virtual const XColorTable& GetPalette() const = 0;
	virtual const std::wstring& GetPaletteName() const = 0;
			
	virtual void 		SetPalette(const XColorTable& palette, const std::wstring& name, bool dirtyDoc = false) = 0;

	// ----- Lights -----
	virtual IConstLightPtr GetAmbientLight() const = 0;
	virtual void 		SetAmbientLight(const ILightPtr& light, bool dirtyDoc = false) = 0;

	virtual uint32	 	GetNumLights() const = 0;
	virtual uint32	 	GetNumPointLights() const = 0;
	virtual uint32	 	GetNumDirectionalLights() const = 0;

	virtual IConstLightPtr GetLight(uint32 index) const = 0;
	virtual void 		SetLight(uint32 index, const ILightPtr& light, bool dirtyDoc = false) = 0;
						/**< Point lights first followed by directional lights. */

	// ----- Misc -----
	virtual XPixMapPtr 	CreateThumbnail(const XSize& size) const = 0;

	virtual void 		Reset(const XInterfacePtr<const IDocInfo>& rhs) = 0;
						/**< Copies all of the state in rhs into 'this' (this includes the camera settings). */
};

typedef XInterfacePtr<IDocInfo> IDocInfoPtr;
typedef XInterfacePtr<const IDocInfo> IConstDocInfoPtr;

