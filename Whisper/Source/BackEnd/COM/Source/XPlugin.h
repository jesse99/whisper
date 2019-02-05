/*
 *  File:       XPlugin.h
 *  Summary:   	The class we use to load and unload plugins.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPlugin.h,v $
 *		Revision 1.4  2001/03/05 05:43:00  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:30:59  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:46:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XFileSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFragment;
class XURI;


// ===================================================================================
//	class XPlugin
//!		The class we use to load and unload plugins.
// ===================================================================================
class COM_EXPORT XPlugin {

//-----------------------------------
//	Types
//
public:
	typedef void (*StartupProc)();			//!< must be named "Startup" and have C linkage
	typedef void (*ShutdownProc)();			//!< must be named "Shutdown" and have C linkage
	typedef void (*MainProc)();				//!< must be named "Main" and have C linkage

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XPlugin();
			 			/**< Removes the plugin's bosses from the object model and calls
			 			the plugin's optional shutdown function. */
	
  						XPlugin(const XFileSpec& spec);
			 			/**< Calls the startup function, adds the plugin's bosses to
			 			the object model (these should be listed in a "Bosses.bXML"
			 			resource), and calls the optional main entry point. */

  						XPlugin(const XFileSpec& spec, const XURI& bosses);
  						/**< Allows you to specify the location of the bosses resource. */

private:
						XPlugin(const XPlugin& rhs);
						
			XPlugin& 	operator=(const XPlugin& rhs);

//-----------------------------------
//	API
//
public:
			XFragment* 	GetFragment() const						{return mFragment;}

			const XFileSpec& GetSpec() const					{return mSpec;}

//-----------------------------------
//	Member Data
//
protected:
	XFragment*	mFragment;
	XFileSpec	mSpec;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
