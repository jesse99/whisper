/*
 *  File:       IPointLightDialog.h
 *  Summary:   	Dialog used to edit IPointLight settings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPointLightDialog.h,v $
 *		Revision 1.4  2001/04/03 01:04:29  jesjones
 *		Pose methods now take a const reference (makes it easier to passs in a temporary).
 *		
 *		Revision 1.3  2001/03/25 06:08:35  jesjones
 *		Added a comment.
 *		
 *		Revision 1.2  2001/02/28 09:54:56  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:32  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.1  2000/12/05 09:01:39  jesjones
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
//	class IPointLightDialog
//!		Dialog used to edit IPointLight settings.
// ===================================================================================
class IPointLightDialog : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		Pose(const IDocInfoPtr& doc, uint32 index) = 0;
						/**< Index is the light index. */
};

typedef XInterfacePtr<IPointLightDialog> IPointLightDialogPtr;
typedef XInterfacePtr<const IPointLightDialog> IConstPointLightDialogPtr;

