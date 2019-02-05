/*
 *  File:       IDocCommands.h
 *  Summary:   	Interface that changes the document's state via undoable commands.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDocCommands.h,v $
 *		Revision 1.6  2001/04/03 04:37:54  jesjones
 *		Added ChangeColorFormula.
 *		
 *		Revision 1.5  2001/04/03 01:04:55  jesjones
 *		Added a bunch of shader related methods.
 *		
 *		Revision 1.4  2001/03/28 09:36:54  jesjones
 *		Added ChangeFormula.
 *		
 *		Revision 1.3  2001/03/27 06:30:21  jesjones
 *		Added camera methods.
 *		
 *		Revision 1.2  2001/03/26 04:25:04  jesjones
 *		Added Orbit, Zoom, and Pan methods.
 *		
 *		Revision 1.1  2001/03/25 06:04:07  jesjones
 *		Checked in
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;


//-----------------------------------
//	Forward References
//
class IShader;

struct SCameraPlacement;
struct SCameraRange;
struct SFractalInfo;
struct SResolution;
class CHyperComplex;
typedef XInterfacePtr<IShader> IShaderPtr;

namespace Whisper {
	class X2DSize;
}


//-----------------------------------
//	Constants
//
const int32 kDefaultRotateBy = 10;
const float kDefaultZoomBy   = 1.2f;
const int32 kDefaultPanBy	 = 10;


// ===================================================================================
//	class IDocCommands
//!		Interface that changes the document's state via undoable commands.
// ===================================================================================
class IDocCommands : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		ChangeBailout(float value) = 0;	
	virtual void 		ChangeW(float value) = 0;
	virtual void 		ChangeConstant(const CHyperComplex& value) = 0;
	virtual void 		ChangeLambda(const CHyperComplex& value) = 0;
	virtual void 		ChangeMaxDwell(int32 value) = 0;
	virtual void 		ChangeFractalInfo(const SFractalInfo& newInfo) = 0;
	virtual void 		ChangeFractalFormula(const std::wstring& formula) = 0;
	
	virtual void 		ChangeResolution(const SResolution& value) = 0;

	virtual void 		OrbitX(int32 units) = 0;
	virtual void 		OrbitY(int32 units) = 0;
	virtual void 		OrbitZ(int32 units) = 0;
	virtual void 		Zoom(bool zoomOut) = 0;
	virtual void 		Pan(int32 horzUnits, int32 vertUnits) = 0;
	
	virtual void 		SetPlacement(const SCameraPlacement& value) = 0;
	virtual void 		SetRange(const SCameraRange& value) = 0;
	virtual void 		SetViewSize(const X2DSize& value) = 0;

	virtual void 		ChangeAmbientCoefficient(float value) = 0;
	virtual void 		ChangeDiffuseCoefficient(float value) = 0;
	virtual void 		ChangeSpecularCoefficient(float value) = 0;
	virtual void 		ChangeSpecularExponent(float value) = 0;
	virtual void 		ChangeShader(const IShaderPtr& shader) = 0;
	virtual void 		ChangeColorFormula(const std::wstring& formula) = 0;
};

typedef XInterfacePtr<IDocCommands> IDocCommandsPtr;
typedef XInterfacePtr<const IDocCommands> IConstDocCommandsPtr;

