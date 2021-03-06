/*
 *	File:		CDissolve.cpp
 *	Function:	A class that randomly returns all the points in a 2D area.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. All Rights Reserved.
 *
 *	Change History (most recent first):	
 *
 *		$Log: CDissolve.cpp,v $
 *		Revision 1.2  2001/04/21 02:23:47  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/28 02:48:57  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "CDissolve.h"


// ========================================================================================
//	class CDissolve
//		Algorithm is from Graphics Gems I.
// ========================================================================================

uint32 CDissolve::msRandMasks[25] = {0,
							 	  	 0,
									 0x03,			// 3 values
									 0x06,			// 7 
									 0x0C,			// 15
									 0x14,			// 31
									 0x30,			// 63
									 0x60,			// 127
									 0xB8,			// 255
									 0x0110,		// 511
									 0x0240,		// 1023
									 0x0500,		// 2047
									 0x0CA0,		// 4095
									 0x1B00,		// 8191
									 0x3500,		// 16383
									 0x6000,		// 32767
									 0xB400,		// 65535
									 0x12000,		// 131071
									 0x20400,		// 262143
									 0x72000,		// 524287
									 0x90000,		// 1048575
									 0x140000,		// 2097151
									 0x300000,		// 4194303
									 0x400000,		// 8388607
									 0xD80000};		// 16777215
							  
//---------------------------------------------------------------
//
// CDissolve::~CDissolve
//
//---------------------------------------------------------------
CDissolve::~CDissolve()
{
}


//---------------------------------------------------------------
//
// CDissolve::CDissolve
//
//---------------------------------------------------------------
CDissolve::CDissolve(const XSize& size)
{
	PRECONDITION(size.width >= 0);
	PRECONDITION(size.height >= 0);
	
	mSize = size;
		
	this->Reset();
}


//---------------------------------------------------------------
//
// CDissolve::FindNext
//
//---------------------------------------------------------------
void CDissolve::FindNext()
{
	PRECONDITION(!mDone);
	
	if (mPoint == kZeroPt) {
		mDone = true;
		
	} else {
		do {
			mPoint.y = (int16) (mElement >> mRowShift); 			
			mPoint.x = (int16) (mElement & mColMask); 			 

			if (mElement & 1)						
				mElement = (mElement >> 1)^mMask;	 
			else 
				mElement = (mElement >> 1); 
		} while ((mPoint.x >= mSize.width || mPoint.y >= mSize.height) && mElement != 1);	// generates up to twice as many points as we actually need
		
		if (mElement == 1)							// we've cycled back to the first element so we're done iterating
			mPoint = kZeroPt;						// however the loop above will never generate zero so we'll do it here
	}
	
	POSTCONDITION(mPoint.x >= 0);
	POSTCONDITION(mPoint.y >= 0);
	POSTCONDITION(mPoint.x < mSize.width);
	POSTCONDITION(mPoint.y < mSize.height);
}


//---------------------------------------------------------------
//
// CDissolve::Reset
//
//---------------------------------------------------------------
void CDissolve::Reset()
{
	mDone = mSize.width <= 1 || mSize.height <= 1; 

	if (!mDone) {
		mRowWidth = this->DoGetBitWidth((uint32) mSize.height);// how many bits needed for height? 
		mColWidth = this->DoGetBitWidth((uint32) mSize.width); // how many bits needed for width? 

		mRegisterWidth = mRowWidth + mColWidth; 	// how wide must the register be? 
		ASSERT(mRegisterWidth >= 2);
		ASSERT(mRegisterWidth <= 25);
		
		mMask = msRandMasks[mRegisterWidth]; 		// which mask is for that width? 
		mColMask = (1<<mColWidth)-1;				// find mask to extract bottom bits (col) 

		mRowShift = mColWidth; 						// find dist to shift to get top bits (row) 

		mElement = 1;								// 1st element (could be any nonzero)
		mPoint = XPoint(1, 1);
	}
}

#pragma mark ~

//---------------------------------------------------------------
//
// CDissolve::DoGetBitWidth
//
//---------------------------------------------------------------
int32 CDissolve::DoGetBitWidth(uint32 n) const
{
	int32 width = 0;		// initially, no bits needed to represent n 
	while (n != 0) {		// loop 'til n has been whittled down to 0 
		n >>= 1;			// shift n right 1 bit (NB: n is unsigned) 
		width++;			// and remember how wide n is 
	}						// end of loop shrinking n down to nothing 
	return width;			// return bit positions counted 
}						

