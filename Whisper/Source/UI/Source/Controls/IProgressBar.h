/*
 *  File:       IProgressBar.h
 *  Summary:   	A control that displays a system progress bar.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IProgressBar.h,v $
 *		Revision 1.4  2000/12/14 08:24:21  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/12 07:54:35  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/09 09:06:10  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 7/14/00	JDJ		Created
 */

#pragma once

#include <XConstants.h>
#include <XControlConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>
#include <XWindowConstants.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XRect;

class IHierarchy;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Constants
//
const uint32 kDefaultProgressBarProcID = kControlProgressBarProc; 
const uint32 kDefaultProgressBarStyle  = WS_CHILD + WS_VISIBLE;	


// ===================================================================================
//	class IProgressBar
//!		A control that displays a system progress bar.
// ===================================================================================
class IProgressBar : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Initialization -----
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, int32 minValue, int32 maxValue,
							 uint32 style = kDefaultProgressBarStyle,
							 uint32 procID = kDefaultProgressBarProcID) = 0;
						/**< You should use a busy cursor if an operation may take longer
						than 0.1 seconds. If it may take longer than a second spawn
						a thread or popup a progress bar. */

	// ----- Visuals -----
	virtual void 		MakeIndeterminate(bool indeterminate = true) = 0;
						/**< Changes the look of the progress bar to something like a 
						rotating barber pole. Does nothing on Windows. */
};


typedef XInterfacePtr<IProgressBar> IProgressBarPtr;
typedef XInterfacePtr<const IProgressBar> IConstProgressBarPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
