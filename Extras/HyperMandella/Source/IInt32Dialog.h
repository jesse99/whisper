/*
 *  File:       IInt32Dialog.h
 *  Summary:   	Interface used to change int32's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IInt32Dialog.h,v $
 *		Revision 1.1  2001/03/26 04:04:11  jesjones
 *		Checked in
 *		
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
//	class IInt32Dialog
//!		Interface used to change int32's on the document.
// ===================================================================================
class IInt32Dialog : public XUnknown {

//-----------------------------------
//	API
//
public:	
	virtual bool 		Pose(int32 value, XCallback1<void, int32> callback) = 0;
						/**< Returns true if the dialog was dismissed via the default button. */
};

typedef XInterfacePtr<IInt32Dialog> IInt32DialogPtr;
typedef XInterfacePtr<const IInt32Dialog> IConstInt32DialogPtr;

