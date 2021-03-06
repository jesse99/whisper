/*
 *  File:       IPoseDialogBox.h
 *  Summary:   	Interface used to display modal dialogs.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPoseDialogBox.h,v $
 *		Revision 1.2  2001/01/01 03:47:40  jesjones
 *		Tweaked a comment.
 *		
 *		Revision 1.1  2001/01/01 00:28:43  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.3  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.2  2000/12/14 08:24:13  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:01:43  jesjones
 *		Checked in	
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IDialogBox;
typedef XInterfacePtr<IDialogBox> IDialogBoxPtr;


// ===================================================================================
//	class IPoseDialogBox
//!		Interface used to display modal dialogs.
/*!		See CWidthDialog.cpp in the DocSkeleton project for an example of how to create
 *		a dialog in Whisper. */
// ===================================================================================
class IPoseDialogBox : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Open(const IDialogBoxPtr& dialog) = 0;
						/**< Opens the dialog window. */
	
	virtual bool 		Run() = 0;
						/**< Starts up a second event loop to process dialog events.
						Returns true if the dialog was dismissed via the default button. */
	
	virtual void 		Close() = 0;

	virtual IDialogBoxPtr GetDialog() const = 0;
};

typedef XInterfacePtr<IPoseDialogBox> IPoseDialogBoxPtr;
typedef XInterfacePtr<const IPoseDialogBox> IConstPoseDialogBoxPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
