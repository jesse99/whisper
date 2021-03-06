/*
 *  File:       CDocMenuHandler.cpp
 *  Summary:   	The implementation that handles menu commands directed to documents.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDocMenuHandler.cpp,v $
 *		Revision 1.8  2001/04/02 23:35:26  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.7  2001/02/23 10:07:28  jesjones
 *		Limited the number of color submenu entries to 16 on Windows (app starts up too slowly if there are too many menu items).
 *		
 *		Revision 1.6  2001/02/17 10:24:51  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.5  2001/02/09 04:29:37  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.4  2001/01/30 07:07:40  jesjones
 *		Menu commands are now handled via callbacks.
 *		
 *		Revision 1.3  2001/01/21 00:22:37  jesjones
 *		Menu handling now uses callbacks.
 *		
 *		Revision 1.2  2001/01/01 03:42:13  jesjones
 *		Changed the app menu commands to use mixed case command names.
 *		
 *		Revision 1.1  2000/12/31 04:23:23  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IInstallMenuHandler.h>

#include <IApplication.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IDocView.h>
#include <IHierarchy.h>
#include <IMenuHandler.h>
#include <IReadXML.h>
#include <IUndoActions.h>
#include <XBind.h>
#include <XColorSet.h>
#include <XIntConversions.h>
#include <XURI.h>

#include "IDialog.h"
#include "IDocInfo.h"


// ===================================================================================
//	class CDocMenuHandler
//!		The implementation that handles menu commands directed to documents.
// ===================================================================================
class CDocMenuHandler : public IInstallMenuHandler {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CDocMenuHandler();
	
						CDocMenuHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		InstallCommands();
	virtual void 		RemoveCommands();

//-----------------------------------
//	Internal API
//
private:
			void 		DoRegisterColors(const XColorSet& colors, const std::wstring& command);
			void 		DoUnRegisterColors(const XColorSet& colors);

			void 		DoSetPenWidth();

			void 		DoCustomColor();
						
			void 		DoSetColor(const XRGBColor& color);
			SCommandStatus DoEnableColor(const XRGBColor& color) const;
};

DEFINE_INTERFACE_FACTORY(CDocMenuHandler)

//---------------------------------------------------------------
//
// CDocMenuHandler::~CDocMenuHandler
//
//---------------------------------------------------------------
CDocMenuHandler::~CDocMenuHandler()
{
}


//---------------------------------------------------------------
//
// CDocMenuHandler::CDocMenuHandler
//
//---------------------------------------------------------------
CDocMenuHandler::CDocMenuHandler(XBoss* boss)
{	
	IInstallMenuHandler::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::InstallCommands
//
// This is called when the document window is activated.
//
//---------------------------------------------------------------
void CDocMenuHandler::InstallCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// Pen Width
	XCallback0<void> action(this, &CDocMenuHandler::DoSetPenWidth);
	handler->RegisterCommand(L"Pen Width", action, kEnabledIfDocWindow, this);			// note that lower case menu commands are reserved for Whisper
	
	// Colors
	this->DoRegisterColors(kWhisperColors, L"Whisper Color: ");
	this->DoRegisterColors(kCrayolaColors, L"Crayola Color: ");
	this->DoRegisterColors(kHTMLColors, L"HTML Color: ");
	this->DoRegisterColors(kNetscapeColors, L"Netscape Color: ");

	// Custom Color
	action = XCallback0<void>(this, &CDocMenuHandler::DoCustomColor);
	handler->RegisterCommand(L"Custom Color", action, kEnabledIfDocWindow, this);
}
		

//---------------------------------------------------------------
//
// CDocMenuHandler::RemoveCommands
//
// This is called when the document window is deactivated.
//
//---------------------------------------------------------------
void CDocMenuHandler::RemoveCommands()
{
	IMenuHandlerPtr handler(L"Application");
	
	// Pen Width
	XCallback0<void> action(this, &CDocMenuHandler::DoSetPenWidth);
	handler->UnRegisterCommand(action);	
	
	// Colors
	this->DoUnRegisterColors(kWhisperColors);
	this->DoUnRegisterColors(kCrayolaColors);
	this->DoUnRegisterColors(kHTMLColors);
	this->DoUnRegisterColors(kNetscapeColors);

	// Custom Color
	action = XCallback0<void>(this, &CDocMenuHandler::DoCustomColor);
	handler->UnRegisterCommand(action);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CDocMenuHandler::DoRegisterColors
//
//---------------------------------------------------------------
void CDocMenuHandler::DoRegisterColors(const XColorSet& colors, const std::wstring& command)
{
	IMenuHandlerPtr handler(L"Application");
	
	XCallback1<SCommandStatus, XRGBColor> enableCallback(this, &CDocMenuHandler::DoEnableColor);
	XCallback1<void, XRGBColor> actionCallback(this, &CDocMenuHandler::DoSetColor);

	uint32 count = colors.size();
#if WIN
	count = std::min(count, 16UL);		// Windows bogs down pretty badly with long menus...
#endif
	for (uint32 i = 0; i < count; ++i) {
		XCallback0<void> action = Bind1(actionCallback, colors[i].color);
		XCallback0<SCommandStatus> enabler = Bind1(enableCallback, colors[i].color);
	
		handler->RegisterCommand(command + UInt32ToStr(i), action, enabler, this);
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoUnRegisterColors
//
//---------------------------------------------------------------
void CDocMenuHandler::DoUnRegisterColors(const XColorSet& colors)
{
	IMenuHandlerPtr handler(L"Application");
	
	XCallback1<void, XRGBColor> actionCallback(this, &CDocMenuHandler::DoSetColor);

	uint32 count = colors.size();
#if WIN
	count = std::min(count, 16UL);		// Windows bogs down pretty badly with long menus...
#endif
	for (uint32 i = 0; i < count; ++i) {
		XCallback0<void> action = Bind1(actionCallback, colors[i].color);
		handler->UnRegisterCommand(action);
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoSetPenWidth
//
//---------------------------------------------------------------
void CDocMenuHandler::DoSetPenWidth()
{
	IApplicationPtr app(L"Application");
	IHierarchy::NodePtr node(app);			// need this for Pro 5.3
	
	XURI uri(L"resource://Windows.bXML");
	IReadXMLPtr persist(L"XML Persist");
	IDialogPtr dialog = persist->CreateBoss(uri, L"Width_Dialog", node);
	
	IDocViewPtr view(this);
	IDocInfoPtr info(view->GetDoc());
	dialog->Pose(info);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoCustomColor
//
//---------------------------------------------------------------
void CDocMenuHandler::DoCustomColor()
{
	IConstDocViewPtr view(this);
	IConstDocInfoPtr doc(view->GetDoc());

	XRGBColor color = doc->GetColor();
	if (XRGBColor::Pick(LoadAppString(L"Pick a fill color"), &color))
		this->DoSetColor(color);
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoSetColor
//
//---------------------------------------------------------------
void CDocMenuHandler::DoSetColor(const XRGBColor& color)
{
	IDocViewPtr view(this);
	IDocInfoPtr doc(view->GetDoc());
	
	// If the color is changing then,
	XRGBColor oldColor = doc->GetColor();
	if (color != oldColor) {
		XCallback2<void, XRGBColor, bool> callback(doc.Get(), &IDocInfo::SetColor);
		XCallback0<void> doIt = Bind2(callback, color, true);
		XCallback0<void> undoIt = Bind2(callback, oldColor, true);

		IUndoCallbackAction::Post(LoadAppString(L"Change Color"), doIt, undoIt);
	}
}


//---------------------------------------------------------------
//
// CDocMenuHandler::DoEnableColor
//
//---------------------------------------------------------------
SCommandStatus CDocMenuHandler::DoEnableColor(const XRGBColor& color) const
{
	SCommandStatus status;
	
	status.enabled = true;

	IConstDocViewPtr view(this);
	IConstDocInfoPtr doc(view->GetDoc());
	status.checkmark = color == doc->GetColor() ? kEntireSelection+kIndependantCheckMark : kNoCheckmark;

	return status;
}

