/*
 *  File:       XAdaptors1.h
 *  Summary:	Classes used to convert 2, 3, and 4 argument callbacks to 1 argument callbacks.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XAdaptors1.h,v $
 *		Revision 1.1  2000/11/12 06:29:21  jesjones
 *		Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		
 *		Revision 1.2  2000/11/10 04:48:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XAdaptors1.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.1  2000/11/12 06:29:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Switched to using Bind functions instead of Adapt functions. Places the internal classes into an Internals namespace. Deleted some obsolete files.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 2/22/00	JDJ		Created
 */

#pragma once

#include <XCallbackHelpers.h>

namespace Whisper {
namespace Internals {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	XAdaptor1xf	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2>
class XAdaptor1xf : public XBaseCallback1<RETURN_TYPE, ARG1> { 

public:
						XAdaptor1xf(FUNCTION callback, ARG2 arg2) : mCallback(callback), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1) const 									{return mCallback(arg1, mArg2);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG1>* rhs) const 	{if (const XAdaptor1xf<FUNCTION, RETURN_TYPE, ARG1, ARG2>* f = dynamic_cast<const XAdaptor1xf<FUNCTION, RETURN_TYPE, ARG1, ARG2>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
};


// ===================================================================================
//	XAdaptor1fx	
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2>
class XAdaptor1fx : public XBaseCallback1<RETURN_TYPE, ARG2> { 

public:
						XAdaptor1fx(FUNCTION callback, ARG1 arg1) : mCallback(callback), mArg1(arg1)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2) const 									{return mCallback(mArg1, arg2);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG2>* rhs) const 	{if (const XAdaptor1fx<FUNCTION, RETURN_TYPE, ARG1, ARG2>* f = dynamic_cast<const XAdaptor1fx<FUNCTION, RETURN_TYPE, ARG1, ARG2>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
};


// ===================================================================================
//	XAdaptor1xff
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor1xff : public XBaseCallback1<RETURN_TYPE, ARG1> { 

public:
						XAdaptor1xff(FUNCTION callback, ARG2 arg2, ARG3 arg3) : mCallback(callback), mArg2(arg2), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1) const 									{return mCallback(arg1, mArg2, mArg3);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG1>* rhs) const 	{if (const XAdaptor1xff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor1xff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor1fxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor1fxf : public XBaseCallback1<RETURN_TYPE, ARG2> { 

public:
						XAdaptor1fxf(FUNCTION callback, ARG1 arg1, ARG3 arg3) : mCallback(callback), mArg1(arg1), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2) const 									{return mCallback(mArg1, arg2, mArg3);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG2>* rhs) const 	{if (const XAdaptor1fxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor1fxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG3		mArg3;
};


// ===================================================================================
//	XAdaptor1ffx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3>
class XAdaptor1ffx : public XBaseCallback1<RETURN_TYPE, ARG3> { 

public:
						XAdaptor1ffx(FUNCTION callback, ARG1 arg1, ARG2 arg2) : mCallback(callback), mArg1(arg1), mArg2(arg2)	{}
						
	virtual RETURN_TYPE Call(ARG3 arg3) const 									{return mCallback(mArg1, mArg2, arg3);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG3>* rhs) const 	{if (const XAdaptor1ffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>* f = dynamic_cast<const XAdaptor1ffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG2		mArg2;
};


// ===================================================================================
//	XAdaptor1xfff
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor1xfff : public XBaseCallback1<RETURN_TYPE, ARG1> { 

public:
						XAdaptor1xfff(FUNCTION callback, ARG2 arg2, ARG3 arg3, ARG4 arg4) : mCallback(callback), mArg2(arg2), mArg3(arg3), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG1 arg1) const 									{return mCallback(arg1, mArg2, mArg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG1>* rhs) const 	{if (const XAdaptor1xfff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor1xfff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg2 == f->mArg2 && mArg3 == f->mArg3 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG2		mArg2;
	ARG3		mArg3;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor1fxff
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor1fxff : public XBaseCallback1<RETURN_TYPE, ARG2> { 

public:
						XAdaptor1fxff(FUNCTION callback, ARG1 arg1, ARG3 arg3, ARG4 arg4) : mCallback(callback), mArg1(arg1), mArg3(arg3), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG2 arg2) const 									{return mCallback(mArg1, arg2, mArg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG2>* rhs) const 	{if (const XAdaptor1fxff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor1fxff<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg3 == f->mArg3 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG3		mArg3;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor1ffxf
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor1ffxf : public XBaseCallback1<RETURN_TYPE, ARG3> { 

public:
						XAdaptor1ffxf(FUNCTION callback, ARG1 arg1, ARG2 arg2, ARG4 arg4) : mCallback(callback), mArg1(arg1), mArg2(arg2), mArg4(arg4)	{}
						
	virtual RETURN_TYPE Call(ARG3 arg3) const 									{return mCallback(mArg1, mArg2, arg3, mArg4);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG3>* rhs) const 	{if (const XAdaptor1ffxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor1ffxf<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2 && mArg4 == f->mArg4; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG3		mArg2;
	ARG4		mArg4;
};


// ===================================================================================
//	XAdaptor1fffx
// ===================================================================================
template <class FUNCTION, class RETURN_TYPE, class ARG1, class ARG2, class ARG3, class ARG4>
class XAdaptor1fffx : public XBaseCallback1<RETURN_TYPE, ARG4> { 

public:
						XAdaptor1fffx(FUNCTION callback, ARG1 arg1, ARG2 arg2, ARG3 arg3) : mCallback(callback), mArg1(arg1), mArg2(arg2), mArg3(arg3)	{}
						
	virtual RETURN_TYPE Call(ARG4 arg4) const 									{return mCallback(mArg1, mArg2, mArg3, arg4);}
		
	virtual bool 		Equal(const XBaseCallback1<RETURN_TYPE, ARG4>* rhs) const 	{if (const XAdaptor1fffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>* f = dynamic_cast<const XAdaptor1fffx<FUNCTION, RETURN_TYPE, ARG1, ARG2, ARG3, ARG4>*>(rhs)) return mCallback == f->mCallback && mArg1 == f->mArg1 && mArg2 == f->mArg2 && mArg3 == f->mArg3; else return false;}
		
private:
	FUNCTION	mCallback;
	ARG1		mArg1;
	ARG3		mArg2;
	ARG3		mArg3;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Internals
}	// namespace Whisper
