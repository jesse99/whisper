/*
 *  File:       XAdaptors3.h
 *  Summary:	Classes used to convert 4 argument callbacks to 3 argument callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XAdaptors3.h,v $
 *		Revision 1.1  2000/11/12 06:29:30  jesjones
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
//	XAdaptor3xxxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor3xxxf : public XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3> { 

public:
						XAdaptor3xxxf(FUNCTION callback, ARG4 arg4) : mCallback(callback), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG3 arg3) const 						{return mCallback(arg1, arg2, arg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG3>* rhs) const	{if (const XAdaptor3xxxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor3xxxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor3xxfx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor3xxfx : public XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG4> { 

public:
						XAdaptor3xxfx(FUNCTION callback, ARG3 arg3) : mCallback(callback), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2, ARG4 arg4) const 						{return mCallback(arg1, arg2, mArg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG1, ARG2, ARG4>* rhs) const	{if (const XAdaptor3xxfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor3xxfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor3xfxx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor3xfxx : public XBaseCallback3<RETURN_TYPE, ARG1, ARG3, ARG4> { 

public:
						XAdaptor3xfxx(FUNCTION callback, ARG2 arg2) : mCallback(callback), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG3 arg3, ARG4 arg4) const 						{return mCallback(arg1, mArg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG1, ARG3, ARG4>* rhs) const	{if (const XAdaptor3xfxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor3xfxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
};


// ===================================================================================
//	XAdaptor3fxxx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor3fxxx : public XBaseCallback3<RETURN_TYPE, ARG2, ARG3, ARG4> { 

public:
						XAdaptor3fxxx(FUNCTION callback, ARG1 arg1) : mCallback(callback), mArg1(arg1)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2, ARG3 arg3, ARG4 arg4) const 						{return mCallback(mArg1, arg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback3<RETURN_TYPE, ARG2, ARG3, ARG4>* rhs) const	{if (const XAdaptor3fxxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor3fxxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Internals
}	// namespace Whisper
