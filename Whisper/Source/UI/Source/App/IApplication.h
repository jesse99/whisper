/*
 *  File:       IApplication.h
 *  Summary:   	The application object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: IApplication.h,v $
 *		Revision 1.7  2001/04/27 09:08:32  jesjones
 *		Nuked Get and SetSleepTime.
 *		
 *		Revision 1.6  2000/12/31 04:19:45  jesjones
 *		Removed DisplayAboutBox.
 *		
 *		Revision 1.5  2000/12/14 08:19:44  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.4  2000/11/26 03:28:35  jesjones
 *		Added an Init method.
 *		
 *		Revision 1.3  2000/11/21 09:27:17  jesjones
 *		Made HandleTime and HandleIdle pure virtual.
 *		
 *		Revision 1.2  2000/11/09 08:59:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <13>	12/21/99	JDJ		Removed the app and Whisper data folder specs.
 *		 <12>	 8/10/99	JDJ		Removed mGlobalUndoContext.
 *		 <11>	 7/14/99	JDJ		Added OnGetMenuBarURI.
 *		 <10>	 7/14/99	JDJ		Added mWhisperXMLFolder, mWhisperBinFolder, mAppXMLFolder, and mAppBinFolder.
 *		 <9>	 5/20/99	JDJ		OnCreateDefaultMenuBar takes an OSWindow instead of XWindow.
 *		 <8>	 5/17/99	JDJ		Added OnTranslateAccelerator. (Win)
 *		 <7>	 5/12/99	JDJ		Added OnPostInit.
 *		 <6>	 5/11/99	JDJ		Added OnCommandLine.
 *		 <5>	 5/03/99	JDJ		Removed OnOpenWithoutDoc.
 *		 <4>	 2/16/99	JDJ		Installs a string table loader. Added locale_begin and locale_end
 *									for iterating over locales supported by the app.
 *		 <3>	 1/15/99	JDJ		Replaced hogging processor flag with sleep time.
 *		 <1>	10/16/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XInterfacePtr.h>
#include <XFileSpec.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XEvent;


// ===================================================================================
//	class IApplication
//!		The application object
// ===================================================================================
class IApplication : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Running
	//@{
	virtual void 		Init() = 0;
						/**< Call this before calling Run. */

	virtual void 		Run() = 0;
						/**< Call this after creating an app object to enter the main
						event loop. It will return when the user selects the Quit
						menu item or Quit() is called. */
	//@}
						
	//! @name Quitting
	//@{
	virtual void 		Quit() = 0;
			
	virtual bool 		IsQuitting() const = 0;
	//@}
					
	//! @name Misc
	//@{
 	virtual std::wstring GetName() const = 0;

 	virtual bool 		IsMemoryLow() const = 0;
	//@}

//-----------------------------------
//	Internal API
//
public:
	virtual void		HandleUpdateMenus(const XEvent& event) = 0;

#if WIN
	virtual	bool 		HandleMessage(const MSG& msg, int32& result) = 0;

 	virtual XFileSpec 	GetHelpFile() const = 0;
	
	virtual void 		HandleTime() = 0;
#endif
};

typedef XInterfacePtr<IApplication> IApplicationPtr;
typedef XInterfacePtr<const IApplication> IConstApplicationPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
