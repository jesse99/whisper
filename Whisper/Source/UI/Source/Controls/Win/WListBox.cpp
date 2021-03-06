/*
 *  File:       WListBox.cpp
 *  Summary:   	A system control that displays a list of strings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WListBox.cpp,v $
 *		Revision 1.3  2001/04/27 09:19:03  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.2  2001/04/21 03:32:28  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/04/01 04:49:40  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IListBox.h>

#include <windowsx.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IFocused.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XKeyEvents.h>
#include <XStringUtils.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class XListBox
//!		A system control that displays a list of strings.
// ===================================================================================
class XListBox : public IListBox, public IFocused, public IControlPressedHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XListBox();
	
						XListBox(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID);

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring* begin, const std::wstring* end,
							 const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID);

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const wchar_t** begin, const wchar_t** end,
							 const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID);

	virtual uint32 		GetCount() const											{return mItems.size();}
	virtual std::wstring GetItem(uint32 index) const								{PRECONDITION(index < mItems.size()); return mItems[index];}
	virtual void 		SetItem(uint32 index, const std::wstring& text, bool redraw = kRedraw);

	virtual void 		AppendItem(const std::wstring& text, bool redraw = kRedraw)	{this->InsertItem(text, this->GetCount(), redraw);}
	virtual void 		InsertItem(const std::wstring& text, uint32 at, bool redraw = kRedraw);
	
	virtual void 		RemoveItem(uint32 index, bool redraw = kRedraw);
	virtual void 		RemoveEveryItem(bool redraw = kRedraw);

	virtual bool 		HasSelection() const;
	virtual bool 		HasOneSelection() const;
	virtual uint32		GetSelection() const;
	virtual bool 		GetNextSelection(uint32* index) const;

	virtual bool 		IsSelected(uint32 index) const;
	virtual void		SelectAll(bool select = true, bool redraw = kRedraw);
	virtual void		Select(uint32 index, bool select = true, bool redraw = kRedraw);
	virtual void		SelectOneItem(uint32 index, bool redraw = kRedraw);
	virtual void 		SetSelectionCallback(const Callback& callback)		{mSelectionCallback = callback;}

	virtual void		ScrollIntoView(uint32 index);					
	virtual bool		FindItem(const std::wstring& item, uint32* index) const;

protected:
	virtual void 		OnPressed(const XMouseDownEvent& event);
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

//-----------------------------------
//	Internal API
//
private:
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);
			void 		DoKeySelection(const std::wstring& selectionString);
			
//-----------------------------------
//	Member Data
//
private:
	std::vector<std::wstring>	mItems;				// need this to avoid loss of information when converting to ASCII (on Win95 and Mac)
	Callback					mSelectionCallback;
	HWND						mControl;
	
	std::wstring				mKeySequence;
	MilliSecond					mLastKeyUpTime;
	SListBoxInfo				mInfo;
};

DEFINE_INTERFACE_FACTORY(XListBox)

//---------------------------------------------------------------
//
// XListBox::~XListBox
//
//---------------------------------------------------------------
XListBox::~XListBox()
{
}


//---------------------------------------------------------------
//
// XListBox::XListBox
//
//---------------------------------------------------------------
XListBox::XListBox(XBoss* boss) 
{	
	IListBox::DoSetBoss(boss);
	IFocused::DoSetBoss(boss);
	IControlPressedHelper::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XListBox::Init (IHierarchyPtr, wstring, XRect, SListBoxInfo, uint32, uint32)
//
//---------------------------------------------------------------
void XListBox::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const SListBoxInfo& info, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	mLastKeyUpTime = 0;
	mInfo = info;

	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	if (info.hasMultipleSelections)
		style |= LBS_EXTENDEDSEL;
		
	if (info.hasVertScrollBar)
		style |= WS_VSCROLL;
		
	if (info.hasHorzScrollBar)
		style |= WS_HSCROLL;
		
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("LISTBOX"),		// class name
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
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Save off the window handle
	mControl = control->GetOSControl();
}


//---------------------------------------------------------------
//
// XListBox::Init (IHierarchyPtr, wstring, XRect, wstring*, wstring*, SListBoxInfo, uint3, uint32)
//
//---------------------------------------------------------------
void XListBox::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const std::wstring* begin, const std::wstring* end, const SListBoxInfo& info, uint32 style, uint32 procID)
{
	this->Init(parent, name, frame, info, style, procID);

	const std::wstring* ptr = begin;
	while (ptr != end) {
		this->AppendItem(*ptr, kDontRedraw);
		++ptr;
	}
}


//---------------------------------------------------------------
//
// XListBox::Init (IHierarchyPtr, wstring, XRect, const wchar_t**, wchar_t**, SListBoxInfo, uint32, uint32)
//
//---------------------------------------------------------------
void XListBox::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const wchar_t** begin, const wchar_t** end, const SListBoxInfo& info, uint32 style, uint32 procID)
{
	this->Init(parent, name, frame, info, style, procID);

	const wchar_t** ptr = begin;
	while (ptr != end) {
		this->AppendItem(*ptr, kDontRedraw);
		++ptr;
	}
}


//---------------------------------------------------------------
//
// XListBox::SetItem
//
//---------------------------------------------------------------
void XListBox::SetItem(uint32 index, const std::wstring& text, bool redraw)
{
	PRECONDITION(index < this->GetCount());
	
	if (text != mItems[index]) {
		this->RemoveItem(index, kDontRedraw);		// Windows doesn't provide a way to change an item...
		this->InsertItem(text, index, redraw);
	}
	
	POSTCONDITION(mItems[index] == text);
}


//---------------------------------------------------------------
//
// XListBox::InsertItem
//
//---------------------------------------------------------------
void XListBox::InsertItem(const std::wstring& text, uint32 at, bool redraw)
{
	PRECONDITION(at <= this->GetCount());
	
	mItems.insert(mItems.begin() + at, text);
		
	if (!redraw)
		VERIFY(::LockWindowUpdate(mControl));

	int32 result;
	if (WSystemInfo::HasUnicode())
		result = SendMessageW(mControl, LB_INSERTSTRING, at, (LPARAM) text.c_str());
	else
		result = SendMessageA(mControl, LB_INSERTSTRING, at, (LPARAM) ToPlatformStr(text).c_str());

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
	ThrowIf(result == LB_ERR || result == LB_ERRSPACE);
		
	if (redraw) {
		IListBox* thisPtr = this;				// invalidate the whole thing since scroll bars may need to be updated
		IControlPtr control(thisPtr);
		control->Invalidate();	
	}
	
	POSTCONDITION(at < this->GetCount());
	POSTCONDITION(mItems[at] == text);
}

	
//---------------------------------------------------------------
//
// XListBox::RemoveItem
//
//---------------------------------------------------------------
void XListBox::RemoveItem(uint32 index, bool redraw)
{
	PRECONDITION(index < this->GetCount());
	
	mItems.erase(mItems.begin() + index);
		
	if (!redraw)
		VERIFY(::LockWindowUpdate(mControl));

	int32 result = SendMessage(mControl, LB_DELETESTRING, index, nil);

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
	ThrowIf(result < 0);
		
	if (redraw) {
		IListBox* thisPtr = this;				// invalidate the whole thing since scroll bars may need to be updated
		IControlPtr control(thisPtr);
		control->Invalidate();	
	}
}


//---------------------------------------------------------------
//
// XListBox::RemoveEveryItem
//
//---------------------------------------------------------------
void XListBox::RemoveEveryItem(bool redraw)
{
	uint32 count = this->GetCount();
	while (count > 0) 
		this->RemoveItem(--count, kDontRedraw);

	if (redraw) {
		IListBox* thisPtr = this;
		IControlPtr control(thisPtr);
		control->Invalidate();	
	}
	
	POSTCONDITION(this->GetCount() == 0);
}


//---------------------------------------------------------------
//
// XListBox::HasSelection
//
//---------------------------------------------------------------
bool XListBox::HasSelection() const
{
	bool hasSelection = false;

	if (mInfo.hasMultipleSelections) {
		int32 count = SendMessage(mControl, LB_GETSELCOUNT, 0, nil);	// note that LB_GETCURSEL is reserved for single selection listboxes
		ThrowIf(count < 0);

		hasSelection = count > 0;
	
	} else {
		int32 index = SendMessage(mControl, LB_GETCURSEL, 0, nil);		// returns LB_ERR if there's no selection...
		hasSelection = index >= 0;
	}
		
	return hasSelection;
}


//---------------------------------------------------------------
//
// XListBox::HasOneSelection
//
//---------------------------------------------------------------
bool XListBox::HasOneSelection() const
{
	bool hasOne = false;

	if (mInfo.hasMultipleSelections) {
		int32 count = SendMessage(mControl, LB_GETSELCOUNT, 0, nil);	// note that LB_GETCURSEL is reserved for single selection listboxes
		ThrowIf(count < 0);

		hasOne = count == 1;
	
	} else {
		int32 index = SendMessage(mControl, LB_GETCURSEL, 0, nil);		// returns LB_ERR if there's no selection...
		hasOne = index >= 0;
	}
		
	return hasOne;
}


//---------------------------------------------------------------
//
// XListBox::GetSelection
//
//---------------------------------------------------------------
uint32 XListBox::GetSelection() const
{
	PRECONDITION(this->HasSelection());

	int32 index;

	if (mInfo.hasMultipleSelections) {		
		int32 result = SendMessage(mControl, LB_GETSELITEMS, 1, (int32) &index);
		ThrowIf(result == LB_ERR);
	
	} else 
		index = SendMessage(mControl, LB_GETCURSEL, 0, nil);		// returns LB_ERR if there's no selection...
		
	return numeric_cast<uint32>(index);
}


//---------------------------------------------------------------
//
// XListBox::GetNextSelection
//
//---------------------------------------------------------------
bool XListBox::GetNextSelection(uint32* index) const
{
	PRECONDITION(index != nil);
	PRECONDITION(*index < this->GetCount() || *index == ULONG_MAX);
	
	bool selected = false;
	
	if (mInfo.hasMultipleSelections) {		
		int32 count = SendMessage(mControl, LB_GETSELCOUNT, 0, nil);
		ThrowIf(count < 0);

		if (count > 0) {
			XTinyVector<int32> items(numeric_cast<uint32>(count));	
			
			int32 result = SendMessage(mControl, LB_GETSELITEMS, numeric_cast<uint32>(count), (int32) items.buffer());
			ThrowIf(result == LB_ERR);
			
			if (*index == ULONG_MAX) {
				*index = numeric_cast<uint32>(items[0]);
				selected = true;
			
			} else if (*index < count) {
				*index = numeric_cast<uint32>(items[*index + 1]);
				selected = true;
			}
		}
	
	} else {
		if (*index == ULONG_MAX && this->HasSelection()) {
			*index = this->GetSelection();
			selected = true;
		} 
	}

	return selected;
}


//---------------------------------------------------------------
//
// XListBox::IsSelected
//
//---------------------------------------------------------------
bool XListBox::IsSelected(uint32 index) const
{
	PRECONDITION(index < this->GetCount());

	int32 selected = SendMessage(mControl, LB_GETSEL, index, nil);
	ThrowIf(selected < 0);
	
	return selected != 0;
}


//---------------------------------------------------------------
//
// XListBox::SelectAll
//
//---------------------------------------------------------------
void XListBox::SelectAll(bool select, bool redraw)
{
	PRECONDITION(!select || mInfo.hasMultipleSelections);

	uint32 count = this->GetCount();

	if (count > 0) {
		if (!redraw)
			VERIFY(::LockWindowUpdate(mControl));

		int32 result;
		if (mInfo.hasMultipleSelections)
			result = SendMessage(mControl, LB_SETSEL, select, MAKELPARAM(-1, 0));
		else
			result = SendMessage(mControl, LB_SETCURSEL, (uint32) -1, nil);
				
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));

		ThrowIf(mInfo.hasMultipleSelections && result == LB_ERR);	// LB_SETCURSEL always returns LB_ERR if index is -1...	
	}

	POSTCONDITION((this->HasSelection() == select) || count == 0);
}


//---------------------------------------------------------------
//
// XListBox::Select
//
//---------------------------------------------------------------
void XListBox::Select(uint32 index, bool select, bool redraw)
{
	PRECONDITION(index < this->GetCount());
	
	if (select != this->IsSelected(index)) {
		if (!redraw)
			VERIFY(::LockWindowUpdate(mControl));
			
		int32 result;
		if (mInfo.hasMultipleSelections)
			result = SendMessage(mControl, LB_SETSEL, select, MAKELPARAM(index, 0));
		else
			result = SendMessage(mControl, LB_SETCURSEL, index, nil);
				
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));

		ThrowIf(mInfo.hasMultipleSelections && result == LB_ERR);	// LB_SETCURSEL always returns LB_ERR if index is -1...	
	}
}


//---------------------------------------------------------------
//
// XListBox::SelectOneItem
//
//---------------------------------------------------------------
void XListBox::SelectOneItem(uint32 index, bool redraw)
{
	PRECONDITION(index < this->GetCount());
	
	uint32 count = this->GetCount();
	for (uint32 i = 0; i < count; ++i)
		this->Select(i, i == index, redraw);	

	POSTCONDITION(this->HasSelection() || this->GetCount() == 0);
}


//---------------------------------------------------------------
//
// XListBox::ScrollIntoView
//
//---------------------------------------------------------------
void XListBox::ScrollIntoView(uint32 index)
{
	PRECONDITION(index < this->GetCount());
	
	IListBox* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	int32 height = geometry->GetHeight();
	
	int32 itemHeight = SendMessage(mControl, LB_GETITEMHEIGHT, 0, nil);
	ThrowIf(itemHeight <= 0);
	
	int32 numDisplayed = height/itemHeight;

	int32 firstItem = SendMessage(mControl, LB_GETTOPINDEX, 0, nil);
	ThrowIf(firstItem < 0);
	
	int32 lastItem = firstItem + numDisplayed - 1;
	
	if (index < firstItem || index > lastItem) {
		int32 result = SendMessage(mControl, LB_SETTOPINDEX, index, nil);
		ThrowIf(result == LB_ERR);
	}
}


//---------------------------------------------------------------
//
// XListBox::FindItem
//
//---------------------------------------------------------------
bool XListBox::FindItem(const std::wstring& item, uint32* index) const
{
	PRECONDITION(index != nil);
	
	bool found = false;

	uint32 count = this->GetCount();
	for (uint32 i = 0; i < count && !found; ++i) {
		const std::wstring& candidate = mItems[i];
		if (candidate == item) {
			*index = i;
			found = true;
		}
	}

	return found;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XListBox::OnPressed
//
//---------------------------------------------------------------
void XListBox::OnPressed(const XMouseDownEvent& event)
{
	UNUSED(event);
	
	if (mSelectionCallback.IsValid())
		mSelectionCallback(this);	
}


//---------------------------------------------------------------
//
// XListBox::OnAcquiredFocus
//
//---------------------------------------------------------------
void XListBox::OnAcquiredFocus()
{
	IKeyHandlerPtr handler(L"Application");
	IListBox* thisPtr = this;
		
	XCallback1<bool, XKeyDownEvent> handleKey(this, &XListBox::DoKey);
	XCallback1<bool, XCharEvent> handleChar(this, &XListBox::DoChar);
	handler->RegisterHandler(handleKey, handleChar, thisPtr);	
}


//---------------------------------------------------------------
//
// XListBox::OnLostFocus
//
//---------------------------------------------------------------
void XListBox::OnLostFocus()
{
	IKeyHandlerPtr handler(L"Application");
	
	XCallback1<bool, XKeyDownEvent> handleKey(this, &XListBox::DoKey);
	handler->UnRegisterHandler(handleKey);	
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XListBox::DoKey
//
//---------------------------------------------------------------
bool XListBox::DoKey(const XKeyDownEvent& event)
{
	int16 key = event.GetKey();
	bool handled = key != kReturnKey && key != kTabKey && key != kEscapeKey;
	
	return handled;
}


//---------------------------------------------------------------
//
// XListBox::DoChar
//
//---------------------------------------------------------------
bool XListBox::DoChar(const XCharEvent& event)
{
	bool handled = false;
	
	if (mInfo.useTypeAhead) {
		wchar_t ch = event.GetChar();

		bool printingChar = IsPrintable(ch);

		if (printingChar && this->GetCount() > 0) {
			if (event.GetTimeStamp() - mLastKeyUpTime >= XKeyDownEvent::GetRepeatDelay())
				mKeySequence = L"";

			mKeySequence += ConvertToLowerCase(std::wstring(1, event.GetChar()));
			this->DoKeySelection(mKeySequence);
			mLastKeyUpTime = event.GetTimeStamp();
				
			if (mSelectionCallback.IsValid())
				mSelectionCallback(this);		// may call the callback more than neccesary, but that shouldn't be a problem

			handled = true;
		}		
	}
		
	return handled;
}

					
//---------------------------------------------------------------
//
// XListBox::DoKeySelection
//
// The standard listbox window proc will do type ahead, but it's
// limited to a single key which much match the start of an existing
// entry (ie if there isn't an entry starting with 'M' typing 'M'
// will do nothing instead of selecting the first entry after 'M').
//
//---------------------------------------------------------------
void XListBox::DoKeySelection(const std::wstring& selectionString)
{
	PRECONDITION(mInfo.useTypeAhead);

	uint32 lastItemIndex = 0;

	for (uint32 index = 0; index < this->GetCount(); index++) {
		std::wstring itemText = ConvertToLowerCase(this->GetItem(index));
		itemText = itemText.substr(0, selectionString.length());
		lastItemIndex = index;
		if (itemText >= selectionString)
			break;
	}

	this->SelectOneItem(lastItemIndex);
	this->ScrollIntoView(lastItemIndex);
}


}	// namespace Whisper
