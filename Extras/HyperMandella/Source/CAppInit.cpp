/*
 *  File:       CAppInit.cpp
 *  Summary:   	The application object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAppInit.cpp,v $
 *		Revision 1.4  2001/04/27 09:08:03  jesjones
 *		Nuked SetSleepTime. Made render delay shorter.
 *		
 *		Revision 1.3  2001/03/28 09:36:02  jesjones
 *		Added a comment.
 *		
 *		Revision 1.2  2001/02/09 04:29:19  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.1  2000/12/28 02:48:06  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.3  2000/11/26 03:24:11  jesjones
 *		Moved menu stuff into CInitMenuBar.
 *		
 *		Revision 1.2  2000/11/09 08:52:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include "AppHeader.h"
#include <IInit.h>

#include <IApplication.h>
#include "IControl.h"
#include <IDocument.h>
#include <IDocView.h>
#include <IHierarchy.h>
#include "IRootControl.h"
#include <ITimer.h>
#include <IWindow.h>
#include <IWindowMgr.h>
#include <XURI.h>

#include "IDocInfo.h"
#include "IRenderer.h"


//-----------------------------------
//	Constants
//
const MilliSecond kRenderInterval = 25;		
const MilliSecond kIdleInterval   = 1000;		// don't suck up CPU time if we're not rendering


// ===================================================================================
//	class CAppInit
//!		The application object.
// ===================================================================================
class CAppInit : public IInit {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CAppInit();
	
  						CAppInit(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		OnInit();
	virtual void		OnCleanup();
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoRender();

//-----------------------------------
//	Member Data
//
private:
	bool	mRendering;
};

DEFINE_INTERFACE_FACTORY(CAppInit)

//---------------------------------------------------------------
//
// CAppInit::~CAppInit
//
//---------------------------------------------------------------
CAppInit::~CAppInit()
{
}


//---------------------------------------------------------------
//
// CAppInit::CAppInit
//
//---------------------------------------------------------------
CAppInit::CAppInit(XBoss* boss) 
{
	IInit::DoSetBoss(boss);

	mRendering = false;
}
	
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAppInit::OnInit		
//
//---------------------------------------------------------------
void CAppInit::OnInit()
{	
	ITimerPtr timer(this);   
	XCallback0<void> callback(this, &CAppInit::DoRender);
	timer->AddTimer(callback, kIdleInterval);
}


//---------------------------------------------------------------
//
// CAppInit::OnCleanup		
//
//---------------------------------------------------------------
void CAppInit::OnCleanup()
{
	ITimerPtr timer(this);   
	XCallback0<void> callback(this, &CAppInit::DoRender);
	timer->RemoveTimer(callback);
}
	
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAppInit::DoRender		
//
//---------------------------------------------------------------
void CAppInit::DoRender()
{	
	bool rendering = false;
	
	IWindowMgrPtr mgr(this);
	IWindowMgr::iterator iter = mgr->begin();
	while (iter != mgr->end() && !rendering) {
		IWindowPtr window = *iter;
		++iter;
		
		IRootControlPtr root = window->GetRootControl();
		if (root) {
			IControlPtr control = root->FindChild(L"Fractal Control");
			if (control) {
				IRendererPtr renderer(control);
	
				if (!renderer->IsDone()) {
					int32 percent = renderer->Render(200);
					
					IDocumentPtr doc = GetParent<IDocument>(window);
					doc->SetDirty(true, kRenderedImage, XAnyValue(percent));
					
					rendering = true;
				}
			}
		}
	}	
	
	if (rendering != mRendering) {
		ITimerPtr timer(this);   
		XCallback0<void> callback(this, &CAppInit::DoRender);

		if (rendering)
			timer->SetInterval(callback, kRenderInterval);
		else
			timer->SetInterval(callback, kIdleInterval);
			
		mRendering = rendering;
	}
}

