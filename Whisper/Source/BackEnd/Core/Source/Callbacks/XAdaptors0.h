/*
 *  File:       XAdaptors0.h
 *  Summary:	Classes used to convert 1, 2, 3, and 4 argument callbacks to 0 argument callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XAdaptors0.h,v $
 *		Revision 1.1  2000/11/12 06:29:17  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		
 */

#pragma once

#include <XCallbackHelpers.h>

namespace Whisper {
namespace Internals {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	XAdaptor0f	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1>
class XAdaptor0f : public XBaseCallback0<RETURN_TYPE> { 

public:
						XAdaptor0f(FUNCTION callback, ARG1 arg1) : mCallback(callback), mArg1(arg1)	{}
						
	virtual RETURN_TYPE Call() const 										{return mCallback(mArg1);}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const 	{if (const XAdaptor0f<FUNCTION, RETURN_TYPE, ARG1>* f = dynamic_cast<const XAdaptor0f<FUNCTION, RETURN_TYPE, ARG1>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
};


// ===================================================================================
//	XAdaptor0ff	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2>
class XAdaptor0ff : public XBaseCallback0<RETURN_TYPE> { 

public:
						XAdaptor0ff(FUNCTION callback, ARG1 arg1, ARG2 arg2) : mCallback(callback), mArg1(arg1), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call() const 										{return mCallback(mArg1, mArg2);}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const 	{if (const XAdaptor0ff<FUNCTION, RETURN_TYPE, ARG1, ARG2>* f = dynamic_cast<const XAdaptor0ff<FUNCTION, RETURN_TYPE, ARG1, ARG2>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG2		mArg2;
};


// ===================================================================================
//	XAdaptor0fff	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor0fff : public XBaseCallback0<RETURN_TYPE> { 

public:
						XAdaptor0fff(FUNCTION callback, ARG1 arg1, ARG2 arg2, ARG3 arg3) : mCallback(callback), mArg1(arg1), mArg2(arg2), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call() const 										{return mCallback(mArg1, mArg2, mArg3);}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const 	{if (const XAdaptor0fff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor0fff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG2		mArg2;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor0ffff	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor0ffff : public XBaseCallback0<RETURN_TYPE> { 

public:
						XAdaptor0ffff(FUNCTION callback, ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) : mCallback(callback), mArg1(arg1), mArg2(arg2), mArg3(arg3), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call() const 										{return mCallback(mArg1, mArg2, mArg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback0<RETURN_TYPE>* rhs) const 	{if (const XAdaptor0ffff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor0ffff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2 && mArg3 == f->mArg3 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG2		mArg2;
	ARG3		mArg3;
	ARG4		mArg4;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Internals
}	// namespace Whisper
