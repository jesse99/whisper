/*
 *  File:       CDrawFractal.cpp
 *  Summary:   	Draws the fractal.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDrawFractal.cpp,v $
 *		Revision 1.6  2001/03/26 04:31:22  jesjones
 *		DoImageChangedSize updates the renderer.
 *		
 *		Revision 1.5  2001/03/09 10:06:22  jesjones
 *		Removed the PNG test code.
 *		
 *		Revision 1.4  2001/02/25 10:48:11  jesjones
 *		Disabled the PNG test code.
 *		
 *		Revision 1.3  2001/02/25 10:18:55  jesjones
 *		Temporary PNG import test code.
 *		
 *		Revision 1.2  2001/02/09 04:29:43  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/28 02:49:19  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:54:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <IDrawHelper.h>

#include <IChildNode.h>
#include <IControl.h>
#include <IDocument.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <ILoaded.h>
#include <IScrollableControl.h>
#include <IWindow.h>
#include <XPixMap.h>
#include <XShapes.h>

#include <XFolderSpec.h>
#include <XResource.h>
#include <XImageImporters.h>

#include "IDocInfo.h"
#include "IRenderer.h"


// ===================================================================================
//	class CDrawFractal
//!		Draws the fractal.
// ===================================================================================
class CDrawFractal : public IDrawHelper, public ILoaded, public IChildNode {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDrawFractal();
	
  						CDrawFractal(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnLoaded();

	virtual void 		OnAdopted();
	virtual void 		OnOrphaned();

	virtual void 		OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive);
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoUpdate(const SDocumentMessage& mesg);		
			void 		DoImageChangedSize(bool redraw);
			
//-----------------------------------
//	Member Data
//
private:
	int32		mPercent;
};

DEFINE_INTERFACE_FACTORY(CDrawFractal)

//---------------------------------------------------------------
//
// CDrawFractal::~CDrawFractal
//
//---------------------------------------------------------------
CDrawFractal::~CDrawFractal()
{
}


//---------------------------------------------------------------
//
// CDrawFractal::CDrawFractal
//
//---------------------------------------------------------------
CDrawFractal::CDrawFractal(XBoss* boss) 
{
	IDrawHelper::DoSetBoss(boss);
	ILoaded::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);

	mPercent = 0;
}
	
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDrawFractal::OnLoaded
//
// Note that we can't do this in the ctor because we may not have
// been hooked up to our parent yet.
//
//---------------------------------------------------------------
void CDrawFractal::OnLoaded()
{
	IDrawHelper* thisPtr = this;
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);   
	IDocument::Callback callback(this, &CDrawFractal::DoUpdate);
	doc->AddCallback(callback);

	this->DoImageChangedSize(kDontRedraw);
}


//---------------------------------------------------------------
//
// CDrawFractal::OnAdopted
//
//---------------------------------------------------------------
void CDrawFractal::OnAdopted()
{
	// nothing to do
}


//---------------------------------------------------------------
//
// CDrawFractal::OnOrphaned
//
// Can't do this in the dtor because GetParent queries for
// IHierarchy amd queries in a dtor always return nil if the
// boss is being destroyed.
//
//---------------------------------------------------------------
void CDrawFractal::OnOrphaned()
{
	IDrawHelper* thisPtr = this;
	IDocumentPtr doc = GetParent<IDocument>(thisPtr);   
	IDocument::Callback callback(this, &CDrawFractal::DoUpdate);
	doc->RemoveCallback(callback);
}


//---------------------------------------------------------------
//
// CDrawFractal::OnDraw
//
//---------------------------------------------------------------
void CDrawFractal::OnDraw(XDrawContext& canvas, const XRegion& dirtyRgn, bool isActive)
{
	UNUSED(dirtyRgn);
	UNUSED(isActive);
	
	IDrawHelper* thisPtr = this;
	IRendererPtr renderer(thisPtr);
	const XPixMap* image = renderer->GetImage();
	
	if (mPercent > 0) {
		IGeometryPtr geometry(thisPtr);
		XRect extent = geometry->GetExtent();
		XSize imageSize = image->GetSize();
		
		int16 height = (int16) (mPercent*imageSize.height/100);
		
		XLineShape::Draw(canvas, XPoint(extent.left, 0), XSize(0, height), kRGBYellow);
		XLineShape::Draw(canvas, XPoint(extent.left, height), XPoint(extent.left, (int16) imageSize.height), kRGBBlack);
				
		extent.left += 1;

		XPixMapShape::Draw(canvas, *image, extent, extent);

	} else
		XPixMapShape::Draw(canvas, *image, kZeroPt);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDrawFractal::DoImageChangedSize
//
//---------------------------------------------------------------
void CDrawFractal::DoImageChangedSize(bool redraw)
{
	IDrawHelper* thisPtr = this;
	IDocInfoPtr info = GetParent<IDocInfo>(thisPtr);
	
	const SResolution& resolution = info->GetResolution();
	XSize imageSize = resolution.GetSize();
	
	IScrollableControlPtr control(thisPtr);

	if (control->GetImageSize() != imageSize) {
		control->SetImageSize(imageSize, redraw);
		
		IWindowPtr window = GetParent<IWindow>(thisPtr);
		IGeometryPtr geometry(window);

		XSize windSize = imageSize + XSize(15, 15);
		geometry->SetMaxSize(windSize);
		
		if (windSize.width <= 640 && windSize.height <= 480)
			geometry->SetSize(windSize);
	}

	IRendererPtr renderer(thisPtr);
	renderer->SetResolution(XSize(resolution.width, resolution.height), resolution.depth);
}


//---------------------------------------------------------------
//
// CDrawFractal::DoUpdate
//
//---------------------------------------------------------------
void CDrawFractal::DoUpdate(const SDocumentMessage& mesg)
{	
	IDrawHelper* thisPtr = this;
	IControlPtr control(thisPtr);

	switch (mesg.change) {
		case kChangedDocument:
			control->Invalidate();
			mPercent = 0;
			break;
			
		case kRenderedImage:
			int32 percent = mesg.data.Get<int32>();
			mPercent = percent < 100 ? percent : 0;
			control->Invalidate();
			break;
			
		case kResizedImage:
		case kOpenedDocument:
		case kRevertedDocument:
			this->DoImageChangedSize(kRedraw);
			mPercent = 0;
			break;
	};
}


