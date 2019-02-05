/*
 *  File:       ZCallbackHelpers.h
 *  Summary:	Helper classes used by the classes in XCallbacks.h
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XCallbackHelpers.h,v $
 *		Revision 1.4  2001/01/22 02:03:41  jesjones
 *		Fixed two of the Equal methods so they compile.
 *		
 *		Revision 1.3  2000/11/12 07:55:10  jesjones
 *		Renamed the callback adaptor functions Bind. Removed the Procedure callback adaptors.
 *		
 *		Revision 1.2  2000/11/12 07:39:53  jesjones
 *		Removed the procedure adaptors.
 *		
 *		Revision 1.1  2000/11/12 06:29:41  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.		
 */

#pragma once

#include <XReferenceCounted.h>

namespace Whisper {
namespace Internals {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	ReturnValue
// ===================================================================================
template <typename RETURN_TYPE>						// $$ this should allow us to get rid of the specializations, but I get "cannot convert void to void" errors with CW 5 (and internal compiler errors if these functions use const references)
struct ReturnValue {
	template <typename T>
	RETURN_TYPE Cast(T value)	{return value;}		// force a compile error if the function returns an incompatible type
};

template <>
struct ReturnValue<void> {
	template <typename T>
	void Cast(T)				{}					// for void callbacks we'll eat the return value
};



// ===================================================================================
//	Callback0
// ===================================================================================
template <typename RETURN_TYPE>
class XBaseCallback0 : public XReferenceCountedMixin { 

public:
	virtual 			~XBaseCallback0()			{}

						XBaseCallback0()			{}

private:
						XBaseCallback0(const XBaseCallback0& rhs);
						
			XBaseCallback0& operator=(const XBaseCallback0& rhs);

public:
	virtual RETURN_TYPE Call() const = 0;

	virtual bool 		Equal(const XBaseCallback0* rhs) const = 0;
};


template <typename FUNCTION, typename RETURN_TYPE>
class XCCallback0 : public XBaseCallback0<RETURN_TYPE> { 

public:
						XCCallback0(FUNCTION function)			{ASSERT(function != nil); mFunction = function;}
						
	virtual RETURN_TYPE Call() const 							{return mFunction();}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const {if (const XCCallback0<FUNCTION, RETURN_TYPE>* f = dynamic_cast<const XCCallback0<FUNCTION, RETURN_TYPE>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename RETURN_TYPE>
class XObjCallback0 : public XBaseCallback0<RETURN_TYPE> { 

public:
						XObjCallback0(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual RETURN_TYPE Call() const 									{return (mObject->*mMethod)();}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const {if (const XObjCallback0<OBJECT, METHOD, RETURN_TYPE>* f = dynamic_cast<const XObjCallback0<OBJECT, METHOD, RETURN_TYPE>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


template <typename FUNCTION>
class XCCallback0<FUNCTION, void> : public XBaseCallback0<void> { 

public:
						XCCallback0(FUNCTION function)			{ASSERT(function != nil); mFunction = function;}
						
	virtual void 		Call() const 							{mFunction();}
		
	virtual bool 		Equal(const XBaseCallback0<void>* rhs) const {if (const XCCallback0<FUNCTION, void>* f = dynamic_cast<const XCCallback0<FUNCTION, void>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD>
class XObjCallback0<OBJECT, METHOD, void> : public XBaseCallback0<void> { 

public:
						XObjCallback0(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual void 		Call() const 									{(mObject->*mMethod)();}
		
	virtual bool 		Equal(const XBaseCallback0<void>* rhs) const {if (const XObjCallback0<OBJECT, METHOD, void>* f = dynamic_cast<const XObjCallback0<OBJECT, METHOD, void>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


// ===================================================================================
//	Callback1
// ===================================================================================
template <typename RETURN_TYPE, typename ARG1>
class XBaseCallback1 : public XReferenceCountedMixin { 

public:
	virtual 			~XBaseCallback1()			{}

						XBaseCallback1()			{}

private:
						XBaseCallback1(const XBaseCallback1& rhs);
						
			XBaseCallback1& operator=(const XBaseCallback1& rhs);

public:
	virtual RETURN_TYPE Call(ARG1 arg1) const = 0;

	virtual bool 		Equal(const XBaseCallback1* rhs) const = 0;
};


template <typename FUNCTION, typename RETURN_TYPE, typename ARG1>
class XCCallback1 : public XBaseCallback1<RETURN_TYPE, ARG1> { 

public:
						XCCallback1(FUNCTION function)			{ASSERT(function != nil); mFunction = function;}
						
	virtual RETURN_TYPE Call(ARG1 arg1) const 					{return mFunction(arg1);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG1>* rhs) const 	{if (const XCCallback1<FUNCTION, RETURN_TYPE, ARG1>* f = dynamic_cast<const XCCallback1<FUNCTION, RETURN_TYPE, ARG1>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename RETURN_TYPE, typename ARG1>
class XObjCallback1 : public XBaseCallback1<RETURN_TYPE, ARG1> { 

public:
						XObjCallback1(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual RETURN_TYPE Call(ARG1 arg1) const 							{return (mObject->*mMethod)(arg1);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG1>* rhs) const {if (const XObjCallback1<OBJECT, METHOD, RETURN_TYPE, ARG1>* f = dynamic_cast<const XObjCallback1<OBJECT, METHOD, RETURN_TYPE, ARG1>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


template <typename FUNCTION, typename ARG1>
class XCCallback1<FUNCTION, void, ARG1> : public XBaseCallback1<void, ARG1> { 

public:
						XCCallback1(FUNCTION function)			{ASSERT(function != nil); mFunction = function;}
						
	virtual void 		Call(ARG1 arg1) const 					{mFunction(arg1);}
		
	virtual bool 		Equal(const XBaseCallback1<void, ARG1>* rhs) const 	{if (const XCCallback1<FUNCTION, void, ARG1>* f = dynamic_cast<const XCCallback1<FUNCTION, void, ARG1>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename ARG1>
class XObjCallback1<OBJECT, METHOD, void, ARG1> : public XBaseCallback1<void, ARG1> { 

public:
						XObjCallback1(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual void 		Call(ARG1 arg1) const 							{(mObject->*mMethod)(arg1);}
		
	virtual bool 		Equal(const XBaseCallback1<void, ARG1>* rhs) const {if (const XObjCallback1<OBJECT, METHOD, void, ARG1>* f = dynamic_cast<const XObjCallback1<OBJECT, METHOD, void, ARG1>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


// ===================================================================================
//	Callback2
// ===================================================================================
template <typename RETURN_TYPE, typename ARG1, typename ARG2>
class XBaseCallback2 : public XReferenceCountedMixin { 

public:
	virtual 			~XBaseCallback2()			{}

						XBaseCallback2()			{}

private:
						XBaseCallback2(const XBaseCallback2& rhs);
						
			XBaseCallback2& operator=(const XBaseCallback2& rhs);

public:
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2) const = 0;

	virtual bool 		Equal(const XBaseCallback2* rhs) const = 0;
};


template <typename FUNCTION, typename RETURN_TYPE, typename ARG1, typename ARG2>
class XCCallback2 : public XBaseCallback2<RETURN_TYPE, ARG1, ARG2> { 

public:
						XCCallback2(FUNCTION function)		{ASSERT(function != nil); mFunction = function;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2) const 	{return mFunction(arg1, arg2);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG2>* rhs) const {if (const XCCallback2<FUNCTION, RETURN_TYPE, ARG1, ARG2>* f = dynamic_cast<const XCCallback2<FUNCTION, RETURN_TYPE, ARG1, ARG2>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename RETURN_TYPE, typename ARG1, typename ARG2>
class XObjCallback2 : public XBaseCallback2<RETURN_TYPE, ARG1, ARG2> { 

public:
						XObjCallback2(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2) const 				{return (mObject->*mMethod)(arg1, arg2);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG2>* rhs) const {if (const XObjCallback2<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2>* f = dynamic_cast<const XObjCallback2<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


template <typename FUNCTION, typename ARG1, typename ARG2>
class XCCallback2<FUNCTION, void, ARG1, ARG2> : public XBaseCallback2<void, ARG1, ARG2> { 

public:
						XCCallback2(FUNCTION function)		{ASSERT(function != nil); mFunction = function;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2) const 	{mFunction(arg1, arg2);}
		
	virtual bool 		Equal(const XBaseCallback2<void, ARG1, ARG2>* rhs) const {if (const XCCallback2<FUNCTION, void, ARG1, ARG2>* f = dynamic_cast<const XCCallback2<FUNCTION, void, ARG1, ARG2>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename ARG1, typename ARG2>
class XObjCallback2<OBJECT, METHOD, void, ARG1, ARG2> : public XBaseCallback2<void, ARG1, ARG2> { 

public:
						XObjCallback2(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2) const 				{(mObject->*mMethod)(arg1, arg2);}
		
	virtual bool 		Equal(const XBaseCallback2<void, ARG1, ARG2>* rhs) const {if (const XObjCallback2<OBJECT, METHOD, void, ARG1, ARG2>* f = dynamic_cast<const XObjCallback2<OBJECT, METHOD, void, ARG1, ARG2>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


// ===================================================================================
//	Callback3
// ===================================================================================
template <typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3>
class XBaseCallback3 : public XReferenceCountedMixin { 

public:
	virtual 			~XBaseCallback3()			{}

						XBaseCallback3()			{}

private:
						XBaseCallback3(const XBaseCallback3& rhs);
						
			XBaseCallback3& operator=(const XBaseCallback3& rhs);

public:
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const = 0;

	virtual bool 		Equal(const XBaseCallback3* rhs) const = 0;
};


template <typename FUNCTION, typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3>
class XCCallback3 : public XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3> { 

public:
						XCCallback3(FUNCTION function)				{ASSERT(function != nil); mFunction = function;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const	{return mFunction(arg1, arg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3>* rhs) const 	{if (const XCCallback3<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XCCallback3<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3>
class XObjCallback3 : public XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3> { 

public:
						XObjCallback3(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const 	{return (mObject->*mMethod)(arg1, arg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3>* rhs) const {if (const XObjCallback3<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XObjCallback3<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


template <typename FUNCTION, typename ARG1, typename ARG2, typename ARG3>
class XCCallback3<FUNCTION, void, ARG1, ARG2, ARG3> : public XBaseCallback3<void, ARG1, ARG2, ARG3> { 

public:
						XCCallback3(FUNCTION function)				{ASSERT(function != nil); mFunction = function;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const	{mFunction(arg1, arg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback3<void, ARG1, ARG2, ARG3>* rhs) const 	{if (const XCCallback3<FUNCTION, void, ARG1, ARG2, ARG3>* f = dynamic_cast<const XCCallback3<FUNCTION, void, ARG1, ARG2, ARG3>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename ARG1, typename ARG2, typename ARG3>
class XObjCallback3<OBJECT, METHOD, void, ARG1, ARG2, ARG3> : public XBaseCallback3<void, ARG1, ARG2, ARG3> { 

public:
						XObjCallback3(OBJECT* object, METHOD method)	{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const 	{(mObject->*mMethod)(arg1, arg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback3<void, ARG1, ARG2, ARG3>* rhs) const {if (const XObjCallback3<OBJECT, METHOD, void, ARG1, ARG2, ARG3>* f = dynamic_cast<const XObjCallback3<OBJECT, METHOD, void, ARG1, ARG2, ARG3>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


// ===================================================================================
//	Callback4
// ===================================================================================
template <typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class XBaseCallback4 : public XReferenceCountedMixin { 

public:
	virtual 			~XBaseCallback4()			{}

						XBaseCallback4()			{}

private:
						XBaseCallback4(const XBaseCallback4& rhs);
						
			XBaseCallback4& operator=(const XBaseCallback4& rhs);

public:
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const = 0;

	virtual bool 		Equal(const XBaseCallback4* rhs) const = 0;
};


template <typename FUNCTION, typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class XCCallback4 : public XBaseCallback4<RETURN_TYPE, ARG1, ARG2, ARG3, ARG4> { 

public:
						XCCallback4(FUNCTION function)							{ASSERT(function != nil); mFunction = function;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const 	{return mFunction(arg1, arg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback4<RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* rhs) const {if (const XCCallback4<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XCCallback4<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename RETURN_TYPE, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class XObjCallback4 : public XBaseCallback4<RETURN_TYPE, ARG1, ARG2, ARG3, ARG4> { 

public:
						XObjCallback4(OBJECT* object, METHOD method)			{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const 	{return (mObject->*mMethod)(arg1, arg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback4<RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* rhs) const {if (const XObjCallback4<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XObjCallback4<OBJECT, METHOD, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


template <typename FUNCTION, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class XCCallback4<FUNCTION, void, ARG1, ARG2, ARG3, ARG4> : public XBaseCallback4<void, ARG1, ARG2, ARG3, ARG4> { 

public:
						XCCallback4(FUNCTION function)							{ASSERT(function != nil); mFunction = function;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const 	{mFunction(arg1, arg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback4<void, ARG1, ARG2, ARG3, ARG4>* rhs) const {if (const XCCallback4<FUNCTION, void, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XCCallback4<FUNCTION, void, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mFunction == f->mFunction; else return false;}
		
private:
	FUNCTION	mFunction;
};


template <typename OBJECT, typename METHOD, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
class XObjCallback4<OBJECT, METHOD, void, ARG1, ARG2, ARG3, ARG4> : public XBaseCallback4<void, ARG1, ARG2, ARG3, ARG4> { 

public:
						XObjCallback4(OBJECT* object, METHOD method)			{ASSERT(object != nil); ASSERT(method != nil); mObject = object; mMethod = method;}
						
	virtual void 		Call(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const 	{(mObject->*mMethod)(arg1, arg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback4<void, ARG1, ARG2, ARG3, ARG4>* rhs) const {if (const XObjCallback4<OBJECT, METHOD, void, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XObjCallback4<OBJECT, METHOD, void, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mObject == f->mObject && mMethod == f->mMethod; else return false;}
		
private:
	OBJECT*	mObject;
	METHOD 	mMethod;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Internals
}	// namespace Whisper
