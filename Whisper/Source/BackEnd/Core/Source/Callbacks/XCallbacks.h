/*
 *  File:       XCallbacks.h
 *  Summary:	Light-weight wrappers around a pointer to a function or method.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XCallbacks.h,v $
 *		Revision 1.7  2001/10/03 02:09:55  jesjones
 *		Callbacks no longer accept functors (Pro 7 bug).
 *		
 *		Revision 1.6  2001/03/28 09:45:16  jesjones
 *		Added Set methods.
 *		
 *		Revision 1.5  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.4  2000/11/20 05:29:44  jesjones
 *		Switched back to using IsValid.
 *		
 *		Revision 1.3  2000/11/12 06:29:50  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		
 *		Revision 1.2  2000/11/09 12:27:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbackHelpers.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XCallback0
//!		Callback class for functions with no arguments/
/*!		This is a reimplementation of Rich Hickey's callback library. There are two 
 *		major differences. The first is that I'm using templatized ctors so makeFunctor 
 *		like functions are no longer neccesary. The second is that I don't stuff pointers 
 *		into a non-template base class. Instead callbacks are executed with a single 
 *		virtual function call. This eliminates a bunch of nasty casts, removes some 
 *		platform specific code for variable sized member function pointers, and IMO 
 *		results in more readable code.
 *
 *		Note that the standard C++ library also provides wrappers around member function 
 *		pointers (eg mem_fun_t). However these classes are insufficiently general: there's 
 *		no way to hide the type of the object pointer which means that mem_fun_t is only 
 *		useful from within template code.
 *
 *		The callback classes are very easy to use. For example, XTextBoxControl defines 
 *		three callbacks: 
 *			- typedef XCallback1<void, XTextBoxControl*> 				 ChangeCallback;	// void Callback(XTextBoxControl*)
 *			- typedef XCallback2<void, XTextBoxControl*, std::wstring>   RejectCallback;	// void Callback(XTextBoxControl*, std::wstring)
 *			- typedef XCallback3<bool, XTextBoxControl*, int16, wchar_t> PassUpCallback;	// bool Callback(XTextBoxControl*, int16, wchar_t)
 *
 *		To wire one of these up you can use code that looks like this: \code
 *			XTextBoxControl::RejectCallback callback1(HandleRejectedText);				// call a global function or a static method
 *			textbox->SetRejectedCallback(callback1);
 *
 *			XTextBoxControl::ChangeCallback callback2(this, &CMyObject::OnTextChanged);	// call a CMyObject method
 *			textbox->SetChangedCallback(callback2); \endcode
 *
 *		When XTextBoxControl executes a callback it uses the normal function pointer syntax: \code
 *			bool passUp = mPassUpCallback(this, key, ch); \endcode
 *
 *		Note that the function the callback is attached to need not be an exact match for 
 *		the callback template arguments. As long as the template arguments are convertable 
 *		to the function arguments and the function return type is convertable to the 
 *		template return type things will work fine. If these are not convertable you'll 
 *		get a compile error. */
// ===================================================================================
template <class RETURN_TYPE>
class XCallback0 {

//-----------------------------------
//	Types
//
public:
	typedef RETURN_TYPE result_type;
	typedef Internals::XBaseCallback0<RETURN_TYPE> Helper;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XCallback0()							{if (mCallback != nil) mCallback->RemoveReference();}
						
						XCallback0() : mCallback(nil)			{}
	
#if __MWERKS__ == 0x2405		// $$$ Pro 7 messes up copy ctors when a template method is present
						template <typename R>
						XCallback0(R (*function)()) : mCallback(new Internals::XCCallback0<R (*)(), RETURN_TYPE>(function))	{}
#else
						template <class FUNCTION>
						XCallback0(FUNCTION function) : mCallback(new Internals::XCCallback0<FUNCTION, RETURN_TYPE>(function))	{}
#endif

						template <class OBJECT, class METHOD>
						XCallback0(OBJECT* object, METHOD method) : mCallback(new Internals::XObjCallback0<OBJECT, METHOD, RETURN_TYPE>(object, method))	{}
						
						XCallback0(Helper* callback, int, int) : mCallback(callback)	{ASSERT(callback != nil);}
	
						XCallback0(const XCallback0& rhs)		{if (rhs.mCallback != nil) rhs.mCallback->AddReference(); mCallback = rhs.mCallback;}
						
			XCallback0& operator=(const XCallback0& rhs) 		{if (mCallback != rhs.mCallback) {if (rhs.mCallback != nil) rhs.mCallback->AddReference(); if (mCallback != nil) mCallback->RemoveReference(); mCallback = rhs.mCallback;} return *this;}

						template <class OBJECT, class METHOD>
			void 		Set(OBJECT* object, METHOD method) 		{(void) this->operator=(XCallback0(object, method));}

//-----------------------------------
//	API
//
public:
			RETURN_TYPE operator()() const						{ASSERT(mCallback != nil); return mCallback->Call();}

			bool 		IsValid() const							{return mCallback != nil;}
			
			bool 		operator==(const XCallback0& rhs) const	{return (mCallback == nil && rhs.mCallback == nil) || (mCallback != nil && mCallback->Equal(rhs.mCallback));}
			bool 		operator!=(const XCallback0& rhs) const	{return !this->operator==(rhs);}
			bool 		operator<(const XCallback0& rhs) const	{return mCallback < rhs.mCallback;}

//-----------------------------------
//	Member Data
//
private:
	Helper*	mCallback;
};


// ===================================================================================
//	class XCallback1
//!		Callback class for functions with one argument.
// ===================================================================================
template <class RETURN_TYPE, class ARG1>
class XCallback1 {

//-----------------------------------
//	Types
//
public:
	typedef RETURN_TYPE result_type;
	typedef ARG1 		argument_type;
	typedef Internals::XBaseCallback1<RETURN_TYPE, ARG1> Helper;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XCallback1()							{if (mCallback != nil) mCallback->RemoveReference();}
						
						XCallback1() : mCallback(nil)			{}

#if __MWERKS__ == 0x2405		// $$$ Pro 7 messes up copy ctors when a template method is present
						template <typename R, typename A1>
						XCallback1(R (*function)(A1)) : mCallback(new Internals::XCCallback1<R (*)(A1), RETURN_TYPE, ARG1>(function))	{}
#else
						template <class FUNCTION>
						XCallback1(FUNCTION function) : mCallback(new Internals::XCCallback1<FUNCTION, RETURN_TYPE, ARG1>(function))	{}
#endif

						template <class OBJECT, class METHOD>
						XCallback1(OBJECT* object, METHOD method) : mCallback(new Internals::XObjCallback1<OBJECT, METHOD, RETURN_TYPE, ARG1>(object, method))	{}
						
						XCallback1(Helper* callback, int, int) : mCallback(callback)	{ASSERT(callback != nil);}
	
						XCallback1(const XCallback1& rhs)		{if (rhs.mCallback != nil) rhs.mCallback->AddReference(); mCallback = rhs.mCallback;}
						
			XCallback1& operator=(const XCallback1& rhs) 		{if (mCallback != rhs.mCallback) {if (rhs.mCallback != nil) rhs.mCallback->AddReference(); if (mCallback != nil) mCallback->RemoveReference(); mCallback = rhs.mCallback;} return *this;}

						template <class OBJECT, class METHOD>
			void 		Set(OBJECT* object, METHOD method) 		{(void) this->operator=(XCallback1(object, method));}

//-----------------------------------
//	API
//
public:
			RETURN_TYPE operator()(ARG1 arg1) const				{ASSERT(mCallback != nil); return mCallback->Call(arg1);}

			bool 		IsValid() const							{return mCallback != nil;}
			
			bool 		operator==(const XCallback1& rhs) const	{return (mCallback == nil && rhs.mCallback == nil) || (mCallback != nil && mCallback->Equal(rhs.mCallback));}
			bool 		operator!=(const XCallback1& rhs) const	{return !this->operator==(rhs);}
			bool 		operator<(const XCallback1& rhs) const	{return mCallback < rhs.mCallback;}

//-----------------------------------
//	Member Data
//
private:
	Helper*	mCallback;
};


// ===================================================================================
//	class XCallback2
//!		Callback class for functions with two arguments.
// ===================================================================================
template <class RETURN_TYPE, class ARG1, class ARG2>
class XCallback2 {

//-----------------------------------
//	Types
//
public:
	typedef RETURN_TYPE result_type;
	typedef ARG1 		first_argument_type;
	typedef ARG2 		second_argument_type;
	typedef Internals::XBaseCallback2<RETURN_TYPE, ARG1, ARG2> Helper;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XCallback2()							{if (mCallback != nil) mCallback->RemoveReference();}
						
						XCallback2() : mCallback(nil)			{}
	
#if __MWERKS__ == 0x2405		// $$$ Pro 7 messes up copy ctors when a template method is present
						template <typename R, typename A1, typename A2>
						XCallback2(R (*function)(A1, A2)) : mCallback(new Internals::XCCallback2<R (*)(A1, A2), RETURN_TYPE, ARG1, ARG2>(function))	{}
#else
						template <class FUNCTION>
						XCallback2(FUNCTION function) : mCallback(new Internals::XCCallback2<FUNCTION, RETURN_TYPE, ARG1, ARG2>(function))	{}
#endif

						template <class OBJECT, class METHOD>
						XCallback2(OBJECT* object, METHOD method) : mCallback(new Internals::XObjCallback2<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2>(object, method))	{}
						
						XCallback2(Helper* callback, int, int) : mCallback(callback)	{ASSERT(callback != nil);}
	
						XCallback2(const XCallback2& rhs)		{if (rhs.mCallback != nil) rhs.mCallback->AddReference(); mCallback = rhs.mCallback;}
						
			XCallback2& operator=(const XCallback2& rhs) 		{if (mCallback != rhs.mCallback) {if (rhs.mCallback != nil) rhs.mCallback->AddReference(); if (mCallback != nil) mCallback->RemoveReference(); mCallback = rhs.mCallback;} return *this;}

						template <class OBJECT, class METHOD>
			void 		Set(OBJECT* object, METHOD method) 		{(void) this->operator=(XCallback2(object, method));}

//-----------------------------------
//	API
//
public:
			RETURN_TYPE operator()(ARG1 arg1, ARG2 arg2) const	{ASSERT(mCallback != nil); return mCallback->Call(arg1, arg2);}

			bool 		IsValid() const							{return mCallback != nil;}

			bool 		operator==(const XCallback2& rhs) const	{return (mCallback == nil && rhs.mCallback == nil) || (mCallback != nil && mCallback->Equal(rhs.mCallback));}
			bool 		operator!=(const XCallback2& rhs) const	{return !this->operator==(rhs);}
			bool 		operator<(const XCallback2& rhs) const	{return mCallback < rhs.mCallback;}

//-----------------------------------
//	Member Data
//
private:
	Helper*	mCallback;
};


// ===================================================================================
//	class XCallback3
//!		Callback class for functions with three arguments.
// ===================================================================================
template <class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XCallback3 {

//-----------------------------------
//	Types
//
public:
	typedef RETURN_TYPE result_type;
	typedef ARG1 		first_argument_type;
	typedef ARG2 		second_argument_type;
	typedef ARG3 		third_argument_type;
	typedef Internals::XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3> Helper;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XCallback3()							{if (mCallback != nil) mCallback->RemoveReference();}
						
						XCallback3() : mCallback(nil)			{}
	
#if __MWERKS__ == 0x2405		// $$$ Pro 7 messes up copy ctors when a template method is present
						template <typename R, typename A1, typename A2, typename A3>
						XCallback3(R (*function)(A1, A2, A3)) : mCallback(new Internals::XCCallback3<R (*)(A1, A2, A3), RETURN_TYPE, ARG1, ARG2, ARG3>(function))	{}
#else
						template <class FUNCTION>
						XCallback3(FUNCTION function) : mCallback(new Internals::XCCallback3<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>(function))	{}
#endif

						template <class OBJECT, class METHOD>
						XCallback3(OBJECT* object, METHOD method) : mCallback(new Internals::XObjCallback3<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3>(object, method))	{}
						
						XCallback3(Helper* callback, int, int) : mCallback(callback)	{ASSERT(callback != nil);}
	
						XCallback3(const XCallback3& rhs)		{if (rhs.mCallback != nil) rhs.mCallback->AddReference(); mCallback = rhs.mCallback;}
						
			XCallback3& operator=(const XCallback3& rhs) 		{if (mCallback != rhs.mCallback) {if (rhs.mCallback != nil) rhs.mCallback->AddReference(); if (mCallback != nil) mCallback->RemoveReference(); mCallback = rhs.mCallback;} return *this;}

						template <class OBJECT, class METHOD>
			void 		Set(OBJECT* object, METHOD method) 		{(void) this->operator=(XCallback3(object, method));}

//-----------------------------------
//	API
//
public:
			RETURN_TYPE operator()(ARG1 arg1, ARG2 arg2, ARG3 arg3) const		{ASSERT(mCallback != nil); return mCallback->Call(arg1, arg2, arg3);}

			bool 		IsValid() const							{return mCallback != nil;}

			bool 		operator==(const XCallback3& rhs) const	{return (mCallback == nil && rhs.mCallback == nil) || (mCallback != nil && mCallback->Equal(rhs.mCallback));}
			bool 		operator!=(const XCallback3& rhs) const	{return !this->operator==(rhs);}
			bool 		operator<(const XCallback3& rhs) const	{return mCallback < rhs.mCallback;}

//-----------------------------------
//	Member Data
//
private:
	Helper*	mCallback;
};


// ===================================================================================
//	class XCallback4
//!		Callback class for functions with four arguments.
// ===================================================================================
template <class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XCallback4 {

//-----------------------------------
//	Types
//
public:
	typedef RETURN_TYPE result_type;
	typedef ARG1 		first_argument_type;
	typedef ARG2 		second_argument_type;
	typedef ARG3 		third_argument_type;
	typedef ARG4		fourth_argument_type;
	typedef Internals::XBaseCallback4<RETURN_TYPE, ARG1, ARG2, ARG3, ARG4> Helper;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XCallback4()							{if (mCallback != nil) mCallback->RemoveReference();}
						
						XCallback4() : mCallback(nil)			{}
	
#if __MWERKS__ == 0x2405		// $$$ Pro 7 messes up copy ctors when a template method is present
						template <typename R, typename A1, typename A2, typename A3, typename A4>
						XCallback4(R (*function)(A1, A2, A3, A4)) : mCallback(new Internals::XCCallback4<R (*)(A1, A2, A3, A4), RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>(function))	{}
#else
						template <class FUNCTION>
						XCallback4(FUNCTION function) : mCallback(new Internals::XCCallback4<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>(function))	{}
#endif

						template <class OBJECT, class METHOD>
						XCallback4(OBJECT* object, METHOD method) : mCallback(new Internals::XObjCallback4<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>(object, method))	{}
						
						XCallback4(Helper* callback, int, int) : mCallback(callback)	{ASSERT(callback != nil);}
	
						XCallback4(const XCallback4& rhs)		{if (rhs.mCallback != nil) rhs.mCallback->AddReference(); mCallback = rhs.mCallback;}
						
			XCallback4& operator=(const XCallback4& rhs) 		{if (mCallback != rhs.mCallback) {if (rhs.mCallback != nil) rhs.mCallback->AddReference(); if (mCallback != nil) mCallback->RemoveReference(); mCallback = rhs.mCallback;} return *this;}
						template <class OBJECT, class METHOD>
			void 		Set(OBJECT* object, METHOD method) 		{(void) this->operator=(XCallback4(object, method));}


//-----------------------------------
//	API
//
public:
			RETURN_TYPE operator()(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const	{ASSERT(mCallback != nil); return mCallback->Call(arg1, arg2, arg3, arg4);}

			bool 		IsValid() const							{return mCallback != nil;}

			bool 		operator==(const XCallback4& rhs) const	{return (mCallback == nil && rhs.mCallback == nil) || (mCallback != nil && mCallback->Equal(rhs.mCallback));}
			bool 		operator!=(const XCallback4& rhs) const	{return !this->operator==(rhs);}
			bool 		operator<(const XCallback4& rhs) const	{return mCallback < rhs.mCallback;}

//-----------------------------------
//	Member Data
//
private:
	Helper*	mCallback;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
