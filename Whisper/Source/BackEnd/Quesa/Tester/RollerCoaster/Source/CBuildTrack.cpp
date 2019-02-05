/*
 *  File:       CBuildTrack.cpp
 *  Summary:	Builds mesh objects representing the track.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CBuildTrack.cpp,v $
 *		Revision 1.5  2001/04/21 02:23:08  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.4  2001/03/17 08:54:50  jesjones
 *		Added std:: in a few places.
 *		
 *		Revision 1.3  2000/12/10 04:02:05  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 08:52:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include "CBuildTrack.h"

#include <X3DMesh.h>
#include <X3DVertexList.h>
#include <XFileSpec.h>
#include <XFloatConversions.h>
#include <XFolderSpec.h>
#include <XNumbers.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XXMLAttribute.h>
#include <XXMLCallbacks.h>
#include <XXMLItems.h>
#include <XXMLParser.h>

#include <MacMemory.h>

using namespace Whisper;


//-----------------------------------
//	Constants
//
const uint32 kPartType = 		 'Part';

const float	LAZY_SUSAN_RADIUS =  38.0F;		// 50.0F
const int32	MAX_SPLINE_POINTS =  40000L;
const float	kTrackSubDivFactor = 1.0F;
const int32 kSkipValue =  		 1;			// 22
const float	kTrackWidth = 		 0.4F;
const int32	kMaxFacesInMesh = 	 20;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//----------------------------------------------------------------
//
// ParsePoint
//
//----------------------------------------------------------------
static X3DPoint ParsePoint(const XXMLElement* element, const std::wstring& name) 
{	
	const XXMLAttribute& attr = element->FindAttribute(name);
	std::wstring value = attr.GetValue();
	
	X3DPoint result;			
	result.x = (float) StrToDouble(Parse(value, L" "));
	result.y = (float) StrToDouble(Parse(value, L" "));
	result.z = (float) StrToDouble(Parse(value, L" "));

	return result;	
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class CBuildTrack
// ===================================================================================

//---------------------------------------------------------------
//
// CBuildTrack::~CBuildTrack
//
//---------------------------------------------------------------
CBuildTrack::~CBuildTrack()
{
}


//---------------------------------------------------------------
//
// CBuildTrack::CBuildTrack
//
//---------------------------------------------------------------
CBuildTrack::CBuildTrack() 
{
	fPartCount = this->DoLoadParts();
}


//---------------------------------------------------------------
//
// CBuildTrack::Build
//
//---------------------------------------------------------------
int32 CBuildTrack::Build(const X3DOrderedDisplayGroup& group, NubEntryType* splinesPtr)
{	
	PRECONDITION(splinesPtr != nil);
	
	fTrackGroup = group;
	fSplinePointsPtr = splinesPtr;
	
	fNumSplineNubs 	 = 0;
	fNumSplinePoints = 0;
			
	if (fPartCount > 0) {
		this->DoMakeRandomTrack();
		this->DoCreateMasterNubList();	
		this->DoCalcSplineCurve();
		this->DoBuildCoasterGeometry();
	}
	
	return fNumSplinePoints;
}

#if __MWERKS__
#pragma mark Ê
#endif

//----------------------------------------------------------------
//
// CBuildTrack::DoLoadParts
//
//----------------------------------------------------------------
int16 CBuildTrack::DoLoadParts()
{
	XXMLDoc doc;
	{
	XFileSpec spec(XFolderSpec::GetAppFolder(), L"Parts.xml");
	XResource data(spec);
	XXMLParser parser(&data);
	XXMLDocumentCallback builder(doc);
		parser.Parse(builder);
	}
	
	const XXMLElement* root = doc.GetRootElement();
	ASSERT(root->GetName() == L"Parts");
	
	uint32 i = 0;
	XXMLElement::const_item_iterator iter = root->item_begin();
	while (iter != root->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {
			ASSERT(child->GetName() == L"Part");
			
			this->DoParsePart(child, i);
			++i;
		}
	}
	
	return (int16) i;
}


//----------------------------------------------------------------
//
// CBuildTrack::DoParsePart
//
//----------------------------------------------------------------
void CBuildTrack::DoParsePart(const XXMLElement* element, uint32 i) 
{
	PRECONDITION(element->GetName() == L"Part");
		
	fPartsList[i].coordsPtr = (NubEntryType*) NewPtr(MAX_PARTS*sizeof(NubEntryType));
	
	uint32 j = 0;
	XXMLElement::const_item_iterator iter = element->item_begin();
	while (iter != element->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		if (const XXMLElement* child = dynamic_cast<const XXMLElement*>(item)) {
			this->DoParseNub(child, i, j);
			++j;
		}
	}
	
	fPartsList[i].numNubs = numeric_cast<uint8>(j);
}


//----------------------------------------------------------------
//
// CBuildTrack::DoParseNub
//
//----------------------------------------------------------------
void CBuildTrack::DoParseNub(const XXMLElement* element, uint32 i, uint32 j) 
{
	PRECONDITION(element->GetName() == L"Nub");
	
	fPartsList[i].coordsPtr[j].sectionNum = 0;
	fPartsList[i].coordsPtr[j].basePt     = ParsePoint(element, L"base_pt");
	fPartsList[i].coordsPtr[j].upPt       = ParsePoint(element, L"up_pt");
}


//----------------------------------------------------------------
//
// CBuildTrack::DoMakeRandomTrack
//
//----------------------------------------------------------------
void CBuildTrack::DoMakeRandomTrack()
{
	for (int32 i = 0; i < MAX_TRACK_SECTIONS; i++) {
		int32 r = Random((int32) MAX_TRACK_SECTIONS);
		r = i;
		
		fTrackSectionList[i].partNum    = i;	
		fTrackSectionList[i].nubCoord.y = 0;
		fTrackSectionList[i].nubCoord.x = std::sin(6.24F/MAX_TRACK_SECTIONS*r)*(LAZY_SUSAN_RADIUS - 6.0F);
		fTrackSectionList[i].nubCoord.z = std::cos(6.24F/MAX_TRACK_SECTIONS*r)*(LAZY_SUSAN_RADIUS - 6.0F);
	}				
}


//----------------------------------------------------------------
//
// CBuildTrack::DoCreateMasterNubList
//
//----------------------------------------------------------------
void CBuildTrack::DoCreateMasterNubList()
{
	fNumSplineNubs = 0;

	X3DMatrix transform;
	for (int32 sectionNum = 0; sectionNum < fPartCount; sectionNum++) {
		int32 partNum = fTrackSectionList[sectionNum].partNum;		// get part # to add 

		// get info for part
		X3DPoint sectionStartCoords = fTrackSectionList[sectionNum].nubCoord;	// get coords where track section should start 
		int32 numNubsInPart = fPartsList[partNum].numNubs;			// get # nubs in part 
		NubEntryType* coordPtr = fPartsList[partNum].coordsPtr;		// get ptr to coord list 

		transform = this->DoAmountToRotateNubs(coordPtr, numNubsInPart, sectionNum);
			
		// copy & adjust part nubs
		for (int32 i = 1; i < (numNubsInPart-1); i++) {				// skip nub 0 & last nub 
			X3DPoint basePt = coordPtr->basePt*transform;			// rotate the points into position 
			X3DPoint upPt = coordPtr->upPt*transform;
			
			fNubArray[fNumSplineNubs].basePt = basePt + sectionStartCoords; // tag nubs to end of previous part 
	
			fNubArray[fNumSplineNubs].upPt = upPt + sectionStartCoords;

			fNubArray[fNumSplineNubs].sectionNum = sectionNum;		// remember which section this belongs to 

			coordPtr++;
			fNumSplineNubs++;
		}		
	}
	
	// create 3 final nubs which wrap back to beginning to close the loop
	int32 partNum = fTrackSectionList[0].partNum;					// get part # 
	NubEntryType* coordPtr = fPartsList[partNum].coordsPtr;			// point back to beginning 
	TQ3Point3D sectionStartCoords = fTrackSectionList[0].nubCoord;	// get coords where track section should start 
	int32 numNubsInPart = fPartsList[partNum].numNubs;				// get # nubs in part 

	transform = this->DoAmountToRotateNubs(coordPtr, numNubsInPart, 0);

	for (int32 i = 0; i < 3; i++) {		
		X3DPoint basePt = coordPtr->basePt*transform;				// rotate the points into position 
		X3DPoint upPt = coordPtr->upPt*transform;
		
		fNubArray[fNumSplineNubs].basePt = basePt + sectionStartCoords;	// tag nubs to end of previous part 
	
		fNubArray[fNumSplineNubs].upPt = upPt + sectionStartCoords;

		fNumSplineNubs++;
		coordPtr++;
	}
}


//----------------------------------------------------------------
//
// CBuildTrack::DoCalcSplineCurve
//
//----------------------------------------------------------------
void CBuildTrack::DoCalcSplineCurve()
{
	int32 numNubs = fNumSplineNubs;	// # nubs 
	fNumSplinePoints = 0;			// # points generated 
	
	// scan thru nubs. Note: skips 1st & last nubs (sorta) 
	for (int32 nubNum = 1; nubNum < numNubs-2; nubNum++) {				
		int32 numSubDivs = (int32) (Distance(fNubArray[nubNum].basePt,
										fNubArray[nubNum+1].basePt)*kTrackSubDivFactor + 0.5F);	// # segments between next nubs 
		if (numSubDivs < 1)
			numSubDivs = 1;
		
		float incVal = 1.0F/numSubDivs;				// increment value 

		float t;
		int32 subCount;
		for (t = 0, subCount = 0; subCount < numSubDivs; t += incVal, subCount++) {
			float tSquared = t*t;
			float tCubed = tSquared*t;
			float a = (-0.166F * tCubed)+(0.5F * tSquared)-(0.5F * t) + 0.166F;
			float b = (0.5F * tCubed) - tSquared + 0.666F;
			float c = (-0.5F * tCubed) + (0.5F * tSquared) + (0.5F * t + 0.166F);
			float d = 0.166F * tCubed;

			if (fNumSplinePoints >= MAX_SPLINE_POINTS)
				throw std::runtime_error("Too many spline points!  Overflowed array!");
				
			// calc seg of base			
			fSplinePointsPtr[fNumSplinePoints].basePt =
								a*fNubArray[nubNum-1].basePt +
								b*fNubArray[nubNum].basePt +
								c*fNubArray[nubNum+1].basePt +
								d*fNubArray[nubNum+2].basePt;

			// calc seg of up			
			fSplinePointsPtr[fNumSplinePoints].upPt =
								a*fNubArray[nubNum-1].upPt +
								b*fNubArray[nubNum].upPt +
								c*fNubArray[nubNum+1].upPt +
								d*fNubArray[nubNum+2].upPt;
							
			// remember what type it is				
			fSplinePointsPtr[fNumSplinePoints].sectionNum = fNubArray[nubNum].sectionNum;

			fNumSplinePoints++;
		}
	}
}


//----------------------------------------------------------------
//
// CBuildTrack::DoBuildCoasterGeometry
//
//----------------------------------------------------------------
void CBuildTrack::DoBuildCoasterGeometry()
{
	// create new mesh object
	X3DMesh* myMesh = new X3DMesh;
	myMesh->DelayUpdates();
								
	// setup general attributes
	X3DAttributeSet generalAttribs;	
	generalAttribs.AddAmbientCoefficient(0.8F);
	generalAttribs.AddSpecularControl(0.0F);

	// create UV attributes for the vertices
	X3DAttributeSet	vAttrib[4];
	vAttrib[0].AddShadingUV(0, 1);
	vAttrib[1].AddShadingUV(1, 1);
	vAttrib[2].AddShadingUV(1, 0);
	vAttrib[3].AddShadingUV(0, 0);

	X3DVertexList<4> vertexList;

	int32 faceCount = 0;
	int32 colorTick = 0;

	TQ3MeshVertex meshVertexList[4];
	for (int32 i = 0; i < fNumSplinePoints - 1 - kSkipValue; i += kSkipValue) {		
		// get spline points
		float x2 = fSplinePointsPtr[i+kSkipValue].basePt.x;		// get coords of end pt #2 (far point) 
		float y2 = fSplinePointsPtr[i+kSkipValue].basePt.y;
		float z2 = fSplinePointsPtr[i+kSkipValue].basePt.z;

		// calc tangent vector
		X3DVector tangentVector = CrossTriangleProduct(fSplinePointsPtr[i].basePt, fSplinePointsPtr[i+kSkipValue].basePt, fSplinePointsPtr[i].upPt);	
		tangentVector = kTrackWidth*Normalize(tangentVector);
		
		// calc new "upper" coords of face
		vertexList.SetPoint(0, x2-tangentVector.x, y2-tangentVector.y, z2-tangentVector.z);	// upper left 
		vertexList.SetPoint(1, x2+tangentVector.x, y2+tangentVector.y, z2+tangentVector.z);	// upper right 

		meshVertexList[0] = myMesh->AddVertex(vertexList[0]);		// get new vertex for "upper/far" 
		meshVertexList[1] = myMesh->AddVertex(vertexList[1]);

		// for 1st face, must rig "bottom" coords	
		if (i == 0) {
			vertexList.SetPoint(2, fSplinePointsPtr[0].basePt + tangentVector);	// lower right 
			vertexList.SetPoint(3, fSplinePointsPtr[0].basePt - tangentVector);	// lower left 

			meshVertexList[2] = myMesh->AddVertex(vertexList[2]);	// set vertex 
			meshVertexList[3] = myMesh->AddVertex(vertexList[3]);
		}

		// create face		
		X3DMeshFace face = myMesh->AddFace(4, meshVertexList, generalAttribs);

		// apply UV coord attributes			
		myMesh->SetCornerAttributes(meshVertexList[0], face, vAttrib[0]);
		myMesh->SetCornerAttributes(meshVertexList[1], face, vAttrib[1]);
		myMesh->SetCornerAttributes(meshVertexList[2], face, vAttrib[2]);
		myMesh->SetCornerAttributes(meshVertexList[3], face, vAttrib[3]);

		colorTick++;		
		faceCount++;
		
		// see if the mesh object is large enough to use now
		if (faceCount > kMaxFacesInMesh) {
			myMesh->ResumeUpdates();

			fTrackGroup.AddObject(*myMesh);					// add mesh to group 
			
			delete myMesh;									// make another mesh object 
			myMesh = new X3DMesh;
			myMesh->DelayUpdates();
			
			meshVertexList[0] = myMesh->AddVertex(vertexList[0]);	// reset these to the new mesh 
			meshVertexList[1] = myMesh->AddVertex(vertexList[1]);
			
			faceCount = 0;		
		}	
	
		// uppers will be lowers on the next polygon
		vertexList.SetPoint(2, vertexList[1].point);		// this is so next poly's bottom will match last poly's top 
		vertexList.SetPoint(3, vertexList[0].point);	
		meshVertexList[2] = meshVertexList[1];
		meshVertexList[3] = meshVertexList[0];		

		if (colorTick & 1) {
			vertexList.SetAttribute(0, vAttrib[0]);			// apply uv attribs to the vertices 
			vertexList.SetAttribute(1, vAttrib[1]);
			vertexList.SetAttribute(2, vAttrib[2]);
			vertexList.SetAttribute(3, vAttrib[3]);
		
		} else {
			vertexList.SetAttribute(0, vAttrib[3]);			
			vertexList.SetAttribute(1, vAttrib[2]);
			vertexList.SetAttribute(2, vAttrib[1]);
			vertexList.SetAttribute(3, vAttrib[0]);
		}
	}

	// create 1 final face to link back to the beginning
	meshVertexList[0] = myMesh->AddVertex(vertexList[0]);	// create vertex based on 1st 
	meshVertexList[1] = myMesh->AddVertex(vertexList[1]);
	
	myMesh->AddFace(4, meshVertexList);
	faceCount++;

	// apply any remaining mesh			
	myMesh->ResumeUpdates();
	
	if (faceCount > 0) 
		fTrackGroup.AddObject(*myMesh);

	delete myMesh;							// kill mesh object 
}

			
//----------------------------------------------------------------
//
// CBuildTrack::DoAmountToRotateNubs
//
// Returns a matrix such that nubs will be rotated on the y axis so 
// that the track section will go from point A to point B. The matrix
// also includes a scaling value which will scale the nub based on 
// the change in length.  
//
//----------------------------------------------------------------
X3DMatrix CBuildTrack::DoAmountToRotateNubs(NubEntryType* coordPtr, long numNubsInPart, long sectionNum) const
{
	// calc original vector 
	X3DPoint startPt = coordPtr[1].basePt;				// use 2nd nub as start coord (remember that we skip the 1st nub in a part) 
	X3DPoint endPt = coordPtr[numNubsInPart-1].basePt;	// use last nub as end coord of original data 
	X3DVector originalVec(startPt, endPt);				// calc vector from start to end 

	// calc desired vector 
	int32 i = sectionNum+1;
	if (i >= MAX_TRACK_SECTIONS)
		i = 0;

	startPt = fTrackSectionList[sectionNum].nubCoord;	// get vector of where we want it to endup 
	endPt = fTrackSectionList[i].nubCoord;				// get vector of where we want it to endup 
	X3DVector desiredVec(startPt, endPt);				// calc vector from start to desired end 

	// calc angle 
	float rotation = (float) Angle(originalVec, desiredVec);	// calc amount we need to rotate all nubs in part 
	if (desiredVec.z > originalVec.z)					// see if need to negate (since we only have absolute angle between vecs) 
		rotation = -rotation;

	// calc scaling to adjust for distance change 
	double originalSize = originalVec.Length();
	double desiredSize = desiredVec.Length();
	float scaling = (float) (desiredSize/originalSize);		// return scaling value 
		
	X3DMatrix rotate;
	rotate.SetRotateY(rotation);

	X3DMatrix scale;
	scale.SetScale(scaling, scaling, scaling);

	return rotate*scale;
}



