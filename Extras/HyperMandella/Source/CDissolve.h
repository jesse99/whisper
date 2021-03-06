/*
 *	File:		CDissolve.h
 *	Function:	A class that randomly returns all the points in a 2D area.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. All Rights Reserved.
 *
 *	Change History (most recent first):	
 *
 *		$Log: CDissolve.h,v $
 *		Revision 1.1  2000/12/28 02:49:00  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:20  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	  2/04/98	JDJ		Created
 */

#include <XPoint.h>

using namespace Whisper;
using namespace std;


// ========================================================================================
//	class CDissolve
// ========================================================================================
class CDissolve {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
						~CDissolve();
	
						CDissolve(const XSize& size);
						// Randomly returns all points in [0, width) x [0, height). 

//-----------------------------------
//	API
//
public:
			XPoint 		GetPoint() const						{ASSERT(!mDone); return mPoint;}
						// Returns the current point in the sequence.
			
			void 		FindNext();
						// Finds the next point in the sequence.
						
			bool 		IsDone() const							{return mDone;}
						// Returns true if all the points have been returned.
			
			void 		Reset();
						// Resets the pointer so that the points are generated from scratch.

//-----------------------------------
//	Internal API
//
protected:
			int32 		DoGetBitWidth(uint32 n) const;

//-----------------------------------
//	Member Data
//
protected:
	XSize			mSize;
	XPoint			mPoint;
	
	int32			mRowWidth;			// bit width for rows
	int32			mColWidth;			// bit width for cols
	int32			mRegisterWidth;		// "width" of sequence generator
	uint32			mMask;				// mask to XOR with to create sequence
	int32			mColMask;			// mask to extract column from element
	int32			mRowShift;			// shift distance to get row from element
	
	uint32			mElement;
	bool			mDone;
		
	static uint32	msRandMasks[25];
};



