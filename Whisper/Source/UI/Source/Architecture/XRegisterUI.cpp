/*
 *  File:       XRegisterUI.cpp
 *  Summary:   	Provides functions to register the UI interfaces and implementations.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterUI.cpp,v $
 *		Revision 1.34  2001/04/27 09:16:42  jesjones
 *		Nuked IIdler and XIdler.
 *		
 *		Revision 1.33  2001/04/19 04:07:42  jesjones
 *		Added MSystemHeapExtension.
 *		
 *		Revision 1.32  2001/04/13 08:03:29  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.31  2001/04/01 04:59:01  jesjones
 *		Added listbox control.
 *		
 *		Revision 1.30  2001/03/27 07:44:01  jesjones
 *		More stuff.
 *		
 *		Revision 1.29  2001/03/23 10:59:31  jesjones
 *		Added ISeperator.
 *		
 *		Revision 1.28  2001/03/20 12:02:15  jesjones
 *		Added bevel button.
 *		
 *		Revision 1.27  2001/03/09 10:18:51  jesjones
 *		More stuff
 */

#include <XWhisperHeader.h>
#include <XRegisterUI.h>

#include <IActivated.h>
#include <IActiveMenuBar.h>
#include <IActiveUndoContext.h>
#include <IAdjustCursorHelper.h>
#include <IApplication.h>
#include <IBevelButton.h>
#include <IBinding.h>
#include <IBrush.h>
#include <IBrushDrawer.h>
#include <ICanClose.h>
#include <ICheckBox.h>
#include <IChildNode.h>
#include <IClickExtensible.h>
#include <IClickExtension.h>
#include <IClickExtensions.h>
#include <IColorSwatch.h>
#include <ICommand.h>
#include <ICommandHelper.h>
#include <ICommandLine.h>
#include <ICommandQueue.h>
#include <ICommands.h>
#include <IContextMenu.h>
#include <IContextMenuHelper.h>
#include <IControl.h>
#include <IControlPressedHelper.h>
#include <ICreateDoc.h>
#include <ICurrentFocus.h>
#include <IDefaultPositionWindow.h>
#include <IDialogBox.h>
#include <IDocReader.h>
#include <IDocument.h>
#include <IDocuments.h>
#include <IDocView.h>
#include <IDocWriter.h>	
#include <IDraw.h>
#include <IDrawExtensible.h>
#include <IDrawExtension.h>
#include <IDrawExtensionHelper.h>
#include <IDrawHelper.h>
#include <IDropApp.h>
#include <IDropAppHelper.h>
#include <IDropAppProgress.h>
#include <IEventLoop.h>
#include <IFocus.h>
#include <IFocused.h>
#include <IGeometry.h>
#include <IGroupBox.h>
#include <IGroupControl.h>
#include <IHelpText.h>
#include <IHierarchy.h>
#include <IInit.h>
#include <IInitMenuBar.h>
#include <IInstallMenuHandler.h>
#include <IInstance.h>
#include <IKeyExtensible.h>
#include <IKeyExtension.h>
#include <IKeyHandler.h>
#include <IListBox.h>
#include <ILoaded.h>
#include <ILocationChanged.h>
#include <IMergeActions.h> 
#include <IMemento.h>
#include <IMenuBar.h>
#include <IMenuExtensible.h>
#include <IMenuExtension.h>
#include <IMenuHandler.h>
#include <IMouseDownHelper.h>
#include <IMouseExtensible.h>
#include <IMouseExtension.h>
#include <IMouseHandler.h>
#include <IMouseUpHelper.h>
#include <IOpened.h>
#include <IParentChangedSize.h>
#include <IPersistDoc.h>
#include <IPoseDialogBox.h>
#include <IPositionWindow.h>
#include <IReadXML.h>
#include <IPen.h>
#include <IProgressBar.h>
#include <IPushButton.h>
#include <IRadioButton.h>
#include <IRange.h>
#include <IRestoreWindowPosition.h>
#include <IRootControl.h>
#include <IScrap.h>
#include <IScrapHelper.h>
#include <IScrollableControl.h>
#include <IScrollBar.h>
#include <IScrollerControl.h>
#include <ISeperator.h>
#include <ISetCheckedHelp.h>
#include <ISetDisabledHelp.h>
#include <ISetHelp.h>
#include <IShowHelp.h>
#include <ISizeChanged.h>
#include <IStaticText.h>
#include <IStringTableLoader.h>
#include <ITabber.h>
#include <IText.h>
#include <ITextBox.h>
#include <ITimer.h>
#include <ITracker.h>
#include <ITrackerHelper.h>
#include <ITrackControlHelper.h>
#include <IValidateText.h>
#include <IValidateTextHelper.h>
#include <IValue.h>
#include <IUndoAction.h>
#include <IUndoActionHelper.h>
#include <IUndoActions.h>
#include <IUndoContext.h>
#include <IUserControl.h>
#include <IValidate.h>
#include <IValueChanged.h>
#include <IValueControl.h>
#include <IWindow.h>
#include <IWindowClick.h>
#include <IWindowMgr.h>
#include <IWriteXML.h>
#include <IXMLMenu.h>
#include <XObjectModel.h>
#include <XTranscode.h>

#if MAC
	#include <IEventDispatcher.h>
	#include <IEventExtensible.h>
	#include <IEventExtension.h>
#elif WIN
	#include <IFrameWindow.h>
	#include <IMsgExtensible.h>
	#include <IMsgExtension.h>
#endif

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterMacImplementations
//
//---------------------------------------------------------------
static void RegisterMacImplementations()
{
#if MAC
	REGISTER_INTERFACE_FACTORY(M16BitValueControl, L"M16BitValueControl");
				
	REGISTER_INTERFACE_FACTORY(MDocWindowClick, L"MDocWindowClick");

	REGISTER_INTERFACE_FACTORY(MEventDispatcher, L"MEventDispatcher");
					
	REGISTER_INTERFACE_FACTORY(MRootGeometry, L"MRootGeometry");
	
	REGISTER_INTERFACE_FACTORY(MScrollBarMouseHandler, L"MScrollBarMouseHandler");
	REGISTER_INTERFACE_FACTORY(MStaticText, L"MStaticText");
	
	REGISTER_INTERFACE_FACTORY(MTETextBox, L"MTETextBox");
	REGISTER_INTERFACE_FACTORY(MThumbMouseHandler, L"MThumbMouseHandler");
	
	REGISTER_INTERFACE_FACTORY(MValueControl, L"MValueControl");
	
	REGISTER_INTERFACE_FACTORY(MWindowClick, L"MWindowClick");

#if DEBUG
	REGISTER_INTERFACE_FACTORY(MMacsbugMenuHandler, L"MMacsbugMenuHandler");
	REGISTER_INTERFACE_FACTORY(MSIOUXExtension, L"MSIOUXExtension");
	REGISTER_INTERFACE_FACTORY(MSystemHeapExtension, L"MSystemHeapExtension");
	REGISTER_INTERFACE_FACTORY(MSystemInfoExtension, L"MSystemInfoExtension");
	REGISTER_INTERFACE_FACTORY(MZoneRangerExtension, L"MZoneRangerExtension");
#endif
#endif
}


//---------------------------------------------------------------
//
// RegisterWinImplementations
//
//---------------------------------------------------------------
static void RegisterWinImplementations() 
{
#if WIN
	REGISTER_INTERFACE_FACTORY(WButtonKeyHandler, L"WButtonKeyHandler");
	REGISTER_INTERFACE_FACTORY(WButtonValueControl, L"WButtonValueControl");
			
	REGISTER_INTERFACE_FACTORY(WFrameWindow, L"WFrameWindow");
	
	REGISTER_INTERFACE_FACTORY(WHTMLHelp, L"WHTMLHelp");

	REGISTER_INTERFACE_FACTORY(WMDIHelp, L"WMDIHelp");
	
	REGISTER_INTERFACE_FACTORY(WProgressValueControl, L"WProgressValueControl");
		
	REGISTER_INTERFACE_FACTORY(WScrollBarValueControl, L"WScrollBarValueControl");
	REGISTER_INTERFACE_FACTORY(WStaticText, L"WStaticText");

	REGISTER_INTERFACE_FACTORY(WTextBox, L"WTextBox");
		
#if DEBUG
	REGISTER_INTERFACE_FACTORY(WSystemInfoExtension, L"WSystemInfoExtension");
#endif
#endif
}


//---------------------------------------------------------------
//
// RegisterXImplementations
//
//---------------------------------------------------------------
static void RegisterXImplementations()
{
	REGISTER_INTERFACE_FACTORY(XActivateMenuHandlers, L"XActivateMenuHandlers");	
	REGISTER_INTERFACE_FACTORY(XActiveMenuBar, L"XActiveMenuBar");	
	REGISTER_INTERFACE_FACTORY(XActiveUndoContext, L"XActiveUndoContext");
	REGISTER_INTERFACE_FACTORY(XAlphaValidator, L"XAlphaValidator");
	REGISTER_INTERFACE_FACTORY(XAlphaNumValidator, L"XAlphaNumValidator");
	REGISTER_INTERFACE_FACTORY(XAppEventLoop, L"XAppEventLoop");
	REGISTER_INTERFACE_FACTORY(XAppFocus, L"XAppFocus");
	REGISTER_INTERFACE_FACTORY(XAppMenuHandler, L"XAppMenuHandler");
	REGISTER_INTERFACE_FACTORY(XApplication, L"XApplication");
	
	REGISTER_INTERFACE_FACTORY(XBeginUndoSequence, L"XBeginUndoSequence");
	REGISTER_INTERFACE_FACTORY(XBevelButton, L"XBevelButton");
	REGISTER_INTERFACE_FACTORY(XBrushImp, L"XBrushImp");
	REGISTER_INTERFACE_FACTORY(XBrushDrawer, L"XBrushDrawer");
	REGISTER_INTERFACE_FACTORY(XBrushWindowDrawer, L"XBrushWindowDrawer");
	REGISTER_INTERFACE_FACTORY(XButtonHelp, L"XButtonHelp"); 
	
	REGISTER_INTERFACE_FACTORY(XCallbackClickExtension, L"XCallbackClickExtension");
	REGISTER_INTERFACE_FACTORY(XCallbackCommand, L"XCallbackCommand");
	REGISTER_INTERFACE_FACTORY(XCheckBox, L"XCheckBox");
	REGISTER_INTERFACE_FACTORY(XClearAction, L"XClearAction");	
	REGISTER_INTERFACE_FACTORY(XClickExtensible, L"XClickExtensible");
	REGISTER_INTERFACE_FACTORY(XColorSwatch, L"XColorSwatch");
	REGISTER_INTERFACE_FACTORY(XCommand, L"XCommand");
	REGISTER_INTERFACE_FACTORY(XCommandClickExtension, L"XCommandClickExtension");
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
	REGISTER_INTERFACE_FACTORY(XDefaultDrawWindow, L"XDefaultDrawWindow");
	REGISTER_INTERFACE_FACTORY(XDefaultHelp, L"XDefaultHelp");
	REGISTER_INTERFACE_FACTORY(XDefaultPositionWindow, L"XDefaultPositionWindow");
	REGISTER_INTERFACE_FACTORY(XDeferredErrorCommand, L"XDeferredErrorCommand");
	REGISTER_INTERFACE_FACTORY(XDeferredMenuCommand, L"XDeferredMenuCommand");
	REGISTER_INTERFACE_FACTORY(XDialogBox, L"XDialogBox");
	REGISTER_INTERFACE_FACTORY(XDocAppMenuHandler, L"XDocAppMenuHandler");
	REGISTER_INTERFACE_FACTORY(XDocFocus, L"XDocFocus");
	REGISTER_INTERFACE_FACTORY(XDocMenuHandler, L"XDocMenuHandler");	
	REGISTER_INTERFACE_FACTORY(XDocument, L"XDocument");
	REGISTER_INTERFACE_FACTORY(XDocuments, L"XDocuments");
	REGISTER_INTERFACE_FACTORY(XDocView, L"XDocView");
	REGISTER_INTERFACE_FACTORY(XDocWindow, L"XDocWindow");
	REGISTER_INTERFACE_FACTORY(XDoubleRange, L"XDoubleRange");
	REGISTER_INTERFACE_FACTORY(XDoubleValidator, L"XDoubleValidator");	
	REGISTER_INTERFACE_FACTORY(XDraw, L"XDraw");
	REGISTER_INTERFACE_FACTORY(XDrawControl, L"XDrawControl");
	REGISTER_INTERFACE_FACTORY(XDrawExtensible, L"XDrawExtensible");
	REGISTER_INTERFACE_FACTORY(XDrawExtension, L"XDrawExtension");
	REGISTER_INTERFACE_FACTORY(XDropApp, L"XDropApp");	
	REGISTER_INTERFACE_FACTORY(XDropAppProgress, L"XDropAppProgress");	
	REGISTER_INTERFACE_FACTORY(XDummyUndoAction, L"XDummyUndoAction");	
	
	REGISTER_INTERFACE_FACTORY(XEndUndoSequence, L"XEndUndoSequence");
	REGISTER_INTERFACE_FACTORY(XEraseExtension, L"XEraseExtension");
	
	REGISTER_INTERFACE_FACTORY(XFloatRange, L"XFloatRange");
	REGISTER_INTERFACE_FACTORY(XFrameExtension, L"XFrameExtension");	
	
	REGISTER_INTERFACE_FACTORY(XGroupBox, L"XGroupBox");
	REGISTER_INTERFACE_FACTORY(XGroupControl, L"XGroupControl");

	REGISTER_INTERFACE_FACTORY(XHexValidator, L"XHexValidator");	
	REGISTER_INTERFACE_FACTORY(XHierarchy, L"XHierarchy");	
	
	REGISTER_INTERFACE_FACTORY(XInstance, L"XInstance");
	REGISTER_INTERFACE_FACTORY(XInt32Range, L"XInt32Range");
	REGISTER_INTERFACE_FACTORY(XIntValidator, L"XIntValidator");
	
	REGISTER_INTERFACE_FACTORY(XKeyExtensible, L"XKeyExtensible");
	REGISTER_INTERFACE_FACTORY(XKeyHandler, L"XKeyHandler");
	
	REGISTER_INTERFACE_FACTORY(XLineBottomExtension, L"XLineBottomExtension");
	REGISTER_INTERFACE_FACTORY(XLineLeftExtension, L"XLineLeftExtension");
	REGISTER_INTERFACE_FACTORY(XLineRightExtension, L"XLineRightExtension");
	REGISTER_INTERFACE_FACTORY(XLineTopExtension, L"XLineTopExtension");	
	REGISTER_INTERFACE_FACTORY(XListBox, L"XListBox");	
	
	REGISTER_INTERFACE_FACTORY(XMenu, L"XMenu");
	REGISTER_INTERFACE_FACTORY(XMenuBar, L"XMenuBar");
	REGISTER_INTERFACE_FACTORY(XMenuExtensible, L"XMenuExtensible");
	REGISTER_INTERFACE_FACTORY(XMenuHandler, L"XMenuHandler");
	REGISTER_INTERFACE_FACTORY(XMergeAction, L"XMergeAction");
	REGISTER_INTERFACE_FACTORY(XMenuCommandClickExtension, L"XMenuCommandClickExtension");
	REGISTER_INTERFACE_FACTORY(XMouseExtensible, L"XMouseExtensible");
	REGISTER_INTERFACE_FACTORY(XMouseHandler, L"XMouseHandler");
	REGISTER_INTERFACE_FACTORY(XMultipleUndoAction, L"XMultipleUndoAction");

	REGISTER_INTERFACE_FACTORY(XNullDrawer, L"XNullDrawer");
	
	REGISTER_INTERFACE_FACTORY(XPasteAction, L"XPasteAction");	
	REGISTER_INTERFACE_FACTORY(XPenImp, L"XPenImp");	
	REGISTER_INTERFACE_FACTORY(XPersistDoc, L"XPersistDoc");
	REGISTER_INTERFACE_FACTORY(XPoseDialogBox, L"XPoseDialogBox");
	REGISTER_INTERFACE_FACTORY(XPrintableValidator, L"XPrintableValidator");
	REGISTER_INTERFACE_FACTORY(XProgressBar, L"XProgressBar");
	REGISTER_INTERFACE_FACTORY(XPushButton, L"XPushButton");	
	
	REGISTER_INTERFACE_FACTORY(XRadioButton, L"XRadioButton");
	REGISTER_INTERFACE_FACTORY(XRadioButtonValue, L"XRadioButtonValue");
	REGISTER_INTERFACE_FACTORY(XReadXML, L"XReadXML");
	REGISTER_INTERFACE_FACTORY(XRestoreDocWindowPos, L"XRestoreDocWindowPos");
	REGISTER_INTERFACE_FACTORY(XRestoreWindowPosition, L"XRestoreWindowPosition");
	REGISTER_INTERFACE_FACTORY(XRootControl, L"XRootControl");
	REGISTER_INTERFACE_FACTORY(XRubberBandTracker, L"XRubberBandTracker");

	REGISTER_INTERFACE_FACTORY(XScrap, L"XScrap");
	REGISTER_INTERFACE_FACTORY(XScrapMenuHandler, L"XScrapMenuHandler");
	REGISTER_INTERFACE_FACTORY(XScrollableControl, L"XScrollableControl");
	REGISTER_INTERFACE_FACTORY(XScrollableKeyHandler, L"XScrollableKeyHandler");
	REGISTER_INTERFACE_FACTORY(XScrollBar, L"XScrollBar");
	REGISTER_INTERFACE_FACTORY(XScrollerControl, L"XScrollerControl");
	REGISTER_INTERFACE_FACTORY(XSeperator, L"XSeperator");
	REGISTER_INTERFACE_FACTORY(XShadowedFrameExtension, L"XShadowedFrameExtension");
	REGISTER_INTERFACE_FACTORY(XStringTableLoader, L"XStringTableLoader"); 	
	
	REGISTER_INTERFACE_FACTORY(XTabber, L"XTabber");
	REGISTER_INTERFACE_FACTORY(XTimer, L"XTimer");
	REGISTER_INTERFACE_FACTORY(XTextBoolValue, L"XTextBoolValue");
	REGISTER_INTERFACE_FACTORY(XTextBoxMenuHandler, L"XTextBoxMenuHandler");	
	REGISTER_INTERFACE_FACTORY(XTextFloatValue, L"XTextFloatValue");
	REGISTER_INTERFACE_FACTORY(XTextInt32Value, L"XTextInt32Value");
	REGISTER_INTERFACE_FACTORY(XTextUInt32Value, L"XTextUInt32Value");
	REGISTER_INTERFACE_FACTORY(XTextDoubleValue, L"XTextDoubleValue");
	REGISTER_INTERFACE_FACTORY(XTracker, L"XTracker");
	
	REGISTER_INTERFACE_FACTORY(XUndoAction, L"XUndoAction");
	REGISTER_INTERFACE_FACTORY(XUndoCallbackAction, L"XUndoCallbackAction");
	REGISTER_INTERFACE_FACTORY(XUndoCommand, L"XUndoCommand");
	REGISTER_INTERFACE_FACTORY(XUndoContext, L"XUndoContext");
	REGISTER_INTERFACE_FACTORY(XUInt32Range, L"XUInt32Range");
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

#if DEBUG
	REGISTER_INTERFACE_FACTORY(XAssertExtension, L"XAssertExtension");
	REGISTER_INTERFACE_FACTORY(XControlExtension, L"XControlExtension");
	REGISTER_INTERFACE_FACTORY(XFocusExtension, L"XFocusExtension");
	REGISTER_INTERFACE_FACTORY(XIntenseExtension, L"XIntenseExtension");
#if !MULTI_FRAGMENT_APP		// $$$
	REGISTER_INTERFACE_FACTORY(XObjectHeapExtension, L"XObjectHeapExtension");
#endif
	REGISTER_INTERFACE_FACTORY(XSeperatorExtension, L"XSeperatorExtension");
	REGISTER_INTERFACE_FACTORY(XTraceExtension, L"XTraceExtension");
	REGISTER_INTERFACE_FACTORY(XUnitTestExtension, L"XUnitTestExtension");
#endif
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterUIInterfaces	
//
//---------------------------------------------------------------
void RegisterUIInterfaces()
{
	REGISTER_INTERFACE_NAME(IActivated, L"IActivated");	
	REGISTER_INTERFACE_NAME(IActiveMenuBar, L"IActiveMenuBar");
	REGISTER_INTERFACE_NAME(IActiveUndoContext, L"IActiveUndoContext");	
	REGISTER_INTERFACE_NAME(IAdjustCursorHelper, L"IAdjustCursorHelper");	
	REGISTER_INTERFACE_NAME(IApplication, L"IApplication");	
		
	REGISTER_INTERFACE_NAME(IBeginUndoSequence, L"IBeginUndoSequence");	
	REGISTER_INTERFACE_NAME(IBevelButton, L"IBevelButton");	
	REGISTER_INTERFACE_NAME(IBinding, L"IBinding");	
	REGISTER_INTERFACE_NAME(IBrush, L"IBrush");
	REGISTER_INTERFACE_NAME(IBrushDrawer, L"IBrushDrawer");
	
	REGISTER_INTERFACE_NAME(ICallbackClickExtension, L"ICallbackClickExtension");
	REGISTER_INTERFACE_NAME(ICallbackCommand, L"ICallbackCommand");
	REGISTER_INTERFACE_NAME(ICanClose, L"ICanClose");
	REGISTER_INTERFACE_NAME(ICheckBox, L"ICheckBox");
	REGISTER_INTERFACE_NAME(IChildNode, L"IChildNode");
	REGISTER_INTERFACE_NAME(IClickExtensible, L"IClickExtensible");		
	REGISTER_INTERFACE_NAME(IClickExtension, L"IClickExtension");
	REGISTER_INTERFACE_NAME(IColorSwatch, L"IColorSwatch");
	REGISTER_INTERFACE_NAME(ICommand, L"ICommand");
	REGISTER_INTERFACE_NAME(ICommandClickExtension, L"ICommandClickExtension");
	REGISTER_INTERFACE_NAME(ICommandHelper, L"ICommandHelper");
	REGISTER_INTERFACE_NAME(ICommandLine, L"ICommandLine");
	REGISTER_INTERFACE_NAME(ICommandQueue, L"ICommandQueue");		
	REGISTER_INTERFACE_NAME(IContextMenu, L"IContextMenu");		
	REGISTER_INTERFACE_NAME(IContextMenuHelper, L"IContextMenuHelper");		
	REGISTER_INTERFACE_NAME(IControl, L"IControl");
	REGISTER_INTERFACE_NAME(IControlPressedHelper, L"IControlPressedHelper");	
	REGISTER_INTERFACE_NAME(ICreateDoc, L"ICreateDoc");
	REGISTER_INTERFACE_NAME(ICurrentFocus, L"ICurrentFocus");
	
	REGISTER_INTERFACE_NAME(IDefaultPositionWindow, L"IDefaultPositionWindow");
	REGISTER_INTERFACE_NAME(IDeferredErrorCommand, L"IDeferredErrorCommand");
	REGISTER_INTERFACE_NAME(IDeferredMenuCommand, L"IDeferredMenuCommand");
	REGISTER_INTERFACE_NAME(IDialogBox, L"IDialogBox");
	REGISTER_INTERFACE_NAME(IDocReader, L"IDocReader");
	REGISTER_INTERFACE_NAME(IDocument, L"IDocument");
	REGISTER_INTERFACE_NAME(IDocuments, L"IDocuments");		
	REGISTER_INTERFACE_NAME(IDocView, L"IDocView");
	REGISTER_INTERFACE_NAME(IDocWriter, L"IDocWriter");
	REGISTER_INTERFACE_NAME(IDraw, L"IDraw");	
	REGISTER_INTERFACE_NAME(IDrawExtensible, L"IDrawExtensible");		
	REGISTER_INTERFACE_NAME(IDrawExtension, L"IDrawExtension");
	REGISTER_INTERFACE_NAME(IDrawExtensionHelper, L"IDrawExtensionHelper");
	REGISTER_INTERFACE_NAME(IDrawHelper, L"IDrawHelper");
	REGISTER_INTERFACE_NAME(IDropApp, L"IDropApp");
	REGISTER_INTERFACE_NAME(IDropAppHelper, L"IDropAppHelper");
	REGISTER_INTERFACE_NAME(IDropAppProgress, L"IDropAppProgress");
	
	REGISTER_INTERFACE_NAME(IEndUndoSequence, L"IEndUndoSequence");	
	REGISTER_INTERFACE_NAME(IEventLoop, L"IEventLoop");

	REGISTER_INTERFACE_NAME(IFocus, L"IFocus");
	REGISTER_INTERFACE_NAME(IFocused, L"IFocused");
	
	REGISTER_INTERFACE_NAME(IGeometry, L"IGeometry");
	REGISTER_INTERFACE_NAME(IGroupBox, L"IGroupBox");
	REGISTER_INTERFACE_NAME(IGroupControl, L"IGroupControl");
	
	REGISTER_INTERFACE_NAME(IHelpText, L"IHelpText");
	REGISTER_INTERFACE_NAME(IHierarchy, L"IHierarchy");
	
	REGISTER_INTERFACE_NAME(IInit, L"IInit");
	REGISTER_INTERFACE_NAME(IInitMenuBar, L"IInitMenuBar");	
	REGISTER_INTERFACE_NAME(IInstallMenuHandler, L"IInstallMenuHandler");
	REGISTER_INTERFACE_NAME(IInstance, L"IInstance");
	
	REGISTER_INTERFACE_NAME(IKeyExtensible, L"IKeyExtensible");
	REGISTER_INTERFACE_NAME(IKeyExtension, L"IKeyExtension");
	REGISTER_INTERFACE_NAME(IKeyHandler, L"IKeyHandler");		

	REGISTER_INTERFACE_NAME(IListBox, L"IListBox");		
	REGISTER_INTERFACE_NAME(ILoaded, L"ILoaded");		
	REGISTER_INTERFACE_NAME(ILocationChanged, L"ILocationChanged");		

	REGISTER_INTERFACE_NAME(IMergeActions, L"IMergeActions");
	REGISTER_INTERFACE_NAME(IMemento, L"IMemento");
	REGISTER_INTERFACE_NAME(IMenu, L"IMenu");
	REGISTER_INTERFACE_NAME(IMenuBar, L"IMenuBar");
	REGISTER_INTERFACE_NAME(IMenuExtensible, L"IMenuExtensible");
	REGISTER_INTERFACE_NAME(IMenuExtension, L"IMenuExtension");
	REGISTER_INTERFACE_NAME(IMenuCommandClickExtension, L"IMenuCommandClickExtension");
	REGISTER_INTERFACE_NAME(IMenuHandler, L"IMenuHandler");
	REGISTER_INTERFACE_NAME(IMouseDownHelper, L"IMouseDownHelper");
	REGISTER_INTERFACE_NAME(IMouseExtensible, L"IMouseExtensible");
	REGISTER_INTERFACE_NAME(IMouseExtension, L"IMouseExtension");
	REGISTER_INTERFACE_NAME(IMouseHandler, L"IMouseHandler");
	REGISTER_INTERFACE_NAME(IMouseUpHelper, L"IMouseUpHelper");
	REGISTER_INTERFACE_NAME(IMultipleUndoAction, L"IMultipleUndoAction");		
		
	REGISTER_INTERFACE_NAME(IOpened, L"IOpened");
	
	REGISTER_INTERFACE_NAME(IParentChangedSize, L"IParentChangedSize");
	REGISTER_INTERFACE_NAME(IPen, L"IPen");
	REGISTER_INTERFACE_NAME(IPersistDoc, L"IPersistDoc");
	REGISTER_INTERFACE_NAME(IPoseDialogBox, L"IPoseDialogBox");
	REGISTER_INTERFACE_NAME(IPositionWindow, L"IPositionWindow");
	REGISTER_INTERFACE_NAME(IProgressBar, L"IProgressBar");
	REGISTER_INTERFACE_NAME(IPushButton, L"IPushButton");	
	
	REGISTER_INTERFACE_NAME(IRadioButton, L"IRadioButton");
	REGISTER_INTERFACE_NAME(IRange<int32>, L"IInt32Range");
	REGISTER_INTERFACE_NAME(IRange<uint32>, L"IUInt32Range");
	REGISTER_INTERFACE_NAME(IRange<float>, L"IFloatRange");
	REGISTER_INTERFACE_NAME(IRange<double>, L"IDoubleRange");
	REGISTER_INTERFACE_NAME(IReadXML, L"IReadXML");
	REGISTER_INTERFACE_NAME(IRestoreWindowPosition, L"IRestoreWindowPosition");	
	REGISTER_INTERFACE_NAME(IRootControl, L"IRootControl");	
	
	REGISTER_INTERFACE_NAME(IScrap, L"IScrap");
	REGISTER_INTERFACE_NAME(IScrapHelper, L"IScrapHelper");
	REGISTER_INTERFACE_NAME(IScrollableControl, L"IScrollableControl");
	REGISTER_INTERFACE_NAME(IScrollBar, L"IScrollBar");
	REGISTER_INTERFACE_NAME(IScrollerControl, L"IScrollerControl");
	REGISTER_INTERFACE_NAME(ISeperator, L"ISeperator");
	REGISTER_INTERFACE_NAME(ISetCheckedHelp, L"ISetCheckedHelp");
	REGISTER_INTERFACE_NAME(ISetDisabledHelp, L"ISetDisabledHelp");	
	REGISTER_INTERFACE_NAME(ISetHelp, L"ISetHelp");
	REGISTER_INTERFACE_NAME(IShowHelp, L"IShowHelp");
	REGISTER_INTERFACE_NAME(ISizeChanged, L"ISizeChanged");
	REGISTER_INTERFACE_NAME(IStaticText, L"IStaticText");
	REGISTER_INTERFACE_NAME(IStringTableLoader, L"IStringTableLoader");
	
	REGISTER_INTERFACE_NAME(ITabber, L"ITabber");	
	REGISTER_INTERFACE_NAME(IText, L"IText");
	REGISTER_INTERFACE_NAME(ITextBox, L"ITextBox");
	REGISTER_INTERFACE_NAME(ITimer, L"ITimer");
	REGISTER_INTERFACE_NAME(ITrackControlHelper, L"ITrackControlHelper");		
	REGISTER_INTERFACE_NAME(ITracker, L"ITracker");	
	REGISTER_INTERFACE_NAME(ITrackerHelper, L"ITrackerHelper");	
	
	REGISTER_INTERFACE_NAME(IValidateText, L"IValidateText");
	REGISTER_INTERFACE_NAME(IValidateTextHelper, L"IValidateTextHelper");	
	REGISTER_INTERFACE_NAME(IValueChanged, L"IValueChanged");
	REGISTER_INTERFACE_NAME(IValueControl, L"IValueControl");
	
	REGISTER_INTERFACE_NAME(IUndoAction, L"IUndoAction");
	REGISTER_INTERFACE_NAME(IUndoActionHelper, L"IUndoActionHelper");
	REGISTER_INTERFACE_NAME(IUndoCallbackAction, L"IUndoCallbackAction");
	REGISTER_INTERFACE_NAME(IUndoContext, L"IUndoContext");
	REGISTER_INTERFACE_NAME(IUserControl, L"IUserControl");
	
	REGISTER_INTERFACE_NAME(IValidate, L"IValidate");
	REGISTER_INTERFACE_NAME(IValue<bool>, L"IBoolValue");
	REGISTER_INTERFACE_NAME(IValue<float>, L"IFloatValue");	
	REGISTER_INTERFACE_NAME(IValue<int32>, L"IInt32Value");
	REGISTER_INTERFACE_NAME(IValue<uint32>, L"IUInt32Value");
	REGISTER_INTERFACE_NAME(IValue<double>, L"IDoubleValue");	

	REGISTER_INTERFACE_NAME(IWindow, L"IWindow");
	REGISTER_INTERFACE_NAME(IWindowClick, L"IWindowClick");
	REGISTER_INTERFACE_NAME(IWindowMgr, L"IWindowMgr");
	REGISTER_INTERFACE_NAME(IWriteXML, L"IWriteXML");
	
	REGISTER_INTERFACE_NAME(IXMLMenu, L"IXMLMenu");

#if MAC
	REGISTER_INTERFACE_NAME(IEventDispatcher, L"IEventDispatcher"); 	
	REGISTER_INTERFACE_NAME(IEventExtensible, L"IEventExtensible");
	REGISTER_INTERFACE_NAME(IEventExtension, L"IEventExtension");	
#elif WIN
	REGISTER_INTERFACE_NAME(IFrameWindow, L"IFrameWindow");
	REGISTER_INTERFACE_NAME(IMsgExtensible, L"IMsgExtensible");
	REGISTER_INTERFACE_NAME(IMsgExtension, L"IMsgExtension");
#endif
}


//---------------------------------------------------------------
//
// RegisterUIImplementations
//
//---------------------------------------------------------------
void RegisterUIImplementations()
{
	RegisterXImplementations();
	RegisterMacImplementations();
	RegisterWinImplementations();
}


}	// namespace Whisper
