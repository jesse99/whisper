/*
 *  File:       IAppDialog.h
 *  Summary:   	Interface used by generic dialogs operating on the app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IAppDialog.h,v $
 *		Revision 1.1  2001/04/05 23:45:28  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IAppDialog
//!		Interface used by generic dialogs operating on the app.
// ===================================================================================
class IAppDialog : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		Pose() = 0;
						/**< Returns true if the dialog was dismissed via the default button. */
};

typedef XInterfacePtr<IAppDialog> IAppDialogPtr;
typedef XInterfacePtr<const IAppDialog> IConstAppDialogPtr;

