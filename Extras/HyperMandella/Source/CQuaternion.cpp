/*
 *  File:       CQuaternion.cpp
 *  Summary:   	Quaternion number class. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CQuaternion.cpp,v $
 *		Revision 1.1  2000/12/28 02:50:40  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:57:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 2/02/98	JDJ		Created
 */

#include "AppHeader.h"
#include "CQuaternion.h"

#include <XNumbers.h>


//-----------------------------------
//	Constants
//
const CQuaternion kZeroQuaternion(0.0f, 0.0f, 0.0f, 0.0f);


// ===================================================================================
//	class CQuaternion
// ===================================================================================

//---------------------------------------------------------------
//
// CQuaternion::operator*=
//
//---------------------------------------------------------------
CQuaternion& CQuaternion::operator*=(const CQuaternion& rhs)
{	
	double xx = (double) x*rhs.x - y*rhs.y - z*rhs.z - w*rhs.w;
	double yy = (double) y*rhs.x + x*rhs.y + w*rhs.z - z*rhs.w;
	double zz = (double) z*rhs.x - w*rhs.y + x*rhs.z + y*rhs.w;
	double ww = (double) w*rhs.x + z*rhs.y - y*rhs.z + x*rhs.w;

	x = (float) xx;
	y = (float) yy;
	z = (float) zz;
	w = (float) ww;
	
	return *this;
}


//---------------------------------------------------------------
//
// CQuaternion::operator/=
//
//---------------------------------------------------------------
CQuaternion& CQuaternion::operator/=(const CQuaternion& rhs)
{
	CQuaternion a = inverse(rhs);
	
	return this->operator*=(a);
}

#pragma mark Ê

//---------------------------------------------------------------
//
// Equal (CQuaternion, CQuaternion, double)
//
//---------------------------------------------------------------
bool Equal(const CQuaternion& lhs, const CQuaternion& rhs, const double& tolerance)
{
	bool equal = false;
	
	if (Equal(lhs.x, rhs.x, tolerance))
		if (Equal(lhs.y, rhs.y, tolerance))
			if (Equal(lhs.z, rhs.z, tolerance))
				if (Equal(lhs.w, rhs.w, tolerance))
					equal = true;
					
	return equal;
}


//---------------------------------------------------------------
//
// sqr (CQuaternion)
//
//---------------------------------------------------------------
CQuaternion sqr(const CQuaternion& rhs)
{
	CQuaternion result;
	
	float temp = rhs.x + rhs.x;

	result.x = rhs.x*rhs.x - (rhs.y*rhs.y + rhs.z*rhs.z + rhs.w*rhs.w);
	result.y = rhs.y*temp;
	result.z = rhs.z*temp;
	result.w = rhs.w*temp;
		
	return result;
}


//---------------------------------------------------------------
//
// inverse (CQuaternion)
//
//---------------------------------------------------------------
CQuaternion inverse(const CQuaternion& rhs)
{
	CQuaternion result;

	float temp = (float) mag2(rhs);
	
	result.x = rhs.x/temp;
	result.y = -rhs.y/temp;
	result.z = -rhs.z/temp;
	result.w = -rhs.w/temp;
	
	return result;
}



