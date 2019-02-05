/*
 *  File:       WTextBox.cpp
 *  Summary:   	ITextBox implementation for Windows.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WTextBox.cpp,v $
 *		Revision 1.10  2001/04/27 09:19:44  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.9  2001/04/21 03:34:08  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.8  2001/03/26 04:17:29  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.7  2001/03/17 12:32:33  jesjones
 *		DoChar returns false for pass up chars.
 *		
 *		Revision 1.6  2001/02/23 10:09:46  jesjones
 *		Got it working again.
 *		
 *		Revision 1.5  2001/02/21 08:16:58  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2001/01/21 00:24:21  jesjones
 *		Descends from IScrapHelper instead of XScrap.
 *		
 *		Revision 1.3  2001/01/05 06:23:01  jesjones
 *		Descends from IScrapHelper instead of XScrap.
 *		
 *		Revision 1.2  2000/12/27 01:03:49  jesjones
 *		Adjusts the cursor and has a context menu.
 *		
 *		Revision 1.1  2000/12/25 00:28:40  jesjones
 *		Checked in (not tested yet though).
 */

#include <XWhisperHeader.h>
#include <ITextBox.h>

#include <windowsx.h>

#include <IActiveUndoContext.h>
#include <IAdjustCursorHelper.h>
#include <IChildNode.h>
#include <ICommand.h>
#include <ICommandQueue.h>
#include <IContextMenu.h>
#include <IContextMenuHelper.h>
#include <IControl.h>
#include <IFocus.h>
#include <IFocused.h>
#include <IHierarchy.h>
#include <IInstance.h>
#include <IKeyHandler.h>
#include <IMemento.h>
#include <IMenuHandler.h>
#include <IScrap.h>
#include <IScrapHelper.h>
#include <IText.h>
#include <IUndoAction.h>
#include <IUndoContext.h>
#include <IValidate.h>
#include <IValidateText.h>
#include <WSystemInfo.h>
#include <WUtils.h>
#include <XCursor.h>
#include <XExceptions.h>
#include <XKeyEvents.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XMouseEvents.h>
#include <XNumbers.h>
#include <XTinyVector.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	struct SEditBoxMemento
//!		SMemento object used by WTextBox.
// ===================================================================================
struct SEditBoxMemento : public SMemento 
{
	HWND			controlH;
	std::wstring	text;
	uint32			selStart;
	uint32			selEnd;

	virtual bool 		CanMerge(const SMemento* newState, const XConstUnknownPtr& object) const;
};


//---------------------------------------------------------------
//
// SEditBoxMemento::CanMerge
//
//---------------------------------------------------------------
bool SEditBoxMemento::CanMerge(const SMemento* oldState, const XConstUnknownPtr& object) const
{
	UNUSED(object);
	
	const SEditBoxMemento* memento = dynamic_cast<const SEditBoxMemento*>(oldState);
	PRECONDITION(memento != nil);					// we're only called if the bosses match so this should never fire
	PRECONDITION(controlH == memento->controlH);	// we're only called if the objects match so this should never fire
	
	// If the selection hasn't changed typing commands can be merged.
	bool can = selStart == memento->selStart && selEnd == memento->selEnd;

	return can;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class WTextBox
//!		ITextBox implementation for Windows.
// ===================================================================================
class WTextBox : public ITextBox, public IText, public IValidate, public IFocused, public IAdjustCursorHelper, public IContextMenuHelper, public IScrapHelper, public IChildNode, public IMemento {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WTextBox();
	
						WTextBox(XBoss* boss);
							
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultTextBoxStyle,
							 uint32 procID = kDefaultTextBoxProcID);

	virtual void		SetSelection(uint32 start, uint32 stop, bool redraw = kRedraw);
	virtual void		SelectAll(bool redraw = kRedraw)						{this->SetSelection(0, 32767, redraw);}
	virtual void		Select(bool redraw = kRedraw);

	virtual void 		SetChangedCallback(const ChangedCallback& callback)		{mChangedCallback = callback;}
	virtual void 		SetPassUpCallback(const PassUpCallback& callback)		{mPassUpCallback = callback;}
	virtual void 		SetRejectedCallback(const RejectCallback& callback)		{mRejectCallback = callback;}

	virtual bool 		DefaultPassUp(ITextBox* textbox, int16 key, wchar_t ch);
	virtual void 		DefaultReject(ITextBox* textbox, const std::wstring& lostChars);
	
	virtual bool 		Validate() const;

	virtual bool 		IsEmpty() const;
	virtual std::wstring GetText() const;
	virtual void 		SetText(const std::wstring& text, bool redraw = kRedraw);

	virtual SMementoPtr GetState() const;
	virtual void 		SetState(const SMementoPtr& memento);
										
	virtual XHandle 	Copy(uint32 type);
	virtual void	 	Paste(uint32 type, const XHandle& data);
	virtual void 		Clear();
	virtual bool 		HasSelection() const;
						
	virtual uint32 		GetCopyCount() const							{return 2;}	
	virtual uint32		GetCopyType(uint32 index) const					{return (uint32) (index == 0 ? CF_UNICODETEXT : CF_TEXT);}	
	virtual uint32 		GetPasteCount() const							{return 2;}
	virtual uint32		GetPasteType(uint32 index) const				{return (uint32) (index == 0 ? CF_UNICODETEXT : CF_TEXT);}

protected:
	virtual void 		OnAdopted()										{}
	virtual void 		OnOrphaned();

	virtual bool 		OnAdjustCursor(const XMouseMoveEvent& event);
	virtual bool 		OnContextMenu(const XMouseDownEvent& event);

	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

//-----------------------------------
//	Internal API
//
private:			
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);
			void 		DoClearKey(const XCharEvent& event);
			void 		DoBackSpaceKey(const XCharEvent& event);
			void 		DoForwardDeleteKey(const XKeyDownEvent& event);

//-----------------------------------
//	Member Data
//
private:
	HWND				mControlH;
	
	ChangedCallback		mChangedCallback;
	RejectCallback		mRejectCallback;
	PassUpCallback		mPassUpCallback;
};

DEFINE_INTERFACE_FACTORY(WTextBox)

//---------------------------------------------------------------
//
// WTextBox::~WTextBox
//
//---------------------------------------------------------------
WTextBox::~WTextBox()
{
}


//---------------------------------------------------------------
//
// WTextBox::WTextBox
//
//---------------------------------------------------------------
WTextBox::WTextBox(XBoss* boss)
{	
	ITextBox::DoSetBoss(boss);
	IText::DoSetBoss(boss);
	IValidate::DoSetBoss(boss);
	IMemento::DoSetBoss(boss);
	IScrapHelper::DoSetBoss(boss);
	IFocused::DoSetBoss(boss);
	IChildNode::DoSetBoss(boss);
	IAdjustCursorHelper::DoSetBoss(boss);
	IContextMenuHelper::DoSetBoss(boss);
	
	mControlH = nil;

	mRejectCallback = RejectCallback(this, &ITextBox::DefaultReject);
	mPassUpCallback = PassUpCallback(this, &ITextBox::DefaultPassUp);
}


//---------------------------------------------------------------
//
// WTextBox::Init
//
//---------------------------------------------------------------
void WTextBox::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& text,
					   uint32 style, uint32 procID)
{
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(WS_EX_CLIENTEDGE,		// extended style
									 _TEXT("EDIT"),			// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 super->GetOSControl(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	ITextBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Save a copy of the control's window handle
	mControlH = control->GetOSControl();
		
	// Set the text		
	this->SetText(text, kDontRedraw);
}


//---------------------------------------------------------------
//
// WTextBox::SetSelection
//
//---------------------------------------------------------------
void WTextBox::SetSelection(uint32 start, uint32 stop, bool redraw)
{
	PRECONDITION(start <= stop);
			
	if (!redraw)
		VERIFY(::LockWindowUpdate(mControlH));

	if (start < stop)
		Edit_SetSel(mControlH, start, (int32) stop);	// note that start and stop refer to the positions between characters
	else
		Edit_SetSel(mControlH, start, (int32) start);

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
}

						
//---------------------------------------------------------------
//
// WTextBox::Select
//
//---------------------------------------------------------------
void WTextBox::Select(bool redraw)
{
	this->SelectAll(redraw);
	
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);
	focus->SetFocus();
}

						
//---------------------------------------------------------------
//
// WTextBox::DefaultPassUp
//
//---------------------------------------------------------------
bool WTextBox::DefaultPassUp(ITextBox* textbox, int16 key, wchar_t ch)
{
	UNUSED(textbox);
	UNUSED(ch);
	
	bool passup = key == kReturnKey || key == kTabKey || key == kEscapeKey;
		
	return passup;
}

						
//---------------------------------------------------------------
//
// WTextBox::DefaultReject
//
//---------------------------------------------------------------
void WTextBox::DefaultReject(ITextBox* textbox, const std::wstring& lostChars)
{
	UNUSED(textbox);
	UNUSED(lostChars);
	
	Whisper::Beep();
}

	
//---------------------------------------------------------------
//
// WTextBox::Validate
//
//---------------------------------------------------------------
bool WTextBox::Validate() const
{
	bool valid = true;
	
	const ITextBox* thisPtr = this;
	IConstValidateTextPtr validator(thisPtr);
	if (validator) {
		valid = validator->Validate(this->GetText());
		
		if (!valid) {
			const_cast<ITextBox*>(thisPtr)->SelectAll();
			
			IFocusPtr focus(const_cast<ITextBox*>(thisPtr));
			focus->SetFocus();
		}
	}
	
	return valid;
}


//---------------------------------------------------------------
//
// WTextBox::IsEmpty
//
//---------------------------------------------------------------
bool WTextBox::IsEmpty() const
{
	int32 length = GetWindowTextLength(mControlH);
	
	return length == 0;
}


//---------------------------------------------------------------
//
// WTextBox::GetText
//
//---------------------------------------------------------------
std::wstring WTextBox::GetText() const
{
	std::wstring text = Whisper::GetText(mControlH);
	
	return text;
}


//---------------------------------------------------------------
//
// WTextBox::SetText
//
//---------------------------------------------------------------
void WTextBox::SetText(const std::wstring& text, bool redraw)
{
	if (text != this->GetText()) {
		if (!redraw)
			VERIFY(::LockWindowUpdate(mControlH));

		int32 succeeded;
		if (WSystemInfo::HasUnicode()) 
			succeeded = ::SetWindowTextW(mControlH, text.c_str());
		else
			succeeded = ::SetWindowTextA(mControlH, ToPlatformStr(text).c_str());
		
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
		ThrowIf(!succeeded);
			
		if (redraw) {
			ITextBox* thisPtr = this;
			IControlPtr control(thisPtr);
			control->Invalidate();
		}
	}
}


//---------------------------------------------------------------
//
// WTextBox::GetState
//
//---------------------------------------------------------------
SMementoPtr WTextBox::GetState() const
{
	SEditBoxMemento* memento = new SEditBoxMemento;
	
	uint32 selection = Edit_GetSel(mControlH);
	
	memento->controlH = mControlH;
	memento->text     = this->GetText();
	memento->selStart = LOWORD(selection);
	memento->selEnd   = HIWORD(selection);
	
	return SMementoPtr(memento);
}


//---------------------------------------------------------------
//
// WTextBox::SetState
//
//---------------------------------------------------------------
void WTextBox::SetState(const SMementoPtr& mementoPtr)
{
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);
	focus->SetFocus();

	const SEditBoxMemento* memento = dynamic_cast<const SEditBoxMemento*>(mementoPtr.Get());
	PRECONDITION(memento != nil);
	PRECONDITION(memento->controlH == mControlH);

	this->SetText(memento->text);
	this->SetSelection(memento->selStart, memento->selEnd);
				
	IValidateTextPtr validator(thisPtr);
	if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
		mChangedCallback(this);
}


//---------------------------------------------------------------
//
// WTextBox::Copy
//
//---------------------------------------------------------------
XHandle WTextBox::Copy(uint32 type)
{
	XHandle data;
	
	uint32 selection = Edit_GetSel(mControlH);
	uint32 start = LOWORD(selection);
	uint32 stop  = HIWORD(selection);
	
	ITextBox* thisPtr = this;
	ITextPtr text(thisPtr);
	std::wstring theText = text->GetText();
	std::wstring theSelection = theText.substr(start, stop - start);
	
	if (type == CF_TEXT) {
		PlatformString str = ToPlatformStr(theSelection);
		
		uint32 bytes = str.length() + 1;		
		data.SetSize(bytes);
		BlockMoveData(str.c_str(), data.GetUnsafePtr(), bytes);
		
	} else if (type == CF_UNICODETEXT) {
		uint32 bytes = (theSelection.length()+1)*sizeof(uint16);	// don't use wchar_t (may be 32-bits)
		data.SetSize(bytes);
		BlockMoveData(theSelection.c_str(), data.GetUnsafePtr(), bytes);
	
	} else
		DEBUGSTR("Bogus type in WTextBox::OnCopy");
		
	return data;
}


//---------------------------------------------------------------
//
// WTextBox::Paste
//
//---------------------------------------------------------------
void WTextBox::Paste(uint32 type, const XHandle& data)
{
	COMPILE_CHECK(sizeof(wchar_t) == 2);
	
	std::wstring accepted, rejected;
	
	{
	// Get the text we're pasting
	XLocker lock(data);	
		if (type == CF_TEXT) {
			const char* str = reinterpret_cast<const char*>(data.GetPtr());
			rejected = FromPlatformStr(str, data.GetSize() - 1);
			
		} else if (type == CF_UNICODETEXT) {
			const wchar_t* str = reinterpret_cast<const wchar_t*>(data.GetPtr());
			rejected.assign(str, (data.GetSize()-1)/sizeof(wchar_t));
		
		} else
			DEBUGSTR("Bogus type in WTextBox::OnPaste");
	}
	
	// Find out how much we can really paste
	bool valid = true;
	
	ITextBox* thisPtr = this;
	IValidateTextPtr validator(thisPtr);
	if (validator) {
		std::wstring text = this->GetText();
		
		uint32 selection = Edit_GetSel(mControlH);
		if (LOWORD(selection) != HIWORD(selection)) 
			text.erase(LOWORD(selection), numeric_cast<uint32>(HIWORD(selection) - LOWORD(selection)));
		
		uint32 start = LOWORD(selection);
		while (valid && rejected.length() > 0) {		
			valid = text.length() < validator->GetMaxChars() && validator->IsValid(rejected[0]);
			if (valid) {
				text.insert(start++, 1UL, rejected[0]);
				
				accepted += rejected[0];
				rejected.erase(0, 1);
			}
		}
		
	} else {
		accepted = rejected;
		rejected = L"";
	}
	
	// If we had to reject some text call the callback
	if (rejected.length() > 0)
		mRejectCallback(this, rejected);
		
	// Insert whatever we can
	if (accepted.length() > 0) {
		if (WSystemInfo::HasUnicode())
			SendMessageW(mControlH, EM_REPLACESEL, 0L, (LPARAM) accepted.c_str());
		else
			SendMessageA(mControlH, EM_REPLACESEL, 0L, (LPARAM) ToPlatformStr(accepted).c_str());

		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


//---------------------------------------------------------------
//
// WTextBox::Clear
//
//---------------------------------------------------------------
void WTextBox::Clear()
{
	PRECONDITION(this->HasSelection());
	
	Edit_ReplaceSel(mControlH, _TEXT(""));

	ITextBox* thisPtr = this;
	IValidateTextPtr validator(thisPtr);
	if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
		mChangedCallback(this);
}


//---------------------------------------------------------------
//
// WTextBox::HasSelection
//
//---------------------------------------------------------------
bool WTextBox::HasSelection() const
{
	uint32 selection = Edit_GetSel(mControlH);
	
	return LOWORD(selection) != HIWORD(selection);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WTextBox::OnOrphaned
//
//---------------------------------------------------------------
void WTextBox::OnOrphaned()
{
	ITextBox* thisPtr = this;
	IUndoContextPtr context(thisPtr);
	if (context)
		context->ClearHistory();
}


//---------------------------------------------------------------
//
// WTextBox::OnAcquiredFocus
//
//---------------------------------------------------------------
void WTextBox::OnAcquiredFocus()
{
	IKeyHandlerPtr handler(L"Application");
	ITextBox* thisPtr = this;
		
	XCallback1<bool, XKeyDownEvent> handleKey(this, &WTextBox::DoKey);
	XCallback1<bool, XCharEvent> handleChar(this, &WTextBox::DoChar);
	handler->RegisterHandler(handleKey, handleChar, thisPtr);	// do this first so that our specific key handlers get invoked first
}


//---------------------------------------------------------------
//
// WTextBox::OnLostFocus
//
//---------------------------------------------------------------
void WTextBox::OnLostFocus()
{
	IKeyHandlerPtr handler(L"Application");
	
	XCallback1<bool, XKeyDownEvent> handleKey(this, &WTextBox::DoKey);
	handler->UnRegisterHandler(handleKey);	
}


//---------------------------------------------------------------
//
// WTextBox::OnAdjustCursor
//
//---------------------------------------------------------------
bool WTextBox::OnAdjustCursor(const XMouseMoveEvent& event)
{
	UNUSED(event);
	
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);

	if (focus->HasFocus())
		XCursorUtils::SetCursor(kIBeamCursor);
	else
		XCursorUtils::SetCursor(kArrowCursor);
	
	return kHandled;
}


//---------------------------------------------------------------
//
// WTextBox::OnContextMenu
//
//---------------------------------------------------------------
bool WTextBox::OnContextMenu(const XMouseDownEvent& event)
{
	// Make 'this' the focus (doesn't work quite right if this
	// isn't done).
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);
	focus->SetFocus();
	
	// Create a context menu
	IContextMenuPtr popup(L"Context Menu");
	popup->Init(event.GetGlobalPosition());

	IMenuPtr menu(popup);
	menu->Init(L"Context Menu");
	
	// Populate it
	IActiveUndoContextPtr activeContext(L"Application");
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();

	if (context && context->CanUndo())
		menu->AppendItem(LoadWhisperString(L"Undo"), kUndoCmd);
	if (context && context->CanRedo())
		menu->AppendItem(LoadWhisperString(L"Redo"), kRedoCmd);
		
	if (menu->GetCount() > 0)
		menu->AppendSeparator();
		
	IScrapPtr scrap(thisPtr);
	if (scrap->CanCut()) 
		menu->AppendItem(LoadWhisperString(L"Cut"), kCutCmd);
	if (scrap->CanCopy()) 
		menu->AppendItem(LoadWhisperString(L"Copy"), kCopyCmd);
	if (scrap->CanPaste()) 
		menu->AppendItem(LoadWhisperString(L"Paste"), kPasteCmd);
	if (scrap->CanClear()) 
		menu->AppendItem(LoadWhisperString(L"Clear"), kClearCmd);
		
	if (!menu->IsSeparator(menu->GetCount() - 1))
		menu->AppendSeparator();

	menu->AppendItem(LoadWhisperString(L"Select All"), kSelectAllCmd);

	// And let the user pick something
	popup->Execute();
				
	return kHandled;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WTextBox::DoKey
//
//---------------------------------------------------------------
bool WTextBox::DoKey(const XKeyDownEvent& event)
{
	bool handled = false;

	wchar_t ch = event.ComputeChar();
	int16 key = event.GetKey();
	
	if (!mPassUpCallback(this, key, ch)) {
		if (key == kFwdDeleteKey)
			this->DoForwardDeleteKey(event);
		
		else {
			ITextBox* thisPtr = this;
			IControlPtr control(thisPtr);
			control->DoKey(event);
		}
		
		handled = true;
	}
	
	return handled;
}

		
//---------------------------------------------------------------
//
// WTextBox::DoChar
//
//---------------------------------------------------------------
bool WTextBox::DoChar(const XCharEvent& event)
{
	ITextBox* thisPtr = this;
	IControlPtr control(thisPtr);

	PRECONDITION(control->IsVisible());
	PRECONDITION(control->IsEnabled());
		
	bool handled = false;		
	wchar_t ch = event.GetChar();
	int16 key = event.ComputeKey();
	
	if (!mPassUpCallback(this, key, ch)) {	
		if (key == kClearKey)				// can't use a SingleKeyHandler because the OS control wants the WM_CHAR events
			this->DoClearKey(event);
		
		else if (key == kBackspaceKey)
			this->DoBackSpaceKey(event);
			
		else {
			IValidateTextPtr validator(thisPtr);

			bool valid = !validator || validator->IsValid(ch);
			if (valid && validator) {
				if (!this->HasSelection()) 			// if there's a selection the text length won't increase
					valid = this->GetText().length() < validator->GetMaxChars();
			}
			
			if (valid) {
				// Post a command for undo
				IInstancePtr instance(L"Typing Action");
				instance->Set(thisPtr);
				
				IUndoActionPtr action(instance);
				action->Init(LoadWhisperString(L"Typing"));

				ICommandQueuePtr queue(L"Application");
				queue->Post(action);

				// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
				control->DoChar(event);

				if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
					mChangedCallback(this);
					
			} else
				mRejectCallback(this, std::wstring(1, ch));
				
			handled = true;
		}
	}
					
	return handled;
}


//---------------------------------------------------------------
//
// WTextBox::DoClearKey
//
//---------------------------------------------------------------
void WTextBox::DoClearKey(const XCharEvent& event)
{
	if (this->HasSelection()) {
		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kClearCmd, event);
	}
}


//---------------------------------------------------------------
//
// WTextBox::DoBackSpaceKey
//
//---------------------------------------------------------------
void WTextBox::DoBackSpaceKey(const XCharEvent& event)
{
	uint32 selection = Edit_GetSel(mControlH);
	if (LOWORD(selection) > 0 || (LOWORD(selection) != HIWORD(selection))) {
		ITextBox* thisPtr = this;

		// Post a command for undo
		IInstancePtr instance(L"Delete Action");
		instance->Set(thisPtr);
		
		IUndoActionPtr action(instance);
		action->Init(LoadWhisperString(L"Backspace"));
		
		ICommandQueuePtr queue(L"Application");
		queue->Post(action);

		// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
		MSG msg = event.GetOSEvent();
		
		IControlPtr control(thisPtr);

		WNDPROC proc = control->GetSystemWindowProc();
		int32 result = ::CallWindowProc(proc, msg.hwnd, msg.message, msg.wParam, msg.lParam);	// $$ assumes that textbox control will always process backspace via WM_CHAR)
//		ASSERT(result == 0);

		IValidateTextPtr validator(thisPtr);
		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


//---------------------------------------------------------------
//
// WTextBox::DoForwardDeleteKey
//
//---------------------------------------------------------------
void WTextBox::DoForwardDeleteKey(const XKeyDownEvent& event)
{
	uint32 selection = Edit_GetSel(mControlH);
	if (LOWORD(selection) < this->GetText().length() || (LOWORD(selection) != HIWORD(selection))) {
		ITextBox* thisPtr = this;

		// Post a command for undo
		IInstancePtr instance(L"Forward Delete Action");
		instance->Set(thisPtr);
		
		IUndoActionPtr action(instance);
		action->Init(LoadWhisperString(L"Forward Delete"));

		ICommandQueuePtr queue(L"Application");
		queue->Post(action);

		// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
		MSG msg = event.GetOSEvent();
		
		IControlPtr control(thisPtr);

		WNDPROC proc = control->GetSystemWindowProc();
		int32 result = ::CallWindowProc(proc, msg.hwnd, msg.message, msg.wParam, msg.lParam);	// $$ assumes that textbox control will always process backspace via WM_CHAR)
		ASSERT(result == 0);

		IValidateTextPtr validator(thisPtr);
		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


}	// namespace Whisper