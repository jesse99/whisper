/*
 *  File:       IRange.h
 *  Summary:   	Template interface for an inclusive range of values.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IRange.h,v $
 *		Revision 1.2  2001/03/23 10:56:32  jesjones
 *		Added XRange.
 *		
 *		Revision 1.1  2000/12/19 07:44:15  jesjones
 *		Checked in
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IRange
//!		Template interface for an inclusive range of values.
// ===================================================================================
template <typename T>
class IRange : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual T 			GetMin() const = 0;
	virtual void 		SetMin(T value) = 0;

	virtual T 			GetMax() const = 0;	
	virtual void 		SetMax(T value) = 0;
};


// ===================================================================================
//	class XRange
//!		Default IRange implementation.
// ===================================================================================
template <typename T>
class XRange : public IRange<T> {

//-----------------------------------
//	Initialization/Destruction
//
public:	
						XRange(XBoss* boss)		{IRange<T>::DoSetBoss(boss);}
						
//-----------------------------------
//	Inherited API
//
public:
	virtual T 			GetMin() const 			{return mMin;}
	virtual void 		SetMin(T value) 		{mMin = value;}

	virtual T 			GetMax() const 			{return mMax;}
	virtual void 		SetMax(T value) 		{mMax = value;}
	
//-----------------------------------
//	Member Data
//
private:
	T		mMin;
	T		mMax;
};


typedef XInterfacePtr<IRange<int32> > IInt32RangePtr;
typedef XInterfacePtr<const IRange<int32> > IConstInt32RangePtr;

typedef XInterfacePtr<IRange<uint32> > IUInt32RangePtr;
typedef XInterfacePtr<const IRange<uint32> > IConstUInt32RangePtr;

typedef XInterfacePtr<IRange<float> > IFloatRangePtr;
typedef XInterfacePtr<const IRange<float> > IConstFloatRangePtr;

typedef XInterfacePtr<IRange<double> > IDoubleRangePtr;
typedef XInterfacePtr<const IRange<double> > IConstDoubleRangePtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
