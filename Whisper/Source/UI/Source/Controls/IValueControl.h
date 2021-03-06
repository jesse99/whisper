/*
 *  File:       IValueControl.h
 *  Summary:   	The interface used for controls that have values.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IValueControl.h,v $
 *		Revision 1.4  2001/03/05 05:38:55  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:25:58  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:07:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XConstants.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IValueControl
//!		The interface used for controls that have values.
// ===================================================================================
class IValueControl : public XUnknown {

//-----------------------------------
//	API
//
public:
	//! @name Value
	//@{
	virtual int32 		GetValue() const = 0;		
	virtual bool 		SetValue(int32 value, bool redraw = kRedraw) = 0;
						/**< Returns true if the control's value changed. */
	//@}

	//! @name Min/Max Values
	//@{
	virtual int32 		GetMinValue() const = 0;
	virtual int32 		GetMaxValue() const = 0;

	virtual void 		SetMinValue(int32 newMin, bool redraw = kRedraw) = 0;
	virtual void 		SetMaxValue(int32 newMax, bool redraw = kRedraw) = 0;

	virtual void 		SetMinMaxValue(int32 newMin, int32 newMax, bool redraw) = 0;
	//@}
};


typedef XInterfacePtr<IValueControl> IValueControlPtr;
typedef XInterfacePtr<const IValueControl> IConstValueControlPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
