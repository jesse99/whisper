/*
 *  File:       App.r
 *  Summary:   	Application resources
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: App.r,v $
 *		Revision 1.4  2001/11/25 23:35:11  jesjones
 *		Added a 'carb' resource.
 *		
 *		Revision 1.3  2001/03/14 11:05:56  jesjones
 *		Corrected a string that said "HyperMandella".
 *		
 *		Revision 1.2  2001/03/08 08:36:14  jesjones
 *		Replaced 68 Stub.rsrc with 68 Stub.r. Moved Whisper.rsrc resources into Whisper.r (this should make it possible to release Whisper via a SourceForge tarball).
 *		
 *		Revision 1.1  2000/12/31 04:22:44  jesjones
 *		Checked in
 *		
 *		Revision 1.1  2000/12/28 02:42:14  jesjones
 *		Moved from the Whisper to the Extras module.
 */

#include <BalloonTypes.r>
#include <Types.r>


//-----------------------------------
//	Constants
//
#define kCreatorType 	'DcSk'		
#define kFileType		'TEXT'
#define kFileType2		'DcSD'


// ===================================================================================
//	XML Resources
// 		Note that resource ids below 256 are reserved for Whisper.
// ===================================================================================
read 'bXML' (256, "Bosses") "Bosses.bXML";
read 'bXML' (257, "Menus") "Menus.bXML";
read 'bXML' (258, "Strings") "Strings.bXML";
read 'bXML' (259, "Windows") "Windows.bXML";

read 'dtd ' (258, "Doc") "Doc.dtd";


// ===================================================================================
//	'carb' Resource
//		Informs OS X that we can run outside the blue box.
// ===================================================================================
data 'carb' (0) {
};


// ===================================================================================
//	Owner Resource
//		Used by the Finder's Get Info window in the version field.
// ===================================================================================
data kCreatorType (0, "Owner resource") {
	"1.0 �2000 Jesse Jones"	
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
			"Draws rectangles. Intended as a starting point for drawing document oriented applications with Whisper."
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


// ===================================================================================
//	'kind' resource
//		This is used by the Finder to provide a more informative message in the Kind
//		column of list views. It's also used by Navigation Services to build popup
//		menus. 
// ===================================================================================
resource 'kind' (256) {
	kCreatorType,			// app signature
	0,						// localization code
	{
		'apnm',		"DocSkeleton",
		kFileType, 	"XML Document",
		kFileType2, "DocSkeleton Document",
	}
};
	
	

