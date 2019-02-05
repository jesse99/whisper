/*
 *  File:       IDialog.h
 *  Summary:   	Interface used to change various IDocInfo settings.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDialog.h,v $
 *		Revision 1.2  2001/02/17 10:25:05  jesjones
 *		Tweaked some comments.
 *		
 *		Revision 1.1  2000/12/31 04:24:28  jesjones
 *		Checked in
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
class IDocInfo;
typedef XInterfacePtr<IDocInfo> IDocInfoPtr;


// ===================================================================================
//	class IDialog
//!		Interface used to change various IDocInfo settings.
// ===================================================================================
class IDialog : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Pose(IDocInfoPtr& doc) = 0;
						/**< This won't return until the user dismisses the dialog. If
						the user presses OK it will fire off an undoable action to change
						the doc. */
};

typedef XInterfacePtr<IDialog> IDialogPtr;
typedef XInterfacePtr<const IDialog> IConstDialogPtr;

