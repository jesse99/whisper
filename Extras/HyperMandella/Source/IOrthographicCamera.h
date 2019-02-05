/*
 *  File:       IOrthographicCamera.h
 *  Summary:   	A non-persepective camera that shoots our parallel rays.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: IOrthographicCamera.h,v $
 *		Revision 1.3  2001/03/26 04:23:59  jesjones
 *		SetViewSize now can dirty the doc.
 *		
 *		Revision 1.2  2001/03/25 06:08:23  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:23  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:05:29  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <X2DPrimitives.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IOrthographicCamera
//!		A non-persepective camera that shoots our parallel rays.
// ===================================================================================
class IOrthographicCamera : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual X2DSize 	GetViewSize() const = 0;

	virtual void 		SetViewSize(const X2DSize& size, bool dirtyDoc = false) = 0;
};

typedef XInterfacePtr<IOrthographicCamera> IOrthographicCameraPtr;
typedef XInterfacePtr<const IOrthographicCamera> IConstOrthographicCameraPtr;
