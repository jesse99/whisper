/*
 *  File:       CRenderer.cpp
 *  Summary:   	Interface that maintains the fractal parameters.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRenderer.cpp,v $
 *		Revision 1.10  2001/05/06 10:36:08  jesjones
 *		Tweaked so we don't try to normalize a zero length vector.
 *		
 *		Revision 1.9  2001/04/27 04:30:50  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.8  2001/04/21 02:25:05  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.7  2001/04/05 23:51:22  jesjones
 *		Added casts for the pickier Intel compiler.
 *		
 *		Revision 1.6  2001/04/04 04:52:40  jesjones
 *		Now anti-aliases.
 *		
 *		Revision 1.5  2001/03/26 04:26:51  jesjones
 *		Fixed some bugs in SetResolution.
 *		
 *		Revision 1.4  2001/03/23 10:55:51  jesjones
 *		DoReset zaps mDepths.
 *		
 *		Revision 1.3  2001/02/09 04:30:04  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2001/01/01 03:43:49  jesjones
 *		Fixed a profile target compile error.
 *		
 *		Revision 1.1  2000/12/28 02:50:56  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/25 00:32:07  jesjones
 *		XRGBColor color values are no longer clamped to [0.0, 1.0]. Added Normalize.
 *		
 *		Revision 1.3  2000/12/17 03:42:14  jesjones
 *		Uses XRGBColor instead of X3DRGBColor.
 *		
 *		Revision 1.2  2000/11/09 08:57:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include "IRenderer.h"

#include <IChildNode.h>
#include <IDocument.h>
#include <IHierarchy.h>
#include <ILoaded.h>
#include <XArray.h>
#include <XAutoPtr.h>
#include <XMiscUtils.h>
#include <XPixMap.h>
#include <XPreference.h>

#include "CColorEvaluator.h"
#include "CDissolve.h"
#include "ICamera.h"
#include "IDocInfo.h"
#include "IFractalFunction.h"
#include "IOrthographicCamera.h"
#include "IShader.h"

#if __profile__
//	#include <MProfiler.h>
#endif

#define PROFILE_RENDER		(__profile__ && 0)


//-----------------------------------
//	Constants
//
const float kNotComputed = (float) -INFINITY;
const float kHitYonPlane = (float) +INFINITY;

const int32 kSuperSampleSize = 9;				// best if a multiple of 3


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// InsideSet
//
//---------------------------------------------------------------
inline bool InsideSet(float depth)
{
	bool inside = depth != kNotComputed && depth != kHitYonPlane;
	
	return inside;
}


//---------------------------------------------------------------
//
// Interpolate
//
//---------------------------------------------------------------
static float Interpolate(double a, double b, double index)
{
	double result = (a + (b - a)*index);
	
	POSTCONDITION(result >= 0.0 && result <= 65535.0);
				
	return (float) result;
}


//---------------------------------------------------------------
//
// GetColor
//
//---------------------------------------------------------------
static XRGBColor GetColor(const XColorTable& colors, double index)
{
	PRECONDITION(index >= 0.0 && index < colors.GetSize());

	int16 start = (int16) index;
	
	XRGBColor color;

	if (start+1 <= colors.GetSize()-1) {
		XRGBColor lhs = colors.GetColor(start);
		XRGBColor rhs = colors.GetColor(start+1UL);
		double delta = index - floor(index);
		
		color = lhs;
		
		color.red   = Interpolate(lhs.red, rhs.red, delta);			// uses a linear interpolation to compute the components
		color.green = Interpolate(lhs.green, rhs.green, delta);
		color.blue  = Interpolate(lhs.blue, rhs.blue, delta);
		
	} else
		color = colors.GetColor(start);

	return color;
}

#pragma mark -

// ===================================================================================
//	class CRenderer
// ===================================================================================
class CRenderer : public IRenderer, public ILoaded, public IChildNode {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CRenderer();
	
						CRenderer(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:						
	virtual bool 		IsDone() const;	
	virtual int32 		Render(MilliSecond delay);
	
	virtual void 		SetResolution(const XSize& resolution, int32 depth);

	virtual const XPixMap* GetImage() const;

	virtual void 		Reset(const IDocInfoPtr& doc);
	virtual void 		Reset();

protected:
	virtual void 		OnLoaded();
						
	virtual void 		OnAdopted();
	virtual void 		OnOrphaned();

//-----------------------------------
//	Internal API
//
private:
			void 		DoReset(const SDocumentMessage& message);
			
			X3DVector 	DoNeighborhoodCast(const IConstFractalFunctionPtr& function, int32 h, int32 v);
			X3DVector 	DoRayCast(const IConstFractalFunctionPtr& function, int32 h, int32 v);
			void 		DoRender(const IConstShaderPtr& shader, int32 h, int32 v, const X3DVector& viewV);

			XRGBColor 	DoGetDiffuseColor(int32 h, int32 v) const;
			X3DVector 	DoComputeNormal(int32 h, int32 v) const;
			X3DPoint 	DoGetPoint(int32 h, int32 v) const;
			
			bool 		DoIsEdgePixel(int32 h, int32 v) const;
			XRGBColor 	DoFilter(const XPixMap* image) const;
			

//-----------------------------------
//	Types
//
private:
	struct SAntiAlias {
		bool			enabled;
		IDocInfoPtr		doc;	
		ICameraPtr		camera;
		IRendererPtr	renderer;
		XPoint			pt;						// next point that needs to be antialiased
	};

//-----------------------------------
//	Member Data
//
private:
	IDocInfoPtr			mDocInfo;
	ICameraPtr			mCamera;
	XColorTable			mPalette;
	
	SFractalInfo		mFractalInfo;
	SResolution			mResolution;
	CColorEvaluator*	mColorExpr;
	
	float				mWidthStep;				// 1.0/mResolution.width
	float				mHeightStep;			// 1.0/mResolution.height
	
	XPixMap*			mImage;
	XArray<float>*		mDepths;				// distance from view pixel to the fractal surface
	CDissolve			mDissolve;				// randomly generates rays to be evaluated
	int32				mCount;					// number of rays that have been generated
		
	MilliSecond			mElapsedTime;			
	SAntiAlias			mAntiAliaser;
	
#if PROFILE_RENDER
	MProfiler*			mRenderProfile;
#endif
};

DEFINE_INTERFACE_FACTORY(CRenderer)

//---------------------------------------------------------------
//
// CRenderer::~CRenderer
//
//---------------------------------------------------------------
CRenderer::~CRenderer()
{
	delete mImage;
	delete mDepths;
	delete mColorExpr;

#if PROFILE_RENDER
	delete mRenderProfile;
#endif

	if (mAntiAliaser.doc)
		mAntiAliaser.renderer->Reset(IDocInfoPtr());		// have to break a cycle
}


//---------------------------------------------------------------
//
// CRenderer::CRenderer
//
//---------------------------------------------------------------
CRenderer::CRenderer(XBoss* boss) : mDissolve(kZeroSize)
{	
	IRenderer::DoSetBoss(boss);
	ILoaded::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
			
	mDepths = nil;	
	mImage  = nil;
	mColorExpr = nil;

	mCount = 0;	
	mElapsedTime = 0;
	mAntiAliaser.pt = kZeroPt;
	mAntiAliaser.enabled = false;

#if PROFILE_RENDER
	mRenderProfile = nil;
#endif
}


//---------------------------------------------------------------
//
// CRenderer::Reset (IDocInfoPtr)
//
//---------------------------------------------------------------
void CRenderer::Reset(const IDocInfoPtr& doc)
{
	IRenderer* thisPtr = this;

	mDocInfo = doc;  
	mCamera = ICameraPtr(doc);  
	  
	if (mDocInfo)
		this->DoReset(SDocumentMessage(nil, kChangedDocument));
}

						
//---------------------------------------------------------------
//
// CRenderer::Reset ()
//
//---------------------------------------------------------------
void CRenderer::Reset()
{
	mImage->Erase(0);
	mDepths->Set(kNotComputed);
	
	mDissolve.Reset();
	mCount = 0;
	mElapsedTime = 0;
}

						
//---------------------------------------------------------------
//
// CRenderer::GetImage
//
//---------------------------------------------------------------
const XPixMap* CRenderer::GetImage() const
{
	PRECONDITION(mImage != nil);
	
	return mImage;
}


//---------------------------------------------------------------
//
// CRenderer::IsDone
//
//---------------------------------------------------------------
bool CRenderer::IsDone() const
{
	bool done = false;
	
	if (mDissolve.IsDone())
		if (!mAntiAliaser.enabled || mAntiAliaser.pt.y >= mResolution.height)
			done = true;
	
	return done;
}


//---------------------------------------------------------------
//
// CRenderer::SetResolution
//
//---------------------------------------------------------------
void CRenderer::SetResolution(const XSize& resolution, int32 depth)
{
	if (resolution != mResolution.GetSize() || mImage == nil) {
		XAutoPtr<XPixMap> image(new XPixMap(resolution, nil, depth));	
		image->Erase(0);

		XArray<float>* depths = new XArray<float>(resolution, kNotComputed);	
	
		delete mImage;
		delete mDepths;
		mImage = image.Release();	
		mDepths = depths;	

		mWidthStep  = 1.0f/resolution.width;
		mHeightStep = 1.0f/resolution.height;
			
		mDissolve = CDissolve(resolution);
		mCount = 0;
			
		mResolution.width  = resolution.width;
		mResolution.height = resolution.height;
		
		mAntiAliaser.doc.Release();
		mAntiAliaser.pt = kZeroPt;
	}
}


//---------------------------------------------------------------
//
// CRenderer::Render
//
// $$$ Need to anti-alias (might be able to do this by creating
// $$$ a render boss that's a subset of the Document boss)
//
//---------------------------------------------------------------
int32 CRenderer::Render(MilliSecond delay)
{		
	PRECONDITION(mImage != nil);
	
	XLocker lock(mImage);				// speeds up SetPixelAt (and disables purging)

	MilliSecond startTime = GetMilliSeconds();
	MilliSecond stopTime = GetMilliSeconds() + delay;
	
	MilliSecond time = startTime;
	int32 percent;

	if (!mDissolve.IsDone()) {
		IConstShaderPtr shader = mDocInfo->GetShader();
		IConstFractalFunctionPtr function = mDocInfo->GetFractalFunction();

		while (time < stopTime && !mDissolve.IsDone()) {			
			XPoint nextRay = mDissolve.GetPoint();
			
			X3DVector eyeRay = this->DoNeighborhoodCast(function, nextRay.x, nextRay.y);		
			this->DoRender(shader, nextRay.x, nextRay.y, -eyeRay);
						
			mDissolve.FindNext();
			++mCount;

			time = GetMilliSeconds();
		}
		
		percent = (int32) (100.0*mCount/mResolution.GetSize().GetArea());	// use float since int32 can overflow for big images

	} else if (mAntiAliaser.enabled && mAntiAliaser.pt.y < mResolution.height) {		
		IConstCameraPtr oldCamera(mDocInfo);

		if (!mAntiAliaser.doc) {
			mAntiAliaser.doc = IDocInfoPtr(L"AntiAliaser");	
			mAntiAliaser.doc->Reset(mDocInfo);
			
			SCameraRange range;
			range.hither = 0.0f;
			range.yon    = oldCamera->GetRange().yon - oldCamera->GetRange().hither;

			ICameraPtr camera(mAntiAliaser.doc);
			camera->SetRange(range, false);

			IOrthographicCameraPtr ortho(mAntiAliaser.doc);
			ortho->SetViewSize(X2DSize(2.0f*mWidthStep, 2.0f*mHeightStep), false);
			
			SResolution resolution;
			resolution.width  = kSuperSampleSize;
			resolution.height = kSuperSampleSize;
			resolution.depth  = mDocInfo->GetResolution().depth;
			mAntiAliaser.doc->SetResolution(resolution, false);
			
			mAntiAliaser.camera = ICameraPtr(mAntiAliaser.doc);		// don't want to do queries in time critical code...
			mAntiAliaser.renderer = IRendererPtr(mAntiAliaser.doc);
			mAntiAliaser.renderer->Reset(mAntiAliaser.doc);
		}			

		const XPixMap* image = mAntiAliaser.renderer->GetImage();
		XLocker lock1(image);
		XLocker lock2(mImage);
	
		SCameraPlacement placement = oldCamera->GetPlacement();
				
		while (time < stopTime && mAntiAliaser.pt.y < mResolution.height) {	
			if (this->DoIsEdgePixel(mAntiAliaser.pt.x, mAntiAliaser.pt.y)) {	

				X2DPoint pixel(mAntiAliaser.pt.x*mWidthStep, mAntiAliaser.pt.y*mHeightStep);
				placement.location = oldCamera->GetHitherPoint(pixel);
				mAntiAliaser.camera->SetPlacement(placement, false, false);			
				
				mAntiAliaser.renderer->Reset();
				while (!mAntiAliaser.renderer->IsDone())
					mAntiAliaser.renderer->Render(1000);
					
				XRGBColor color = this->DoFilter(image);
				int32 value = mImage->ColorToPixel(color.GetOSColor());
				mImage->SetPixelAt(mAntiAliaser.pt.x, mAntiAliaser.pt.y, value);
			}
						
			mAntiAliaser.pt.x++;
			if (mAntiAliaser.pt.x >= mResolution.width) {
				mAntiAliaser.pt.x = 0;
				mAntiAliaser.pt.y++;
			}

			time = GetMilliSeconds();
		}
		
		percent = (int32) (100.0*mAntiAliaser.pt.y/mResolution.height);	

	} else {
		percent = 100;
		// $$$ delete mDepths
	}
		
	mElapsedTime += time - startTime;
		
	POSTCONDITION(percent >= 0 && percent <= 100);
		
	return percent;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CRenderer::OnLoaded
//
//---------------------------------------------------------------
void CRenderer::OnLoaded()
{
	IRenderer* thisPtr = this;
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);

	mDocInfo = IDocInfoPtr(doc);  
	mCamera = ICameraPtr(doc);  
	mAntiAliaser.enabled = true;
	   
	IDocument::Callback callback(this, &CRenderer::DoReset);
	doc->AddCallback(callback);

	this->DoReset(SDocumentMessage(doc.Get(), kChangedDocument));
}

						
//---------------------------------------------------------------
//
// CRenderer::OnAdopted
//
//---------------------------------------------------------------
void CRenderer::OnAdopted()
{
	// nothing to do
}


//---------------------------------------------------------------
//
// CRenderer::OnOrphaned
//
//---------------------------------------------------------------
void CRenderer::OnOrphaned()
{
	IRenderer* thisPtr = this;
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);

	IDocument::Callback callback(this, &CRenderer::DoReset);
	doc->RemoveCallback(callback);
	
	mCamera.Release();
	mDocInfo.Release();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CRenderer::DoReset
//
//---------------------------------------------------------------
void CRenderer::DoReset(const SDocumentMessage& message)
{
	if (message.change == kChangedDocument) {
		const SResolution& resolution = mDocInfo->GetResolution();

		XPreference<bool> useMillions(L"Use Millions of Colors", true);
		int32 depth = *useMillions ? 32 : 16;
		this->SetResolution(XSize(resolution.width, resolution.height), depth);
		mResolution.depth = resolution.depth;
				
		const SShaderInfo& shader = mDocInfo->GetShaderInfo();
		CColorEvaluator* expr = new CColorEvaluator(shader.colorFormula);
		delete mColorExpr;
		mColorExpr = expr;
		
		mFractalInfo = mDocInfo->GetFractalInfo();
		mPalette = mDocInfo->GetPalette();
		
		mImage->Erase(0);
		mDepths->Set(kNotComputed);
		
		mDissolve.Reset();
		mCount = 0;
		mElapsedTime = 0;
	}
}

#include <XOptimize.h>	

//---------------------------------------------------------------
//
// CRenderer::DoNeighborhoodCast
//
//---------------------------------------------------------------
X3DVector CRenderer::DoNeighborhoodCast(const IConstFractalFunctionPtr& function, int32 h, int32 v)
{	
	if (h > 0 && mDepths->Get(h - 1, v) == kNotComputed) 
		(void) this->DoRayCast(function, h - 1, v);			// do these so we can compute the normal at (h, v)
	
	if (h + 1 < mResolution.width && mDepths->Get(h + 1, v) == kNotComputed) 
		(void) this->DoRayCast(function, h + 1, v);
	
	if (v > 0 && mDepths->Get(h, v - 1) == kNotComputed) 
		(void) this->DoRayCast(function, h, v - 1);
	
	if (v + 1 < mResolution.height && mDepths->Get(h, v + 1) == kNotComputed) 
		(void) this->DoRayCast(function, h, v + 1);

	X3DVector eyeRay = this->DoRayCast(function, h, v);
	
	return eyeRay;
}


//---------------------------------------------------------------
//
// CRenderer::DoRayCast
//
//---------------------------------------------------------------
X3DVector CRenderer::DoRayCast(const IConstFractalFunctionPtr& function, int32 h, int32 v)
{
	X2DPoint pixel(h*mWidthStep, v*mHeightStep);
			
	X3DPoint hitherPt = mCamera->GetHitherPoint(pixel);
	X3DVector eyeRay = mCamera->GetEyeRay(hitherPt);

	if (mDepths->Get(h, v) == kNotComputed) {		
		float viewDepth = mCamera->GetRange().yon - mCamera->GetRange().hither;

		float depth = function->ComputeDepth(mFractalInfo, hitherPt, eyeRay, viewDepth, mResolution.depth);
		ASSERT(depth >= 0.0);
		
		mDepths->Set(h, v, depth);
	}
	
	return eyeRay;
}


//---------------------------------------------------------------
//
// CRenderer::DoRender
//
//---------------------------------------------------------------
void CRenderer::DoRender(const IConstShaderPtr& shader, int32 h, int32 v, const X3DVector& viewV)
{
#if PROFILE_RENDER
	if (mRenderProfile != nil)
		mRenderProfile->Enable();
#endif
	
	float depth = mDepths->Get(h, v);
	ASSERT(depth != kNotComputed);
	
	XRGBColor color;

	if (depth != kHitYonPlane) {		
		// Compute the normal for (h, v).
		X3DVector normalV = this->DoComputeNormal(h, v);
		
		if (normalV == kZero3DVector)			// if none of the neighbors are in the set we'll use a normal that points away from the eye
			normalV = -viewV;

		normalV = Normalize(normalV);

		// Compute the color at (h, v).
		XRGBColor diffuseColor = this->DoGetDiffuseColor(h, v);
				
		X3DPoint pt = this->DoGetPoint(h, v);

		XRGBColor shaderColor = shader->GetColor(IConstDocInfoPtr(mDocInfo), diffuseColor, pt, normalV, viewV);
		color = Normalize(shaderColor);
	
	} else
		color = kRGBBlack;
	
	int32 pixel = mImage->ColorToPixel(color.GetOSColor());
	mImage->SetPixelAt((int16) h, (int16) v, pixel);

#if PROFILE_RENDER
	if (mRenderProfile != nil)
		mRenderProfile->Disable();
#endif
}


//---------------------------------------------------------------
//
// CRenderer::DoGetPoint
//
//---------------------------------------------------------------
X3DPoint CRenderer::DoGetPoint(int32 h, int32 v) const
{
	float depth = mDepths->Get(h, v);
	ASSERT(InsideSet(depth));
	
	X2DPoint pixel(h*mWidthStep, v*mHeightStep);
	
	X3DPoint pt = mCamera->GetHitherPoint(pixel);
	pt += depth*mCamera->GetZAxis();
	
	return pt;
}


//---------------------------------------------------------------
//
// CRenderer::DoComputeNormal
//
// The normal at (h, v) is computed by averaging the normals of 
// the four normals on the triangles about (h, v):
//
//                     (h, v-1)
//                   /    |    \
//                  /  1  |  2  \
//                 /      |      \
//         (h-1, v)-----(h, v)----(h+1, v)
//                 \      |      /
//                  \  4  |  3  /
//                   \    |    /
//                    (h, v+1)
//
// We do this by treating two sides of a triangle as a vector and
// taking their cross product. Note that it's important that these
// vectors are constructed in a clockwise fashion:
//
// Triangle		Vector 1				Vector 2
//    1			(h-1, v) - (h, v)		(h, v) - (h, v-1)
//	  2			(h, v) - (h+1, v)		(h, v-1) - (h, v)
//	  3			(h+1, v) - (h, v)		(h, v) - (h, v+1)
//	  4			(h, v) - (h-1, v)		(h, v+1) - (h, v)
//
// Another way to compute the normal is to compute the gradient
// using equation below for each partial derivative
// 		f'(x) ~= (f(x + h) - f(x))/h	where h goes to zero
// However because we're dealing with fractals we do not have a
// continuous surface and the equation above will not converge.
//
// $$�This code isn't quite right: if (h, v) is on an edge and
// $$�the points on the outside of the edge are also in the set
// $$�we'll get a somewhat bogus normal.
//
//---------------------------------------------------------------
X3DVector CRenderer::DoComputeNormal(int32 h, int32 v) const
{
	X3DVector normalV = kZero3DVector;

	X3DPoint center = this->DoGetPoint(h, v);

	if (h > 0 && v > 0 && h + 1 < mResolution.width && v + 1 < mResolution.height &&
		InsideSet(mDepths->Get(h - 1, v)) && InsideSet(mDepths->Get(h, v - 1)) &&
		InsideSet(mDepths->Get(h + 1, v)) && InsideSet(mDepths->Get(h, v + 1))) {

		// Typically a point on the fractal is not on the edge of
		// the window and is surrounded by other points on the
		// fractal so we can compute the normal with the optimized
		// code below.
		X3DPoint left   = this->DoGetPoint(h - 1, v);
		X3DPoint right  = this->DoGetPoint(h + 1, v);
		X3DPoint top    = this->DoGetPoint(h, v - 1);
		X3DPoint bottom = this->DoGetPoint(h, v + 1);

		double lx = left.x - center.x;
		double ly = left.y - center.y;
		double lz = left.z - center.z;

		double rx = right.x - center.x;
		double ry = right.y - center.y;
		double rz = right.z - center.z;

		double tx = top.x - center.x;
		double ty = top.y - center.y;
		double tz = top.z - center.z;

		double bx = bottom.x - center.x;
		double by = bottom.y - center.y;
		double bz = bottom.z - center.z;

		// 1		
		normalV.x += (float) (ly*tz - lz*ty);
		normalV.y += (float) (lz*tx - lx*tz);
		normalV.z += (float) (lx*ty - ly*tx);

		// 4
		normalV.x += (float) (-ly*bz + lz*by);
		normalV.y += (float) (-lz*bx + lx*bz);
		normalV.z += (float) (-lx*by + ly*bx);

		// 2
		normalV.x += (float) (-ry*tz + rz*ty);
		normalV.y += (float) (-rz*tx + rx*tz);
		normalV.z += (float) (-rx*ty + ry*tx);

		// 3
		normalV.x += (float) (-by*rz + bz*ry);
		normalV.y += (float) (-bz*rx + bx*rz);
		normalV.z += (float) (-bx*ry + by*rx);

	} else {
		// Compute the normals for the four triangles about (h, v)
		// using a high level version of the above code.
		X3DVector normal1 = kZero3DVector;
		X3DVector normal2 = kZero3DVector;
		X3DVector normal3 = kZero3DVector;
		X3DVector normal4 = kZero3DVector;
		
		if (h > 0 && v > 0) 
			if (InsideSet(mDepths->Get(h - 1, v)) && InsideSet(mDepths->Get(h, v - 1)))
				normal1 = CrossProduct(X3DVector(this->DoGetPoint(h - 1, v) - center), X3DVector(center - this->DoGetPoint(h, v - 1)));
		
		if (h + 1 < mResolution.width && v > 0) 
			if (InsideSet(mDepths->Get(h + 1, v)) && InsideSet(mDepths->Get(h, v - 1)))
				normal2 = CrossProduct(X3DVector(center - this->DoGetPoint(h + 1, v)), X3DVector(this->DoGetPoint(h, v - 1) - center));
		
		if (h + 1 < mResolution.width && v + 1 < mResolution.height) 
			if (InsideSet(mDepths->Get(h + 1, v)) && InsideSet(mDepths->Get(h, v + 1)))
				normal3 = CrossProduct(X3DVector(this->DoGetPoint(h + 1, v) - center), X3DVector(center - this->DoGetPoint(h, v + 1)));
		
		if (h > 0 && v + 1 < mResolution.height) 
			if (InsideSet(mDepths->Get(h - 1, v)) && InsideSet(mDepths->Get(h, v + 1)))
				normal4 = CrossProduct(X3DVector(center - this->DoGetPoint(h - 1, v)), X3DVector(this->DoGetPoint(h, v + 1) - center));
		
		// The normal at (h, v) is then the average of the four surrounding
		// normals (we don't divide by the number of normals because the
		// only scaling that matters happens in Normalize).
		normalV = normal1 + normal2 + normal3 + normal4;
	}

	return normalV;
}


//---------------------------------------------------------------
//
// CRenderer::DoGetDiffuseColor		
//
//---------------------------------------------------------------
XRGBColor CRenderer::DoGetDiffuseColor(int32 h, int32 v) const
{	
	X3DPoint pt = this->DoGetPoint(h, v);
	
	double index = mColorExpr->Evaluate(pt);
	
	index = fmod(index, 1.0);
	if (index < 0.0)
		index += 1.0;
	else if (isnan((float) index))
		index = 0.0;
	
	ASSERT(index >= 0.0 && index <= 1.0);
		
	uint32 numColors = mPalette.GetSize();
	double n = (numColors-1)*index;
	XRGBColor color = GetColor(mPalette, n);

	return color;
}


//---------------------------------------------------------------
//
// CRenderer::DoIsEdgePixel	
//
// Returns true if the specified point is on the fractal's edge
// (ie at least one of the neighboring points z value is much
// different).	
//
//---------------------------------------------------------------
bool CRenderer::DoIsEdgePixel(int32 h, int32 v) const
{
	bool isEdge = false;
		
	float viewDepth = mCamera->GetRange().yon - mCamera->GetRange().hither;
	const float kThreshold = 5.0f*viewDepth/mResolution.depth;

	float depth = mDepths->Get(h, v);
	ASSERT(depth != kNotComputed);
		
	if (h > 0 && Abs(mDepths->Get(h - 1, v) - depth) > kThreshold)
		isEdge = true;
	
	if (!isEdge && h + 1 < mResolution.width && Abs(mDepths->Get(h + 1, v) - depth) > kThreshold)
		isEdge = true;
	
	if (!isEdge && v > 0 && Abs(mDepths->Get(h, v - 1) - depth) > kThreshold)
		isEdge = true;
	
	if (!isEdge && v + 1 < mResolution.height && Abs(mDepths->Get(h, v + 1) - depth) > kThreshold)
		isEdge = true;
	
	return isEdge;
}


//---------------------------------------------------------------
//
// CRenderer::DoFilter
//
// Returns a color for the kSuperSampleSize x kSuperSampleSize
// image computed during the anti-alias phase. The box filter
// returns a simple average for those pixels which isn't too good
// because the high frequency components cause aliasing. The
// Mitchell and Netravali filter is a 2D low-passs filter so the
// aliasing effects are less pronounced.
//
//---------------------------------------------------------------
XRGBColor CRenderer::DoFilter(const XPixMap* image) const
{	
	PRECONDITION(image != nil);
	PRECONDITION(image->GetWidth() == kSuperSampleSize);
	PRECONDITION(image->GetHeight() == kSuperSampleSize);
	
	XRGBColor result(0.0f, 0.0f, 0.0f);

#if 0
	// Box filter
	for (int16 row = 0; row < kSuperSampleSize; row++) {
		for (int16 col = 0; col < kSuperSampleSize; col++) {
			int32 value = image->GetPixelAt(col, row);
			XRGBColor color = image->PixelToColor(value);
			
			result.red   += color.red;
			result.green += color.green;
			result.blue  += color.blue;
		}
	}

	float volume = kSuperSampleSize*kSuperSampleSize;
	
	result.red   /= volume;
	result.green /= volume;
	result.blue  /= volume;

#elif 1
	// Mitchell and Netravali filter (_Principles of Digital Image Synthesis Vol 1_ section 10.10.6)
	const double B = 0.3333;
	const double C = 0.3333;
	
	double volume = 0.0; 
	
	double scaleFactor = 4.0/sqrt(2.0*kSuperSampleSize*kSuperSampleSize);	// so r <= 2.0
	for (int16 row = 0; row < kSuperSampleSize; row++) {
		double dy = row - 0.5f*kSuperSampleSize;
		
		for (int16 col = 0; col < kSuperSampleSize; col++) {
			int32 value = image->GetPixelAt(col, row);
			XRGBColor color = image->PixelToColor(value);
			
			double dx = col - 0.5*kSuperSampleSize;
			double r = sqrt(dx*dx + dy*dy);
			r *= scaleFactor;
			ASSERT(r <= 2.0);
			
			double f;
			if (r < 1.0)
				f = (12.0 - 9.0*B - 6.0*C)*r*r*r + (-18.0 + 12.0*B + 6.0*C)*r*r + (6.0 - 2.0*B);
			else
				f = (-B - 6.0*C)*r*r*r + (6.0*B + 30.0*C)*r*r + (-12.0*B - 48.0*C)*r + (8.0*B + 24.0*C);
			
			f /= 6.0;
			
			volume += f;

			result.red   += (float) f*color.red;
			result.green += (float) f*color.green;
			result.blue  += (float) f*color.blue;
		}
	}
	
	result.red   = (float) MinMax(0.0, result.red/volume, 1.0);
	result.green = (float) MinMax(0.0, result.green/volume, 1.0);
	result.blue  = (float) MinMax(0.0, result.blue/volume, 1.0);
#endif
	
	return result;
}


