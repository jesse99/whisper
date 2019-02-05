/*
 *  File:       XDialogUtils.h
 *  Summary:   	Handy dialog manager related routines.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDialogUtils.h,v $
 *		Revision 1.1  2001/01/01 00:29:22  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.3  2000/12/31 04:47:41  jesjones
 *		Backed out of implementing GetString.
 *		
 *		Revision 1.2  2000/11/09 12:32:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	12/21/99	JDJ		Added some simple input dialogs.
 *		 <1>	12/06/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XInterfacePtr.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XURI;
class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
enum EAnswerType {kYes, kNo, kCancel};


// ===================================================================================
//	Message Alerts
// ===================================================================================
UI_EXPORT void DoNote(const std::wstring& mainStr, const std::wstring& supplementalStr);
			   // Use this alert to inform the user of a minor mistake or to display
			   // information. The mainStr should be used to provide a short summary
			   // of the problem. The supplementalStr should be used to provide a more
			   // detailed description of the problem and it's consequences.

UI_EXPORT bool DoCaution(const std::wstring& mainStr, const std::wstring& supplementalStr);
			   // Use this to warn the user in advance of a potentially dangerous action.
			   // It will return true if the user wants to proceed.

UI_EXPORT void DoStop(const std::wstring& mainStr, const std::wstring& supplementalStr);
			   // Use this to tell the user that an action cannot be completed.


// ===================================================================================
//	Question Alerts
// ===================================================================================
UI_EXPORT EAnswerType Ask(const std::wstring& prompt);
			   // Asks a question and lets the user choose yes, no, or cancel. Yes
			   // is the default button.
			
UI_EXPORT bool AnswerYes(const std::wstring& prompt);
			   // Asks a yes or no question. Returns true if the default button (Yes)
			   // was pressed.

UI_EXPORT bool AnswerNo(const std::wstring& prompt);
			   // Asks a yes or no question. Returns true if the default button (No)
			   // was pressed.


// ===================================================================================
//	Simple Input Dialogs
// ===================================================================================
#if 0			// $$ not sure if these should be implemented
UI_EXPORT bool GetString(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, std::wstring* value);
			   // uri should point to a dialog with a textbox named "Text".
			   // id should point to the dialog element.
			   // parent is used on Windows and should normally be a window, the doc, or the app

UI_EXPORT bool GetInt16(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, int16* value);
UI_EXPORT bool GetInt32(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, int32* value);
UI_EXPORT bool GetUInt32(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, uint32* value);

UI_EXPORT bool GetFloat(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, float* value);
UI_EXPORT bool GetDouble(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent, double* value);
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

