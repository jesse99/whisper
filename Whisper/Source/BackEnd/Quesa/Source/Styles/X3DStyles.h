/*
 *  File:       X3DStyles.h
 *  Summary:	QuickDraw 3D style objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DSubdivisionStyle		- Controls how the renderer decomposes curves and
 *										  surfaces into polylines and meshes.
 *				X3DPickIDStyle			- Allows you to attach an arbitrary pick ID to
 *										  a geometric object.
 *				X3DPickPartsStyle		- Determines what kinds of objects you want to
 *										  receive in a pick list.
 *				X3DReceiveShadowsStyle	- Determines whether objects receive shadows
 *										  cast by other objects in the scene.
 *				X3DFillStyle			- Determines whether objects are drawn as solids
 *										  or as edges and points.
 *				X3DBackfacingStyle		- Determines whether or not the renderer draws
 *										  polygons that face away from the camera.
 *				X3DInterpolationStyle	- Determines how the renderer applies shading
 *										  effects (eg lighting) to a surface.
 *				X3DHighlightStyle		- Overrides the normal material attributes of
 *										  a geometric object.
 *				X3DOrientationStyle		- Determines which side of a planar surface is
 *										  considered the front.
 *				X3DAntiAliasStyle		- $$$$ ????
 *				X3DFogStyle				- Objects that are far away from the camera will
 *										  be progressively fogged out.
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStyles.h,v $
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/13 08:54:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	  4/19/00	JDJ		Added X3DFogStyle.
 *		 <1>	  2/18/97	JDJ		Created.
 */

#pragma once

#include <X3DStyle.h>
#include <XCountedPtr.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DSubdivisionStyle
//!		Controls how the renderer decomposes curves and surfaces into polylines and meshes.
// ===================================================================================
class QUESA_EXPORT X3DSubdivisionStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DSubdivisionStyle();
						
						X3DSubdivisionStyle(TQ3SubdivisionMethod method, float c1, float c2);
						/**< Method can be kQ3SubdivisionMethodConstant, kQ3SubdivisionMethodWorldSpace, 
						or kQ3SubdivisionMethodScreenSpace. */

						X3DSubdivisionStyle(const TQ3SubdivisionStyleData& data);

						X3DSubdivisionStyle(TQ3StyleObject object);
						
	virtual X3DSubdivisionStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3SubdivisionMethod method, float c1, float c2, TQ3ViewObject view);

	static	void 		Submit(const TQ3SubdivisionStyleData& data, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3SubdivisionStyleData GetData() const;

			void 		SetData(TQ3SubdivisionMethod method, float c1, float c2);

			void 		SetData(const TQ3SubdivisionStyleData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DSubdivisionStyle. */
	//@}
};


// ===================================================================================
//	class X3DPickIDStyle
//!		Allows you to attach an arbitrary pick ID to a geometric object.
// ===================================================================================
class QUESA_EXPORT X3DPickIDStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPickIDStyle();
						
						X3DPickIDStyle(uint32 id);

						X3DPickIDStyle(TQ3StyleObject object);
						
	virtual X3DPickIDStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(uint32 id, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			uint32 		Get() const;

			void 		Set(uint32 id);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPickIDStyle. */
	//@}
};


// ===================================================================================
//	class X3DPickPartsStyle
//!		Determines what kinds of objects you want to receive in a pick list.
// ===================================================================================
class QUESA_EXPORT X3DPickPartsStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPickPartsStyle();
						
						X3DPickPartsStyle(TQ3PickParts parts);
						/**< Parts can be kQ3PickPartsObject, kQ3PickPartsMaskFace, kQ3PickPartsMaskEdge, 
						or kQ3PickPartsMaskVertex. */

						X3DPickPartsStyle(TQ3StyleObject object);
						
	virtual X3DPickPartsStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3PickParts parts, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3PickParts Get() const;

			void 		Set(TQ3PickParts parts);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPickPartsStyle. */
	//@}
};


// ===================================================================================
//	class X3DReceiveShadowsStyle
//!		Determines whether objects receive shadows cast by other objects in the scene.
// ===================================================================================
class QUESA_EXPORT X3DReceiveShadowsStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DReceiveShadowsStyle();
						
						X3DReceiveShadowsStyle(bool receives);

						X3DReceiveShadowsStyle(TQ3StyleObject object);
						
	virtual X3DReceiveShadowsStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(bool receives, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			bool 		Get() const;

			void 		Set(bool receives);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DReceiveShadowsStyle. */
	//@}
};


// ===================================================================================
//	class X3DFillStyle
//!		Determines whether objects are drawn as solids or as edges and points.
// ===================================================================================
class QUESA_EXPORT X3DFillStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DFillStyle();
						
						X3DFillStyle(TQ3FillStyle style);
						/**< Style can be kQ3FillStyleFilled, kQ3FillStyleEdges, or kQ3FillStylePoints. */

						X3DFillStyle(TQ3StyleObject object);
						
	virtual X3DFillStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3FillStyle style, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3FillStyle Get() const;

			void 		Set(TQ3FillStyle style);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DFillStyle. */
}	//@}
;


// ===================================================================================
//	class X3DBackfacingStyle
//!		Determines whether or not the renderer draws polygons that face away from the camera.
// ===================================================================================
class QUESA_EXPORT X3DBackfacingStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DBackfacingStyle();
						
						X3DBackfacingStyle(TQ3BackfacingStyle style);
						/**< Style can be kQ3BackfacingStyleBoth, kQ3BackfacingStyleRemove, 
						or kQ3BackfacingStyleFlip. */

						X3DBackfacingStyle(TQ3StyleObject object);
						
	virtual X3DBackfacingStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3BackfacingStyle style, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3BackfacingStyle Get() const;

			void 		Set(TQ3BackfacingStyle style);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DBackfacingStyle. */
	//@}
};


// ===================================================================================
//	class X3DInterpolationStyle
//!		Determines how the renderer applies shading effects (eg lighting) to a surface.
// ===================================================================================
class QUESA_EXPORT X3DInterpolationStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DInterpolationStyle();
						
						X3DInterpolationStyle(TQ3InterpolationStyle style);
						/**< Style can be kQ3InterpolationStyleNone, kQ3InterpolationStyleVertex, 
						or kQ3InterpolationStylePixel. */

						X3DInterpolationStyle(TQ3StyleObject object);
						
	virtual X3DInterpolationStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3InterpolationStyle style, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3InterpolationStyle Get() const;

			void 		Set(TQ3InterpolationStyle style);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DInterpolationStyle. */
	//@}
};


// ===================================================================================
//	class X3DHighlightStyle
//!		Overrides the normal material attributes of a geometric object.
// ===================================================================================
class QUESA_EXPORT X3DHighlightStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DHighlightStyle();
						
						X3DHighlightStyle(TQ3AttributeSet attributes);

						X3DHighlightStyle(TQ3StyleObject object, bool dummy);
						/**< TQ3AttributeSet and TQ3StyleObject have the same type... */
						
	virtual X3DHighlightStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3AttributeSet attributes, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3AttributeSet Get() const;

			void 		Set(TQ3AttributeSet attributes);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DHighlightStyle. */
	//@}
};


// ===================================================================================
//	class X3DOrientationStyle
//!		Determines which side of a planar surface is considered the front.
// ===================================================================================
class QUESA_EXPORT X3DOrientationStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DOrientationStyle();
						
						X3DOrientationStyle(TQ3OrientationStyle direction);
						/**< Direction can be kQ3OrientationStyleCounterClockwise or
						kQ3OrientationStyleClockwise. */

						X3DOrientationStyle(TQ3StyleObject object);
						
	virtual X3DOrientationStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(TQ3OrientationStyle direction, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3OrientationStyle Get() const;

			void 		Set(TQ3OrientationStyle direction);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DOrientationStyle. */
	//@}
};


// ===================================================================================
//	class X3DAntiAliasStyle
//!		Enables and disabled anti-aliasing.
// ===================================================================================
class QUESA_EXPORT X3DAntiAliasStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DAntiAliasStyle();
						
						X3DAntiAliasStyle(const TQ3AntiAliasStyleData& data);

						X3DAntiAliasStyle(TQ3StyleObject object);
						
	virtual X3DAntiAliasStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3AntiAliasStyleData& data, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3AntiAliasStyleData GetData() const;

			void 		SetData(const TQ3AntiAliasStyleData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DAntiAliasStyle. */
	//@}
};


// ===================================================================================
//	class X3DFogStyle
//!		Objects that are far away from the camera will be progressively fogged out.
// ===================================================================================
class QUESA_EXPORT X3DFogStyle : public X3DStyle {

	typedef X3DStyle Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DFogStyle();
						
						X3DFogStyle(const TQ3FogStyleData& data);
						/**< Requires QD3D 1.6 */

						X3DFogStyle(TQ3StyleObject object);
						
	virtual X3DFogStyle* Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Submitting (Retained Mode)
	//@{
			void 		Submit(TQ3ViewObject view) const			{Inherited::Submit(view);}
	//@}

	//! @name Submitting (Immediate Mode)
	//@{
	static	void 		Submit(const TQ3FogStyleData& data, TQ3ViewObject view);
	//@}

	//! @name Data
	//@{
			TQ3FogStyleData Get() const;

			void 		Set(const TQ3FogStyleData& data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DFogStyle. */
	//@}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
