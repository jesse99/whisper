/*
 *  File:       XAdaptors2.h
 *  Summary:	Classes used to convert 3 and 4 argument callbacks to 2 argument callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XAdaptors2.h,v $
 *		Revision 1.1  2000/11/12 06:29:26  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		
 *		Revision 1.2  2000/11/10 04:48:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XAdaptors2.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2000/11/12 06:29:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 2/23/00	JDJ		Created
 */

#pragma once

#include <XCallbackHelpers.h>

namespace Whisper {
namespace Internals {


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	XAdaptor2xxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor2xxf : public XBaseCallback2<RETURN_TYPE, ARG1, ARG2> { 

public:
						XAdaptor2xxf(FUNCTION callback, ARG3 arg3) : mCallback(callback), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2) const 							{return mCallback(arg1, arg2, mArg3);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG2>* rhs) const	{if (const XAdaptor2xxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor2xxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor2xfx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor2xfx : public XBaseCallback2<RETURN_TYPE, ARG1, ARG3> { 

public:
						XAdaptor2xfx(FUNCTION callback, ARG2 arg2) : mCallback(callback), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG3 arg3) const 							{return mCallback(arg1, mArg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG3>* rhs) const	{if (const XAdaptor2xfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor2xfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
};


// ===================================================================================
//	XAdaptor2fxx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor2fxx : public XBaseCallback2<RETURN_TYPE, ARG2, ARG3> { 

public:
						XAdaptor2fxx(FUNCTION callback, ARG1 arg1) : mCallback(callback), mArg1(arg1)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2, ARG3 arg3) const 							{return mCallback(mArg1, arg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG2, ARG3>* rhs) const	{if (const XAdaptor2fxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor2fxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
};


// ===================================================================================
//	XAdaptor2xxff
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2xxff : public XBaseCallback2<RETURN_TYPE, ARG1, ARG2> { 

public:
						XAdaptor2xxff(FUNCTION callback, ARG3 arg3, ARG4 arg4) : mCallback(callback), mArg3(arg3), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG2 arg2) const 							{return mCallback(arg1, arg2, mArg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG2>* rhs) const	{if (const XAdaptor2xxff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2xxff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg3 == f->mArg3 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG3		mArg3;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor2xfxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2xfxf : public XBaseCallback2<RETURN_TYPE, ARG1, ARG3> { 

public:
						XAdaptor2xfxf(FUNCTION callback, ARG2 arg2, ARG4 arg4) : mCallback(callback), mArg2(arg2), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG3 arg3) const 							{return mCallback(arg1, mArg2, arg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG3>* rhs) const	{if (const XAdaptor2xfxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2xfxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor2xffx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2xffx : public XBaseCallback2<RETURN_TYPE, ARG1, ARG4> { 

public:
						XAdaptor2xffx(FUNCTION callback, ARG2 arg2, ARG3 arg3) : mCallback(callback), mArg2(arg2), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1, ARG4 arg4) const 							{return mCallback(arg1, mArg2, mArg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG1, ARG4>* rhs) const	{if (const XAdaptor2xffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2xffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor2fxxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2fxxf : public XBaseCallback2<RETURN_TYPE, ARG2, ARG3> { 

public:
						XAdaptor2fxxf(FUNCTION callback, ARG1 arg1, ARG4 arg4) : mCallback(callback), mArg1(arg1), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2, ARG3 arg3) const 							{return mCallback(mArg1, arg2, arg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG2, ARG3>* rhs) const	{if (const XAdaptor2fxxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2fxxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor2fxfx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2fxfx : public XBaseCallback2<RETURN_TYPE, ARG2, ARG4> { 

public:
						XAdaptor2fxfx(FUNCTION callback, ARG1 arg1, ARG3 arg3) : mCallback(callback), mArg1(arg1), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2, ARG4 arg4) const 							{return mCallback(mArg1, arg2, mArg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG2, ARG4>* rhs) const	{if (const XAdaptor2fxfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2fxfx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor2ffxx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor2ffxx : public XBaseCallback2<RETURN_TYPE, ARG3, ARG4> { 

public:
						XAdaptor2ffxx(FUNCTION callback, ARG1 arg1, ARG2 arg2) : mCallback(callback), mArg1(arg1), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call(ARG3 arg3, ARG4 arg4) const 							{return mCallback(mArg1, mArg2, arg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback2<RETURN_TYPE, ARG3, ARG4>* rhs) const	{if (const XAdaptor2ffxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor2ffxx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG2		mArg2;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Internals
}	// namespace Whisper
