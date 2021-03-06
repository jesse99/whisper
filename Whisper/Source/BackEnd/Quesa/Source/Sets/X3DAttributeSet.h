/*
 *  File:       X3DAttributeSet.h
 *  Summary:	QuickDraw 3D attribute set object.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DAttributeSet.h,v $
 *		Revision 1.3  2000/12/13 08:50:22  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:13:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DAttributeSet.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:50:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <->	  3/06/97	JDJ		Created.
 */

#pragma once

#include <X3DColors.h>
#include <X3DSet.h>
#include <X3DVectors.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DAttributeSet
//!		Holds various 3D attributes.
// ===================================================================================
class QUESA_EXPORT X3DAttributeSet : public X3DSet {

	typedef X3DSet Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DAttributeSet();
						
						X3DAttributeSet();

						X3DAttributeSet(TQ3AttributeSet object);
						
	virtual X3DAttributeSet* Clone() const;
	
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3AttributeSet() const				{return mObject;}
	//@}

	//! @name Standard Attributes
	//@{
			void 		AddSurfaceUV(const TQ3Param2D& data)			{this->Add(kQ3AttributeTypeSurfaceUV, &data);}
			void 		AddSurfaceUV(float u, float v)					{TQ3Param2D data = {u, v}; this->Add(kQ3AttributeTypeSurfaceUV, &data);}

			void 		AddShadingUV(const TQ3Param2D& data)			{this->Add(kQ3AttributeTypeShadingUV, &data);}
			void 		AddShadingUV(float u, float v)					{TQ3Param2D data = {u, v}; this->Add(kQ3AttributeTypeShadingUV, &data);}

			void 		AddNormal(const X3DVector& data)				{this->Add(kQ3AttributeTypeNormal, &data);}

			void 		AddAmbientCoefficient(float data)				{this->Add(kQ3AttributeTypeAmbientCoefficient, &data);}

			void 		AddDiffuseColor(const X3DColorRGB& data)		{this->Add(kQ3AttributeTypeDiffuseColor, &data);}

			void 		AddSpecularColor(const X3DColorRGB& data)		{this->Add(kQ3AttributeTypeSpecularColor, &data);}

			void 		AddSpecularControl(float data)					{this->Add(kQ3AttributeTypeSpecularControl, &data);}

			void 		AddTransparencyColor(const X3DColorRGB& data)	{this->Add(kQ3AttributeTypeTransparencyColor, &data);}

			void 		AddSurfaceTangent(const TQ3Tangent2D& data)		{this->Add(kQ3AttributeTypeSurfaceTangent, &data);}

			void 		AddHighlightState(const TQ3Switch& data)		{this->Add(kQ3AttributeTypeHighlightState, &data);}

			void 		AddSurfaceShader(TQ3SurfaceShaderObject data)	{this->Add(kQ3AttributeTypeSurfaceShader, &data);}
	//@}

	//! @name Submitting (retained mode)
	//@{
			void 		Submit(TQ3ViewObject view);
	//@}

	//! @name Submitting (immediate mode)
	//@{
	static	void 		Submit(TQ3AttributeType type, const void* data, TQ3ViewObject view);

	static	void 		Submit(TQ3AttributeSet attributes, TQ3ViewObject view);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DAttributeSet. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

