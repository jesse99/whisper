/*
 *  File:       MTETextBox.cpp
 *  Summary:   	ITextBox implementation that uses TextEdit (so it's limited to 32K and doesn't handle Unicode well).
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MTETextBox.cpp,v $
 *		Revision 1.16  2001/04/27 04:22:35  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.15  2001/04/21 03:29:01  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.14  2001/04/02 23:35:36  jesjones
 *		Tweaked for CW Pro 5.3
 *		
 *		Revision 1.13  2001/04/01 04:55:57  jesjones
 *		Uses the new XCallback::Set methods.
 *		
 *		Revision 1.12  2001/03/26 04:17:17  jesjones
 *		Added IsEmpty.
 *		
 *		Revision 1.11  2001/03/02 11:22:57  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.10  2001/02/12 09:45:36  jesjones
 *		DoKey no longer eats everything.
 *		
 *		Revision 1.9  2001/02/04 03:35:36  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.8  2001/01/30 07:09:16  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.7  2001/01/21 00:34:42  jesjones
 *		Reworked the menu handling code.
 *		
 *		Revision 1.6  2001/01/05 06:20:12  jesjones
 *		Descends from IScrapHelper instead of XScrap.
 *		
 *		Revision 1.5  2000/12/27 01:00:52  jesjones
 *		Tweaked to allow for absent undo context.
 *		
 *		Revision 1.4  2000/12/25 00:35:02  jesjones
 *		Overrides OnAdjustCursor. Enabled F1-F4 keys.
 *		
 *		Revision 1.3  2000/12/24 03:21:49  jesjones
 *		Made typing undoable. Fixed a minor bug in the paste code.
 *		
 *		Revision 1.2  2000/12/22 09:27:01  jesjones
 *		Added support for arrow keys, scrap operations, and memento's.
 *		
 *		Revision 1.1  2000/12/19 07:45:20  jesjones
 *		Checked in (still needs quite a bit of work)	
 */

#include <XWhisperHeader.h>
#include <ITextBox.h>

#include <cctype>

#include <IActiveUndoContext.h>
#include <IAdjustCursorHelper.h>
#include <ICommand.h>
#include <ICommandQueue.h>
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
#include <XExceptions.h>
#include <XCursor.h>
#include <XKeyEvents.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XNumbers.h>
#include <XTinyVector.h>
#include <XTranscode.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kIndeterminateAnchor = ULONG_MAX;

const bool kPreferPreviousLine = true;
const bool kPreferNextLine     = false;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// TEIsFrontOfLine
//
// Returns true if the offset is at the beginning of a line.
//
//---------------------------------------------------------------
static int16 TEIsFrontOfLine(int16 offset, TEHandle theTE)
{
	int16 line = 0;

	if ((**theTE).teLength == 0 || offset == 0)
		return true;

	if (offset >= (**theTE).teLength)
		return ((*((**theTE).hText))[(**theTE).teLength - 1] == kReturnCharCode);

	while ((**theTE).lineStarts[line] < offset)
		line++;

	return ((**theTE).lineStarts[line] == offset);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct STEMemento
//!		SMemento object used by MTETextBox.
// ===================================================================================
struct STEMemento : public SMemento 
{
	TEHandle		textH;
	std::wstring	text;
	uint32			selStart;
	uint32			selEnd;

	virtual bool 		CanMerge(const SMemento* newState, const XConstUnknownPtr& object) const;
};


//---------------------------------------------------------------
//
// STEMemento::CanMerge
//
//---------------------------------------------------------------
bool STEMemento::CanMerge(const SMemento* oldState, const XConstUnknownPtr& object) const
{
	UNUSED(object);
	
	const STEMemento* memento = dynamic_cast<const STEMemento*>(oldState);
	PRECONDITION(memento != nil);				// we're only called if the bosses match so this should never fire
	PRECONDITION(textH == memento->textH);	// we're only called if the objects match so this should never fire
	
	// If the selection hasn't changed typing commands can be merged.
	bool can = selStart == memento->selStart && selEnd == memento->selEnd;

	return can;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class MTETextBox
//!		ITextBox implementation that uses TextEdit (so it's limited to 32K and doesn't handle Unicode well).
// ===================================================================================
class MTETextBox : public ITextBox, public IText, public IValidate, public IFocused, public IAdjustCursorHelper, public IScrapHelper, public IMemento {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MTETextBox();
	
						MTETextBox(XBoss* boss);
						
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
	virtual uint32		GetCopyType(uint32 index) const					{return index == 0 ? 'utxt' : 'TEXT';}	
	virtual uint32 		GetPasteCount() const							{return 2;}
	virtual uint32		GetPasteType(uint32 index) const				{return index == 0 ? 'utxt' : 'TEXT';}

protected:
	virtual bool 		OnAdjustCursor(const XMouseMoveEvent& event);

	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

//-----------------------------------
//	Internal API
//
private:			
			void 		DoUndoKey();
			void 		DoCutKey();
			void 		DoCopyKey();
			void 		DoPasteKey();
			void 		DoClearKey();
			void 		DoHomeKey();
			void 		DoEndKey();
			void 		DoBackSpaceKey();
			void 		DoForwardDeleteKey();
			void 		DoArrowKey();
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);
								
			int16 		DoFindLineNumber(uint32 offset, bool lefty) const;

			void 		DoGetWordBreaks(uint32 index, uint32* startP, uint32* stopP) const;
			uint32 		DoFindLeftWordEdge(uint32 index) const;
			uint32 		DoFindRightWordEdge(uint32 index) const;

			uint32 		DoFindLineStart(uint32 offset) const;
			uint32 		DoFindLineEnd(uint32 offset) const;

			uint32 		DoFindPreviousLine(uint32 offset) const;
			uint32 		DoFindNextLine(uint32 offset) const;

			XPoint 		DoOffsetToPoint(uint32 offset) const;
			uint32 		DoPointToOffset(const XPoint& pt) const;

//-----------------------------------
//	Member Data
//
private:
	ControlHandle		mControlH;
	TEHandle			mTextH;

	uint32				mSelAnchor;			// used when extending selections with the arrow keys
	int16				mUpDownOffset;		// the pixel offset from the start of the line the up/down arrowing started on
	bool				mWasUpDown;			// last arrow key as up or down

	ChangedCallback		mChangedCallback;
	RejectCallback		mRejectCallback;
	PassUpCallback		mPassUpCallback;
};

DEFINE_INTERFACE_FACTORY(MTETextBox)

//---------------------------------------------------------------
//
// MTETextBox::~MTETextBox
//
//---------------------------------------------------------------
MTETextBox::~MTETextBox()
{
}


//---------------------------------------------------------------
//
// MTETextBox::MTETextBox
//
//---------------------------------------------------------------
MTETextBox::MTETextBox(XBoss* boss)
{	
	IAdjustCursorHelper::DoSetBoss(boss);
	ITextBox::DoSetBoss(boss);
	IText::DoSetBoss(boss);
	IValidate::DoSetBoss(boss);
	IMemento::DoSetBoss(boss);
	IScrapHelper::DoSetBoss(boss);
	IFocused::DoSetBoss(boss);
	
	mControlH = nil;
	mTextH = nil;
	
	mSelAnchor    = kIndeterminateAnchor;
	mUpDownOffset = 0;
	mWasUpDown    = false;

	mRejectCallback = RejectCallback(this, &ITextBox::DefaultReject);
	mPassUpCallback = PassUpCallback(this, &ITextBox::DefaultPassUp);
}


//---------------------------------------------------------------
//
// MTETextBox::Init
//
//---------------------------------------------------------------
void MTETextBox::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& text,
					   uint32 style, uint32 procID)
{
	UNUSED(style);
		
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	ITextBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Save a copy of the control's window handle
	mControlH = control->GetOSControl();
		
	// Get the TEHandle and enable auto-scrolling
	int32 actualLength = 0;
	OSErr err = GetControlData(mControlH, kControlEntireControl, kControlEditTextTEHandleTag, sizeof(mTextH), (Ptr) &mTextH, &actualLength);
	ThrowIfOSErr(err);
	
	::TEAutoView(true, mTextH);					

	// Set the text		
	this->SetText(text, kDontRedraw);
}


//---------------------------------------------------------------
//
// MTETextBox::SetSelection
//
//---------------------------------------------------------------
void MTETextBox::SetSelection(uint32 start, uint32 stop, bool redraw)
{
	PRECONDITION(start <= stop);

	if (start != (**mTextH).selStart || stop != (**mTextH).selEnd) {
		ControlEditTextSelectionRec selection;
		selection.selStart = numeric_cast<int16>(start);
		selection.selEnd   = numeric_cast<int16>(stop);
		
		ITextBox* thisPtr = this;
		IControlPtr control(thisPtr);
		
		if (!redraw) {
			::SetPort(control->GetOSDrawContext());
			::HidePen();
		}
		
		OSErr err = SetControlData(mControlH, kControlEntireControl, kControlEditTextSelectionTag, sizeof(selection), (Ptr) &selection);
		if (err == noErr && ((**mTextH).selStart != start || (**mTextH).selEnd != stop))	// SetControlData will do nothing if the control does not have the focus which causes problems with undo (I tried dropping SetControlData altogether but I could never get drawing to work correctly when using the arrow keys)
			::TESetSelect((int32) start, (int32) stop, mTextH);
		
		if (!redraw) 
			::ShowPen();
		ThrowIfOSErr(err);
			
		mSelAnchor = kIndeterminateAnchor;
		mWasUpDown = false;
	}
}

						
//---------------------------------------------------------------
//
// MTETextBox::Select
//
//---------------------------------------------------------------
void MTETextBox::Select(bool redraw)
{
	this->SelectAll(redraw);
	
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);
	focus->SetFocus();
}

						
//---------------------------------------------------------------
//
// MTETextBox::DefaultPassUp
//
//---------------------------------------------------------------
bool MTETextBox::DefaultPassUp(ITextBox* textbox, int16 key, wchar_t ch)
{
	UNUSED(textbox);
	UNUSED(ch);
	
	bool passup = key == kReturnKey || key == kEnterKey || key == kTabKey || key == kEscapeKey;
		
	return passup;
}

						
//---------------------------------------------------------------
//
// MTETextBox::DefaultReject
//
//---------------------------------------------------------------
void MTETextBox::DefaultReject(ITextBox* textbox, const std::wstring& lostChars)
{
	UNUSED(textbox);
	UNUSED(lostChars);
	
	Whisper::Beep();
}

	
//---------------------------------------------------------------
//
// MTETextBox::Validate
//
//---------------------------------------------------------------
bool MTETextBox::Validate() const
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
// MTETextBox::IsEmpty
//
//---------------------------------------------------------------
bool MTETextBox::IsEmpty() const
{
	int32 length = 0;
			
	OSErr err = GetControlDataSize(mControlH, kControlEntireControl, kControlEditTextTextTag, &length);
	ThrowIfOSErr(err);
		
	return length == 0;
}


//---------------------------------------------------------------
//
// MTETextBox::GetText
//
//---------------------------------------------------------------
std::wstring MTETextBox::GetText() const
{
	int32 length = 0;
			
	OSErr err = GetControlDataSize(mControlH, kControlEntireControl, kControlEditTextTextTag, &length);
	ThrowIfOSErr(err);
	
	XTinyVector<char> buffer(length+1UL);
	
	int32 actualLength = 0;
	err = GetControlData(mControlH, kControlEntireControl, kControlEditTextTextTag, length, buffer.buffer(), &actualLength);
	ThrowIfOSErr(err);
	
	ASSERT(actualLength <= length);
	
	return FromPlatformStr(buffer.buffer(), (uint32) actualLength);
}


//---------------------------------------------------------------
//
// MTETextBox::SetText
//
//---------------------------------------------------------------
void MTETextBox::SetText(const std::wstring& text, bool redraw)
{
	if (text != this->GetText()) {
		ITextBox* thisPtr = this;
		IControlPtr control(thisPtr);
		
		::SetPort(control->GetOSDrawContext());
		::HidePen();
		OSErr err = SetControlData(mControlH, kControlEntireControl, kControlEditTextTextTag, (int32) text.length(), (Ptr) (const char*) ToPlatformStr(text).c_str());
		::ShowPen();
		ThrowIfOSErr(err);

		mSelAnchor = (**mTextH).selStart;
		mWasUpDown = false;
		
		if (redraw) 
			control->Invalidate();
	}
}


//---------------------------------------------------------------
//
// MTETextBox::GetState
//
//---------------------------------------------------------------
SMementoPtr MTETextBox::GetState() const
{
	STEMemento* memento = new STEMemento;
	
	memento->textH    = mTextH;
	memento->text     = this->GetText();
	memento->selStart = (**mTextH).selStart;
	memento->selEnd   = (**mTextH).selEnd;
	
	return SMementoPtr(memento);
}


//---------------------------------------------------------------
//
// MTETextBox::SetState
//
//---------------------------------------------------------------
void MTETextBox::SetState(const SMementoPtr& mementoPtr)
{
	ITextBox* thisPtr = this;
	IFocusPtr focus(thisPtr);
	focus->SetFocus();

	const STEMemento* memento = dynamic_cast<const STEMemento*>(mementoPtr.Get());
	ASSERT(memento != nil);
	ASSERT(memento->textH == mTextH);

	this->SetText(memento->text);
	this->SetSelection(memento->selStart, memento->selEnd);
	
	mSelAnchor = kIndeterminateAnchor;
			
	IValidateTextPtr validator(thisPtr);
	if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
		mChangedCallback(this);
}


//---------------------------------------------------------------
//
// MTETextBox::Copy
//
//---------------------------------------------------------------
XHandle MTETextBox::Copy(uint32 type)
{
	XHandle data;
	
	PRECONDITION((**mTextH).selStart < (**mTextH).selEnd);
	uint32 start = (**mTextH).selStart;
	uint32 stop  = (**mTextH).selEnd;
	
	ITextBox* thisPtr = this;
	ITextPtr text(thisPtr);
	std::wstring theText = text->GetText();
	std::wstring selection = theText.substr(start, stop - start);
	
	if (type == 'TEXT') {
		PlatformString str = ToPlatformStr(selection);
		
		uint32 bytes = str.length();		
		data.SetSize(bytes);
		BlockMoveData(str.c_str(), data.GetUnsafePtr(), bytes);
		
	} else if (type == 'utxt') {
		uint32 bytes = selection.length()*sizeof(uint16);		// don't use wchar_t (may be 32-bits)
		data.SetSize(bytes);
		BlockMoveData(selection.c_str(), data.GetUnsafePtr(), bytes);
	
	} else
		DEBUGSTR("Bogus type in MTETextBox::OnCopy");
		
	return data;
}


//---------------------------------------------------------------
//
// MTETextBox::Paste
//
//---------------------------------------------------------------
void MTETextBox::Paste(uint32 type, const XHandle& data)
{
	COMPILE_CHECK(sizeof(wchar_t) == 2);
	
	std::wstring accepted, rejected;
	
	{
	// Get the text we're pasting
	XLocker lock(data);	
		if (type == 'TEXT') {
			const char* str = reinterpret_cast<const char*>(data.GetPtr());
			rejected = FromPlatformStr(str, data.GetSize());
			
		} else if (type == 'utxt') {
			const wchar_t* str = reinterpret_cast<const wchar_t*>(data.GetPtr());
			rejected.assign(str, data.GetSize()/sizeof(wchar_t));
		
		} else
			DEBUGSTR("Bogus type in MTETextBox::OnPaste");
	}
	
	// Find out how much we can really paste
	bool valid = true;
	
	ITextBox* thisPtr = this;
	IValidateTextPtr validator(thisPtr);
	if (validator) {
		std::wstring text = this->GetText();
		
		if (this->HasSelection())
			text.erase((**mTextH).selStart, numeric_cast<uint32>((**mTextH).selEnd - (**mTextH).selStart));
		
		uint32 start = (**mTextH).selStart;
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
		ITextBox* thisPtr = this;
		IControlPtr control(thisPtr);
		
		PlatformString str = ToPlatformStr(accepted);
		
		::SetPort(control->GetOSDrawContext());
		::HidePen();
		::TEDelete(mTextH);
		::TEInsert(str.c_str(), (int32) (str.length()), mTextH);
		::ShowPen();
		
		control->Invalidate();

		mSelAnchor = kIndeterminateAnchor;
		mWasUpDown = false;

		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::Clear
//
//---------------------------------------------------------------
void MTETextBox::Clear()
{
	PRECONDITION(this->HasSelection());
	
	ITextBox* thisPtr = this;
	IControlPtr control(thisPtr);
		
	::SetPort(control->GetOSDrawContext());
	::HidePen();
	::TEDelete(mTextH);
	::ShowPen();
	
	control->Invalidate();
	
	mSelAnchor = kIndeterminateAnchor;
	mWasUpDown = false;

	IValidateTextPtr validator(thisPtr);
	if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
		mChangedCallback(this);
}


//---------------------------------------------------------------
//
// MTETextBox::HasSelection
//
//---------------------------------------------------------------
bool MTETextBox::HasSelection() const
{
	bool has = (**mTextH).selEnd > (**mTextH).selStart;
	
	return has;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MTETextBox::OnAdjustCursor
//
//---------------------------------------------------------------
bool MTETextBox::OnAdjustCursor(const XMouseMoveEvent& event)
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
// MTETextBox::OnAcquiredFocus
//
//---------------------------------------------------------------
void MTETextBox::OnAcquiredFocus()
{
	IKeyHandlerPtr handler(L"Application");
	ITextBox* thisPtr = this;
		
	XCallback1<bool, XKeyDownEvent> handleKey(this, &MTETextBox::DoKey);
	XCallback1<bool, XCharEvent> handleChar(this, &MTETextBox::DoChar);
	handler->RegisterHandler(handleKey, handleChar, thisPtr);	// do this first so that our specific key handlers get invoked first
	
	XCallback0<void> handle(this, &MTETextBox::DoUndoKey);
	handler->RegisterHandler(kF1Key, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoCutKey);
	handler->RegisterHandler(kF2Key, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoCopyKey);
	handler->RegisterHandler(kF3Key, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoPasteKey);
	handler->RegisterHandler(kF4Key, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoClearKey);
	handler->RegisterHandler(kClearKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoHomeKey);
	handler->RegisterHandler(kHomeKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoEndKey);
	handler->RegisterHandler(kEndKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoBackSpaceKey);
	handler->RegisterHandler(kBackspaceKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoForwardDeleteKey);
	handler->RegisterHandler(kFwdDeleteKey, handle, thisPtr);

	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->RegisterHandler(kLeftArrowKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->RegisterHandler(kRightArrowKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->RegisterHandler(kUpArrowKey, handle, thisPtr);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->RegisterHandler(kDownArrowKey, handle, thisPtr);
}


//---------------------------------------------------------------
//
// MTETextBox::OnLostFocus
//
//---------------------------------------------------------------
void MTETextBox::OnLostFocus()
{
	IKeyHandlerPtr handler(L"Application");
	
	XCallback1<bool, XKeyDownEvent> handleKey(this, &MTETextBox::DoKey);
	handler->UnRegisterHandler(handleKey);	
	
	XCallback0<void> handle(this, &MTETextBox::DoUndoKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoCutKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoCopyKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoPasteKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoClearKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoHomeKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoEndKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoBackSpaceKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoForwardDeleteKey);
	handler->UnRegisterHandler(handle);

	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &MTETextBox::DoArrowKey);
	handler->UnRegisterHandler(handle);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MTETextBox::DoKey
//
//---------------------------------------------------------------
bool MTETextBox::DoKey(const XKeyDownEvent& event)
{
	wchar_t ch = event.ComputeChar();
	int16 key = event.GetKey();
	
	bool handled = !mPassUpCallback(this, key, ch);
	
	return handled;
}


//---------------------------------------------------------------
//
// MTETextBox::DoChar
//
//---------------------------------------------------------------
bool MTETextBox::DoChar(const XCharEvent& event)
{
	ITextBox* thisPtr = this;
	IControlPtr control(thisPtr);

	PRECONDITION(control->IsVisible());
	PRECONDITION(control->IsEnabled());
		
	bool handled = false;		
	wchar_t ch = event.GetChar();
	int16 key = event.ComputeKey();
	
	if (!mPassUpCallback(this, key, ch)) {	
		IValidateTextPtr validator(thisPtr);

		bool valid = !validator || validator->IsValid(ch);
		if (valid && validator) {
			if (!this->HasSelection()) 			// if there's a selection the text length won't increase
				valid = (**mTextH).teLength < validator->GetMaxChars();
		}
		
		if (valid) {
			// Post a command for undo
			IInstancePtr instance(L"Typing Action");
			instance->Set(thisPtr);
			
			IUndoActionPtr action(instance);
			action->Init(LoadWhisperString(L"Typing"));

			ICommandQueuePtr queue(L"Application");
			queue->Post(ICommandPtr(action));				// need the ICommandPtr for Pro 5.3

			// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
			control->DoChar(event);

			mSelAnchor = kIndeterminateAnchor;	
			mWasUpDown = false;

			if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
				mChangedCallback(this);
				
		} else
			mRejectCallback(this, std::wstring(1, ch));
	}
					
	return kHandled;
}


//---------------------------------------------------------------
//
// MTETextBox::DoUndoKey
//
//---------------------------------------------------------------
void MTETextBox::DoUndoKey()
{
	IActiveUndoContextPtr activeContext(L"Application");
	IUndoContextPtr context;
	if (activeContext)
		context = activeContext->Get();

	if (context && context->CanUndo()) {
		const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();

		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kUndoCmd, event);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoCutKey
//
//---------------------------------------------------------------
void MTETextBox::DoCutKey()
{
	if (this->HasSelection()) {
		const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();

		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kCutCmd, event);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoCopyKey
//
//---------------------------------------------------------------
void MTETextBox::DoCopyKey()
{
	if (this->HasSelection()) {
		const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();

		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kCopyCmd, event);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoPasteKey
//
//---------------------------------------------------------------
void MTETextBox::DoPasteKey()
{
	ITextBox* thisPtr = this;
	IScrapPtr scrap(thisPtr);
	if (scrap->CanPaste()) {
		const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();

		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kPasteCmd, event);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoClearKey
//
//---------------------------------------------------------------
void MTETextBox::DoClearKey()
{
	if (this->HasSelection()) {
		const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();

		IMenuHandlerPtr handler(L"Application");
		handler->HandleCommand(kClearCmd, event);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoHomeKey
//
//---------------------------------------------------------------
void MTETextBox::DoHomeKey()
{
	this->SetSelection(0, 0);					// $$�should scroll instead of changing selection
}


//---------------------------------------------------------------
//
// MTETextBox::DoEndKey
//
//---------------------------------------------------------------
void MTETextBox::DoEndKey()
{
	uint32 length = (**mTextH).teLength;
	this->SetSelection(length, length);			// $$�should scroll instead of changing selection
}


//---------------------------------------------------------------
//
// MTETextBox::DoBackSpaceKey
//
//---------------------------------------------------------------
void MTETextBox::DoBackSpaceKey()
{
	if ((**mTextH).selStart > 0 || (**mTextH).selStart != (**mTextH).selEnd) {
		ITextBox* thisPtr = this;

		// Post a command for undo
		IInstancePtr instance(L"Delete Action");
		instance->Set(thisPtr);
		
		IUndoActionPtr action(instance);
		action->Init(LoadWhisperString(L"Backspace"));
		
		ICommandQueuePtr queue(L"Application");
		queue->Post(ICommandPtr(action));				// need the ICommandPtr for Pro 5.3

		// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
		(void) ::HandleControlKey(mControlH, 0x33, kBackspaceCharCode, 0);

		mSelAnchor = kIndeterminateAnchor;
		mWasUpDown = false;

		IValidateTextPtr validator(thisPtr);
		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoForwardDeleteKey
//
//---------------------------------------------------------------
void MTETextBox::DoForwardDeleteKey()
{
	if ((**mTextH).selStart < (**mTextH).teLength || (**mTextH).selStart != (**mTextH).selEnd) {
		ITextBox* thisPtr = this;

		// Post a command for undo
		IInstancePtr instance(L"Forward Delete Action");
		instance->Set(thisPtr);
		
		IUndoActionPtr action(instance);
		action->Init(LoadWhisperString(L"Forward Delete"));

		ICommandQueuePtr queue(L"Application");
		queue->Post(ICommandPtr(action));				// need the ICommandPtr for Pro 5.3

		// Handle Do here (the command is an XMergeAction which works a bit differently than most commands)
		(void) ::HandleControlKey(mControlH, 0x75, 0x7F, 0);

		mSelAnchor = kIndeterminateAnchor;
		mWasUpDown = false;

		IValidateTextPtr validator(thisPtr);
		if (mChangedCallback.IsValid() && (!validator || validator->IsValid(this->GetText())))
			mChangedCallback(this);
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoArrowKey
//
// TextEdit doesn't correctly handle arrow keys so we have to do
// a ton of work ourselves...
//
//---------------------------------------------------------------
void MTETextBox::DoArrowKey()
{
	IKeyHandlerPtr handler(L"Application");
	const XKeyDownEvent& event = handler->GetEvent();
	
	if ((**mTextH).teLength > 0) {
		int16 key = event.GetKey();
		PRECONDITION(key == kUpArrowKey || key == kLeftArrowKey || key == kRightArrowKey || key == kDownArrowKey);
			
		bool gotoEnd      = (event.GetOSEvent().modifiers & cmdKey) != 0;
		bool gotoNextWord = (event.GetOSEvent().modifiers & optionKey) != 0;

		bool selecting = (event.GetOSEvent().modifiers & shiftKey) != 0;
		bool towardStart = key == kLeftArrowKey || key == kUpArrowKey;
		
		uint32 left   = (**mTextH).selStart;
		uint32 right  = (**mTextH).selEnd;
		uint32 anchor = mSelAnchor;
		
		if (anchor == kIndeterminateAnchor) {
			if (towardStart)
				anchor = right;
			else
				anchor = left;
		}
		
		if (key != kUpArrowKey && key != kDownArrowKey)
			mWasUpDown = false;
		else if (key == kUpArrowKey && this->DoFindLineNumber(left, kPreferPreviousLine) == 0)
			mWasUpDown = false;
		else if (key == kDownArrowKey && this->DoFindLineNumber(right, kPreferNextLine) == (*mTextH)->nLines)
			mWasUpDown = false;
		else if (gotoEnd)
			mWasUpDown = false;

		if (key == kUpArrowKey && !mWasUpDown) {
			mWasUpDown = true;
			mUpDownOffset = numeric_cast<int16>(this->DoOffsetToPoint(left).x);
		} else if (key == kDownArrowKey && !mWasUpDown) {
			mWasUpDown = true;
			mUpDownOffset = numeric_cast<int16>(this->DoOffsetToPoint(right).x);
		}
		
		if (!selecting) {
			switch (key) {
				case kUpArrowKey:
					if (gotoEnd)
						left = right = anchor = 0;
					else
						left = right = anchor = this->DoFindPreviousLine(left);
					break;
					
				case kLeftArrowKey:
					if (gotoEnd)
						left = right = anchor = this->DoFindLineStart(left);
					else if (gotoNextWord && left > 0)
						left = right = anchor = this->DoFindLeftWordEdge(left-1);
					else if (left > 0)
						left = right = anchor = left - 1;
					else
						left = right = anchor = 0;
					break;
					
				case kRightArrowKey:
					if (gotoEnd)
						right = left = anchor = this->DoFindLineEnd(right);
					else if (gotoNextWord && right < (**mTextH).teLength)
						right = left = anchor = this->DoFindRightWordEdge(right);
					else
						right = left = anchor = Min(right + 1, (uint32) (**mTextH).teLength);
					break;
				
				case kDownArrowKey:
					if (gotoEnd)
						right = left = anchor = (**mTextH).teLength;
					else
						right = left = anchor = this->DoFindNextLine(right);
					break;
			}
		
		} else {
			switch (key) {
				case kUpArrowKey:
					if (gotoEnd)
						left = 0;
					else
						left = this->DoFindPreviousLine(left);
					right = anchor;
					break;
					
				case kLeftArrowKey:
					if (gotoEnd) {
						left  = this->DoFindLineStart(left);
						right = anchor;
						
					} else if (left < anchor || left == right) {
						if (gotoNextWord && left > 0)
							left = this->DoFindLeftWordEdge(left-1);
						else if (left > 0)
							left = left - 1;
						else
							left = 0;
							
					} else if (gotoNextWord && right > 0) {
						right = this->DoFindLeftWordEdge(right-1);
						if (right < left) {
							left = right;
							right = anchor;
						}
					
					} else if (right > 0)
						right = right - 1;

					else
						right = 0;
					break;
					
				case kRightArrowKey:
					if (gotoEnd) {
						right = this->DoFindLineEnd(right);
						left  = anchor;
						
					} else if (right > anchor || left == right) {
						if (gotoNextWord && right < (**mTextH).teLength)
							right = this->DoFindRightWordEdge(right);
						else
							right = Min(right + 1, (uint32) (**mTextH).teLength);
							
					} else if (gotoNextWord && left < (**mTextH).teLength) {
						left = this->DoFindRightWordEdge(left);
						if (left > right) {
							right = left;
							left = anchor;
						}

					} else
						left = Min(left + 1, (uint32) (**mTextH).teLength);
					break;
				
				case kDownArrowKey:
					if (gotoEnd)
						right = (**mTextH).teLength;
					else
						right = this->DoFindNextLine(right);
					left = anchor;
					break;
			}
		}

		if (left != (**mTextH).selStart || right != (**mTextH).selEnd) {
		
			// The offset to the start of a line can also refer to the
			// end of the previous line. In order to tell TextEdit which
			// we want we need to stuff some magic numbers into the text
			// edit record's clikStuff field (see tech note TE 18 for more
			// details).
#if 0											// $$$ when this is used remnants of the selection are left behind after typing command-right arrow
			if (gotoEnd && left == right) {
				::TEDeactivate(mTextH);
				(**mTextH).clikStuff = (int16) (key == kLeftArrowKey ? 255 : 0);
			}
#endif
			
			this->SetSelection(left, right);
			mSelAnchor = Min(anchor, (uint32) (**mTextH).teLength);
			
			if (gotoEnd && left == right) 
				::TEActivate(mTextH);

			::TESelView(mTextH);	
		}
	}
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindLineNumber
//
//---------------------------------------------------------------
int16 MTETextBox::DoFindLineNumber(uint32 offset, bool lefty) const
{
	PRECONDITION(offset <= (**mTextH).teLength);
	
	TERec& teRec = **mTextH;
	
	int16 line = -1;
	for (int16 index = 1; index <= teRec.nLines && line < 0; index++) {
	
		// If we've found a line start greater than offset then 
		// offset is on the previous line.
		if (teRec.lineStarts[index] > offset)
			line = numeric_cast<int16>(index - 1);
			
		// If we've found a line start equal to offset then offset
		// can be at the start of this line or at the end of the
		// previous line. We'll use the lefty argument as a tie
		// breaker.
		else if (teRec.lineStarts[index] == offset)	
			if (lefty)
				line = numeric_cast<int16>(index - 1);
			else
				line = index;
	}
	POSTCONDITION(line != -1);
	
	return line;
}


//---------------------------------------------------------------
//
// MTETextBox::DoGetWordBreaks
//
//---------------------------------------------------------------
void MTETextBox::DoGetWordBreaks(uint32 index, uint32* startP, uint32* stopP) const
{
	PRECONDITION(index < (**mTextH).teLength);
	
	uint32 start = index;
	uint32 stop  = index;
	
	Handle hand = (**mTextH).hText;
	XLocker lock(hand);
	
	const char* text = reinterpret_cast<const char*>(*hand);
	if (!std::isspace(text[index])) {						
		OffsetTable offsets;								
		::FindWordBreaks(const_cast<char*>(text), (**mTextH).teLength, numeric_cast<int16>(index), true, nil, offsets, smSystemScript);
		start = offsets[0].offFirst;
		stop  = offsets[0].offSecond;
	}

	if (startP != nil)
		*startP = start;

	if (stopP != nil)
		*stopP = stop;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindLeftWordEdge
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindLeftWordEdge(uint32 index) const
{
	PRECONDITION(index != ULONG_MAX);
	
	uint32 start, stop;
	do {
		this->DoGetWordBreaks(index, &start, &stop);
	} while (--index != ULONG_MAX && start == stop);
	
	return start;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindRightWordEdge
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindRightWordEdge(uint32 index) const
{
	uint32 start, stop;
	do {
		this->DoGetWordBreaks(index, &start, &stop);
	} while (++index < (**mTextH).teLength && start == stop);
	
	return stop;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindLineStart
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindLineStart(uint32 offset) const
{
	PRECONDITION(offset <= (**mTextH).teLength);
	
	uint32 start = 0;
	
	if ((**mTextH).nLines > 1) {
		int16 line = this->DoFindLineNumber(offset, kPreferPreviousLine);
		start = (**mTextH).lineStarts[line];
	}

	return start;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindLineEnd
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindLineEnd(uint32 offset) const
{
	PRECONDITION(offset <= (**mTextH).teLength);
	
	TERec& teRec = **mTextH;
	uint32 end = teRec.teLength;

	if (teRec.nLines > 1) {
		int16 line = this->DoFindLineNumber(offset, kPreferNextLine);

		if (line < teRec.nLines - 1)
			end = (**mTextH).lineStarts[line+1];
	}

	return end;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindPreviousLine
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindPreviousLine(uint32 offset) const
{
	PRECONDITION(offset <= (**mTextH).teLength);
	
	uint32 prev;
	
	int16 line = this->DoFindLineNumber(offset, kPreferPreviousLine);
	if (line == 0)
		prev = 0;
		
	else {
		int16 lineHeight = (**mTextH).lineHeight;
		
		XPoint pt = this->DoOffsetToPoint(offset);
		pt.x = mUpDownOffset;
		pt.y -= lineHeight;
		
		prev = this->DoPointToOffset(pt);
		
		if (this->DoFindLineNumber(prev, kPreferPreviousLine) == line)
			--prev;
	}
	
	return prev;
}


//---------------------------------------------------------------
//
// MTETextBox::DoFindNextLine
//
//---------------------------------------------------------------
uint32 MTETextBox::DoFindNextLine(uint32 offset) const
{
	PRECONDITION(offset <= (**mTextH).teLength);
	
	uint32 next;
	
	int16 line = this->DoFindLineNumber(offset, kPreferNextLine);
	if (line == (**mTextH).nLines - 1)
		next = (**mTextH).teLength;
		
	else {
		int16 lineHeight = (**mTextH).lineHeight;
		
		XPoint pt = this->DoOffsetToPoint(offset);
		pt.x = mUpDownOffset;
		pt.y += lineHeight;
		
		next = this->DoPointToOffset(pt);
		
		if (this->DoFindLineNumber(next, kPreferNextLine) == line)
			next++;
	}
	
	return next;
}


//---------------------------------------------------------------
//
// MTETextBox::DoOffsetToPoint
//
//---------------------------------------------------------------
XPoint MTETextBox::DoOffsetToPoint(uint32 offset) const
{
	int16 height;
	TextStyle theStyle;
	int16 ascent;
	TEGetStyle(numeric_cast<int16>(offset), &theStyle, &height, &ascent, mTextH);

	XPoint pt = TEGetPoint(numeric_cast<int16>(offset), mTextH);
	if ((!TEIsFrontOfLine(numeric_cast<int16>(offset), mTextH)) || (offset > 0 && ((*((*mTextH)->hText))[offset - 1] != kReturnCharCode)))
		pt.y += numeric_cast<int16>(ascent - height);

	return pt;
}


//---------------------------------------------------------------
//
// MTETextBox::DoPointToOffset
//
//---------------------------------------------------------------
uint32 MTETextBox::DoPointToOffset(const XPoint& pt) const
{
	int16 offset = TEGetOffset(pt.GetOSPoint(), mTextH);

	if (TEIsFrontOfLine(offset, mTextH) && offset > 0 && (*((*mTextH)->hText))[offset - 1] == kReturnCharCode && TEGetPoint(numeric_cast<int16>(offset - 1), mTextH).h < pt.x)
		offset--;

	return offset;
}


}	// namespace Whisper