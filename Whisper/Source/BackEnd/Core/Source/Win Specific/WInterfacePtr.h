/*
 *  File:       WInterfacePtr.h
 *  Summary:    Smart pointer that wraps a COM interface pointer.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WInterfacePtr.h,v $
 *		Revision 1.5  2001/03/17 07:42:12  jesjones
 *		Moved the template specializations into the cpp.
 *		
 *		Revision 1.4  2001/03/05 05:40:11  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:46  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:09:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <unknwn.h>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class WInterfacePtr
//!		Smart pointer that wraps a COM interface pointer.
// ===================================================================================
template<class INTERFACE, const IID* ID> class WInterfacePtr {	// $$$ change ID to a reference when compiler gets better

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~WInterfacePtr()							{if (mPtr != nil) (void) mPtr->Release();}

	explicit 			WInterfacePtr(INTERFACE* takePtr = nil)		{mPtr = takePtr;}
						/**< Ctor is explicit because the pointer is taken. */
	
			 			WInterfacePtr(IUnknown* ptr);
						/**< Will throw if ptr cannot be converted to INTERFACE. */

						WInterfacePtr(const CLSID& classID, uint32 context = CLSCTX_INPROC_SERVER, IUnknown* outerPtr = nil);	
						WInterfacePtr(const std::wstring& classID, uint32 context = CLSCTX_INPROC_SERVER, IUnknown* outerPtr = nil);	
						/**< Calls CoCreateInstance. */

						WInterfacePtr(const WInterfacePtr& rhs);
			
			WInterfacePtr& operator=(const WInterfacePtr& rhs)		{this->Reset(rhs.mPtr); return *this;}
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			INTERFACE&	operator*() const							{ASSERT(mPtr != nil); return *mPtr;}
			
			INTERFACE*	operator->() const							{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const WInterfacePtr& rhs) const	{return mPtr == rhs.mPtr;}

			bool 		operator!=(const WInterfacePtr& rhs) const	{return mPtr != rhs.mPtr;}

			bool 		operator<(const WInterfacePtr& rhs) const	{return mPtr < rhs.mPtr;}

			bool 		operator<=(const WInterfacePtr& rhs) const	{return mPtr <= rhs.mPtr;}

			bool 		operator>(const WInterfacePtr& rhs) const	{return mPtr > rhs.mPtr;}

			bool 		operator>=(const WInterfacePtr& rhs) const	{return mPtr >= rhs.mPtr;}

	// ----- Misc -----
			INTERFACE*	Get() const									{return mPtr;}
						/**< This is a dangerous operation: if you save the pointer and the
						last WInterfacePtr goes away you'll be left with a dangling pointer. */
			
			INTERFACE*	Reset(INTERFACE* takePtr);
						/**< Modifies 'this' to point to takePtr. */
	
						operator const void*() const				{return mPtr;}
						/**< Used for validity tests (this is safer than using bool). */
			
	// ----- COM Specific -----
			const IID&	GetIID() const								{return *ID;}

			IUnknown*	GetIUnknown() const							{return mPtr;}
						/**< May be nil. */

//-----------------------------------
//	Member Data
//
protected:
	INTERFACE*	mPtr;
};


// ===================================================================================
//	class WInterfacePtr (specialized for IUnknown)
// ===================================================================================
template<> class CORE_EXPORT WInterfacePtr<IUnknown, &IID_IUnknown> {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~WInterfacePtr()							{if (mPtr != nil) (void) mPtr->Release();}

	explicit 			WInterfacePtr(IUnknown* takePtr = nil)		{mPtr = takePtr;}
						// Ctor is explicit because the pointer is taken.
	
						WInterfacePtr(const WInterfacePtr& rhs);
			
			WInterfacePtr& operator=(const WInterfacePtr& rhs)		{this->Reset(rhs.mPtr); return *this;}
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			IUnknown&	operator*() const							{ASSERT(mPtr != nil); return *mPtr;}
			
			IUnknown*	operator->() const							{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const WInterfacePtr& rhs) const	{return mPtr == rhs.mPtr;}

			bool 		operator!=(const WInterfacePtr& rhs) const	{return mPtr != rhs.mPtr;}

			bool 		operator<(const WInterfacePtr& rhs) const	{return mPtr < rhs.mPtr;}

			bool 		operator<=(const WInterfacePtr& rhs) const	{return mPtr <= rhs.mPtr;}

			bool 		operator>(const WInterfacePtr& rhs) const	{return mPtr > rhs.mPtr;}

			bool 		operator>=(const WInterfacePtr& rhs) const	{return mPtr >= rhs.mPtr;}

	// ----- Misc -----
			IUnknown*	Get() const									{return mPtr;}
						// This is a dangerous operation: if you save the pointer and the
						// last WInterfacePtr goes away you'll be left with a dangling pointer.
			
			IUnknown*	Reset(IUnknown* takePtr);
						// Modifies 'this' to point to takePtr.
						
						operator const void*() const						{return mPtr;}
						// Used for validity tests (this is safer than using bool).
			
	// ----- COM Specific -----
			const IID&	GetIID() const								{return IID_IUnknown;}

			IUnknown*	GetIUnknown() const							{return mPtr;}
						// May be nil.

//-----------------------------------
//	Member Data
//
protected:
	IUnknown*	mPtr;
};


// ===================================================================================
//	Methods
// ===================================================================================
template<class INTERFACE, const IID* ID>
WInterfacePtr<INTERFACE, ID>::WInterfacePtr(IUnknown* ptr)
{
	mPtr = nil;

	if (ptr != nil) {
		HRESULT result = ptr->QueryInterface(*ID, (void**) &mPtr);
		ThrowIfFailed(result);
	}
}

template<class INTERFACE, const IID* ID>
WInterfacePtr<INTERFACE, ID>::WInterfacePtr(const CLSID& classID, uint32 context, IUnknown* outerPtr)
{
	mPtr = nil;

	HRESULT result = CoCreateInstance(classID, outerPtr, context, *ID, (void**) &mPtr);
	ThrowIfFailed(result);
}

template<class INTERFACE, const IID* ID>
WInterfacePtr<INTERFACE, ID>::WInterfacePtr(const std::wstring& classID, uint32 context, IUnknown* outerPtr)
{
	ASSERT(classID.length() > 0);

	CLSID clsid;
	HRESULT result;
	if (clsidString[0] == '{')
		result = CLSIDFromString(classID.c_str(), &id);
	else
		result = CLSIDFromProgID(classID.c_str(), &id);
	ThrowIfFailed(result);

	mPtr = nil;

	HRESULT result = CoCreateInstance(id, outerPtr, context, *ID, (void**) &mPtr);
	ThrowIfFailed(result);
}

template<class INTERFACE, const IID* ID>
WInterfacePtr<INTERFACE, ID>::WInterfacePtr(const WInterfacePtr<INTERFACE, ID>& rhs)
{
	mPtr = rhs.mPtr;

	if (mPtr != nil)
		(void) mPtr->AddRef();
}

template<class INTERFACE, const IID* ID>
INTERFACE* WInterfacePtr<INTERFACE, ID>::Reset(INTERFACE* takePtr)
{
	if (mPtr != takePtr) {
		if (takePtr != nil)
			(void) takePtr->AddRef();

		if (mPtr != nil)
			(void) mPtr->Release();

		mPtr = takePtr;
	}
	
	return mPtr;
}


}	// namespace Whisper

