/*
 *  File:       IFloatDialog.h
 *  Summary:   	Interface used to change float's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IFloatDialog.h,v $
 *		Revision 1.2  2001/03/26 04:24:37  jesjones
 *		Pose no longer takes a name.
 *		
 *		Revision 1.1  2001/03/25 06:04:10  jesjones
 *		Checked in
 */

#pragma once

#include <XCallbacks.h>
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
//	class IFloatDialog
//!		Interface used to change float's on the document.
// ===================================================================================
class IFloatDialog : public XUnknown {

//-----------------------------------
//	API
//
public:	
	virtual bool 		Pose(float value, XCallback1<void, float> callback) = 0;
						/**< Returns true if the dialog was dismissed via the default button. */
};

typedef XInterfacePtr<IFloatDialog> IFloatDialogPtr;
typedef XInterfacePtr<const IFloatDialog> IConstFloatDialogPtr;

