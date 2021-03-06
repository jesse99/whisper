/*
 *  File:       IListBox.h
 *  Summary:   	A system control that displays a list of strings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IListBox.h,v $
 *		Revision 1.1  2001/04/01 04:49:26  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XCallbacks.h>
#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultListBoxProcID = kControlListBoxProc; 
const uint32 kDefaultListBoxStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + WS_BORDER;	


// ===================================================================================
//	struct SListBoxInfo
// ===================================================================================
struct UI_EXPORT SListBoxInfo {
	bool	hasVertScrollBar;
	bool	hasHorzScrollBar;
	bool	hasGrowBox;					// Mac only
	bool	hasMultipleSelections;
	bool	useTypeAhead;
	
		SListBoxInfo() {hasVertScrollBar = true; hasHorzScrollBar = false; hasGrowBox = false; 
						hasMultipleSelections = false; useTypeAhead = false;}
};


// ===================================================================================
//	class IListBox
//!		A system control that displays a list of strings.
// ===================================================================================
class IListBox : public XUnknown {

//-----------------------------------
//	Types
//
public:
		typedef XCallback1<void, IListBox*> Callback;
		
//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{			
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID) = 0;

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring* begin, const std::wstring* end,
							 const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID) = 0;

	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const wchar_t** begin, const wchar_t** end,
							 const SListBoxInfo& info = SListBoxInfo(),
							 uint32 style = kDefaultListBoxStyle,
							 uint32 procID = kDefaultListBoxProcID) = 0;
	//@}

	//! @name Item Access
	//@{			
	virtual uint32 		GetCount() const = 0;

	virtual std::wstring GetItem(uint32 index) const = 0;
	
	virtual void 		SetItem(uint32 index, const std::wstring& text, bool redraw = kRedraw) = 0;
	//@}

	//! @name Adding Items
	//@{			
	virtual void 		AppendItem(const std::wstring& text, bool redraw = kRedraw) = 0;
	
	virtual void 		InsertItem(const std::wstring& text, uint32 at, bool redraw = kRedraw) = 0;
	//@}
	
	//! @name Removing Items
	//@{			
	virtual void 		RemoveItem(uint32 index, bool redraw = kRedraw) = 0;

	virtual void 		RemoveEveryItem(bool redraw = kRedraw) = 0;
	//@}
	
	//! @name Selections
	//@{			
	virtual bool 		HasSelection() const = 0;
						/**< Returns true if an item is selected. */

	virtual bool 		HasOneSelection() const = 0;
						/**< Returns true if only one item is selected. */

	virtual uint32		GetSelection() const = 0;
						/**< Returns the first selected item. Note that it's an error to call this 
						if there is no selection. */

	virtual bool 		GetNextSelection(uint32* index) const = 0;
						/**< Returns true if another selection was found.
						Note that you can pass in ULONG_MAX for the index to start at the first
						selected item. */

	virtual bool 		IsSelected(uint32 index) const = 0;

	virtual void		SelectAll(bool select = true, bool redraw = kRedraw) = 0;

	virtual void		Select(uint32 index, bool select = true, bool redraw = kRedraw) = 0;

	virtual void		SelectOneItem(uint32 index, bool redraw = kRedraw) = 0;
	
	virtual void 		SetSelectionCallback(const Callback& callback) = 0;
						/**< The callback will be called whenever the user changes the selection.
						Note this will always be called when the selection changes, but on
						some platforms, may also be called when the selection hasn't changed. */
	//@}

	//! @name Misc
	//@{			
	virtual void		ScrollIntoView(uint32 index) = 0;
						
	virtual bool		FindItem(const std::wstring& item, uint32* index) const = 0;
						/**< Returns false if the item isn't found. */

#if MAC				
	virtual ListHandle 	GetOSListBox() = 0;
#endif
	//@}
};


typedef XInterfacePtr<IListBox> IListBoxPtr;
typedef XInterfacePtr<const IListBox> IConstListBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
