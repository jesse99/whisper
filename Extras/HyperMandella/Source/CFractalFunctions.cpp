/*
 *  File:       CFractalFunctions.cpp
 *  Summary:   	Concrete classes for objects that iterate fractal formulas. 
 *  Written by: Jesse Jones
 *
 *	Notes:		1) These functions are the primary bottleneck in HyperMandella.
 *				Because of the enormous number of times the formulas are iterated
 *				all function calls have been eliminated and only simple types are
 *				used within the loops.
 *
 *				2) To add a new formula define a new class here and add a new item
 *				to the Formula menu in Menus.xml. 
 *
 *  Copyright © 1998 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFractalFunctions.cpp,v $
 *		Revision 1.1  2000/12/28 02:49:33  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 2/02/98	JDJ		Created
 */

#include "AppHeader.h"
#include "CFractalFunction.h"

#include <XOptimize.h>

#include "CQuaternion.h"
#include "IDocInfo.h"


inline double sqr(double x)	{return x*x;}

// ===================================================================================
//	Quaternion Functions
// ===================================================================================

//---------------------------------------------------------------
//
// AddQ
//
// lhs = lhs + rhs
//
//---------------------------------------------------------------
inline void AddQ(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	lhsX += rhsX;
	lhsY += rhsY;
	lhsZ += rhsZ;
	lhsW += rhsW;
}


//---------------------------------------------------------------
//
// MultQ
//
// lhs = lhs * rhs
//
//---------------------------------------------------------------
inline void MultQ(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double xx = lhsX*rhsX - lhsY*rhsY - lhsZ*rhsZ - lhsW*rhsW;
	double yy = lhsY*rhsX + lhsX*rhsY + lhsW*rhsZ - lhsZ*rhsW;
	double zz = lhsZ*rhsX - lhsW*rhsY + lhsX*rhsZ + lhsY*rhsW;
	double ww = lhsW*rhsX + lhsZ*rhsY - lhsY*rhsZ + lhsX*rhsW;

	lhsX = xx;
	lhsY = yy;
	lhsZ = zz;
	lhsW = ww;
}


//---------------------------------------------------------------
//
// SqrQ
//
// result = rhs*rhs
//
//---------------------------------------------------------------
inline void SqrQ(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double temp = rhsX + rhsX;

	resultX = rhsX*rhsX - (rhsY*rhsY + rhsZ*rhsZ + rhsW*rhsW);
	resultY = rhsY*temp;
	resultZ = rhsZ*temp;
	resultW = rhsW*temp;
}


//---------------------------------------------------------------
//
// DivideQ
//
// lhs = lhs / rhs
//
//---------------------------------------------------------------
inline void DivideQ(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double temp = rhsX*rhsX + rhsY*rhsY + rhsZ*rhsZ + rhsW*rhsW;
	
	rhsX /= temp;
	rhsY /= -temp;
	rhsZ /= -temp;
	rhsW /= -temp;

	MultQ(lhsX, lhsY, lhsZ, lhsW, rhsX, rhsY, rhsZ, rhsW);
}

#pragma mark -

// ===================================================================================
//	HyperComplex Functions
// ===================================================================================

//---------------------------------------------------------------
//
// AddH
//
// lhs = lhs + rhs
//
//---------------------------------------------------------------
inline void AddH(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	lhsX += rhsX;
	lhsY += rhsY;
	lhsZ += rhsZ;
	lhsW += rhsW;
}


//---------------------------------------------------------------
//
// MultH
//
// lhs = lhs * rhs
//
//---------------------------------------------------------------
inline void MultH(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double xx = lhsX*rhsX - lhsY*rhsY - lhsZ*rhsZ + lhsW*rhsW;
	double yy = lhsY*rhsX + lhsX*rhsY - lhsW*rhsZ - lhsZ*rhsW;
	double zz = lhsZ*rhsX - lhsW*rhsY + lhsX*rhsZ - lhsY*rhsW;
	double ww = lhsW*rhsX + lhsZ*rhsY + lhsY*rhsZ + lhsX*rhsW;

	lhsX = xx;
	lhsY = yy;
	lhsZ = zz;
	lhsW = ww;
}


//---------------------------------------------------------------
//
// SqrH
//
// result = rhs*rhs
//
//---------------------------------------------------------------
inline void SqrH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double xx = rhsX*rhsX - rhsY*rhsY - rhsZ*rhsZ + rhsW*rhsW;
	double yy = rhsX*rhsY - rhsZ*rhsW;
	double zz = rhsX*rhsZ - rhsY*rhsW;
	double ww = rhsX*rhsW + rhsY*rhsZ;

	resultX = xx;
	resultY = yy + yy;
	resultZ = zz + zz;
	resultW = ww + ww;
}


//---------------------------------------------------------------
//
// DivideH
//
// lhs = lhs / rhs
//
//---------------------------------------------------------------
inline void DivideH(double& lhsX, double& lhsY, double& lhsZ, double& lhsW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	double det = (sqr(rhsX - rhsW) + sqr(rhsY + rhsZ))*(sqr(rhsX + rhsW) + sqr(rhsY - rhsZ));
	
	double xx = rhsX*rhsX;
	double yy = rhsY*rhsY;
	double zz = rhsZ*rhsZ;
	double ww = rhsW*rhsW;
	
	double t = 2.0*(rhsX*rhsW - rhsY*rhsZ);

	double tx = ( rhsX*(xx + yy + zz + ww) - rhsW*t)/det;
	double ty = (-rhsY*(xx + yy + zz + ww) - rhsZ*t)/det;
	double tz = (-rhsZ*(xx + yy + zz + ww) - rhsY*t)/det;
	double tw = ( rhsW*(xx + yy + zz + ww) - rhsX*t)/det;

	MultH(lhsX, lhsY, lhsZ, lhsW, tx, ty, tz, tw);
}


//---------------------------------------------------------------
//
// Theta
//
//---------------------------------------------------------------
inline double Theta(double ar, double ai)
{
	double theta;
	
	if (ai == 0.0) {
		if (ar >= 0.0)
			theta = 0.0;				// positive real 
		else
			theta = kPi;				// negative real 
		
	} else if (ar == 0.0) {
		if (ai >= 0.0)
			theta = kHalfPi;			// positive imag 
		else
			theta = -kHalfPi;			// negative imag 
		
	} else {
		theta = atan(ai/ar);
		if (ar < 0.0) {					// make sure theta is in the correct quadrant 
			if (ai > 0.0)
				theta = kPi + theta;
			else
				theta = theta - kPi;
		}
	}
		
	return theta;
}


//---------------------------------------------------------------
//
// PowH
//
// result = pow(rhs, pwr)
//
//---------------------------------------------------------------
static void PowH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW, double pwr)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.
	double r = pow(ar*ar + ai*ai, 0.5*pwr);	
	double t = pwr*Theta(ar, ai);

	double cr = r*cos(t);
	double ci = r*sin(t);

	r = pow(br*br + bi*bi, 0.5*pwr);				
	t = pwr*Theta(br, bi);

	double dr = r*cos(t);
	double di = r*sin(t);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}


//---------------------------------------------------------------
//
// ExpH
//
// result = exp(rhs)
//
//---------------------------------------------------------------
static void ExpH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.
	double t  = exp(ar);
	double cr = t*cos(ai);
	double ci = t*sin(ai);
	
	t         = exp(br);
	double dr = t*cos(bi);
	double di = t*sin(bi);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}


//---------------------------------------------------------------
//
// SqrtH
//
// result = sqrt(rhs)
//
//---------------------------------------------------------------
static void SqrtH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.
	double r = sqrt(ar*ar + ai*ai);
	
	double cr = 0.5*(r + ar);
	double ci = 0.5*(r - ar);
	cr = sqrt(cr);
	ci = Sign(ai)*sqrt(ci);

	r = sqrt(br*br + bi*bi);
	
	double dr = 0.5*(r + br);
	double di = 0.5*(r - br);
	dr = sqrt(dr);
	di = Sign(bi)*sqrt(di);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}


//---------------------------------------------------------------
//
// SinH
//
// result = sin(rhs)
//
//---------------------------------------------------------------
static void SinH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.	
	double cr = sin(ar)*cosh(ai);
	double ci = cos(ar)*sinh(ai);
	
	double dr = sin(br)*cosh(bi);
	double di = cos(br)*sinh(bi);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}


//---------------------------------------------------------------
//
// CosH
//
// result = cos(rhs)
//
//---------------------------------------------------------------
static void CosH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.	
	double cr = cos(ar)*cosh(ai);
	double ci = -sin(ar)*sinh(ai);
	
	double dr = cos(br)*cosh(bi);
	double di = -sin(br)*sinh(bi);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}


//---------------------------------------------------------------
//
// CoshH
//
// result = cosh(rhs)
//
//---------------------------------------------------------------
static void CoshH(double& resultX, double& resultY, double& resultZ, double& resultW, double rhsX, double rhsY, double rhsZ, double rhsW)
{
	// Seperate the hypercomplex number into two complex numbers.
	double ar = rhsX - rhsW;
	double ai = rhsY + rhsZ;
	
	double br = rhsX + rhsW;
	double bi = rhsY - rhsZ;

	// Do the operation using the complex numbers.	
	double cr = cosh(ar)*cos(ai);
	double ci = sinh(ar)*sin(ai);
	
	double dr = cosh(br)*cos(bi);
	double di = sinh(br)*sin(bi);

	// Convert the new complex numbers back into a hypercomplex number.
	resultX = 0.5*(cr + dr);
	resultY = 0.5*(ci + di);
	resultZ = 0.5*(ci - di);
	resultW = 0.5*(dr - cr);
}

#pragma mark -

// ===================================================================================
//	class CSqrQ
// ===================================================================================
class CSqrQ : public CFractalFunction {

public:	
  						CSqrQ(XBoss* boss);

	virtual std::wstring GetFormula() const			{return L"q^2 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CSqrQ)

//---------------------------------------------------------------
//
// CSqrQ::CSqrQ
//
//---------------------------------------------------------------
CSqrQ::CSqrQ(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CSqrQ::OnComputeDwell								
//
// q = q^2 + c
//
//---------------------------------------------------------------
uint32 CSqrQ::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;
	
	while (dwell < maxDwell && mag < bailout) {
		double temp = x + x;
		
		double x2 = x*x;
		double y2 = y*y;
		double z2 = z*z;
		double w2 = w*w;
		
		x = x2 - (y2 + z2 + w2);
		y *= temp;
		z *= temp;
		w *= temp;

		x += cx;
		y += cy;
		z += cz;
		w += cw;

		mag = x2 + y2 + z2 + w2;	// cheat a bit and use the values from the previous iteration
		++dwell;
	}
		
	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CCubeQ
// ===================================================================================
class CCubeQ : public CFractalFunction {

public:	
  						CCubeQ(XBoss* boss);

	virtual std::wstring GetFormula() const			{return L"q^3 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CCubeQ)

//---------------------------------------------------------------
//
// CCubeQ::CCubeQ
//
//---------------------------------------------------------------
CCubeQ::CCubeQ(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CCubeQ::OnComputeDwell							
//
// q = q^3 + c
//
//---------------------------------------------------------------
uint32 CCubeQ::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

#if 1
	double tx, ty, tz, tw;
	while (dwell < maxDwell && mag < bailout) {
		SqrQ(tx, ty, tz, tw, x, y, z, w);
		MultQ(x, y, z, w, tx, ty, tz, tw);
		AddQ(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

#else
	CQuaternion q((float) x, (float) y, (float) z, (float) w);
	
	CQuaternion c((float) cx, (float) cy, (float) cz, (float) cw);
	
	while (dwell < maxDwell && mag < bailout) {
		q = q*sqr(q) + c;
		
		mag = mag2(q);
		++dwell;
	}
#endif

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CQuadQ
// ===================================================================================
class CQuadQ : public CFractalFunction {

public:	
  						CQuadQ(XBoss* boss);

	virtual std::wstring GetFormula() const					{return L"q^4 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CQuadQ)

//---------------------------------------------------------------
//
// CQuadQ::CQuadQ
//
//---------------------------------------------------------------
CQuadQ::CQuadQ(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CQuadQ::OnComputeDwell							
//
// q = q^4 + c
//
//---------------------------------------------------------------
uint32 CQuadQ::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

#if 1
	while (dwell < maxDwell && mag < bailout) {
		SqrQ(x, y, z, w, x, y, z, w);
		SqrQ(x, y, z, w, x, y, z, w);
		AddQ(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

#else
	CQuaternion q((float) x, (float) y, (float) z, (float) w);
	
	CQuaternion c((float) cx, (float) cy, (float) cz, (float) cw);
						
	while (dwell < maxDwell && mag < bailout) {
		q = sqr(q);
		q = sqr(q);
		q += c;
		
		mag = mag2(q);
		++dwell;
	}
#endif

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CLambdaQ
// ===================================================================================
class CLambdaQ : public CFractalFunction {

public:	
  						CLambdaQ(XBoss* boss);

	virtual std::wstring GetFormula() const					{return L"c*q*(1 - q)";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CLambdaQ)

//---------------------------------------------------------------
//
// CLambdaQ::CLambdaQ
//
//---------------------------------------------------------------
CLambdaQ::CLambdaQ(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CLambdaQ::OnComputeDwell							
//
// q = c*q*(1 - q)
//
//---------------------------------------------------------------
uint32 CLambdaQ::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	double tx, ty, tz, tw;
	while (dwell < maxDwell && mag < bailout) {
		tx = 1.0 - x;
		ty = -y;
		tz = -z;
		tw = -w;
		
		MultQ(x, y, z, w, cx, cy, cz, cw);
		MultQ(x, y, z, w, tx, ty, tz, tw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CPotts1Q
// ===================================================================================
class CPotts1Q : public CFractalFunction {

public:	
  						CPotts1Q(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"Potts1q";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CPotts1Q)

//---------------------------------------------------------------
//
// CPotts1Q::CPotts1Q
//
//---------------------------------------------------------------
CPotts1Q::CPotts1Q(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CPotts1Q::OnComputeDwell							
//
// q = [(q^2 + c - 1)/(2*q + c - 2)]^2
//
//---------------------------------------------------------------
uint32 CPotts1Q::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

#if 1
	double nx, ny, nz, nw;
	double dx, dy, dz, dw;
	while (dwell < maxDwell && mag < bailout) {
		SqrQ(nx, ny, nz, nw, x, y, z, w);			// n = sqr(q) + c - 1.0
		AddQ(nx, ny, nz, nw, cx, cy, cz, cw);
		nx -= 1.0;
		
		dx = x+x + cx - 2.0;						// d = 2.0*q + c - 2.0
		dy = y+y + cy;
		dz = z+z + cz;
		dw = w+w + cw;
		
		DivideQ(nx, ny, nz, nw, dx, dy, dz, dw);	// q = sqr(n/d)
		SqrQ(x, y, z, w, nx, ny, nz, nw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

#else
	CQuaternion q((float) x, (float) y, (float) z, (float) w);
	
	CQuaternion c((float) cx, (float) cy, (float) cz, (float) cw);
				
	while (dwell < maxDwell && mag < bailout) {
		CQuaternion num   = sqr(q) + c - 1.0;
		CQuaternion denom = 2.0*q + c - 2.0;
		q = sqr(num/denom);
		
		mag = mag2(q);
		++dwell;
	}
#endif

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CPotts2Q
// ===================================================================================
class CPotts2Q : public CFractalFunction {

public:	
  						CPotts2Q(XBoss* boss);

	virtual std::wstring GetFormula() const					{return L"Potts2q";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CPotts2Q)

//---------------------------------------------------------------
//
// CPotts2Q::CPotts2Q
//
//---------------------------------------------------------------
CPotts2Q::CPotts2Q(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CPotts2Q::OnComputeDwell							
//
// num   = q^3 + 3*(c - 1)*q + (c - 1)*(c - 2)
// denom = 3*q^2 + 3*(c - 2)*q + c^2 - 3*c + 3
// q     = (num/denom)^2
//
//---------------------------------------------------------------
uint32 CPotts2Q::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	double nx, ny, nz, nw;
	double dx, dy, dz, dw;
	double tx, ty, tz, tw;
	double ux, uy, uz, uw;
	while (dwell < maxDwell && mag < bailout) {
		SqrQ(nx, ny, nz, nw, x, y, z, w);			// n = q^3 + 3*(c - 1)*q + (c - 1)*(c - 2)
		MultQ(nx, ny, nz, nw, x, y, z, w);
		
		tx = 3.0*cx - 1.0;
		ty = 3.0*cy;
		tz = 3.0*cz;
		tw = 3.0*cw;
		MultQ(tx, ty, tz, tw, x, y, z, w);
		AddQ(nx, ny, nz, nw, tx, ty, tz, tw);
		
		tx = cx - 1.0;
		ty = cy;
		tz = cz;
		tw = cw;
		
		ux = cx - 2.0;
		uy = cy;
		uz = cz;
		uw = cw;
		MultQ(tx, ty, tz, tw, ux, uy, uz, uw);
		AddQ(nx, ny, nz, nw, tx, ty, tz, tw);
		
		SqrQ(dx, dy, dz, dw, x, y, z, w);			// d = 3*q^2 + 3*(c - 2)*q + c^2 - 3*c + 3
		dx *= 3.0;
		dy *= 3.0;
		dz *= 3.0;
		dw *= 3.0;
		
		tx = 3.0*cx - 6.0;
		ty = 3.0*cy;
		tz = 3.0*cz;
		tw = 3.0*cw;
		MultQ(tx, ty, tz, tw, x, y, z, w);
		AddQ(dx, dy, dz, dw, tx, ty, tz, tw);
		
		SqrQ(tx, ty, tz, tw, cx, cy, cz, cw);
		AddQ(dx, dy, dz, dw, tx, ty, tz, tw);
		
		dx -= 3.0*cx + 3.0;
		dy -= 3.0*cy;
		dz -= 3.0*cz;
		dw -= 3.0*cw;
		
		DivideQ(nx, ny, nz, nw, dx, dy, dz, dw);	// q = sqr(n/d)
		SqrQ(x, y, z, w, nx, ny, nz, nw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CSqrH
// ===================================================================================
class CSqrH : public CFractalFunction {

public:	
  						CSqrH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"h^2 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CSqrH)

//---------------------------------------------------------------
//
// CSqrH::CSqrH
//
//---------------------------------------------------------------
CSqrH::CSqrH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CSqrH::OnComputeDwell								
//
// h = h^2 + c
//
//---------------------------------------------------------------
uint32 CSqrH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{		
	uint32 dwell = 0;
	double mag = 0.0;
	
	while (dwell < maxDwell && mag < bailout) {
		double x2 = x*x;			
		double y2 = y*y;			
		double z2 = z*z;
		double w2 = w*w;

		double xx = x2 - y2 - z2 + w2;
		double yy = x*y - z*w;
		double zz = x*z - y*w;
		double ww = x*w + y*z;

		x = xx;
		y = yy + yy;
		z = zz + zz;
		w = ww + ww;

		x += cx;
		y += cy;
		z += cz;
		w += cw;

		mag = x2 + y2 + z2 + w2;	// cheat a bit and use the values from the previous iteration
		++dwell;
	}
		
	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CCubeH
// ===================================================================================
class CCubeH : public CFractalFunction {

public:	
  						CCubeH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"h^3 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CCubeH)

//---------------------------------------------------------------
//
// CCubeH::CCubeH
//
//---------------------------------------------------------------
CCubeH::CCubeH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CCubeH::OnComputeDwell							
//
// h = h^3 + c
//
//---------------------------------------------------------------
uint32 CCubeH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;
						
	double tx, ty, tz, tw;
	while (dwell < maxDwell && mag < bailout) {
		SqrH(tx, ty, tz, tw, x, y, z, w);
		MultH(x, y, z, w, tx, ty, tz, tw);
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CQuadH
// ===================================================================================
class CQuadH : public CFractalFunction {

public:	
  						CQuadH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"h^4 + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CQuadH)

//---------------------------------------------------------------
//
// CQuadH::CQuadH
//
//---------------------------------------------------------------
CQuadH::CQuadH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CQuadH::OnComputeDwell							
//
// h = h^4 + c
//
//---------------------------------------------------------------
uint32 CQuadH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	while (dwell < maxDwell && mag < bailout) {
		SqrH(x, y, z, w, x, y, z, w);
		SqrH(x, y, z, w, x, y, z, w);
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CEPwrH
// ===================================================================================
class CEPwrH : public CFractalFunction {

public:	
  						CEPwrH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"h^e + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CEPwrH)

//---------------------------------------------------------------
//
// CEPwrH::CEPwrH
//
//---------------------------------------------------------------
CEPwrH::CEPwrH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CEPwrH::OnComputeDwell							
//
// h = h^e + c
//
//---------------------------------------------------------------
uint32 CEPwrH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	while (dwell < maxDwell && mag < bailout) {
		PowH(x, y, z, w, x, y, z, w, 2.718281828);
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CPiPwrH
// ===================================================================================
class CPiPwrH : public CFractalFunction {

public:	
  						CPiPwrH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"h^pi + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CPiPwrH)

//---------------------------------------------------------------
//
// CPiPwrH::CPiPwrH
//
//---------------------------------------------------------------
CPiPwrH::CPiPwrH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CPiPwrH::OnComputeDwell							
//
// h = h^pi + c
//
//---------------------------------------------------------------
uint32 CPiPwrH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	while (dwell < maxDwell && mag < bailout) {
		PowH(x, y, z, w, x, y, z, w, kPi);
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CSqrtH
// ===================================================================================
class CSqrtH : public CFractalFunction {

public:	
  						CSqrtH(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"sqrt(h^4) + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CSqrtH)

//---------------------------------------------------------------
//
// CSqrtH::CSqrtH
//
//---------------------------------------------------------------
CSqrtH::CSqrtH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CSqrtH::OnComputeDwell							
//
// h = sqrt(h^4) + c
//
//---------------------------------------------------------------
uint32 CSqrtH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;
					
#if 1
	while (dwell < maxDwell && mag < bailout) {
		SqrH(x, y, z, w, x, y, z, w);
		SqrH(x, y, z, w, x, y, z, w);
		SqrtH(x, y, z, w, x, y, z, w);
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

#else
	CHyperComplex h((float) x, (float) y, (float) z, (float) w);
		
	CHyperComplex c((float) cx, (float) cy, (float) cz, (float) cw);

	while (dwell < maxDwell && mag < bailout) {
		h = sqrt(sqr(sqr(h)));
		h += c;
		
		mag = mag2(h);
		++dwell;
	}
#endif

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CPotts1H
// ===================================================================================
class CPotts1H : public CFractalFunction {

public:	
  						CPotts1H(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"Potts1h";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CPotts1H)

//---------------------------------------------------------------
//
// CPotts1H::CPotts1H
//
//---------------------------------------------------------------
CPotts1H::CPotts1H(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CPotts1H::OnComputeDwell							
//
// h = [(h^2 + c - 1)/(2*h + c - 2)]^2
//
//---------------------------------------------------------------
uint32 CPotts1H::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	double nx, ny, nz, nw;
	double dx, dy, dz, dw;
	while (dwell < maxDwell && mag < bailout) {
		SqrH(nx, ny, nz, nw, x, y, z, w);			// n = sqr(h) + c - 1.0
		AddH(nx, ny, nz, nw, cx, cy, cz, cw);
		nx -= 1.0;
		
		dx = x+x + cx - 2.0;						// d = 2.0*h + c - 2.0
		dy = y+y + cy;
		dz = z+z + cz;
		dw = w+w + cw;
		
		DivideH(nx, ny, nz, nw, dx, dy, dz, dw);	// h = sqr(n/d)
		SqrH(x, y, z, w, nx, ny, nz, nw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CPotts2H
// ===================================================================================
class CPotts2H : public CFractalFunction {

public:	
  						CPotts2H(XBoss* boss);

	virtual std::wstring GetFormula() const						{return L"Potts2h";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CPotts2H)

//---------------------------------------------------------------
//
// CPotts2H::CPotts2H
//
//---------------------------------------------------------------
CPotts2H::CPotts2H(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CPotts2H::OnComputeDwell							
//
// num   = h^3 + 3*(c - 1)*h + (c - 1)*(c - 2)
// denom = 3*h^2 + 3*(c - 2)*h + c^2 - 3*c + 3
// h     = (num/denom)^2
//
//---------------------------------------------------------------
uint32 CPotts2H::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;

	double nx, ny, nz, nw;
	double dx, dy, dz, dw;
	double tx, ty, tz, tw;
	double ux, uy, uz, uw;
	while (dwell < maxDwell && mag < bailout) {
		SqrH(nx, ny, nz, nw, x, y, z, w);			// n = h^3 + 3*(c - 1)*h + (c - 1)*(c - 2)
		MultH(nx, ny, nz, nw, x, y, z, w);
		
		tx = 3.0*cx - 1.0;
		ty = 3.0*cy;
		tz = 3.0*cz;
		tw = 3.0*cw;
		MultH(tx, ty, tz, tw, x, y, z, w);
		AddH(nx, ny, nz, nw, tx, ty, tz, tw);
		
		tx = cx - 1.0;
		ty = cy;
		tz = cz;
		tw = cw;
		
		ux = cx - 2.0;
		uy = cy;
		uz = cz;
		uw = cw;
		MultH(tx, ty, tz, tw, ux, uy, uz, uw);
		AddH(nx, ny, nz, nw, tx, ty, tz, tw);
		
		SqrH(dx, dy, dz, dw, x, y, z, w);			// d = 3*h^2 + 3*(c - 2)*h + c^2 - 3*c + 3
		dx *= 3.0;
		dy *= 3.0;
		dz *= 3.0;
		dw *= 3.0;
		
		tx = 3.0*cx - 6.0;
		ty = 3.0*cy;
		tz = 3.0*cz;
		tw = 3.0*cw;
		MultH(tx, ty, tz, tw, x, y, z, w);
		AddH(dx, dy, dz, dw, tx, ty, tz, tw);
		
		SqrH(tx, ty, tz, tw, cx, cy, cz, cw);
		AddH(dx, dy, dz, dw, tx, ty, tz, tw);
		
		dx -= 3.0*cx + 3.0;
		dy -= 3.0*cy;
		dz -= 3.0*cz;
		dw -= 3.0*cw;
		
		DivideH(nx, ny, nz, nw, dx, dy, dz, dw);	// h = sqr(n/d)
		SqrH(x, y, z, w, nx, ny, nz, nw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CExpH
// ===================================================================================
class CExpH : public CFractalFunction {

public:	
  						CExpH(XBoss* boss);

	virtual std::wstring GetFormula() const							{return L"l*e^h + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CExpH)

//---------------------------------------------------------------
//
// CExpH::CExpH
//
//---------------------------------------------------------------
CExpH::CExpH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = true;
}


//---------------------------------------------------------------
//
// CExpH::OnComputeDwell								
//
// h = l*e^h + c
//
//---------------------------------------------------------------
uint32 CExpH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;
	
	double lx = mLambda.x;
	double ly = mLambda.y;
	double lz = mLambda.z;
	double lw = mLambda.w;

	double tx, ty, tz, tw;
	while (dwell < maxDwell && mag < bailout) {
		ExpH(tx, ty, tz, tw, x, y, z, w);			// temp = exp(h)
		MultH(tx, ty, tz, tw, lx, ly, lz, lw);		// temp *= l
		
		x = tx + cx;								// h = temp + c
		y = ty + cy;
		z = tz + cz;
		w = tw + cw;

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CSinSqrH
// ===================================================================================
class CSinSqrH : public CFractalFunction {

public:	
  						CSinSqrH(XBoss* boss);

	virtual std::wstring GetFormula() const					{return L"h^2 + sin(h) + c";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CSinSqrH)

//---------------------------------------------------------------
//
// CSinSqrH::CSinSqrH
//
//---------------------------------------------------------------
CSinSqrH::CSinSqrH(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CSinSqrH::OnComputeDwell							
//
// h = h^2 + sin(h) + c	
//
//---------------------------------------------------------------
uint32 CSinSqrH::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{	
	uint32 dwell = 0;
	double mag = 0.0;
					
#if 1
	double tx, ty, tz, tw;
	while (dwell < maxDwell && mag < bailout) {
		SqrH(tx, ty, tz, tw, x, y, z, w);
		
		SinH(x, y, z, w, x, y, z, w);
		AddH(x, y, z, w, tx, ty, tz, tw);
		
		AddH(x, y, z, w, cx, cy, cz, cw);

		mag = x*x + y*y + z*z + w*w;

		++dwell;
	}

#else
	CHyperComplex h((float) x, (float) y, (float) z, (float) w);
		
	CHyperComplex c((float) cx, (float) cy, (float) cz, (float) cw);

	while (dwell < maxDwell && mag < bailout) {
		h = sqr(h) + sin(h);
		h += c;
		
		mag = mag2(h);
		++dwell;
	}
#endif

	return dwell;
}

#pragma mark -

// ===================================================================================
//	class CSphere
// ===================================================================================
class CSphere : public CFractalFunction {

public:	
  						CSphere(XBoss* boss);

	virtual std::wstring  GetFormula() const					{return L"Sphere";}

protected:
	virtual uint32 		OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const;
};

DEFINE_INTERFACE_FACTORY(CSphere)

//---------------------------------------------------------------
//
// CSphere::CSphere
//
//---------------------------------------------------------------
CSphere::CSphere(XBoss* boss) : CFractalFunction(boss)
{
	mUsesConstant = true;
	mUsesLambda   = false;
}


//---------------------------------------------------------------
//
// CSphere::OnComputeDwell							
//
//---------------------------------------------------------------
uint32 CSphere::OnComputeDwell(double x, double y, double z, double w, double cx, double cy, double cz, double cw, double bailout, uint32 maxDwell) const
{			
	UNUSED(w);
	UNUSED(cw);
	UNUSED(bailout);
	
	double mag = x*x + y*y + z*z;
	double r2 = cx*cx + cy*cy + cz*cz;

	uint32 dwell = 0;
	if (mag <= 1.0)
		dwell = maxDwell;
		
	return dwell;
}

