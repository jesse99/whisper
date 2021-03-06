/*
 *  File:       WAppEventLoop.cpp
 *  Summary:   	Main event loop.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WAppEventLoop.cpp,v $
 *		Revision 1.7  2001/04/27 09:14:08  jesjones
 *		Got rid of the processor hogging code (may need to put something like this back).
 *		
 *		Revision 1.6  2001/01/21 00:40:57  jesjones
 *		Changed the way accelerator keys work.
 *		
 *		Revision 1.5  2001/01/05 06:22:13  jesjones
 *		Implementation is no longer exposed.
 *		
 *		Revision 1.4  2001/01/03 05:53:36  jesjones
 *		No longer assumes the menubar never changes.
 *		
 *		Revision 1.3  2000/11/20 01:37:04  jesjones
 *		Got rid of the dollar signs in the initial checkin comment.
 *		
 *		Revision 1.2  2000/11/09 12:04:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IEventLoop.h>

#include <IActiveMenuBar.h>
#include <IApplication.h>
#include <ICommandQueue.h>
#include <IFrameWindow.h>
#include <IHierarchy.h>
#include <IMenuBar.h>
#include <XError.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XAppEventLoop
//!		Main event loop.
// ===================================================================================
class XAppEventLoop : public IEventLoop {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XAppEventLoop();
	
  						XAppEventLoop(XBoss* boss);

//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Run();

	virtual void 		SetErrHandler(ErrHandler handler)	{mErrHandler = handler;}

//-----------------------------------
//	Internal API
//
private:
	virtual void 		DoException(const std::exception& e);
						
	virtual	bool 		DoTranslateAccelerator(MSG& msg);

//-----------------------------------
//	Member Data
//
private:	
	ErrHandler	mErrHandler;
};

DEFINE_INTERFACE_FACTORY(XAppEventLoop)

//---------------------------------------------------------------
//
// XAppEventLoop::~XAppEventLoop
//
//---------------------------------------------------------------
XAppEventLoop::~XAppEventLoop()
{
}


//---------------------------------------------------------------
//
// XAppEventLoop::XAppEventLoop
//
//---------------------------------------------------------------
XAppEventLoop::XAppEventLoop(XBoss* boss) 
{	
	IEventLoop::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XAppEventLoop::Run
//
//---------------------------------------------------------------
void XAppEventLoop::Run()
{
	IApplicationPtr app(this);
	ICommandQueuePtr commands(this);
	IActiveMenuBarPtr activeMenubar(this);

	while (!app->IsQuitting()) {
		try {
			MSG msg;

			bool gotOne;
//			if (app->GetSleepTime() == 0)
//				gotOne = ::PeekMessage(&msg, nil, 0, 0, PM_REMOVE) > 0;	// $$$ may want to support this somehow (Windows timers have a pretty coarse resolution)
//			else
				gotOne = ::GetMessage(&msg, nil, 0, 0) >= 0;
				
			if (gotOne) {
				if (!this->DoTranslateAccelerator(msg)) {
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}

			app->HandleTime();

			if (commands && commands->ExecuteCommands()) {
				if (activeMenubar) {
					IMenuBarPtr menubar = activeMenubar->Get();
					if (menubar)
						menubar->Invalidate();
				}
			}
			
		} catch (const std::exception& e) {
			this->DoException(e);

		} catch (...) {
			this->DoException(std::runtime_error(ToUTF8Str(LoadWhisperString(L"Unknown error"))));
		}
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XAppEventLoop::DoException
//
//---------------------------------------------------------------
void XAppEventLoop::DoException(const std::exception& e)
{
	if (mErrHandler.IsValid())
		mErrHandler(e);
	else
		XError::Instance()->ReportError(LoadWhisperString(L"Couldn't complete the operation."), e);
}


//---------------------------------------------------------------
//
// XAppEventLoop::DoTranslateAccelerator
//
//---------------------------------------------------------------
bool XAppEventLoop::DoTranslateAccelerator(MSG& msg)
{
	bool handled = false;
	
	// If we're an MDI app we need to call TranslateMDISysAccel.
	IConstHierarchyPtr node(this);	
	XCallback1<bool, IHierarchyPtr> predicate(MatchInterface<IFrameWindow>());
	IFrameWindowPtr frame = node->FindChild(predicate, kNonRecursive);
	
	if (frame) 
		handled = ::TranslateMDISysAccel(frame->GetClientWindow(), &msg) != 0;

	// If the accelerator isn't for an MDI command see if it's used
	// by the active menubar.
	if (!handled) {
		IActiveMenuBarPtr activeMenubar(this);
		IMenuBarPtr menubar;
		if (activeMenubar) {
			menubar = activeMenubar->Get();
			
			if (menubar) {
				HACCEL accTable = menubar->GetAcceleratorTable();
				handled = accTable != nil && ::TranslateAccelerator(msg.hwnd, accTable, &msg);
			}
		}
	}
			
	return handled;
}


}	// namespace Whisper

