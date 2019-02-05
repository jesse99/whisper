/*
 *  File:       App.r
 *  Summary:   	Application resources
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: App.r,v $
 *		Revision 1.2  2001/03/08 08:36:21  jesjones
 *		Replaced 68 Stub.rsrc with 68 Stub.r. Moved Whisper.rsrc resources into Whisper.r (this should make it possible to release Whisper via a SourceForge tarball).
 *		
 *		Revision 1.1.1.1  2000/12/27 03:12:46  jesjones
 *		First Imported.
 *		
 *		Revision 1.2  2000/11/09 08:51:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <BalloonTypes.r>
#include <Types.r>


//-----------------------------------
//	Constants
//
#define kCreatorType 	'ApSz'	
#define kFileType		'fold'
#define kFileType2		'TEXT'		// might want to change this as well


// ===================================================================================
//	XML Resources
// 		Note that resource ids below 256 are reserved for Whisper.
// ===================================================================================
read 'bXML' (256, "Bosses") "Bosses.bXML";
read 'bXML' (257, "Strings") "Strings.bXML";


// ===================================================================================
//	Strings
// ===================================================================================
resource 'STR#' (256, "Map Section Names") {{
	"Code section,",		// 1 
	"Import container",		// 2 
	"Data section",			// 3
}};

resource 'STR#' (257, "Namespace Names") {{
	"std",			// 1 
	"Whisper",		// 2 
	"PowerPlant",	// 3
	"",				// (signals last name)
}};


// ===================================================================================
//	Owner Resource
//		Used by the Finder's Get Info window in the version field.
// ===================================================================================
data kCreatorType (0, "Owner resource") {
	"1.0 ©2000 Jesse Jones"							
};

	
// ===================================================================================
//	Finder Icon Help Message 
//		The message displayed by balloon help when the user moves over the application
//		icon.
// ===================================================================================
resource 'hfdr' (-5696, "Finder Help") {
	HelpMgrVersion,
	hmDefaultOptions,
	0, 0,
	
	{
		HMStringItem {
			"Processes CodeWarrior map files and creates a file listing the amount of memory taken up by each class and function in the app."		
		},
	}
};


// ===================================================================================
//	'open' resource
//		This is used by the Translation Manager (and Navigation Services) to determine
//		which file types an app can open (the 'FREF' resource is then only used to
//		asssociate icons with file types). If it's present it's also used when drop
//		launching the application. You may need to rebuild the desktop database (or 
//		clear the app's Inited flag using something like ResEdit) after changing this.
// ===================================================================================
resource 'open' (256) {		
	kCreatorType,			
	{
		kFileType, 			
		kFileType2, 
	}
};


