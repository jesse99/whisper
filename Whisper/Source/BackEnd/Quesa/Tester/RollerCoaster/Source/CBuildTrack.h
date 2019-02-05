/*
 *  File:       CBuildTrack.h
 *  Summary:	Builds mesh objects representing the track.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CBuildTrack.h,v $
 *		Revision 1.2  2000/11/09 08:52:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 4/18/00	JDJ		Created (from the QD3D 1.6 RollerCoasterSampleMac sample app)
 */

#pragma once

#include <X3DGroups.h>
#include <X3DMatrix.h>
#include <X3DPrimitives.h>

//using namespace Whisper;

namespace Whisper {
	class XXMLElement;
}


//-----------------------------------
//	Constants
//
const int32	MAX_TRACK_SECTIONS =		18;
const int32	MAX_PARTS = 				100;
const int32	MAX_SPLINE_NUBS = 			300L;


// ===================================================================================
//	struct TrackSectionType
// ===================================================================================
struct TrackSectionType {
	int32				partNum;
	Whisper::X3DPoint	nubCoord;
};


// ===================================================================================
//	struct NubEntryType
// ===================================================================================
struct NubEntryType {
	Whisper::X3DPoint	basePt;			// base spline coord 
	Whisper::X3DPoint	upPt;			// point above base 
	int32				sectionNum;		// section # that this nub belongs to 
};


// ===================================================================================
//	struct PartType
// ===================================================================================
struct PartType {
	uint8			numNubs;
	NubEntryType*	coordsPtr;
};


// ===================================================================================
//	class CBuildTrack
// ===================================================================================
class CBuildTrack {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~CBuildTrack();
						
						CBuildTrack();
						
private:
						CBuildTrack(const CBuildTrack& rhs);
						
			CBuildTrack& operator=(const CBuildTrack& rhs);

//-----------------------------------
//	API
//
public:
			int32 		Build(const Whisper::X3DOrderedDisplayGroup& group, NubEntryType* splinesPtr);
			
//-----------------------------------
//	Internal API
//
private:			
			int16 		DoLoadParts();
			void 		DoMakeRandomTrack();
			void 		DoCreateMasterNubList();
			void 		DoCalcSplineCurve();
			void 		DoBuildCoasterGeometry();
			Whisper::X3DMatrix 	DoAmountToRotateNubs(NubEntryType* coordPtr, long numNubsInPart, long sectionNum) const;
									
			void 		DoParsePart(const Whisper::XXMLElement* element, uint32 i);
			void 		DoParseNub(const Whisper::XXMLElement* element, uint32 i, uint32 j);
			
//-----------------------------------
//	Member Data
//
private:
	Whisper::X3DOrderedDisplayGroup	fTrackGroup;		
	int16							fPartCount;

	TrackSectionType				fTrackSectionList[MAX_TRACK_SECTIONS];
	PartType						fPartsList[MAX_PARTS];
	NubEntryType					fNubArray[MAX_SPLINE_NUBS];
	NubEntryType*					fSplinePointsPtr;
	int32							fNumSplineNubs;
	int32							fNumSplinePoints;
};


