/*
 *  File:       ITrackControlHelper.h
 *  Summary:   	Mac interface that is called when the mouse is held down inside a control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITrackControlHelper.h,v $
 *		Revision 1.1  2001/02/04 03:41:25  jesjones
 *		Checked in
 *		
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
class XMouseDownEvent;


// ===================================================================================
//	class ITrackControlHelper
//!		Mac interface that is called when the mouse is held down inside a control.
// ===================================================================================
class ITrackControlHelper : public XUnknown {

//-----------------------------------
//	API
//
public:					
	virtual int32		OnTrackAction(int16 part) = 0;
						/**< Part is usually kControlPageUpPart, kControlPageDownPart,
						kControlUpButtonPart, or kControlDownButtonPart. Returns the
						amount by which the control's value should change. */
};

typedef XInterfacePtr<ITrackControlHelper> ITrackControlHelperPtr;
typedef XInterfacePtr<const ITrackControlHelper> IConstTrackControlHelperPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
