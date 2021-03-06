/*
 *  File:       X3DStyle.h
 *  Summary:	Base class for QuickDraw 3D style objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStyle.h,v $
 *		Revision 1.4  2001/04/14 07:32:18  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:54:31  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:24  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Quesa.h>
#include <QuesaStyle.h>

#include <X3DShape.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DStyle
//!		Base class for Quesa style objects.
// ===================================================================================
class QUESA_EXPORT X3DStyle : public X3DShape {

	typedef X3DShape Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DStyle();
						
						X3DStyle(TQ3StyleObject object);
						
	virtual X3DStyle* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	// ----- Conversion Operator -----
						operator TQ3StyleObject() const				{return mObject;}

	// ----- Submitting (Retained Mode) -----
			void 		Submit(TQ3ViewObject view) const;

	// ----- Misc -----
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DAntiAliasStyle. */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

