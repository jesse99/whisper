/*
 *  File:       IComplexDialog.h
 *  Summary:   	Interface used to change CHyperComplex's on the document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IComplexDialog.h,v $
 *		Revision 1.2  2001/03/26 04:25:22  jesjones
 *		Pose no longer takes a name.
 *		
 *		Revision 1.1  2001/03/25 06:04:05  jesjones
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
class CHyperComplex;
class IDocInfo;
typedef XInterfacePtr<IDocInfo> IDocInfoPtr;


// ===================================================================================
//	class IComplexDialog
//!		Interface used to change CHyperComplex's on the document.
// ===================================================================================
class IComplexDialog : public XUnknown {

//-----------------------------------
//	API
//
public:	
	virtual bool 		Pose(const CHyperComplex& value, XCallback1<void, CHyperComplex> callback) = 0;
						/**< Returns true if the dialog was dismissed via the default button. */
};

typedef XInterfacePtr<IComplexDialog> IComplexDialogPtr;
typedef XInterfacePtr<const IComplexDialog> IConstComplexDialogPtr;

