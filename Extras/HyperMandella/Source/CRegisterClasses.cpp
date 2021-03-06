/*
 *  File:       CRegisterClasses.cpp
 *  Summary:   	Registers the interface and implementation classes used by the app. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRegisterClasses.cpp,v $
 *		Revision 1.14  2001/04/05 23:52:17  jesjones
 *		Added pref dialog.
 *		
 *		Revision 1.13  2001/04/03 04:37:13  jesjones
 *		More stuff
 *		
 *		Revision 1.12  2001/04/01 04:52:11  jesjones
 *		Added add palette and delete palette dialogs.
 *		
 *		Revision 1.11  2001/03/30 04:11:48  jesjones
 *		Added Color menu.
 *		
 *		Revision 1.10  2001/03/28 09:39:53  jesjones
 *		More stuff
 *		
 *		Revision 1.9  2001/03/27 06:27:51  jesjones
 *		Added camera dialog.
 *		
 *		Revision 1.8  2001/03/26 04:27:03  jesjones
 *		More dialogs.
 *		
 *		Revision 1.7  2001/03/25 06:06:06  jesjones
 *		Made the dialog code a bit more reuseable by introducing IDocCommands, IFloatDialog, and IComplexDialog.
 *		
 *		Revision 1.6  2001/03/23 10:52:50  jesjones
 *		Added W dialog.
 *		
 *		Revision 1.5  2001/03/03 11:15:28  jesjones
 *		Instead of specializing selected algorithms for POD types, we use the new MSL mechanism to declare POD types.
 *		
 *		Revision 1.4  2001/02/28 09:52:09  jesjones
 *		Added a bailout dialog.
 *		
 *		Revision 1.3  2001/02/09 04:29:54  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.2  2000/12/31 04:19:02  jesjones
 *		Fixed a comment.
 *		
 *		Revision 1.1  2000/12/28 02:50:48  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.4  2000/12/05 09:04:22  jesjones
 *		Registered some new stuff.
 *		
 *		Revision 1.3  2000/11/26 03:22:23  jesjones
 *		Added CInitMenuBar.
 *		
 *		Revision 1.2  2000/11/09 08:57:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include "AppHeader.h"
#include "CRegisterClasses.h"

#include <IReadXML.h>
#include <IWriteXML.h>
#include <XObjectModel.h>
#include <XRegisterUI.h>
#include <XRegisterXMLLoaders.h>
#include <XTranscode.h>

#include "CPersistXML.h"
#include "IAppDialog.h"
#include "ICamera.h"
#include "IColorFormulas.h"
#include "IComplexDialog.h"
#include "IDirectionalLight.h"
#include "IDocCommands.h"
#include "IDocDialog.h"
#include "IDocInfo.h"
#include "IFloatDialog.h"
#include "IFormulaState.h"
#include "IFractalFunction.h"
#include "IInt32Dialog.h"
#include "ILight.h"
#include "IOrthographicCamera.h"
#include "IPalettes.h"
#include "IPointLight.h"
#include "IPointLightDialog.h"
#include "IRandomPalette.h"
#include "IReflectedLight.h"
#include "IRenderer.h"
#include "IShader.h"
#include "ITemporaryPalettes.h"


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterWhisperImplementations	
// 
//---------------------------------------------------------------
namespace Whisper {
static void RegisterWhisperImplementations()
{	
#if 1
	RegisterUIImplementations();		// $$$ better to register just what we use
#else
	REGISTER_INTERFACE_FACTORY(XCallbackCommand, L"XCallbackCommand");
	REGISTER_INTERFACE_FACTORY(XClickExtensible, L"XClickExtensible");
	REGISTER_INTERFACE_FACTORY(XCommandClickExtension, L"XCommandClickExtension");
	REGISTER_INTERFACE_FACTORY(XCommandQueue, L"XCommandQueue");
	REGISTER_INTERFACE_FACTORY(XControlMouseHandler, L"XControlMouseHandler");
	REGISTER_INTERFACE_FACTORY(XFocus, L"Focus");
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");
	REGISTER_INTERFACE_FACTORY(XIdler, L"XIdler");
	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader");
	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	REGISTER_INTERFACE_FACTORY(XWindow, L"Window");
	REGISTER_INTERFACE_FACTORY(XWindowFocus, L"XWindowFocus");
	
	REGISTER_INTERFACE_FACTORY(M16BitValueControl, L"M16BitValueControl");
	REGISTER_INTERFACE_FACTORY(MControl, L"Control");
	REGISTER_INTERFACE_FACTORY(MControlText, L"ControlText");
	REGISTER_INTERFACE_FACTORY(MDrawControl, L"DrawControl");
	REGISTER_INTERFACE_FACTORY(MDrawWindow, L"DrawWindow");
	REGISTER_INTERFACE_FACTORY(MEventDispatcher, L"MEventDispatcher");
	REGISTER_INTERFACE_FACTORY(MProgressBar, L"ProgressBar");
	REGISTER_INTERFACE_FACTORY(MPushButton, L"PushButton");
	REGISTER_INTERFACE_FACTORY(MRootControl, L"RootControl");
	REGISTER_INTERFACE_FACTORY(MRootGeometry, L"MRootGeometry");
	REGISTER_INTERFACE_FACTORY(MStaticText, L"MStaticText");
	REGISTER_INTERFACE_FACTORY(MWindowGeometry, L"WindowGeometry");
	REGISTER_INTERFACE_FACTORY(MWindowMgr, L"WindowMgr");
	REGISTER_INTERFACE_FACTORY(MWindowMouseHandler, L"WindowMouseHandler");
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
	RegisterWhisperImplementations();	

	// Register our implementation classes.
	REGISTER_INTERFACE_FACTORY(CAddPaletteDialog, L"CAddPaletteDialog");	
	REGISTER_INTERFACE_FACTORY(CAmbientLight, L"CAmbientLight");	
	REGISTER_INTERFACE_FACTORY(CAppInit, L"CAppInit");
	REGISTER_INTERFACE_FACTORY(CAppMenuHandler, L"CAppMenuHandler");
		
	REGISTER_INTERFACE_FACTORY(CCameraDialog, L"CCameraDialog");
	REGISTER_INTERFACE_FACTORY(CCubeH, L"CCubeH");
	REGISTER_INTERFACE_FACTORY(CCubeQ, L"CCubeQ");
	REGISTER_INTERFACE_FACTORY(CColorFormulas, L"CColorFormulas");
	REGISTER_INTERFACE_FACTORY(CComplexDialog, L"CComplexDialog");

	REGISTER_INTERFACE_FACTORY(CDeleteColorFormulaDialog, L"CDeleteColorFormulaDialog");	
	REGISTER_INTERFACE_FACTORY(CDeletePaletteDialog, L"CDeletePaletteDialog");			
	REGISTER_INTERFACE_FACTORY(CDirectionalLight, L"CDirectionalLight");		
	REGISTER_INTERFACE_FACTORY(CDocCommands, L"CDocCommands");
	REGISTER_INTERFACE_FACTORY(CDocInfo, L"CDocInfo");
	REGISTER_INTERFACE_FACTORY(CDocMenuHandler, L"CDocMenuHandler");
	REGISTER_INTERFACE_FACTORY(CDrawFractal, L"CDrawFractal");
	
	REGISTER_INTERFACE_FACTORY(CEditColorFormulaDialog, L"CEditColorFormulaDialog");	
	REGISTER_INTERFACE_FACTORY(CEPwrH, L"CEPwrH");	
	REGISTER_INTERFACE_FACTORY(CExpH, L"CExpH");
	
	REGISTER_INTERFACE_FACTORY(CFloatDialog, L"CFloatDialog");
	REGISTER_INTERFACE_FACTORY(CFormulaState, L"CFormulaState");
	REGISTER_INTERFACE_FACTORY(CFractalMouseHandler, L"CFractalMouseHandler");

	REGISTER_INTERFACE_FACTORY(CInfoWriter, L"CInfoWriter");
	REGISTER_INTERFACE_FACTORY(CInitMenuBar, L"CInitMenuBar");
	REGISTER_INTERFACE_FACTORY(CInfoReader, L"CInfoReader");
	REGISTER_INTERFACE_FACTORY(CInt32Dialog, L"CInt32Dialog");

	REGISTER_INTERFACE_FACTORY(CLambdaQ, L"CLambdaQ");
	REGISTER_INTERFACE_FACTORY(CLambertShader, L"CLambertShader");

	REGISTER_INTERFACE_FACTORY(COrthographicCamera, L"COrthographicCamera");	
	
	REGISTER_INTERFACE_FACTORY(CPalettes, L"CPalettes");
	REGISTER_INTERFACE_FACTORY(CPhongShader, L"CPhongShader");
	REGISTER_INTERFACE_FACTORY(CPiPwrH, L"CPiPwrH");
	REGISTER_INTERFACE_FACTORY(CPngWriter, L"CPngWriter");
	REGISTER_INTERFACE_FACTORY(CPngReader, L"CPngReader");
	REGISTER_INTERFACE_FACTORY(CPointLight, L"CPointLight");
	REGISTER_INTERFACE_FACTORY(CPointLightDialog, L"CPointLightDialog");
	REGISTER_INTERFACE_FACTORY(CPotts1H, L"CPotts1H");
	REGISTER_INTERFACE_FACTORY(CPotts1Q, L"CPotts1Q");
	REGISTER_INTERFACE_FACTORY(CPotts2H, L"CPotts2H");
	REGISTER_INTERFACE_FACTORY(CPotts2Q, L"CPotts2Q");
	REGISTER_INTERFACE_FACTORY(CPrefDialog, L"CPrefDialog");
	
	REGISTER_INTERFACE_FACTORY(CQuadH, L"CQuadH");
	REGISTER_INTERFACE_FACTORY(CQuadQ, L"CQuadQ");	

	REGISTER_INTERFACE_FACTORY(CRandomPalette, L"CRandomPalette");
	REGISTER_INTERFACE_FACTORY(CRandomPaletteDialog, L"CRandomPaletteDialog");
	REGISTER_INTERFACE_FACTORY(CRenderer, L"CRenderer");
	REGISTER_INTERFACE_FACTORY(CResolutionDialog, L"CResolutionDialog");	

	REGISTER_INTERFACE_FACTORY(CSinSqrH, L"CSinSqrH");
	REGISTER_INTERFACE_FACTORY(CSqrH, L"CSqrH");
	REGISTER_INTERFACE_FACTORY(CSqrQ, L"CSqrQ");
	REGISTER_INTERFACE_FACTORY(CSqrtH, L"CSqrtH");	
	REGISTER_INTERFACE_FACTORY(CSphere, L"CSphere");

	REGISTER_INTERFACE_FACTORY(CTemporaryPalettes, L"CTemporaryPalettes");
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
	REGISTER_INTERFACE_NAME(IAppDialog, L"IAppDialog");		
	REGISTER_INTERFACE_NAME(ICamera, L"ICamera");		
	REGISTER_INTERFACE_NAME(IColorFormulas, L"IColorFormulas");
	REGISTER_INTERFACE_NAME(IComplexDialog, L"IComplexDialog");
	REGISTER_INTERFACE_NAME(IDirectionalLight, L"IDirectionalLight");		
	REGISTER_INTERFACE_NAME(IDocCommands, L"IDocCommands");
	REGISTER_INTERFACE_NAME(IDocDialog, L"IDocDialog");
	REGISTER_INTERFACE_NAME(IDocInfo, L"IDocInfo");
	REGISTER_INTERFACE_NAME(IFloatDialog, L"IFloatDialog");
	REGISTER_INTERFACE_NAME(IFormulaState, L"IFormulaState");
	REGISTER_INTERFACE_NAME(IFractalFunction, L"IFractalFunction");
	REGISTER_INTERFACE_NAME(IInt32Dialog, L"IInt32Dialog");
	REGISTER_INTERFACE_NAME(ILight, L"ILight");
	REGISTER_INTERFACE_NAME(IOrthographicCamera, L"IOrthographicCamera");		
	REGISTER_INTERFACE_NAME(IPalettes, L"IPalettes");
	REGISTER_INTERFACE_NAME(IPointLight, L"IPointLight");
	REGISTER_INTERFACE_NAME(IPointLightDialog, L"IPointLightDialog");	
	REGISTER_INTERFACE_NAME(IRandomPalette, L"IRandomPalette");
	REGISTER_INTERFACE_NAME(IReflectedLight, L"IReflectedLight");	
	REGISTER_INTERFACE_NAME(IRenderer, L"IRenderer");
	REGISTER_INTERFACE_NAME(IShader, L"IShader");
	REGISTER_INTERFACE_NAME(ITemporaryPalettes, L"ITemporaryPalettes");
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
	RegisterWhisperLoaders();		
	
	// Register our XML readers and writers
	IWriteXMLPtr writer(L"XML Persist");
	IReadXMLPtr reader(L"XML Persist");
	
	writer->AddWriter(L"IDocInfo", WriteDocInfo);
	reader->AddReader(L"IDocInfo", ReadDocInfo);

	writer->AddWriter(L"ICamera", WriteCamera);
	reader->AddReader(L"ICamera", ReadCamera);

	writer->AddWriter(L"IOrthographicCamera", WriteOrthoCamera);
	reader->AddReader(L"IOrthographicCamera", ReadOrthoCamera);
}
