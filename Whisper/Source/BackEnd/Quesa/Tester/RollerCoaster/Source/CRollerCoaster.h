/*
 *  File:       CRollerCoaster.h
 *  Summary:	Moves a camera along a roller coaster.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRollerCoaster.h,v $
 *		Revision 1.2  2000/11/09 08:57:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/17/00	JDJ		Created (from the QD3D 1.6 RollerCoasterSampleMac sample app)
 */

#pragma once

#include <X3DStyles.h>
#include <X3DView.h>

#include "CBuildTrack.h"


// ===================================================================================
//	class CRollerCoaster
// ===================================================================================
class CRollerCoaster {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~CRollerCoaster();
						
						CRollerCoaster();
						
private:
						CRollerCoaster(const CRollerCoaster& rhs);
						
			CRollerCoaster&	operator=(const CRollerCoaster& rhs);

//-----------------------------------
//	API
//
public:
			void 		Update();
			
			void 		Invalidate();
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoInitView();
			void 		DoInitGround();
									
			void 		DoMoveCamera();
			void 		DoPutCameraOnTrack();
			
#if WIN
			void 		DoRegisterWindow();

	static LRESULT CALLBACK DoWindowProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam);  
#endif

//-----------------------------------
//	Member Data
//
private:
	Whisper::OSWindow				fWindow;			
	Whisper::X3DView				fView;				// the view for the scene 
	Whisper::X3DOrderedDisplayGroup	fTrackGroup;		// object in the scene being modelled 
	Whisper::X3DOrderedDisplayGroup	fGroundGroup;		// object in the scene being modelled 
	Whisper::X3DInterpolationStyle	fInterpolation;		// interpolation style used when rendering 
	Whisper::X3DBackfacingStyle		fBackFacing;		// whether to draw shapes that face away from the camera 
	Whisper::X3DFillStyle			fFillStyle;			// whether drawn as solid filled object or decomposed to components 

	NubEntryType*					fSplinePointsPtr;
	int32							fNumSplinePoints;
	int32 							fTrackIndex;
	
#if WIN
	static bool 					fsRegisteredWindow;
#endif
};


