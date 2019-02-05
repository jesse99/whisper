/*
 *  File:       XProcedure.h
 *  Summary:	Template funtions that are used to convert a callback that returns a
 *				value to one that returns void.
 *  Written by: Jesse Jones
 *
 *	Usage:		These are used like this:
 *					XCallback2<bool, XMenuCommand, XEvent> temp(this, &CApplication::HandleMenuCommand);
 *					XCallback0<bool> temp2 = Adapt0(temp, kQuitCmd, XEvent());
 *					XCallback0<void> callback = Procedure0(temp2);
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XProcedure.h,v $
 *		Revision 1.3  2000/11/12 07:40:11  jesjones
 *		Removed the procedure adaptors.
 *		
 *		Revision 1.2  2000/11/09 12:47:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XProcedure.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/11/12 07:40:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Removed the procedure adaptors.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 2/24/00	JDJ		Created
 */

#pragma once

#include <XCallbacks.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	Global Functions
// ===================================================================================
template <class FUNCTION>													// 0 arg
XCallback0<void> 
Procedure0(FUNCTION callback)
{	
	XCallback0<void> result(new ZProcedure0<FUNCTION>(callback), 1, 2);
	
	return result;
}

template <class FUNCTION>													// 1 arg
XCallback1<void, FUNCTION::argument_type> 
Procedure1(FUNCTION callback)
{	
	XCallback1<void, FUNCTION::argument_type> result(new ZProcedure1<FUNCTION, FUNCTION::argument_type>(callback), 1, 2);
	
	return result;
}

template <class FUNCTION>													// 2 arg
XCallback2<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type> 
Procedure2(FUNCTION callback)
{	
	XCallback2<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type> result(new ZProcedure2<FUNCTION, FUNCTION::first_argument_type, FUNCTION::second_argument_type>(callback), 1, 2);
	
	return result;
}

template <class FUNCTION>													// 3 arg
XCallback3<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> 
Procedure3(FUNCTION callback)
{	
	XCallback3<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> result(new ZProcedure3<FUNCTION, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback), 1, 2);
	
	return result;
}

template <class FUNCTION>													// 4 arg
XCallback4<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> 
Procedure4(FUNCTION callback)
{	
	XCallback4<void, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> result(new ZProcedure4<FUNCTION, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback), 1, 2);
	
	return result;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
