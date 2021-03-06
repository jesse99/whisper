/*
 *  File:       IDocDialog.h
 *  Summary:   	Interface used by generic dialogs operating on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDocDialog.h,v $
 *		Revision 1.2  2001/04/03 01:04:26  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.1  2001/03/27 06:24:14  jesjones
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
//	class IDocDialog
//!		Interface used by generic dialogs operating on the document.
// ===================================================================================
class IDocDialog : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual bool 		Pose(const IDocInfoPtr& doc) = 0;
						/**< Returns true if the dialog was dismissed via the default button. */
};

typedef XInterfacePtr<IDocDialog> IDocDialogPtr;
typedef XInterfacePtr<const IDocDialog> IConstDocDialogPtr;

