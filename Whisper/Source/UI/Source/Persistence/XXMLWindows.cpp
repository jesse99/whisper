/*
 *  File:       XMLWindows.cpp
 *  Summary:   	Functions uses to stream window interfaces into and out of XML.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLWindows.cpp,v $
 *		Revision 1.6  2001/04/27 04:39:44  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/03/01 11:40:35  jesjones
 *		Tweaked the TRACEFLOW statements so less code is generated.
 *		
 *		Revision 1.4  2000/12/05 09:21:24  jesjones
 *		Added support for dialog boxes.
 *		
 *		Revision 1.3  2000/11/10 04:48:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLWindows.h>

#include <IDefaultPositionWindow.h>
#include <IDialogBox.h>
#include <IWindow.h>
#include <XStringUtils.h>
#include <XTranscode.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	struct SWindowStyle
// ===================================================================================

//---------------------------------------------------------------
//
// StreamOut
//
//---------------------------------------------------------------
static void StreamOut(SWindowStyle& style, XXMLElement& element)
{	
	// Cross platform attributes
	switch (style.type) {
		case kDocumentWindow:
			element.AppendAttribute(XXMLAttribute(L"type", L"document"));	
			break;
			
		case kModalDialog:
			element.AppendAttribute(XXMLAttribute(L"type", L"modal"));	
			break;
			
		case kAlert:
			element.AppendAttribute(XXMLAttribute(L"type", L"alert"));	
			break;
			
		case kFloatingWindow:
			element.AppendAttribute(XXMLAttribute(L"type", L"floating"));	
			break;
			
		default:
			DEBUGSTR("Bad window type in SWindowStyle::HandleStreamOut"); 
	}
	
#if 0
	switch (style.autoPosition) {
		case kDontAutoPosition:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"dont_auto_position"));	
			break;
			
		case kCenterOnMainMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"center_on_main_monitor"));	
			break;
			
		case kAlertPositionOnMainMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"alert_position_on_main_monitor"));	
			break;
			
		case kStaggerOnMainMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"stagger_on_main_monitor"));	
			break;
			
		case kCenterOnTopWindow:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"center_on_top_window"));	
			break;
			
		case kAlertPositionOnTopWindow:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"alert_position_on_top_window"));	
			break;
			
		case kStaggerOnTopWindow:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"stagger_on_top_window"));	
			break;
			
		case kCenterOnParentMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"center_on_parent_monitor"));	
			break;
			
		case kAlertPositionOnParentMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"alert_position_on_parent_monitor"));	
			break;
			
		case kStaggerOnParentMonitor:
			element.AppendAttribute(XXMLAttribute(L"auto_position", L"stagger_on_parent_monitor"));	
			break;
			
		default:
			DEBUGSTR("Bad auto position in XWindow::OnStreamOut"); 
	}
#endif

	element.AppendAttribute(XXMLAttribute(L"show_new", style.showNew));	
	element.AppendAttribute(XXMLAttribute(L"targetable", style.targetable));	
	element.AppendAttribute(XXMLAttribute(L"hide_on_suspend", style.hideOnSuspend));	
	
	// Win attributes
	std::wstring attr;
	if (style.style & WS_OVERLAPPED)
		attr += L"WS_OVERLAPPED+";
	if (style.style & WS_POPUP)
		attr += L"WS_POPUP+";
	if (style.style & WS_CHILD)
		attr += L"WS_CHILD+";
	if (style.style & WS_MINIMIZE)
		attr += L"WS_MINIMIZE+";
	if (style.style & WS_VISIBLE)
		attr += L"WS_VISIBLE+";
	if (style.style & WS_DISABLED)
		attr += L"WS_DISABLED+";
	if (style.style & WS_CLIPSIBLINGS)
		attr += L"WS_CLIPSIBLINGS+";
	if (style.style & WS_CLIPCHILDREN)
		attr += L"WS_CLIPCHILDREN+";
	if (style.style & WS_MAXIMIZE)
		attr += L"WS_MAXIMIZE+";
	if (style.style & WS_BORDER)
		attr += L"WS_BORDER+";
	if (style.style & WS_DLGFRAME)
		attr += L"WS_DLGFRAME+";
	if (style.style & WS_VSCROLL)
		attr += L"WS_VSCROLL+";
	if (style.style & WS_HSCROLL)
		attr += L"WS_HSCROLL+";
	if (style.style & WS_SYSMENU)
		attr += L"WS_SYSMENU+";
	if (style.style & WS_THICKFRAME)
		attr += L"WS_THICKFRAME+";
	if (style.style & WS_MINIMIZEBOX)
		attr += L"WS_MINIMIZEBOX+";
	if (style.style & WS_MAXIMIZEBOX)
		attr += L"WS_MAXIMIZEBOX+";
		
	attr.resize(attr.length() - 1);		
	element.AppendAttribute(XXMLAttribute(L"style", attr));	

	attr.resize(0);
	if (style.styleEx & WS_EX_DLGMODALFRAME)
		attr += L"WS_EX_DLGMODALFRAME+";
	if (style.styleEx & WS_EX_NOPARENTNOTIFY)
		attr += L"WS_EX_NOPARENTNOTIFY+";
	if (style.styleEx & WS_EX_TOPMOST)
		attr += L"WS_EX_TOPMOST+";
	if (style.styleEx & WS_EX_ACCEPTFILES)
		attr += L"WS_EX_ACCEPTFILES+";
	if (style.styleEx & WS_EX_TRANSPARENT)
		attr += L"WS_EX_TRANSPARENT+";
	if (style.styleEx & WS_EX_MDICHILD)
		attr += L"WS_EX_MDICHILD+";
	if (style.styleEx & WS_EX_TOOLWINDOW)
		attr += L"WS_EX_TOOLWINDOW+";
	if (style.styleEx & WS_EX_WINDOWEDGE)
		attr += L"WS_EX_WINDOWEDGE+";
	if (style.styleEx & WS_EX_CLIENTEDGE)
		attr += L"WS_EX_CLIENTEDGE+";
	if (style.styleEx & WS_EX_CONTEXTHELP)
		attr += L"WS_EX_CONTEXTHELP+";
	if (style.styleEx & WS_EX_RIGHT)
		attr += L"WS_EX_RIGHT+";
	if (style.styleEx & WS_EX_LEFT)
		attr += L"WS_EX_LEFT+";
	if (style.styleEx & WS_EX_RTLREADING)
		attr += L"WS_EX_RTLREADING+";
	if (style.styleEx & WS_EX_LTRREADING)
		attr += L"WS_EX_LTRREADING+";
	if (style.styleEx & WS_EX_LEFTSCROLLBAR)
		attr += L"WS_EX_LEFTSCROLLBAR+";
	if (style.styleEx & WS_EX_CONTROLPARENT)
		attr += L"WS_EX_CONTROLPARENT+";
	if (style.styleEx & WS_EX_RIGHTSCROLLBAR)
		attr += L"WS_EX_RIGHTSCROLLBAR+";
	if (style.styleEx & WS_EX_STATICEDGE)
		attr += L"WS_EX_STATICEDGE+";
	if (style.styleEx & WS_EX_APPWINDOW)
		attr += L"WS_EX_APPWINDOW+";
#if WIN && WINVER >= 0x0500
	if (style.styleEx & WS_EX_LAYERED)
		attr += L"WS_EX_LAYERED+";
	if (style.styleEx & WS_EX_NOINHERITLAYOUT)
		attr += L"WS_EX_NOINHERITLAYOUT+";
	if (style.styleEx & WS_EX_LAYOUTRTL)
		attr += L"WS_EX_LAYOUTRTL+";
	if (style.styleEx & WS_EX_NOACTIVATE)
		attr += L"WS_EX_NOACTIVATE+";
#endif
		
	if (attr.length() == 0)
		attr = L"0";
	else
		attr.resize(attr.length() - 1);
		
	element.AppendAttribute(XXMLAttribute(L"style_ex", attr));	
	
	attr.resize(0);
	if (style.classStyle & CS_VREDRAW)
		attr += L"CS_VREDRAW+";
	if (style.classStyle & CS_HREDRAW)
		attr += L"CS_HREDRAW+";
	if (style.classStyle & CS_DBLCLKS)
		attr += L"CS_DBLCLKS+";
	if (style.classStyle & CS_OWNDC)
		attr += L"CS_OWNDC+";
	if (style.classStyle & CS_CLASSDC)
		attr += L"CS_CLASSDC+";
	if (style.classStyle & CS_PARENTDC)
		attr += L"CS_PARENTDC+";
	if (style.classStyle & CS_NOCLOSE)
		attr += L"CS_NOCLOSE+";
	if (style.classStyle & CS_SAVEBITS)
		attr += L"CS_SAVEBITS+";
	if (style.classStyle & CS_BYTEALIGNCLIENT)
		attr += L"CS_BYTEALIGNCLIENT+";
	if (style.classStyle & CS_BYTEALIGNWINDOW)
		attr += L"CS_BYTEALIGNWINDOW+";
	if (style.classStyle & CS_GLOBALCLASS)
		attr += L"CS_GLOBALCLASS+";
	if (style.classStyle & CS_IME)
		attr += L"CS_IME+";
		
	if (attr.length() == 0)
		attr = L"0";
	else
		attr.resize(attr.length() - 1);
		
	element.AppendAttribute(XXMLAttribute(L"class_style", attr));	
	element.AppendAttribute(XXMLAttribute(L"icon_id", (uint32) style.iconID));	
	element.AppendAttribute(XXMLAttribute(L"draw_size_grip", style.drawSizeGrip));	
											  					
	// Mac attributes
	switch (style.windClass) {
		case kAlertWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"alert"));	
			break;
			
		case kMovableAlertWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"moveable_alert"));	
			break;
			
		case kModalWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"modal"));	
			break;
			
		case kMovableModalWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"moveable_modal"));	
			break;
			
		case kFloatingWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"floating"));	
			break;
			
		case kDocumentWindowClass:
			element.AppendAttribute(XXMLAttribute(L"window_class", L"document"));	
			break;
			
		default:
			DEBUGSTR("Bad window type in SWindowStyle::HandleStreamOut"); 
	}
	
	attr.resize(0);
	if (style.attributes & kWindowCloseBoxAttribute)
		attr += L"has_close_box+";
	if (style.attributes & kWindowHorizontalZoomAttribute)
		attr += L"has_horz_zoom+";
	if (style.attributes & kWindowVerticalZoomAttribute)
		attr += L"has_vert_zoom+";
	if (style.attributes & kWindowCollapseBoxAttribute)
		attr += L"has_collapse_box+";
	if (style.attributes & kWindowResizableAttribute)
		attr += L"resizeable+";
	if (style.attributes & kWindowSideTitlebarAttribute)
		attr += L"side_floater+";
	if (style.attributes & kWindowNoUpdatesAttribute)
		attr += L"no_updates+";
	if (style.attributes & kWindowNoActivatesAttribute)
		attr += L"no_activates+";
		
	if (attr.length() == 0)
		attr = L"none";
	else
		attr.resize(attr.length() - 1);
		
	element.AppendAttribute(XXMLAttribute(L"attributes", attr));	
	
	element.AppendAttribute(XXMLAttribute(L"click_through", style.clickThrough));	
	element.AppendAttribute(XXMLAttribute(L"get_select_click", style.getSelectClick));	
}


//---------------------------------------------------------------
//
// StreamIn
//
//---------------------------------------------------------------
static void StreamIn(SWindowStyle& style, const XXMLElement& element)
{
	ASSERT(element.item_begin() == element.item_end());		// we don't have any child elements
	ASSERT(element.attr_begin() != element.attr_end());		// we do have required attributes
	
	std::wstring str, temp;
	
	XXMLElement::const_attr_iterator iter = element.attr_begin();	
	while (iter != element.attr_end()) {
		const XXMLAttribute& attr = *iter;
		++iter;
	
		const std::wstring& name = attr.GetName();
		const std::wstring& value = attr.GetValue();
	
		// Cross platform attributes
		if (name == L"type") {			
			if (value == L"document")
				style.type = kDocumentWindow;
			else if (value == L"modal")
				style.type = kModalDialog;
			else if (value == L"alert")
				style.type = kAlert;
			else if (value == L"floating")
				style.type = kFloatingWindow;
			else 
				DEBUGSTR(value + L" isn't a valid SWindowStyle type attribute!");
						  					
		} else if (name == L"show_new") {			
			style.showNew = attr.Get<bool>();
						  											  											  					
		} else if (name == L"targetable") {			
			style.targetable = attr.Get<bool>();
						  					
		} else if (name == L"hide_on_suspend") {			
			style.hideOnSuspend = attr.Get<bool>();
		
		// Win attributes
		} else if (name == L"style") {
			style.style = 0;
			
			str = value;
			while (str.length() > 0) {
				temp = Whisper::Parse(str, L" +");
			
				if (temp == L"WS_OVERLAPPED")
					style.style = style.style | WS_OVERLAPPED;
				else if (temp == L"WS_POPUP")
					style.style |= WS_POPUP;
				else if (temp == L"WS_CHILD")
					style.style |= WS_CHILD;
				else if (temp == L"WS_MINIMIZE")
					style.style |= WS_MINIMIZE;
				else if (temp == L"WS_VISIBLE")
					style.style |= WS_VISIBLE;
				else if (temp == L"WS_DISABLED")
					style.style |= WS_DISABLED;
				else if (temp == L"WS_CLIPSIBLINGS")
					style.style |= WS_CLIPSIBLINGS;
				else if (temp == L"WS_CLIPCHILDREN")
					style.style |= WS_CLIPCHILDREN;
				else if (temp == L"WS_MAXIMIZE")
					style.style |= WS_MAXIMIZE;
				else if (temp == L"WS_CAPTION")
					style.style |= WS_CAPTION;
				else if (temp == L"WS_BORDER")
					style.style |= WS_BORDER;
				else if (temp == L"WS_DLGFRAME")
					style.style |= WS_DLGFRAME;
				else if (temp == L"WS_VSCROLL")
					style.style |= WS_VSCROLL;
				else if (temp == L"WS_HSCROLL")
					style.style |= WS_HSCROLL;
				else if (temp == L"WS_SYSMENU")
					style.style |= WS_SYSMENU;
				else if (temp == L"WS_THICKFRAME")
					style.style |= WS_THICKFRAME;
				else if (temp == L"WS_MINIMIZEBOX")
					style.style |= WS_MINIMIZEBOX;
				else if (temp == L"WS_MAXIMIZEBOX")
					style.style |= WS_MAXIMIZEBOX;
				else 
					throw std::domain_error(ToUTF8Str((temp + L" isn't a valid SWindowStyle style attribute!")));	// throw since the DTD won't catch this
			}
			
		} else if (name == L"style_ex") {
			style.styleEx = 0;
					
			str = value;
			while (str.length() > 0) {
				temp = Whisper::Parse(str, L" +");
			
				if (temp == L"WS_EX_DLGMODALFRAME")
					style.styleEx |= WS_EX_DLGMODALFRAME;
				else if (temp == L"WS_EX_NOPARENTNOTIFY")
					style.styleEx |= WS_EX_NOPARENTNOTIFY;
				else if (temp == L"WS_EX_TOPMOST")
					style.styleEx |= WS_EX_TOPMOST;
				else if (temp == L"WS_EX_ACCEPTFILES")
					style.styleEx |= WS_EX_ACCEPTFILES;
				else if (temp == L"WS_EX_TRANSPARENT")
					style.styleEx |= WS_EX_TRANSPARENT;
				else if (temp == L"WS_EX_MDICHILD")
					style.styleEx |= WS_EX_MDICHILD;
				else if (temp == L"WS_EX_TOOLWINDOW")
					style.styleEx |= WS_EX_TOOLWINDOW;
				else if (temp == L"WS_EX_WINDOWEDGE")
					style.styleEx |= WS_EX_WINDOWEDGE;
				else if (temp == L"WS_EX_CLIENTEDGE")
					style.styleEx |= WS_EX_CLIENTEDGE;
				else if (temp == L"WS_EX_CONTEXTHELP")
					style.styleEx |= WS_EX_CONTEXTHELP;
				else if (temp == L"WS_EX_RIGHT")
					style.styleEx |= WS_EX_RIGHT;
				else if (temp == L"WS_EX_LEFT")
					style.styleEx = style.styleEx | WS_EX_LEFT;
				else if (temp == L"WS_EX_RTLREADING")
					style.styleEx |= WS_EX_RTLREADING;
				else if (temp == L"WS_EX_LTRREADING")
					style.styleEx = style.styleEx | WS_EX_LTRREADING;
				else if (temp == L"WS_EX_LEFTSCROLLBAR")
					style.styleEx |= WS_EX_LEFTSCROLLBAR;
				else if (temp == L"WS_EX_RIGHTSCROLLBAR")
					style.styleEx = style.styleEx | WS_EX_RIGHTSCROLLBAR;
				else if (temp == L"WS_EX_CONTROLPARENT")
					style.styleEx |= WS_EX_CONTROLPARENT;
				else if (temp == L"WS_EX_STATICEDGE")
					style.styleEx |= WS_EX_STATICEDGE;
				else if (temp == L"WS_EX_APPWINDOW")
					style.styleEx |= WS_EX_APPWINDOW;
#if WIN && WINVER >= 0x0500
				else if (temp == L"WS_EX_LAYERED")
					style.styleEx |= WS_EX_LAYERED;
				else if (temp == L"WS_EX_NOINHERITLAYOUT")
					style.styleEx |= WS_EX_NOINHERITLAYOUT;
				else if (temp == L"WS_EX_LAYOUTRTL")
					style.styleEx |= WS_EX_LAYOUTRTL;
				else if (temp == L"WS_EX_NOACTIVATE")
					style.styleEx |= WS_EX_NOACTIVATE;
#endif
				else if (temp == L"0")
					;
				else 
					throw std::domain_error(ToUTF8Str((temp + L" isn't a valid SWindowStyle style_ex attribute!")));	// throw since the DTD won't catch this
			}
			
		} else if (name == L"class_style") {
			style.classStyle = 0;
					
			str = value;
			while (str.length() > 0) {
				temp = Whisper::Parse(str, L" +");
			
				if (temp == L"CS_VREDRAW")
					style.classStyle |= CS_VREDRAW;
				else if (temp == L"CS_HREDRAW")
					style.classStyle |= CS_HREDRAW;
				else if (temp == L"CS_DBLCLKS")
					style.classStyle |= CS_DBLCLKS;
				else if (temp == L"CS_OWNDC")
					style.classStyle |= CS_OWNDC;
				else if (temp == L"CS_CLASSDC")
					style.classStyle |= CS_CLASSDC;
				else if (temp == L"CS_PARENTDC")
					style.classStyle |= CS_PARENTDC;
				else if (temp == L"CS_NOCLOSE")
					style.classStyle |= CS_NOCLOSE;
				else if (temp == L"CS_SAVEBITS")
					style.classStyle |= CS_SAVEBITS;
				else if (temp == L"CS_BYTEALIGNCLIENT")
					style.classStyle |= CS_BYTEALIGNCLIENT;
				else if (temp == L"CS_BYTEALIGNWINDOW")
					style.classStyle |= CS_BYTEALIGNWINDOW;
				else if (temp == L"CS_GLOBALCLASS")
					style.classStyle |= CS_GLOBALCLASS;
				else if (temp == L"CS_IME")
					style.classStyle |= CS_IME;
				else if (temp == L"0")
					;
				else 
					throw std::domain_error(ToUTF8Str((temp + L" isn't a valid SWindowStyle class_style attribute!")));	// throw since the DTD won't catch this
			}
			
		} else if (name == L"icon_id") {			
			attr.Get((uint16) 0, style.iconID, (uint16) USHRT_MAX);
						  					
		} else if (name == L"icon_id") {			
			attr.Get((uint16) 0, style.iconID, (uint16) USHRT_MAX);
						  					
		} else if (name == L"draw_size_grip") {		
			style.drawSizeGrip = attr.Get<bool>();
											  					
		// Mac attributes
		} else if (name == L"window_class") {			
			if (value == L"alert")
				style.windClass = kAlertWindowClass;
			else if (value == L"moveable_alert")
				style.windClass = kMovableAlertWindowClass;
			else if (value == L"modal")
				style.windClass = kModalWindowClass;
			else if (value == L"moveable_modal")
				style.windClass = kMovableModalWindowClass;
			else if (value == L"floating")
				style.windClass = kFloatingWindowClass;
			else if (value == L"document")
				style.windClass = kDocumentWindowClass;
			else 
				DEBUGSTR(value + L" isn't a valid SWindowStyle window_class attribute!");
		
		} else if (name == L"attributes") {
			style.attributes = 0;
					
			str = value;
			while (str.length() > 0) {
				temp = Whisper::Parse(str, L" +");
			
				if (temp == L"has_close_box")
					style.attributes |= kWindowCloseBoxAttribute;
				else if (temp == L"has_horz_zoom")
					style.attributes |= kWindowHorizontalZoomAttribute;
				else if (temp == L"has_vert_zoom")
					style.attributes |= kWindowVerticalZoomAttribute;
				else if (temp == L"has_collapse_box")
					style.attributes |= kWindowCollapseBoxAttribute;
				else if (temp == L"resizeable")
					style.attributes |= kWindowResizableAttribute;
				else if (temp == L"side_floater")
					style.attributes |= kWindowSideTitlebarAttribute;
				else if (temp == L"no_updates")
					style.attributes |= kWindowNoUpdatesAttribute;
				else if (temp == L"no_activates")
					style.attributes |= kWindowNoActivatesAttribute;
				else if (temp == L"none")
					style.attributes = kWindowNoAttributes;
				else 
					throw std::domain_error(ToUTF8Str((temp + L" isn't a valid SWindowStyle attributes attribute!")));	// throw since the DTD won't catch this
			}
			
		} else if (name == L"click_through") {			
			style.clickThrough = attr.Get<bool>();
						  											  					
		} else if (name == L"get_select_click") {			
			style.getSelectClick = attr.Get<bool>();
						  											  					
		} else
			TRACEFLOW("Warning", name + L" isn't a valid attribute of SWindowStyle!");
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
// LoadDefaultPositionWindow
//
//---------------------------------------------------------------
void LoadDefaultPositionWindow(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	if (pass == kLoadingBoss) {
		IDefaultPositionWindowPtr position(boss, interface);			// this should always be valid because IPersistXML checks to make sure the names match
				
		const XXMLAttribute& attr = element.FindAttribute(L"position");	
		const std::wstring& value = attr.GetValue();

		if (value == L"dont_auto_position")
			position->Init(kDontAutoPosition);
		else if (value == L"center_on_main_monitor")
			position->Init(kCenterOnMainMonitor);
		else if (value == L"alert_position_on_main_monitor")
			position->Init(kAlertPositionOnMainMonitor);
		else if (value == L"stagger_on_main_monitor")
			position->Init(kStaggerOnMainMonitor);
		else if (value == L"center_on_top_window")
			position->Init(kCenterOnTopWindow);
		else if (value == L"alert_position_on_top_window")
			position->Init(kAlertPositionOnTopWindow);
		else if (value == L"stagger_on_top_window")
			position->Init(kStaggerOnTopWindow);
		else if (value == L"center_on_parent_monitor")
			position->Init(kCenterOnParentMonitor);
		else if (value == L"alert_position_on_parent_monitor")
			position->Init(kAlertPositionOnParentMonitor);
		else if (value == L"stagger_on_parent_monitor")
			position->Init(kStaggerOnParentMonitor);
		else 
			DEBUGSTR(value + L" isn't a valid window position attribute!");								  					
	}		
}


//---------------------------------------------------------------
//
// LoadDialogBox
//
//---------------------------------------------------------------
void LoadDialogBox(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	UNUSED(parent);
	
	if (pass == kLoadedBoss) {
		IDialogBoxPtr dialog(boss, interface);					// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() == element.item_end());		// we have don't have any child elements
		ASSERT(element.attr_begin() != element.attr_end());		// we have required attributes
					
		// Read in the dialog attributes
		std::wstring defaultName = L"????";
		std::wstring cancelName = L"????";
		
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
			const std::wstring& value = attr.GetValue();
		
			if (name == L"default_btn") {			
				defaultName = value;
							  						
			} else if (name == L"cancel_btn") {			
				cancelName = value;
							  						
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IDialogBox!");
		}
		
		dialog->SetDefaultButton(defaultName);
		dialog->SetCancelButton(cancelName);
	}		
}


//---------------------------------------------------------------
//
// LoadWindow
//
//---------------------------------------------------------------
void LoadWindow(XBoss* boss, XBoss::iterator interface, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass)
{
	PRECONDITION(boss != nil);
	PRECONDITION(interface != boss->end());
	
	if (pass == kLoadingBoss) {
		IWindowPtr window(boss, interface);			// this should always be valid because IPersistXML checks to make sure the names match
		
		ASSERT(element.item_begin() != element.item_end());		// we have one child element
		ASSERT(element.attr_begin() != element.attr_end());		// we have required attributes
			
		// Read in the window style struct
		SWindowStyle style;
		const XXMLElement* child = dynamic_cast<const XXMLElement*>(element.item_front());
		ASSERT(child != nil);
		StreamIn(style, *child);
		
		// Initialize the window
		window->Init(parent, L"", XSize(128, 128), style);
		
		// Read in the window attributes
		XXMLElement::const_attr_iterator iter = element.attr_begin();	
		while (iter != element.attr_end()) {
			const XXMLAttribute& attr = *iter;
			++iter;
		
			const std::wstring& name = attr.GetName();
		
			if (name == L"name") {			
				window->SetName(LoadAppString(attr.GetValue()));
							  						
			} else
				TRACEFLOW("XPersistXML", name + L" isn't a valid attribute of IWindow!");
		}
	}		
}


}	// namespace Whisper
