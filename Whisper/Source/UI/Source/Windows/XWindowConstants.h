/*
 *  File:       XWindowConstants.h
 *  Summary:   	Base class for cross platform window objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XWindowConstants.h,v $
 *		Revision 1.4  2001/02/07 04:59:19  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/11/09 12:59:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Constants
//
#if MAC
	/*
	 * Window Styles
	 */
	#define WS_OVERLAPPED       0x00000000L
	#define WS_POPUP            0x80000000L
	#define WS_CHILD            0x40000000L
	#define WS_MINIMIZE         0x20000000L
	#define WS_VISIBLE          0x10000000L
	#define WS_DISABLED         0x08000000L
	#define WS_CLIPSIBLINGS     0x04000000L
	#define WS_CLIPCHILDREN     0x02000000L
	#define WS_MAXIMIZE         0x01000000L
	#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
	#define WS_BORDER           0x00800000L
	#define WS_DLGFRAME         0x00400000L
	#define WS_VSCROLL          0x00200000L
	#define WS_HSCROLL          0x00100000L
	#define WS_SYSMENU          0x00080000L
	#define WS_THICKFRAME       0x00040000L
	#define WS_GROUP            0x00020000L
	#define WS_TABSTOP          0x00010000L
	
	#define WS_MINIMIZEBOX      0x00020000L
	#define WS_MAXIMIZEBOX      0x00010000L
	
	#define WS_TILED            WS_OVERLAPPED
	#define WS_ICONIC           WS_MINIMIZE
	#define WS_SIZEBOX          WS_THICKFRAME
	#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW
	
	/*
	 * Common Window Styles
	 */
	#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
	                             WS_CAPTION        | \
	                             WS_SYSMENU        | \
	                             WS_THICKFRAME     | \
	                             WS_MINIMIZEBOX    | \
	                             WS_MAXIMIZEBOX)
	
	#define WS_POPUPWINDOW      (WS_POPUP          | \
	                             WS_BORDER         | \
	                             WS_SYSMENU)
	
	#define WS_CHILDWINDOW      (WS_CHILD)
	
	/*
	 * Extended Window Styles
	 */
	#define WS_EX_DLGMODALFRAME     0x00000001L
	#define WS_EX_NOPARENTNOTIFY    0x00000004L
	#define WS_EX_TOPMOST           0x00000008L
	#define WS_EX_ACCEPTFILES       0x00000010L
	#define WS_EX_TRANSPARENT       0x00000020L
	#define WS_EX_MDICHILD          0x00000040L
	#define WS_EX_TOOLWINDOW        0x00000080L
	#define WS_EX_WINDOWEDGE        0x00000100L
	#define WS_EX_CLIENTEDGE        0x00000200L
	#define WS_EX_CONTEXTHELP       0x00000400L
	
	#define WS_EX_RIGHT             0x00001000L
	#define WS_EX_LEFT              0x00000000L
	#define WS_EX_RTLREADING        0x00002000L
	#define WS_EX_LTRREADING        0x00000000L
	#define WS_EX_LEFTSCROLLBAR     0x00004000L
	#define WS_EX_RIGHTSCROLLBAR    0x00000000L
	
	#define WS_EX_CONTROLPARENT     0x00010000L
	#define WS_EX_STATICEDGE        0x00020000L
	#define WS_EX_APPWINDOW         0x00040000L
	
	#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
	#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
	
	#define WS_EX_LAYERED           0x00080000
	#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
	#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
	#define WS_EX_NOACTIVATE        0x08000000L
	
	/*
	 * Class styles
	 */
	#define CS_VREDRAW          0x0001
	#define CS_HREDRAW          0x0002
	#define CS_DBLCLKS          0x0008
	#define CS_OWNDC            0x0020
	#define CS_CLASSDC          0x0040
	#define CS_PARENTDC         0x0080
	#define CS_NOCLOSE          0x0200
	#define CS_SAVEBITS         0x0800
	#define CS_BYTEALIGNCLIENT  0x1000
	#define CS_BYTEALIGNWINDOW  0x2000
	#define CS_GLOBALCLASS      0x4000
	
	#define CS_IME              0x00010000

#elif WIN
	typedef uint32 							WindowClass;
	
	enum {
		kAlertWindowClass			= 1L,							/* ÒI need your attention now.Ó*/
		kMovableAlertWindowClass	= 2L,							/* ÒI need your attention now, but IÕm kind enough to let you switch out of this app to do other things.Ó*/
		kModalWindowClass			= 3L,							/* system modal, not draggable*/
		kMovableModalWindowClass	= 4L,							/* application modal, draggable*/
		kFloatingWindowClass		= 5L,							/* floats above all other application windows*/
		kDocumentWindowClass		= 6L							/* everything else*/
	};

	typedef uint32 							WindowAttributes;
	
	enum {
		kWindowNoAttributes			= 0L,							/* no attributes*/
		kWindowCloseBoxAttribute	= (1L << 0),					/* window has a close box*/
		kWindowHorizontalZoomAttribute = (1L << 1),					/* window has horizontal zoom box*/
		kWindowVerticalZoomAttribute = (1L << 2),					/* window has vertical zoom box*/
		kWindowFullZoomAttribute	= (kWindowVerticalZoomAttribute | kWindowHorizontalZoomAttribute),
		kWindowCollapseBoxAttribute	= (1L << 3),					/* window has a collapse box*/
		kWindowResizableAttribute	= (1L << 4),					/* window is resizable*/
		kWindowSideTitlebarAttribute = (1L << 5),					/* window wants a titlebar on the side	(floating window class only)*/
		kWindowNoUpdatesAttribute	= (1L << 16),					/* this window receives no update events*/
		kWindowNoActivatesAttribute	= (1L << 17),					/* this window receives no activate events*/
		kWindowStandardDocumentAttributes = (kWindowCloseBoxAttribute | kWindowFullZoomAttribute | kWindowCollapseBoxAttribute | kWindowResizableAttribute),
		kWindowStandardFloatingAttributes = (kWindowCloseBoxAttribute | kWindowCollapseBoxAttribute)
	};
#endif


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
