/*
 *  File:       XBind.h
 *  Summary:	Template funtions that are used to fix the arguments of a callback (producing a new
 *				callback object with fewer arguments).
 *  Written by: Jesse Jones
 *
 *	Usage:		These are most commonly used to convert a callback that has arguments
 *				into a callback with no arguments. The callback can then be attached to
 *				something like a push button so that the original callback is executed
 *				when the button is pushed. You can do this with something like the 
 *				following:
 *					XCallback2<bool, XMenuCommand, XEvent> temp(this, &CApplication::HandleMenuCommand);
 *					XCallback0<bool> callback = Bind2(temp, kQuitCmd, XEvent());
 *				So, what we're doing here is building a temporary callback to our
 *				function and then using Bind2 to create a new callback object that
 *				fixes the function's arguments.
 *
 *				You can also leave one or more of the arguments unbound. For example
 *				this will convert a two argument callback to one that takes a single
 *				argument:
 *					XCallback2<bool, XMenuCommand, XEvent> temp(this, &CApplication::HandleMenuCommand);
 *					XCallback1<bool, XMenuCommand> callback = Bind1(temp, kUnbound, XEvent());
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XBind.h,v $
 *		Revision 1.1  2000/11/12 06:29:35  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		
 */

#pragma once

#include <XCallbacks.h>
#include <XAdaptors0.h>
#include <XAdaptors1.h>
#include <XAdaptors2.h>
#include <XAdaptors3.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SUnbound
// ===================================================================================
struct SUnbound {};

const SUnbound kUnbound = SUnbound();


// ===================================================================================
//	Bind1
// ===================================================================================
template <class FUNCTION>													// 1 arg
XCallback0<FUNCTION::result_type> 
Bind1(FUNCTION callback, FUNCTION::argument_type arg1)
{	
	using namespace Internals;
	
	XCallback0<FUNCTION::result_type> result(new XAdaptor0f<FUNCTION, FUNCTION::result_type, FUNCTION::argument_type>(callback, arg1), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> 			// 2 args
Bind1(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> result(new XAdaptor1xf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type>(callback, arg2), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> 
Bind1(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> result(new XAdaptor1fx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type>(callback, arg1), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type> 	// 3 args
Bind1(FUNCTION callback, SUnbound, SUnbound, FUNCTION::third_argument_type arg3)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type> result(new XAdaptor2xxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type> 		
Bind1(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, SUnbound)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type> result(new XAdaptor2xfx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg2), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> 		
Bind1(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, SUnbound)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> result(new XAdaptor2fxx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg1), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>	// 4 args
Bind1(FUNCTION callback, SUnbound, SUnbound, SUnbound, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> result(new XAdaptor3xxxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::fourth_argument_type>	
Bind1(FUNCTION callback, SUnbound, SUnbound, FUNCTION::third_argument_type arg3, SUnbound)
{	
	using namespace Internals;
	
	XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor3xxfx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>	
Bind1(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, SUnbound, SUnbound)
{	
	using namespace Internals;
	
	XCallback3<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor3xfxx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg2), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback3<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>	
Bind1(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, SUnbound, SUnbound)
{	
	using namespace Internals;
	
	XCallback3<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor3fxxx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1), 1, 2);
	
	return result;
}


// ===================================================================================
//	Bind2
// ===================================================================================
template <class FUNCTION>													// 2 arg
XCallback0<FUNCTION::result_type> 
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2)
{	
	using namespace Internals;
	
	XCallback0<FUNCTION::result_type> result(new XAdaptor0ff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type>(callback, arg1, arg2), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> 			// 3 args
Bind2(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> result(new XAdaptor1xff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg2, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> 		
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, FUNCTION::third_argument_type arg3)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> result(new XAdaptor1fxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg1, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::third_argument_type> 		
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, SUnbound)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::third_argument_type> result(new XAdaptor1ffx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg1, arg2), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type> 		// 4 args
Bind2(FUNCTION callback, SUnbound, SUnbound, FUNCTION::third_argument_type arg3, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type> result(new XAdaptor2xxff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg3, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type> 		
Bind2(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, SUnbound, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::third_argument_type> result(new XAdaptor2xfxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg2, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::fourth_argument_type> 		
Bind2(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3, SUnbound)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor2xffx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg2, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> 		
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, SUnbound, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type> result(new XAdaptor2fxxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::fourth_argument_type> 		
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, FUNCTION::third_argument_type arg3, SUnbound)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::second_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor2fxfx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback2<FUNCTION::result_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> 		
Bind2(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, SUnbound, SUnbound)
{	
	using namespace Internals;
	
	XCallback2<FUNCTION::result_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type> result(new XAdaptor2ffxx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg2), 1, 2);
	
	return result;
}


// ===================================================================================
//	Bind3
// ===================================================================================
template <class FUNCTION>													// 3 arg
XCallback0<FUNCTION::result_type> 
Bind3(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3)
{	
	using namespace Internals;
	
	XCallback0<FUNCTION::result_type> result(new XAdaptor0fff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type>(callback, arg1, arg2, arg3), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> 			// 4 args
Bind3(FUNCTION callback, SUnbound, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::first_argument_type> result(new XAdaptor1xfff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg2, arg3, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> 			
Bind3(FUNCTION callback, FUNCTION::first_argument_type arg1, SUnbound, FUNCTION::third_argument_type arg3, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::second_argument_type> result(new XAdaptor1fxff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg3, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::third_argument_type> 			
Bind3(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, SUnbound, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::third_argument_type> result(new XAdaptor1ffxf<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg2, arg4), 1, 2);
	
	return result;
}

template <class FUNCTION>
XCallback1<FUNCTION::result_type, FUNCTION::fourth_argument_type> 			
Bind3(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3, SUnbound)
{	
	using namespace Internals;
	
	XCallback1<FUNCTION::result_type, FUNCTION::fourth_argument_type> result(new XAdaptor1fffx<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg2, arg3), 1, 2);
	
	return result;
}


// ===================================================================================
//	Bind4
// ===================================================================================
template <class FUNCTION>													// 4 args
XCallback0<FUNCTION::result_type> 
Bind4(FUNCTION callback, FUNCTION::first_argument_type arg1, FUNCTION::second_argument_type arg2, FUNCTION::third_argument_type arg3, FUNCTION::fourth_argument_type arg4)
{	
	using namespace Internals;
	
	XCallback0<FUNCTION::result_type> result(new XAdaptor0ffff<FUNCTION, FUNCTION::result_type, FUNCTION::first_argument_type, FUNCTION::second_argument_type, FUNCTION::third_argument_type, FUNCTION::fourth_argument_type>(callback, arg1, arg2, arg3, arg4), 1, 2);
	
	return result;
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
