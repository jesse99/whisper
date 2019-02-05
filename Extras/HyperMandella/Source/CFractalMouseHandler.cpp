/*
 *  File:       CFractalMouseHandler.cpp
 *  Summary:   	Handles mouse clicks in the fractal window.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CFractalMouseHandler.cpp,v $
 *		Revision 1.3  2001/04/06 04:42:01  jesjones
 *		No changes (stupid WinCVS...)
 *		
 *		Revision 1.2  2001/04/04 04:51:09  jesjones
 *		Added context menu.
 *		
 *		Revision 1.1  2001/03/26 04:04:00  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include <IAdjustCursorHelper.h>
#include <IContextMenuHelper.h>
#include <IMouseDownHelper.h>

#include <IContextMenu.h>
#include <IDocument.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IMenu.h>
#include <XCursor.h>
#include <XMouseEvents.h>

#include "IDocCommands.h"
#include "IDocInfo.h"
#include "IFractalFunction.h"


//-----------------------------------
//	Constants
//
const XCursor kPanLeftCursor(256);
const XCursor kPanRightCursor(257);
const XCursor kPanUpCursor(258);
const XCursor kPanDownCursor(259);

const XCursor kOrbitLeftCursor(260);
const XCursor kOrbitRightCursor(261);
const XCursor kOrbitUpCursor(262);
const XCursor kOrbitDownCursor(263);

const XCursor kZoomInCursor(264);
const XCursor kZoomOutCursor(265);


// ===================================================================================
//	class CFractalMouseHandler
//!		Handles mouse clicks in the fractal window.
// ===================================================================================
class CFractalMouseHandler : public IAdjustCursorHelper, public IMouseDownHelper, public IContextMenuHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CFractalMouseHandler();
	
  						CFractalMouseHandler(XBoss* boss);	

//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnContextMenu(const XMouseDownEvent& event);
	virtual bool 		OnAdjustCursor(const XMouseMoveEvent& event);
	virtual bool 		OnMouseDown(const XMouseDownEvent& event);
	
//-----------------------------------
//	Internal API
//
private:
			int32 		DoGetQuadrant(const XPoint& pt) const;
};

DEFINE_INTERFACE_FACTORY(CFractalMouseHandler)

//---------------------------------------------------------------
//
// CFractalMouseHandler::~CFractalMouseHandler
//
//---------------------------------------------------------------
CFractalMouseHandler::~CFractalMouseHandler()
{
}


//---------------------------------------------------------------
//
// CFractalMouseHandler::CFractalMouseHandler
//
//---------------------------------------------------------------
CFractalMouseHandler::CFractalMouseHandler(XBoss* boss) 
{
	IAdjustCursorHelper::DoSetBoss(boss);
	IMouseDownHelper::DoSetBoss(boss);
	IContextMenuHelper::DoSetBoss(boss);
}
	
#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CFractalMouseHandler::OnContextMenu
//
//---------------------------------------------------------------
bool CFractalMouseHandler::OnContextMenu(const XMouseDownEvent& event)
{
	IAdjustCursorHelper* thisPtr = this;

	IContextMenuPtr popup(L"Context Menu");
	popup->Init(event.GetGlobalPosition());

	IMenuPtr menu(popup);
	menu->Init(L"Context Menu");
	
	menu->AppendItem(LoadAppString(L"Randomize W"), L"Randomize W");	
	
	IDocInfoPtr doc = GetParent<IDocInfo>(thisPtr);   
	IConstFractalFunctionPtr formula = doc->GetFractalFunction();
	
	if (formula->UsesConstant())
		menu->AppendItem(LoadAppString(L"Randomize Constant"), L"Randomize Constant");	
		
	if (formula->UsesLambda())
		menu->AppendItem(LoadAppString(L"Randomize Lambda"), L"Randomize Lambda");
			
	menu->AppendItem(LoadAppString(L"Max Dwell..."), L"Max Dwell");	
	menu->AppendSeparator();			
	
	menu->AppendItem(LoadAppString(L"Lambert Shader"), L"Lambert Shader");	
	menu->AppendItem(LoadAppString(L"Phong Shader"), L"Phong Shader");	
	menu->AppendSeparator();			

	menu->AppendItem(LoadAppString(L"Ambient Coefficient..."), L"Ambient Coefficient");	
	menu->AppendItem(LoadAppString(L"Diffuse Coefficient..."), L"Diffuse Coefficient");	
	menu->AppendItem(LoadAppString(L"Specular Coefficient..."), L"Specular Coefficient");	
	menu->AppendItem(LoadAppString(L"Specular Exponent..."), L"Specular Exponent");
	
#if DEBUG
	menu->AppendSeparator();			
	menu->AppendItem(LoadAppString(L"Make Default"), L"Make Default");	
#endif	

	popup->Execute();

	return kHandled;
}


//---------------------------------------------------------------
//
// CFractalMouseHandler::OnMouseDown
//
//---------------------------------------------------------------
bool CFractalMouseHandler::OnMouseDown(const XMouseDownEvent& event)
{
	XPoint loc = event.GetPosition();	
	int32 quadrant = this->DoGetQuadrant(loc);
	
	const IAdjustCursorHelper* thisPtr = this;
	IDocCommandsPtr commands = GetParent<IDocument>(thisPtr);   
	
	if (event.WasShiftKeyDown()) {			// $$$ drag if the option key is down
		switch (quadrant) {
			case 0:
			case 4:
				commands->Pan(0, -1);
				break;
				
			case 1:
				commands->Pan(+1, 0);
				break;
				
			case 2:
			case 5:
				commands->Pan(0, +1);
				break;
				
			case 3:
				commands->Pan(-1, 0);
				break;
			
			default:
				DEBUGSTR("Bad quadrant in CFractalMouseHandler::OnMouseDown");
		}
				
	} else {
		switch (quadrant) {
			case 0:
				commands->OrbitX(-1);
				break;
				
			case 1:
				commands->OrbitY(+1);
				break;
				
			case 2:
				commands->OrbitX(+1);
				break;
				
			case 3:
				commands->OrbitY(-1);
				break;
			
			case 4:
				commands->Zoom(false);
				break;
			
			case 5:
				commands->Zoom(true);
				break;
			
			default:
				DEBUGSTR("Bad quadrant in CFractalMouseHandler::OnMouseDown");
		}
	}
		
	return kHandled;
}


//---------------------------------------------------------------
//
// CFractalMouseHandler::OnAdjustCursor
//
//---------------------------------------------------------------
bool CFractalMouseHandler::OnAdjustCursor(const XMouseMoveEvent& event)
{
	XPoint loc = event.GetPosition();	
	int32 quadrant = this->DoGetQuadrant(loc);
	
	if (event.WasShiftKeyDown()) {
		switch (quadrant) {
			case 0:
			case 4:
				XCursorUtils::SetCursor(kPanUpCursor);
				break;
				
			case 1:
				XCursorUtils::SetCursor(kPanRightCursor);
				break;
				
			case 2:
			case 5:
				XCursorUtils::SetCursor(kPanDownCursor);
				break;
				
			case 3:
				XCursorUtils::SetCursor(kPanLeftCursor);
				break;
			
			default:
				DEBUGSTR("Bad quadrant in CFractalMouseHandler::OnAdjustCursor");
		}
				
	} else {
		switch (quadrant) {
			case 0:
				XCursorUtils::SetCursor(kOrbitUpCursor);
				break;
				
			case 1:
				XCursorUtils::SetCursor(kOrbitRightCursor);
				break;
				
			case 2:
				XCursorUtils::SetCursor(kOrbitDownCursor);
				break;
				
			case 3:
				XCursorUtils::SetCursor(kOrbitLeftCursor);
				break;
			
			case 4:
				XCursorUtils::SetCursor(kZoomInCursor);
				break;
			
			case 5:
				XCursorUtils::SetCursor(kZoomOutCursor);
				break;
			
			default:
				DEBUGSTR("Bad quadrant in CFractalMouseHandler::OnAdjustCursor");
		}
	}

	return kHandled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CFractalMouseHandler::DoGetQuadrant
//
//---------------------------------------------------------------
int32 CFractalMouseHandler::DoGetQuadrant(const XPoint& pt) const
{
	int32 quadrant;
	
	const IAdjustCursorHelper* thisPtr = this;
	IConstGeometryPtr geometry(thisPtr);
	XRect extent = geometry->GetExtent();
	
	XSize size = extent.GetSize();
	XPoint center(size.width/2, size.height/2);
	
	XRect zoomRect = extent;
	zoomRect.Inset(size.width/3, size.height/3);
	
	if (zoomRect.Contains(pt)) {
		if (pt.y < center.y) 
			quadrant = 4;
		else
			quadrant = 5;
			
	} else {
		if (pt.y < center.y) {
			if (pt.y > pt.x)
				quadrant = 3;
			else if (pt.x > size.height - pt.y)
				quadrant = 1;
			else
				quadrant = 0;
				
		} else {
			if (pt.x < size.height - pt.y)
				quadrant = 3;
			else if (pt.x > pt.y)
				quadrant = 1;
			else
				quadrant = 2;
		}
	}
	
	return quadrant;
}

