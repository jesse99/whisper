/*
 *  File:       ITextBox.h
 *  Summary:   	A light weight text entry control.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITextBox.h,v $
 *		Revision 1.2  2000/12/25 02:46:24  jesjones
 *		Changed the Windows style flags.
 *		
 *		Revision 1.1  2000/12/19 07:48:34  jesjones
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
const uint32 kDefaultTextBoxProcID = kControlEditTextProc; 
const uint32 kDefaultTextBoxStyle  = WS_CHILD + WS_VISIBLE + WS_TABSTOP + ES_AUTOHSCROLL + ES_AUTOVSCROLL + ES_LEFT + ES_MULTILINE;


// ===================================================================================
//	class ITextBox
//!		A light weight text entry control.
// ===================================================================================
class ITextBox : public XUnknown {

//-----------------------------------
//	Types
//
public:	
	typedef XCallback1<void, ITextBox*> 					ChangedCallback;
	typedef XCallback3<bool, ITextBox*, int16, wchar_t> 	PassUpCallback;
	typedef XCallback2<void, ITextBox*, std::wstring>		RejectCallback;

//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 uint32 style = kDefaultTextBoxStyle,
							 uint32 procID = kDefaultTextBoxProcID) = 0;
	//@}
	
	//! @name Selections
	//@{
	virtual void		SetSelection(uint32 start, uint32 stop, bool redraw = kRedraw) = 0;
						/**< Selects characters from start to stop. Note that the
						character positions are between individual characters
						(so start == 0, stop == 0 specifies an insertion point
						before the first character). */
						
	virtual void		SelectAll(bool redraw = kRedraw) = 0;

	virtual void		Select(bool redraw = kRedraw) = 0;
						/**< Selects the text and makes the textbox the focus. */
	//@}

	//! @name Callbacks
	//@{
	virtual void 		SetChangedCallback(const ChangedCallback& callback) = 0;
						/**< Called if the user changes the text and the text is
						valid. Default does nothing. */

	virtual void 		SetPassUpCallback(const PassUpCallback& callback) = 0;
						/**< Callback should return true if the parent control
						should handle the character. */

	virtual void 		SetRejectedCallback(const RejectCallback& callback) = 0;
						/**< Called when the user types too many characters or the
						validator rejects a key (the callback is passed the
						rejected text). */
	//@}
						
	//! @name Default Callbacks
	//@{
	virtual bool 		DefaultPassUp(ITextBox* textbox, int16 key, wchar_t ch) = 0;
						/**< Returns true for return, enter, tab, and escape. */
						
	virtual void 		DefaultReject(ITextBox* textbox, const std::wstring& lostChars) = 0;
						/**< Calls Whisper::Beep. */
	//@}
};

typedef XInterfacePtr<ITextBox> ITextBoxPtr;
typedef XInterfacePtr<const ITextBox> IConstTextBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
