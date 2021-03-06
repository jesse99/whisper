/*
 *  File:       IFrameWindow.h
 *  Summary:   	The topmost window of a Windows MDI app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *  
 *  Change History (most recent first):		
 *
 *		$Log: IFrameWindow.h,v $
 *		Revision 1.2  2000/12/14 08:22:01  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/11/20 00:58:21  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IFrameWindow
//!		The topmost window of a Windows MDI app.
// ===================================================================================
class IFrameWindow : public XUnknown {
	
//-----------------------------------
//	API
//
public:
	//! @name Initialization
	//@{
	virtual void 		Init() = 0;
						/**< Call this after initing the IWindow. */
	//@}

	//! @name Client Window
	//@{
	virtual	HWND 		GetClientWindow() const = 0;
	//@}
			
	//! @name Status Window
	//@{
	virtual	void 		SetStatusMesg(const std::wstring& mesg, uint32 part = 0) = 0;

	virtual	void 		SetNumParts(const int32* begin, const int32* end) = 0;
						/**< Use this to divide the status bar into multiple sections. Each
						value contains a pixel width for that part. Use -1 if the width
						should extend to the right side of the frame window. By default 
						the status bar has only one part and OnMenuSelect defaults to 
						using the first part to display menu help. */
			
	virtual HWND 		GetStatusWindow() const = 0;
	//@}
};

typedef XInterfacePtr<IFrameWindow> IFrameWindowPtr;
typedef XInterfacePtr<const IFrameWindow> IConstFrameWindowPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
