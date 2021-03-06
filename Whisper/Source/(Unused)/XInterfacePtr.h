/*
 *  File:       XInterfacePtr.h
 *  Summary:    Smart pointer classes that automate reference counting and QueryInterface.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfacePtr.h,v $
 *		Revision 1.2  2000/11/09 12:39:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <4>	 5/09/00	JDJ		Added XInterfacePtr ctors that XBoss::iterator's.
 *		 <3>	 4/29/00	JDJ		Removed XInterfacePtr::Get and Reset.
 *		 <2>	 4/28/00	JDJ		Added XInterfacePtr ctors that take raw pointers.
 *		 <1>	11/27/99	JDJ		Created (from XCountedPtr)
 */

#pragma once

#include <typeinfo>

#include <XBoss.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class ZBaseInterfacePtr
// ===================================================================================
class ZBaseInterfacePtr {

	friend class ZBaseConstInterfacePtr;

public:
			 			~ZBaseInterfacePtr();
	
protected:
						ZBaseInterfacePtr();

						ZBaseInterfacePtr(XUnknown* unknown);
						ZBaseInterfacePtr(const std::wstring& boss);
						ZBaseInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter);

						ZBaseInterfacePtr(XUnknown* unknown, const char* encodedName);
						ZBaseInterfacePtr(const std::wstring& boss, const char* encodedName);
						ZBaseInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter, const char* encodedName);
						ZBaseInterfacePtr(const ZBaseInterfacePtr& unknown, const char* encodedName);

						ZBaseInterfacePtr(const ZBaseInterfacePtr& rhs);
			ZBaseInterfacePtr& operator=(const ZBaseInterfacePtr& rhs);
			
protected:
			void 		AddReference()				{ASSERT(mPtr != nil); mPtr->DoAddReference();}	
			void 		RemoveReference()			{ASSERT(mPtr != nil); mPtr->DoRemoveReference();}

protected:
	XUnknown*	mPtr;
};

				
// ===================================================================================
//	class XInterfacePtr
// ===================================================================================
template<class BASE, class INHERITED = BASE> class XInterfacePtr : public ZBaseInterfacePtr {

	typedef ZBaseInterfacePtr Inherited;

//-----------------------------------
//	Internal Template Magic
//
public:	
	template <class T, class U>
	struct differ			 		{enum {does = true};};
				
	template <class T>
	struct differ<T, BASE>	 		{enum {does = false};};
				
	template <bool b> 
	struct chooser					{};

//-----------------------------------
//	Initialization/Destruction
//
public:	
						~XInterfacePtr()	{}
						// Decrements the boss's ref count, but won't delete the interface
						// (unless the boss's ref count goes to zero).
						
						XInterfacePtr()		{}
						
						XInterfacePtr(XUnknown* unknown) : ZBaseInterfacePtr(unknown, typeid(BASE).name())	{this->DoDowncast(chooser<differ<BASE, INHERITED>::does>());}
						// Queries for the base interface and increment's the boss's ref count. 
						// If it's not found mPtr is set to nil. Note that this may wind up 
						// instantiating a new interface object.

						template<class U>
						XInterfacePtr(const XInterfacePtr<U>& unknown) : ZBaseInterfacePtr(unknown, typeid(BASE).name())	{this->DoDowncast(chooser<differ<BASE, INHERITED>::does>());}
						// Queries for the base interface like the ctor above.

	explicit			XInterfacePtr(const std::wstring& boss) : ZBaseInterfacePtr(boss, typeid(BASE).name())	{this->DoDowncast(chooser<differ<BASE, INHERITED>::does>());}
						// Normal bosses are created the first time you use this method to
						// query for an interface. They are deleted when their last interface 
						// goes away. Singleton bosses are created when their plugin loads 
						// and deleted when the plugin unloads. 

						XInterfacePtr(XBoss* boss, const XBoss::iterator& iter) : ZBaseInterfacePtr(boss, iter, typeid(BASE).name())	{this->DoDowncast(chooser<differ<BASE, INHERITED>::does>());}
						// Bosses can contain multiple instances of the same interface (this is
						// how extension objects work). In order to minimize surprises the only
						// way you can query for an interface that occurs more than once is via
						// boss iteration.
						
protected:
			void 		DoDowncast(chooser<false>)		{}
			void 		DoDowncast(chooser<true>)		{if (mPtr != nil && dynamic_cast<INHERITED*>(mPtr) == nil) {this->RemoveReference(); mPtr = nil;}}
						// Only do the dynamic_cast if we're actually using interface inheritance
				
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			INHERITED&	operator*() const											{ASSERT(mPtr != nil); return (INHERITED&) (*mPtr);}
			
			INHERITED*	operator->() const											{ASSERT(mPtr != nil); return (INHERITED*) mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr == rhs.mPtr;}
			bool 		operator!=(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr != rhs.mPtr;}
			bool 		operator<(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr < rhs.mPtr;}
			bool 		operator<=(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr <= rhs.mPtr;}
			bool 		operator>(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr > rhs.mPtr;}
			bool 		operator>=(const XInterfacePtr<BASE, INHERITED>& rhs) const	{return mPtr >= rhs.mPtr;}

			bool 		operator==(const INHERITED* rhs) const						{return mPtr == rhs;}
			bool 		operator!=(const INHERITED* rhs) const						{return mPtr != rhs;}
			bool 		operator<(const INHERITED* rhs) const						{return mPtr < rhs;}
			bool 		operator<=(const INHERITED* rhs) const						{return mPtr <= rhs;}
			bool 		operator>(const INHERITED* rhs) const						{return mPtr > rhs;}
			bool 		operator>=(const INHERITED* rhs) const						{return mPtr >= rhs;}

	// ----- Misc -----
			bool 		IsValid() const												{return mPtr != nil;}
};


// ===================================================================================
//	class XInterfacePtr<XUnknown>
// ===================================================================================
class XInterfacePtr<XUnknown> : public ZBaseInterfacePtr {

	typedef ZBaseInterfacePtr Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:	
						XInterfacePtr()	{}

						template<class U>
						XInterfacePtr(const XInterfacePtr<U>& unknown) : ZBaseInterfacePtr(unknown)	{}
						// Every non-nil interface pointer can be successfully converted
						// into an XUnknown*.
						
						XInterfacePtr(XUnknown* unknown) : ZBaseInterfacePtr(unknown)	{}

	explicit			XInterfacePtr(const std::wstring& boss) : ZBaseInterfacePtr(boss)	{}

						XInterfacePtr(XBoss* boss, const XBoss::iterator& iter) : ZBaseInterfacePtr(boss, iter)	{}
						
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			XUnknown&	operator*() const										{ASSERT(mPtr != nil); return *mPtr;}
			
			XUnknown*	operator->() const										{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const XInterfacePtr<XUnknown>& rhs) const	{return mPtr == rhs.mPtr;}
			bool 		operator!=(const XInterfacePtr<XUnknown>& rhs) const	{return mPtr != rhs.mPtr;}
			bool 		operator<(const XInterfacePtr<XUnknown>& rhs) const		{return mPtr < rhs.mPtr;}
			bool 		operator<=(const XInterfacePtr<XUnknown>& rhs) const	{return mPtr <= rhs.mPtr;}
			bool 		operator>(const XInterfacePtr<XUnknown>& rhs) const		{return mPtr > rhs.mPtr;}
			bool 		operator>=(const XInterfacePtr<XUnknown>& rhs) const	{return mPtr >= rhs.mPtr;}

			bool 		operator==(const XUnknown* rhs) const					{return mPtr == rhs;}
			bool 		operator!=(const XUnknown* rhs) const					{return mPtr != rhs;}
			bool 		operator<(const XUnknown* rhs) const					{return mPtr < rhs;}
			bool 		operator<=(const XUnknown* rhs) const					{return mPtr <= rhs;}
			bool 		operator>(const XUnknown* rhs) const					{return mPtr > rhs;}
			bool 		operator>=(const XUnknown* rhs) const					{return mPtr >= rhs;}

	// ----- Misc -----
			bool 		IsValid() const											{return mPtr != nil;}
};

typedef XInterfacePtr<XUnknown> XUnknownPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

