/*
 *  File:       CRegisterClasses.cpp
 *  Summary:   	Registers the interface and implementation classes used by the app. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRegisterClasses.cpp,v $
 *		Revision 1.8  2002/01/29 06:50:19  jesjones
 *		Removed obsolete XIdler and MBalloonHelp registration.
 *		
 *		Revision 1.7  2001/03/17 12:57:49  jesjones
 *		Registered some more classes for the Mac release build.
 *		
 *		Revision 1.6  2001/03/17 12:24:00  jesjones
 *		Registered more classes for release builds.
 *		
 *		Revision 1.5  2001/03/12 02:36:06  jesjones
 *		Removed some obsolete code from the release version.
 *		
 *		Revision 1.4  2001/02/07 02:22:18  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2001/02/04 03:31:02  jesjones
 *		Removed implementation inheritance from the mouse handling code.
 *		
 *		Revision 1.2  2001/01/05 06:15:50  jesjones
 *		*** empty log message ***
 *		
 *		Revision 1.1  2000/12/31 04:23:48  jesjones
 *		Checked in
 */
 
#include "AppHeader.h"
#include "CRegisterClasses.h"

#include <IWriteXML.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XRegisterXMLLoaders.h>
#include <XXMLArchitecture.h>
#include <XXMLControls.h>
#include <XXMLWindows.h>

#include "CPersistXML.h"
#include "IDialog.h"
#include "IDocInfo.h"


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterWhisperImplementations	
// 
// To ease development we'll call RegisterUIImplementations for 
// debug builds and manually register factories in release to 
// allow the linker to dead-strip classes we don't use.
//
//---------------------------------------------------------------
namespace Whisper {
static void RegisterWhisperImplementations()
{	
#if DEBUG
	RegisterUIImplementations();		
#else
	REGISTER_INTERFACE_FACTORY(XActivateMenuHandlers, L"XActivateMenuHandlers");	
	REGISTER_INTERFACE_FACTORY(XActiveMenuBar, L"XActiveMenuBar");	
	REGISTER_INTERFACE_FACTORY(XActiveUndoContext, L"XActiveUndoContext");
	REGISTER_INTERFACE_FACTORY(XAppEventLoop, L"XAppEventLoop");
	REGISTER_INTERFACE_FACTORY(XAppFocus, L"XAppFocus");
	REGISTER_INTERFACE_FACTORY(XAppMenuHandler, L"XAppMenuHandler");
	REGISTER_INTERFACE_FACTORY(XApplication, L"XApplication");
	
	REGISTER_INTERFACE_FACTORY(XButtonHelp, L"XButtonHelp"); 
	
	REGISTER_INTERFACE_FACTORY(XCallbackClickExtension, L"XCallbackClickExtension");
	REGISTER_INTERFACE_FACTORY(XCallbackCommand, L"XCallbackCommand");
	REGISTER_INTERFACE_FACTORY(XClearAction, L"XClearAction");	
	REGISTER_INTERFACE_FACTORY(XClickExtensible, L"XClickExtensible");
	REGISTER_INTERFACE_FACTORY(XCommand, L"XCommand");
	REGISTER_INTERFACE_FACTORY(XCommandQueue, L"XCommandQueue");
	REGISTER_INTERFACE_FACTORY(XContextMenu, L"XContextMenu");
	REGISTER_INTERFACE_FACTORY(XControl, L"XControl");
	REGISTER_INTERFACE_FACTORY(XControlFocus, L"XControlFocus");
	REGISTER_INTERFACE_FACTORY(XControlGeometry, L"XControlGeometry");
	REGISTER_INTERFACE_FACTORY(XControlHelp, L"XControlHelp");
	REGISTER_INTERFACE_FACTORY(XControlMouseHandler, L"XControlMouseHandler");
	REGISTER_INTERFACE_FACTORY(XControlText, L"XControlText");
	REGISTER_INTERFACE_FACTORY(XCopyAction, L"XCopyAction");	
	REGISTER_INTERFACE_FACTORY(XCreateDoc, L"XCreateDoc");	
	REGISTER_INTERFACE_FACTORY(XCurrentFocus, L"XCurrentFocus");	
	REGISTER_INTERFACE_FACTORY(XCutAction, L"XCutAction");	
	
	REGISTER_INTERFACE_FACTORY(XDefaultChangedSize, L"XDefaultChangedSize");
	REGISTER_INTERFACE_FACTORY(XDefaultHelp, L"XDefaultHelp");
	REGISTER_INTERFACE_FACTORY(XDefaultDrawWindow, L"XDefaultDrawWindow");
	REGISTER_INTERFACE_FACTORY(XDefaultPositionWindow, L"XDefaultPositionWindow");
	REGISTER_INTERFACE_FACTORY(XDeferredErrorCommand, L"XDeferredErrorCommand");
	REGISTER_INTERFACE_FACTORY(XDialogBox, L"XDialogBox");
	REGISTER_INTERFACE_FACTORY(XDocAppMenuHandler, L"XDocAppMenuHandler");
	REGISTER_INTERFACE_FACTORY(XDocFocus, L"XDocFocus");
	REGISTER_INTERFACE_FACTORY(XDocMenuHandler, L"XDocMenuHandler");	
	REGISTER_INTERFACE_FACTORY(XDocument, L"XDocument");
	REGISTER_INTERFACE_FACTORY(XDocuments, L"XDocuments");
	REGISTER_INTERFACE_FACTORY(XDocView, L"XDocView");
	REGISTER_INTERFACE_FACTORY(XDocWindow, L"XDocWindow");
	REGISTER_INTERFACE_FACTORY(XDraw, L"XDraw");	
	REGISTER_INTERFACE_FACTORY(XDrawControl, L"XDrawControl");
			
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");	
	
	REGISTER_INTERFACE_FACTORY(XInstance, L"XInstance");
	REGISTER_INTERFACE_FACTORY(XIntValidator, L"XIntValidator");
	
	REGISTER_INTERFACE_FACTORY(XKeyHandler, L"XKeyHandler");
		
	REGISTER_INTERFACE_FACTORY(XMenu, L"XMenu");
	REGISTER_INTERFACE_FACTORY(XMenuBar, L"XMenuBar");
	REGISTER_INTERFACE_FACTORY(XMenuExtensible, L"XMenuExtensible");
	REGISTER_INTERFACE_FACTORY(XMenuHandler, L"XMenuHandler");
	REGISTER_INTERFACE_FACTORY(XMergeAction, L"XMergeAction");
	REGISTER_INTERFACE_FACTORY(XMouseHandler, L"XMouseHandler");

	REGISTER_INTERFACE_FACTORY(XNullDrawer, L"XNullDrawer");
	
	REGISTER_INTERFACE_FACTORY(XPasteAction, L"XPasteAction");	
	REGISTER_INTERFACE_FACTORY(XPersistDoc, L"XPersistDoc");
	REGISTER_INTERFACE_FACTORY(XPoseDialogBox, L"XPoseDialogBox");	
	REGISTER_INTERFACE_FACTORY(XPushButton, L"XPushButton");	
	
	REGISTER_INTERFACE_FACTORY(XReadXML, L"XReadXML");
	REGISTER_INTERFACE_FACTORY(XRestoreDocWindowPos, L"XRestoreDocWindowPos");
	REGISTER_INTERFACE_FACTORY(XRestoreWindowPosition, L"XRestoreWindowPosition");
	REGISTER_INTERFACE_FACTORY(XRootControl, L"XRootControl");
	REGISTER_INTERFACE_FACTORY(XRubberBandTracker, L"XRubberBandTracker");

	REGISTER_INTERFACE_FACTORY(XScrap, L"XScrap");	
	REGISTER_INTERFACE_FACTORY(XScrapMenuHandler, L"XScrapMenuHandler");
	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader"); 
	
	REGISTER_INTERFACE_FACTORY(XTabber, L"XTabber");
	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	REGISTER_INTERFACE_FACTORY(XTextBoxMenuHandler, L"XTextBoxMenuHandler");	
	REGISTER_INTERFACE_FACTORY(XTextInt32Value, L"XTextInt32Value");
	REGISTER_INTERFACE_FACTORY(XTracker, L"XTracker");
	
	REGISTER_INTERFACE_FACTORY(XUndoAction, L"XUndoAction");
	REGISTER_INTERFACE_FACTORY(XUndoCallbackAction, L"XUndoCallbackAction");
	REGISTER_INTERFACE_FACTORY(XUndoCommand, L"XUndoCommand");
	REGISTER_INTERFACE_FACTORY(XUndoContext, L"XUndoContext");
	REGISTER_INTERFACE_FACTORY(XUserControl, L"XUserControl");
	
	REGISTER_INTERFACE_FACTORY(XValidateText, L"XValidateText");	

	REGISTER_INTERFACE_FACTORY(XWindow, L"XWindow");
	REGISTER_INTERFACE_FACTORY(XWindowFocus, L"XWindowFocus");
	REGISTER_INTERFACE_FACTORY(XWindowGeometry, L"XWindowGeometry");
	REGISTER_INTERFACE_FACTORY(XWindowKeyHandler, L"XWindowKeyHandler");
	REGISTER_INTERFACE_FACTORY(XWindowMenuHandler, L"XWindowMenuHandler");
	REGISTER_INTERFACE_FACTORY(XWindowMgr, L"XWindowMgr");
	REGISTER_INTERFACE_FACTORY(XWriteXML, L"XWriteXML");
	
	REGISTER_INTERFACE_FACTORY(XXMLMenu, L"XXMLMenu");

#if MAC				
	REGISTER_INTERFACE_FACTORY(MDocWindowClick, L"MDocWindowClick");

	REGISTER_INTERFACE_FACTORY(MEventDispatcher, L"MEventDispatcher");
				
	REGISTER_INTERFACE_FACTORY(MRootGeometry, L"MRootGeometry");
	
	REGISTER_INTERFACE_FACTORY(MStaticText, L"MStaticText");
	
	REGISTER_INTERFACE_FACTORY(MTETextBox, L"MTETextBox");

	REGISTER_INTERFACE_FACTORY(MWindowClick, L"MWindowClick");
#endif

#if WIN
	REGISTER_INTERFACE_FACTORY(WButtonKeyHandler, L"WButtonKeyHandler");
	REGISTER_INTERFACE_FACTORY(WButtonValueControl, L"WButtonValueControl");
			
	REGISTER_INTERFACE_FACTORY(WFrameWindow, L"WFrameWindow");
	
	REGISTER_INTERFACE_FACTORY(WHTMLHelp, L"WHTMLHelp");	

	REGISTER_INTERFACE_FACTORY(WMDIHelp, L"WMDIHelp");
		
	REGISTER_INTERFACE_FACTORY(WStaticText, L"WStaticText");

	REGISTER_INTERFACE_FACTORY(WTextBox, L"WTextBox");
#endif
#endif
}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterImplementations	
// 
// When someone asks for an interface on a boss Whisper needs to
// create the interface using the implementation class listed in
// a Bosses.XML file. This requires looking up a factory function
// using an implementation name. The implementation classes define
// the factories using the DEFINE_INTERFACE_FACTORY macros, but
// they don't register the factories with the object model. So,
// we register them here. 
//
//---------------------------------------------------------------
void RegisterImplementations()
{	
	// Register the Whisper classes we're using.	
	RegisterWhisperImplementations();	

	// Register our implementation classes.
	REGISTER_INTERFACE_FACTORY(CAdjustCursor, L"CAdjustCursor");
	REGISTER_INTERFACE_FACTORY(CAppInit, L"CAppInit");
	REGISTER_INTERFACE_FACTORY(CAppMenuHandler, L"CAppMenuHandler");
	
	REGISTER_INTERFACE_FACTORY(CBinaryReader, L"CBinaryReader");
	REGISTER_INTERFACE_FACTORY(CBinaryWriter, L"CBinaryWriter");
	
	REGISTER_INTERFACE_FACTORY(CDocInfo, L"CDocInfo");
	REGISTER_INTERFACE_FACTORY(CDocMenuHandler, L"CDocMenuHandler");
	REGISTER_INTERFACE_FACTORY(CDrawDoc, L"CDrawDoc");
		
	REGISTER_INTERFACE_FACTORY(CInitMenuBar, L"CInitMenuBar");

	REGISTER_INTERFACE_FACTORY(CMouseHandler, L"CMouseHandler");

	REGISTER_INTERFACE_FACTORY(CWidthDialog, L"CWidthDialog");

	REGISTER_INTERFACE_FACTORY(CXMLReader, L"CXMLReader");
	REGISTER_INTERFACE_FACTORY(CXMLWriter, L"CXMLWriter");
}


//---------------------------------------------------------------
//
// RegisterInterfaces
//
//---------------------------------------------------------------
void RegisterInterfaces()
{
	// Tell the object model which interface class is associated
	// with each interface name. Note that we can do this for all
	// the Whisper interfaces because we're only dealing with
	// interfaces so there are no worries about implementation
	// code bloat.
	RegisterUIInterfaces();	

	// Register our interfaces.
	REGISTER_INTERFACE_NAME(IDialog, L"IDialog");
	REGISTER_INTERFACE_NAME(IDocInfo, L"IDocInfo");
}


//---------------------------------------------------------------
//
// RegisterLoaders
//
//---------------------------------------------------------------
void RegisterLoaders()
{
	// Register the Whisper readers. We'll go ahead and register all
	// the loaders (these talk to interfaces so they don't cause much
	// bloat).
#if 1
	RegisterWhisperLoaders();		
#else
	IReadXMLPtr persist(L"XML Persist");
	
	persist->AddReader(L"IBinding", LoadBinding);
	
	persist->AddReader(L"IControl", LoadControl);
	 
	persist->AddReader(L"IDefaultPositionWindow", LoadDefaultPositionWindow);
	persist->AddReader(L"IDialogBox", LoadDialogBox);

	persist->AddReader(L"IGeometry", LoadGeometry);
	
	persist->AddReader(L"IHierarchy", LoadHierarchy);
	
	persist->AddReader(L"IInt32Range", LoadInt32Range);

	persist->AddReader(L"IPushButton", LoadPushButton);
	
	persist->AddReader(L"IRootControl", LoadRootControl);
	
	persist->AddReader(L"ISetHelp", LoadSetHelp);
	persist->AddReader(L"IStaticText", LoadStaticText);

	persist->AddReader(L"ITextBox", LoadTextBox);
	
	persist->AddReader(L"IUserControl", LoadUserControl);
	
	persist->AddReader(L"IValidateText", LoadValidateText);
	
	persist->AddReader(L"IWindow", LoadWindow);
#endif
	
	// Register our XML readers and writers
	IWriteXMLPtr writer(L"XML Persist");
	IReadXMLPtr reader(L"XML Persist");
	
	writer->AddWriter(L"IDocInfo", WriteDocInfo);
	reader->AddReader(L"IDocInfo", ReadDocInfo);
}


