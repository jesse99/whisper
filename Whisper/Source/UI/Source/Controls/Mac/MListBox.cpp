/*
 *  File:       MListBox.cpp
 *  Summary:   	A system control that displays a list of strings.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MListBox.cpp,v $
 *		Revision 1.3  2001/04/27 04:21:03  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.2  2001/04/21 03:26:45  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/04/01 04:49:33  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IListBox.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IFocused.h>
#include <IGeometry.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IMouseExtension.h>
#include <ISizeChanged.h>
#include <XExceptions.h>
#include <XKeyEvents.h>
#include <XMouseEvents.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class XListBox
//!		A system control that displays a list of strings.
// ===================================================================================
class XListBox : public IListBox, public ISizeChanged, public IFocused, public IControlPressedHelper, public IMouseExtension {

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
	virtual ListHandle 	GetOSListBox()										{return mList;}

	virtual std::wstring GetName() const 									{return mExtName;}
	virtual void 		SetName(const std::wstring& name) 					{mExtName = name;}
									
	virtual bool 		HandleMouseDown(XMouseDownEvent& event);
	virtual bool 		HandleMouseMove(XMouseMoveEvent&) 					{return kNotHandled;}
	virtual bool 		HandleMouseUp(XMouseUpEvent&) 						{return kNotHandled;}

protected:
	virtual void 		OnSizeChanged(const XSize& oldSize, const XSize& newSize);
	virtual void 		OnPressed(const XMouseDownEvent& event);
	virtual void 		OnAcquiredFocus();
	virtual void 		OnLostFocus();

//-----------------------------------
//	Internal API
//
private:
			void 		DoHomeKey();
			void 		DoEndKey();
			void 		DoPageUpKey();
			void 		DoPageDownKey();
			void 		DoUpArrowKey();
			void 		DoDownArrowKey();
			bool 		DoKey(const XKeyDownEvent& event);
			bool 		DoChar(const XCharEvent& event);
			void 		DoKeySelection(const std::wstring& selectionString);
								
			void 		DoAdjustSize(bool redraw);
			void 		DoInvalidateItem(uint32 index);
			void 		DoFocus();
			
//-----------------------------------
//	Member Data
//
private:
	std::vector<std::wstring>	mItems;				// need this to avoid loss of information when converting to ASCII (on Win95 and Mac)
	Callback					mSelectionCallback;
	std::wstring				mExtName;

	std::wstring				mKeySequence;
	MilliSecond					mLastKeyUpTime;
	
	ListHandle					mList;
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
	ISizeChanged::DoSetBoss(boss);
	IControlPressedHelper::DoSetBoss(boss);
	IMouseExtension::DoSetBoss(boss);
	IFocused::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XListBox::Init (IHierarchyPtr, wstring, XRect, SListBoxInfo, uint32, uint32)
//
//---------------------------------------------------------------
void XListBox::Init(const IHierarchyPtr& parent, const std::wstring& name, const XRect& frame, const SListBoxInfo& info, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	mList = nil;
	mLastKeyUpTime = 0;
	mInfo = info;

	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	int16 rsrcID = 128;				// the only way to specify this information when using the Appearance Manager controls is through a resource so we hard-code eight 'ldes' resources...
	if (info.hasVertScrollBar)
		rsrcID += 1;
	if (info.hasHorzScrollBar)
		rsrcID += 2;
	if (info.hasGrowBox)
		rsrcID += 4;
		
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, rsrcID, 0, 0, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);

	// Save off the list handle and tweak a few settings
	int32 actualLength = 0;
	OSErr err = GetControlData(control->GetOSControl(), kControlEntireControl, kControlListBoxListHandleTag, sizeof(mList), (char*) &mList, &actualLength);
	ThrowIfOSErr(err);

	if (!info.hasMultipleSelections)
		(**mList).selFlags = lNoNilHilite + lNoExtend + lNoDisjoint + lOnlyOne;
	
	LSetDrawingMode(false, mList);
	LAddColumn(1, 0, mList);
	LSetDrawingMode(true, mList);

	this->DoAdjustSize(kDontRedraw);
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

	this->DoAdjustSize(kDontRedraw);
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

	this->DoAdjustSize(kDontRedraw);
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
		if (text.length() > mItems[index].length() && GetHandleSize((**mList).cells) + text.length() - mItems[index].length() > 32*1024)	// $$$ check for OS X?
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The listbox cannot hold this much data.")));
		
		else {
			mItems[index] = text;
			
			Point cell;
			cell.h = 0;
			cell.v = numeric_cast<int16>(index);
			
			IListBox* thisPtr = this;
			IControlPtr control(thisPtr);
			SetPort(control->GetOSDrawContext());

			LSetDrawingMode(false, mList);
			PlatformString str = ToPlatformStr(text);
			LSetCell(str.c_str(), numeric_cast<int16>(str.length()), cell, mList);
			LSetDrawingMode(true, mList);
		
			if (redraw)
				this->DoInvalidateItem(index);
		}
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
	
	if (GetHandleSize((**mList).cells) + text.length() > 32*1024)	// $$$ check for OS X?
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The listbox cannot hold this much data.")));
	
	else {
		mItems.insert(mItems.begin() + at, text);
		
		Point cell;
		cell.h = 0;
		cell.v = numeric_cast<int16>(at);
		
		IListBox* thisPtr = this;
		IControlPtr control(thisPtr);
		SetPort(control->GetOSDrawContext());

		LSetDrawingMode(false, mList);
		LAddRow(1, cell.v, mList);
		PlatformString str = ToPlatformStr(text);
		LSetCell(str.c_str(), numeric_cast<int16>(str.length()), cell, mList);
		LSetDrawingMode(true, mList);
		
		int16 offset, len;
		LGetCellDataLocation(&offset, &len, cell, mList);
		const char* entry = (const char*) (*(**mList).cells + offset);
			
		if (redraw)
			control->Invalidate();			// invalidate the whole thing since scroll bars may need to be updated
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
		
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	SetPort(control->GetOSDrawContext());

	LSetDrawingMode(false, mList);		
	LDelRow(1, numeric_cast<int16>(index), mList);		
	LSetDrawingMode(true, mList);
		
	if (redraw)
		control->Invalidate();				// invalidate the whole thing since scroll bars may need to be updated
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
	bool selected = false;
	
	if (this->GetCount() > 0) {
		Point cell;
		cell.h = 0;
		cell.v = 0;
		
		selected = LGetSelect(true, &cell, mList);
	}
		
	return selected;
}


//---------------------------------------------------------------
//
// XListBox::HasOneSelection
//
//---------------------------------------------------------------
bool XListBox::HasOneSelection() const
{
	bool selected = false;
	
	if (this->GetCount() > 0) {
		Point cell;
		cell.h = 0;
		cell.v = 0;
		
		if (LGetSelect(true, &cell, mList)) {
			++cell.v;
		
			if (cell.v < this->GetCount())
				selected = LGetSelect(true, &cell, mList);
		}
	}
		
	return selected;
}


//---------------------------------------------------------------
//
// XListBox::GetSelection
//
//---------------------------------------------------------------
uint32 XListBox::GetSelection() const
{
	PRECONDITION(this->HasSelection());

	Point cell;
	cell.h = 0;
	cell.v = 0;
	
	VERIFY(LGetSelect(true, &cell, mList));
	
	return cell.v;
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
	
	Point cell;
	cell.h = 0;
	cell.v = numeric_cast<int16>(*index == ULONG_MAX ? 0 : *index+1);
	
	if (cell.v < this->GetCount()) {
		selected = LGetSelect(true, &cell, mList);
		
		if (selected)
			*index = cell.v;
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

	Point cell;
	cell.h = 0;
	cell.v = numeric_cast<int16>(index);
	
	bool selected = LGetSelect(false, &cell, mList);
	
	return selected;
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
	for (uint32 index = 0; index < count; ++index)
		this->Select(index, select, redraw);	

	POSTCONDITION((this->HasSelection() == select) || this->GetCount() == 0);
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
		if (select && !mInfo.hasMultipleSelections)
			this->SelectAll(false, redraw);

		Point cell;
		cell.h = 0;
		cell.v = numeric_cast<int16>(index);
		
		this->DoFocus();					// LScroll probably assumes port is right...

		if (!redraw)
			LSetDrawingMode(false, mList);	
				
		LSetSelect(select, cell, mList);		

		if (!redraw)
			LSetDrawingMode(true, mList);
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
	
	Rect visibleCells = (**mList).visible;
	
	Point cell;
	cell.h = 0;
	cell.v = numeric_cast<int16>(index);
	
	if (!::PtInRect(cell, &visibleCells)) {
		int16 delta = 0;	
				
		if (cell.v > visibleCells.bottom - 1) 
			delta = numeric_cast<int16>(cell.v - visibleCells.bottom + 1);
		else if (cell.v < visibleCells.top) 
			delta = numeric_cast<int16>(cell.v - visibleCells.top);
		
		this->DoFocus();
		LScroll(0, delta, mList);
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


//---------------------------------------------------------------
//
// XListBox::HandleMouseDown
//
//---------------------------------------------------------------
bool XListBox::HandleMouseDown(XMouseDownEvent& event)
{	
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	Point pt = control->LocalToWindow(event.GetPosition()).GetOSPoint();

#if 1
	Rect extent = (**mList).rView;
	if (!PtInRect(pt, &extent) && event.GetClickCount() == 2)	// convert double clicks outside the cells to single clicks
		event.SetClickCount(1);

#else
	int16 part = ::TestControl(mControl, pt);									// none of the part codes will tell us when the user has clicked in a scroll bar...
	if (part != kControlListBoxDoubleClickPart && event.GetClickCount() == 2)	// convert double clicks in scroll bars to single clicks
		event.SetClickCount(1);
#endif

	return kNotHandled;			// allow the normal event processing
}

#if __MWERKS__
#pragma mark ~
#endif

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
	handler->RegisterHandler(handleKey, handleChar, thisPtr);	// do this first so that our specific key handlers get invoked first
	
	XCallback0<void> handle(this, &XListBox::DoHomeKey);
	handler->RegisterHandler(kHomeKey, handle, thisPtr);
	
	handle.Set(this, &XListBox::DoEndKey);
	handler->RegisterHandler(kEndKey, handle, thisPtr);
	
	handle.Set(this, &XListBox::DoPageUpKey);
	handler->RegisterHandler(kPageUpKey, handle, thisPtr);
	
	handle.Set(this, &XListBox::DoPageDownKey);
	handler->RegisterHandler(kPageDownKey, handle, thisPtr);
	
	handle.Set(this, &XListBox::DoUpArrowKey);
	handler->RegisterHandler(kUpArrowKey, handle, thisPtr);
	
	handle.Set(this, &XListBox::DoDownArrowKey);
	handler->RegisterHandler(kDownArrowKey, handle, thisPtr);
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
	
	XCallback0<void> handle(this, &XListBox::DoHomeKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &XListBox::DoEndKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &XListBox::DoPageUpKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &XListBox::DoPageDownKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &XListBox::DoUpArrowKey);
	handler->UnRegisterHandler(handle);
	
	handle.Set(this, &XListBox::DoDownArrowKey);
	handler->UnRegisterHandler(handle);
}


//---------------------------------------------------------------
//
// XListBox::OnPressed
//
//---------------------------------------------------------------
void XListBox::OnPressed(const XMouseDownEvent& event)
{
	UNUSED(event);
	
	// It kind of sucks to call this each time, but we really
	// don't want to cache all the selected items and the Control
	// Manager is much too lame to inform us when this happens.
	if (mSelectionCallback.IsValid())
		mSelectionCallback(this);	
}


//---------------------------------------------------------------
//
// XListBox::OnSizeChanged
//
//---------------------------------------------------------------
void XListBox::OnSizeChanged(const XSize& oldSize, const XSize& newSize)
{
	UNUSED(oldSize);
	UNUSED(newSize);
	
	this->DoAdjustSize(kRedraw);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XListBox::DoHomeKey
//
//---------------------------------------------------------------
void XListBox::DoHomeKey()
{
	this->DoFocus();
	LScroll(0, -16000, mList);
}


//---------------------------------------------------------------
//
// XListBox::DoEndKey
//
//---------------------------------------------------------------
void XListBox::DoEndKey()
{
	this->DoFocus();
	LScroll(0, 16000, mList);
}


//---------------------------------------------------------------
//
// XListBox::DoPageUpKey
//
//---------------------------------------------------------------
void XListBox::DoPageUpKey()
{
	int16 delta = numeric_cast<int16>(-((**mList).visible.bottom - (**mList).visible.top - 1));

	this->DoFocus();
	LScroll(0, delta, mList);
}


//---------------------------------------------------------------
//
// XListBox::DoPageDownKey
//
//---------------------------------------------------------------
void XListBox::DoPageDownKey()
{
	int16 delta = numeric_cast<int16>((**mList).visible.bottom - (**mList).visible.top - 1);

	this->DoFocus();
	LScroll(0, delta, mList);
}


//---------------------------------------------------------------
//
// XListBox::DoUpArrowKey
//
//---------------------------------------------------------------
void XListBox::DoUpArrowKey()
{
	const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();
	bool cmdKeyDown = event.WasCommandKeyDown();
	bool extendSelection = event.WasShiftKeyDown() && mInfo.hasMultipleSelections;
	
	uint32 count = this->GetCount();
	if (this->HasSelection()) {
		uint32 index = this->GetSelection();

		if (cmdKeyDown && extendSelection) {
			for (uint32 i = 0; i < index; ++i)
				this->Select(i);
						
		} else if (cmdKeyDown) {
			this->SelectOneItem(0);
		
		} else if (extendSelection) {
			if (index > 0)
				this->Select(index - 1);
		
		} else {			
			if (index > 0)
				this->SelectOneItem(index - 1);
		}

	} else if (count > 0)
		this->SelectOneItem(count - 1);

	if (this->HasSelection()) {
		this->ScrollIntoView(this->GetSelection());

		IListBox* thisPtr = this;
		IControlPtr control(thisPtr);
		control->HandleUpdate();
		
		if (mSelectionCallback.IsValid())
			mSelectionCallback(this);		// may call the callback more than neccesary, but that shouldn't be a problem
	}
}


//---------------------------------------------------------------
//
// XListBox::DoDownArrowKey
//
//---------------------------------------------------------------
void XListBox::DoDownArrowKey()
{
	const XKeyDownEvent& event = IKeyHandlerPtr(L"Application")->GetEvent();
	bool cmdKeyDown = event.WasCommandKeyDown();
	bool extendSelection = event.WasShiftKeyDown() && mInfo.hasMultipleSelections;
	
	uint32 index, count = this->GetCount();
	if (this->HasSelection()) {
		index = this->GetSelection();
		while (this->GetNextSelection(&index))
			;

		if (cmdKeyDown && extendSelection) {
			for (uint32 i = index+1; i < count; ++i)
				this->Select(i);
						
		} else if (cmdKeyDown) {
			this->SelectOneItem(count - 1);
		
		} else if (extendSelection) {
			if (index+1 < count)
				this->Select(index + 1);
		
		} else {			
			if (index+1 < count)
				this->SelectOneItem(index + 1);
		}

	} else if (count > 0)
		this->SelectOneItem(0);

	if (this->HasSelection()) {
		index = this->GetSelection();
		while (this->GetNextSelection(&index))
			;

		this->ScrollIntoView(index);
		
		IListBox* thisPtr = this;
		IControlPtr control(thisPtr);
		control->HandleUpdate();
		
		if (mSelectionCallback.IsValid())
			mSelectionCallback(this);		// may call the callback more than neccesary, but that shouldn't be a problem
	}
}


//---------------------------------------------------------------
//
// XListBox::DoKey
//
//---------------------------------------------------------------
bool XListBox::DoKey(const XKeyDownEvent& event)
{
	int16 key = event.GetKey();
	bool handled = key != kReturnKey && key != kEnterKey && key != kTabKey && key != kEscapeKey;
	
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

		bool cmdKeyDown = event.WasCommandKeyDown();
		bool printingChar = IsPrintable(ch) && !cmdKeyDown;

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


//---------------------------------------------------------------
//
// XListBox::DoAdjustSize
//
// A listbox that displays only part of the last line looks very
// ugly so we'll use this function to ensure that this never
// happens to us.
//
//---------------------------------------------------------------
void XListBox::DoAdjustSize(bool redraw)
{
	// Only display an integral number of lines
	int32 cellHeight = (**mList).cellSize.v;		// we can't use LRect because the list may be empty
	
	IListBox* thisPtr = this;
	IGeometryPtr geometry(thisPtr);
	XSize oldSize = geometry->GetSize();
	
	XSize newSize;
	newSize.width  = oldSize.width;
	newSize.height = Max(cellHeight*(oldSize.height/cellHeight), cellHeight);	// round down so that we don't encroach on other controls
	
	if (newSize != oldSize)
		geometry->SetSize(newSize, redraw);

	// Make sure the cell width is OK
	Point cellSize = (**mList).cellSize;			// can't use LRect because we may not have any cells
	cellSize.h = numeric_cast<int16>(oldSize.width);
	if (mInfo.hasVertScrollBar)
		cellSize.h -= 15;

	LSetDrawingMode(false, mList);
	LCellSize(cellSize, mList);
	LSetDrawingMode(true, mList);
}


//---------------------------------------------------------------
//
// XListBox::DoInvalidateItem
//
//---------------------------------------------------------------
void XListBox::DoInvalidateItem(uint32 index)
{
	PRECONDITION(index < this->GetCount());

	Point cell;
	cell.h = 0;
	
	Rect bounds, temp;
		
	if (index > 0) {						// we need to jump through these hoops to ensure that we invalidate the selection
		cell.v = numeric_cast<int16>(index - 1);
		LRect(&temp, cell, mList);	
		bounds.top = numeric_cast<int16>(temp.bottom - (**mList).rView.top);

	} else
		bounds.top = 0;
	
	if (index+1 < this->GetCount()) {
		cell.v = numeric_cast<int16>(index + 1);
		LRect(&temp, cell, mList);	
		bounds.bottom = numeric_cast<int16>(temp.top - (**mList).rView.top);

	} else
		bounds.bottom = 16000;
	
	bounds.left  = 0;
	bounds.right = 16000;	
	
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	control->Invalidate(bounds);
}


//---------------------------------------------------------------
//
// XListBox::DoFocus
//
// Note that we have to set the colors and the font or LScroll
// screws up. The Control Manager (still) blows chunks and
// makes life difficult for us. There's no way to have the
// Control Manager do this for us (SetUpControlBackground and
// SetUpControlTextColor seem like they could take care of
// most of our needs, but when I tried them they did nothing).
//
// However controls do include a ControlFontStyleRec which has
// all the information we need. Unfortunately, at least for
// listboxes, this appears to only be filled out if clients
// call SetControlFontStyle!
//
//---------------------------------------------------------------
void XListBox::DoFocus()
{
	IListBox* thisPtr = this;
	IControlPtr control(thisPtr);
	SetPort(control->GetOSDrawContext());
	
	ControlFontStyleRec fontStyle;

	int32 actualLength = 0;
	OSErr err = GetControlData(control->GetOSControl(), kControlListBoxPart, kControlFontStyleTag, sizeof(fontStyle), (char*) &fontStyle, &actualLength);
	if (err != noErr)
		fontStyle.flags = 0;
	
	if (fontStyle.flags & kControlUseFontMask)
		::TextFont(fontStyle.font);
	else
		::TextFont(systemFont);						// docs say the "system default font is used" $$$ does this mean we should use kThemeSystemFont? $$$
	
	if (fontStyle.flags & kControlUseSizeMask)
		::TextSize(fontStyle.size);
	else
		::TextSize(12);								// $$$ undocumented (in the final "8.5 Control Manager.pdf")

	if (fontStyle.flags & kControlUseFaceMask)
		::TextFace(fontStyle.style);
	else
		::TextFace(0);

	if (fontStyle.flags & kControlUseModeMask)
		::TextMode(fontStyle.mode);
	else
		::TextMode(srcCopy);

	RGBColor black = {0, 0, 0};
	if (fontStyle.flags & kControlUseForeColorMask)
		::RGBForeColor(&fontStyle.foreColor);
	else
		::RGBForeColor(&black);

	RGBColor white = {65535, 65535, 65535};
	if (fontStyle.flags & kControlUseBackColorMask)
		::RGBBackColor(&fontStyle.backColor);
	else
		::RGBBackColor(&white);						// $$$ undocumented (and nothing in Appearance.h looks right)
}


}	// namespace Whisper
