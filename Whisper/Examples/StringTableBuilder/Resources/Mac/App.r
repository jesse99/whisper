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
 *		Revision 1.3  2001/03/08 08:36:07  jesjones
 *		Replaced 68 Stub.rsrc with 68 Stub.r. Moved Whisper.rsrc resources into Whisper.r (this should make it possible to release Whisper via a SourceForge tarball).
 *		
 *		Revision 1.2  2000/11/09 08:51:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <BalloonTypes.r>
#include <Types.r>


//-----------------------------------
//	Constants
//
#define kCreatorType 	'SttM'		
#define kFileType		'fold'


// ===================================================================================
//	XML Resources
// 		Note that resource ids below 256 are reserved for Whisper.
// ===================================================================================
read 'bXML' (256, "Bosses") "Bosses.bXML";
read 'bXML' (257, "Strings") "Strings.bXML";


// ===================================================================================
//	File Extensions
// ===================================================================================
resource 'STR#' (256, "Extensions") {{
	".h";
	".c";
	".cc";
	".cp";
	".cpp";
	".hpp";
	".c++";
	".inl";
	".inc";
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
			"Grovels through source code and builds an XML string table file to be used for localization."		
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
	}
};


