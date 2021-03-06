/*
 *  File:       IClickExtension.h
 *	Function:	Interface for objects that affect the way their owner responds to 
 *				clicks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IClickExtension.h,v $
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/14 08:20:14  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:00:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 4/01/99	JDJ		Created
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
//	class IClickExtension
//!		Interface for objects that affect the way their owner responds to clicks.
/*!		Click extensions differ from mouse extensions in that they are invoked after 
 *		their owner has processed the mouse down event. Mouse extensions are used to 
 *		subvert or augment the normal mouse handling (eg a visual layout tool might use
 *		a mouse extension to intercept double clicks and display an edit dialog). OTOH 
 *		click extensions are used to respond to mouse down events. For example, you can
 *		attach a click extension to a push button that sends an XMenuCommand to a 
 *		particular target. Or you can attach a click extension to a checkbox that calls 
 *		a member function of an arbitrary object. You can also attach multiple click 
 *		extensions to a single owner. */
// ===================================================================================
class IClickExtension : public XUnknown {
	
//-----------------------------------
//	API 
//
public:
	// ----- Name -----
	virtual std::wstring GetName() const = 0;
	
	virtual void 		SetName(const std::wstring& name) = 0;
									
	// ----- Click Count -----
	virtual uint32 		GetClickCount() const = 0;

	virtual void 		SetClickCount(uint32 clickCount) = 0;
	
	// ----- Action -----
	virtual void		HandleClick(const XMouseDownEvent& event) = 0;
						/**< Will only be called if the click count is zero or equal to
						the event's click count. */
};

typedef XInterfacePtr<IClickExtension> IClickExtensionPtr;
typedef XInterfacePtr<const IClickExtension> IConstClickExtensionPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
