/*
 *  File:       IContextMenu.h
 *  Summary:   	Cross platform wrapper around contextual popup menus (ie menus not attached to a menubar).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IContextMenu.h,v $
 *		Revision 1.4  2001/02/08 03:50:02  jesjones
 *		Commands are directed to the menu handler on the app now.
 *		
 *		Revision 1.3  2000/12/14 08:20:36  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 6/30/99	JDJ		Added XXMLDoc ctor. Added SetContext.
 *		 <3>	 4/29/99	JDJ		Renamed Post Execute. Added deleteMenu argument.
 *		 <2>	 4/28/99	JDJ		Added GetContext.
 *		 <1>	10/10/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XInterfacePtr.h>
#include <XPoint.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IContextMenu
//!		Cross platform wrapper around contextual popup menus (ie menus not attached to a menubar).
// ===================================================================================
class IContextMenu : public XUnknown {

//-----------------------------------
//	Initialization
//
public:
	virtual void 		Init(const XPoint& globalLoc) = 0;
						/**< globalLoc is the position at which the menu will pop up. */

#if MAC
	virtual void 		Init(const XPoint& globalLoc, const AEDesc* selection, 
							 uint32 helpType = kCMHelpItemNoHelp, const std::wstring& helpText = L"") = 0;
						/**< selection will be used by context menu plugins
						helpType can be kCMHelpItemNoHelp, kCMHelpItemAppleGuide, or 
						kCMHelpItemOtherHelp (if kCMHelpItemOtherHelp is used helpText
						should be set). */
#endif
		
//-----------------------------------
//	API
//
public:
	virtual XMenuCommand Execute() = 0;
						/**< Pops up the menu and dispatches the command to the app's menu 
						handler. If Help is selected kHelpCmd is dispatched. Returns the 
						command chosen by the user (you'll rarely need this). */
};

typedef XInterfacePtr<IContextMenu> IContextMenuPtr;
typedef XInterfacePtr<const IContextMenu> IConstContextMenuPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
