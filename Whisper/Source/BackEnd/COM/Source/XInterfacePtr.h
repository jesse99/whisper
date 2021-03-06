/*
 *  File:       XInterfacePtr.h
 *  Summary:    Smart pointer classes that automate reference counting and QueryInterface.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfacePtr.h,v $
 *		Revision 1.7  2001/10/03 02:12:41  jesjones
 *		Added a null type which can be used to avoid ambiguities.
 *		
 *		Revision 1.6  2001/03/13 11:09:06  jesjones
 *		Decorated ZBaseInterfacePtr with COM_EXPORT.
 *		
 *		Revision 1.5  2001/03/03 11:10:25  jesjones
 *		Tweaked formatting.
 *		
 *		Revision 1.4  2000/12/11 01:30:30  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/26 03:26:59  jesjones
 *		Changed the copy ctor and assignment operator so that they no longer do a query when assigning a T to a const T.
 *		
 *		Revision 1.2  2000/11/09 12:40:00  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <11>	11/02/00	JDJ		Added template assignment operators. Replaced IsValid with a
 *									const void* conversion operator.
 *		 <10>	11/01/00	JDJ		Removed XConstInterfacePtr.
 *		 <9>	 9/17/00	JDJ		Added copy constructors (compiler wasn't choosing the compiler
 *									provided copy ctors).
 *		 <8>	 7/30/00	JDJ		Added a (const XBoss*, const XImplementation*) ctor.
 *		 <7>	 7/19/00	JDJ		Removed operator*. Added a Get method.
 *		 <6>	 7/14/00	JDJ		Added ZBaseInterfacePtr::Release.
 *		 <5>	 5/29/00	JDJ		Added casts to the comparison operators that take raw pointers.
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
//	remove_const	$$$ use boost instead
// ===================================================================================
namespace Internals {
	template <class T>
	struct cv_traits_imp{};
	
	template <class T>
	struct cv_traits_imp<T*>
	{
	   static const bool is_const = false;
	   static const bool is_volatile = false;
	   typedef T non_const_type;
	   typedef T non_volatile_type;
	   typedef T unqualified_type;
	};
	
	template <class T>
	struct cv_traits_imp<const T*>
	{
	   static const bool is_const = true;
	   static const bool is_volatile = false;
	   typedef T non_const_type;
	   typedef const T non_volatile_type;
	   typedef T unqualified_type;
	};

	template <class T>
	struct cv_traits_imp<volatile T*>
	{
	   static const bool is_const = false;
	   static const bool is_volatile = true;
	   typedef volatile T non_const_type;
	   typedef T non_volatile_type;
	   typedef T unqualified_type;
	};
	
	template <class T>
	struct cv_traits_imp<const volatile T*>
	{
	   static const bool is_const = true;
	   static const bool is_volatile = true;
	   typedef volatile T non_const_type;
	   typedef const T non_volatile_type;
	   typedef T unqualified_type;
	};
}

template <typename T>
struct remove_const
{
   typedef typename Internals::cv_traits_imp<T*>::non_const_type type;
};
template <typename T> struct remove_const<T&>	{typedef T& type;};

template <typename T>
struct is_const
{
   static const bool value = Internals::cv_traits_imp<T*>::is_const;
};

template <typename T, typename U>
struct is_same {static const bool value = false; typedef char RET;};

template <typename T>
struct is_same<T, T> {static const bool value = true; typedef int RET;};


// ===================================================================================
//	null
// ===================================================================================
struct null_t		
{
	template <typename T> operator T*() const 				{return 0;}
//	template <typename T, class U> operator T U::*() const	{return 0;}		// $$$ CW 6.2 doesn't like this
};  

template <typename T> inline bool operator==(const T* p, const null_t&)		{return p == 0;}
template <typename T> inline bool operator==(const null_t&, const T* p) 	{return p == 0;}
template <typename T> inline bool operator!=(const T* p, const null_t&) 	{return p != 0;}
template <typename T> inline bool operator!=(const null_t&, const T* p) 	{return p != 0;}

//template <typename T, class U> inline bool operator==(T U::* const p, const null_t&)	{return p == 0;}
//template <typename T, class U> inline bool operator==(const null_t&, T U::* const p) 	{return p == 0;}
//template <typename T, class U> inline bool operator!=(T U::* const p, const null_t&) 	{return p != 0;}
//template <typename T, class U> inline bool operator!=(const null_t&, T U::* const p) 	{return p != 0;}

extern const null_t null;


// ===================================================================================
//	class ZBaseInterfacePtr
//!		Base class for XInterfacePtr.
// ===================================================================================
class COM_EXPORT ZBaseInterfacePtr {

	friend class ZBaseConstInterfacePtr;

public:
			 			~ZBaseInterfacePtr();
			 			
			 void 		Release();
	
			XUnknown*	GetPtr() const					{return mPtr;}
			
protected:
						ZBaseInterfacePtr();

						ZBaseInterfacePtr(const XUnknown* unknown);
						ZBaseInterfacePtr(const XUnknown* unknown, const char* encodedName);

						ZBaseInterfacePtr(const std::wstring& boss);
						ZBaseInterfacePtr(const std::wstring& boss, const char* encodedName);

						ZBaseInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter);
						ZBaseInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter, const char* encodedName);
						
						ZBaseInterfacePtr(const ZBaseInterfacePtr& unknown);
						ZBaseInterfacePtr(const ZBaseInterfacePtr& unknown, const char* encodedName);

						ZBaseInterfacePtr(const XBoss* boss, const XImplementation* imp, const char* encodedName);

			ZBaseInterfacePtr& operator=(const ZBaseInterfacePtr& rhs);
			
protected:
#if !GARBAGE_COLLECT_COM
			void 		DoAddReference()			{mPtr->DoAddReference();}
#else
			void 		DoAddReference()			{}
#endif

			template <class T>
			void 		DoCast() 					{if (mAggregated) mPtr = dynamic_cast<T*>(mBasePtr); if (mPtr != nil) this->DoAddReference();}

protected:
	XUnknown*	mBasePtr;		// the pointer we got back from the object model
	XUnknown*	mPtr;			// the pointer we're really querying for (may differ from mBasePtr if more than one interface is aggregated within the implementation class)
	bool 		mAggregated;
};

				
// ===================================================================================
//	class XInterfacePtr
//!		Smart pointer class that automates reference counting and QueryInterface.
// ===================================================================================
template<class T> class XInterfacePtr : public ZBaseInterfacePtr {

	typedef ZBaseInterfacePtr Inherited;
	typedef remove_const<T>::type unqualified_type;

//-----------------------------------
//	Initialization/Destruction
//
public:	
						~XInterfacePtr()	{}
						/**< Decrements the boss's ref count, but won't delete the interface
						(unless the boss's ref count goes to zero). */
						
						XInterfacePtr()		{}
						
	explicit			XInterfacePtr(const std::wstring& boss) : ZBaseInterfacePtr(boss, typeid(unqualified_type).name())		{this->DoCast<unqualified_type>();}
						/**< Normal bosses are created the first time you use this method to
						query for an interface. They are deleted when their last interface 
						goes away. Singleton bosses are created when their plugin loads 
						and deleted when the plugin unloads. */

						XInterfacePtr(XUnknown* unknown)       : ZBaseInterfacePtr(unknown, typeid(unqualified_type).name())	{this->DoCast<unqualified_type>();}
						XInterfacePtr(const XUnknown* unknown) : ZBaseInterfacePtr(unknown, typeid(unqualified_type).name())	{this->DoCast<unqualified_type>(); COMPILE_CHECK(is_const<T>::value);}
						/**< Queries for the base interface and increment's the boss's ref count. 
						If it's not found mPtr is set to nil. Note that this may wind up 
						instantiating a new interface object. */

						template<class U>
						XInterfacePtr(const XInterfacePtr<U>& rhs) 							{this->DoAssign(rhs, is_same<unqualified_type, U>::RET()); COMPILE_CHECK(!is_const<U>::value || is_const<T>::value);}	// if rhs is const lhs must also be const
						/**< Queries for the base interface as in the ctor above. */

						XInterfacePtr(XBoss* boss, const XBoss::iterator& iter)             : ZBaseInterfacePtr(boss, iter, typeid(unqualified_type).name())	{this->DoCast<unqualified_type>();}
						XInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter) : ZBaseInterfacePtr(boss, iter, typeid(unqualified_type).name())	{this->DoCast<unqualified_type>(); COMPILE_CHECK(is_const<T>::value);}
						/**< Bosses can contain multiple instances of the same interface (this is
						how extension objects work). In order to minimize surprises the only
						way you can query for an interface that occurs more than once is via
						boss iteration. */
										
						XInterfacePtr(const XBoss* boss, const XImplementation* imp) : ZBaseInterfacePtr(boss, imp, typeid(unqualified_type).name()) {this->DoCast<unqualified_type>();}
						/**< The ctor above allows you to iterate over all the interfaces on a boss,
						but once you've found an interface you may want to get another interface
						on that implementation. For example,
								XBoss::const_iterator iter = boss->begin();
								while (iter != boss->end()) {
									IDrawExtensionPtr drawer(boss, iter);
									
									if (drawer && drawer->GetName() == L"draw frame") {
										IStrokeExtensionPtr stroke(boss, iter->GetImplementation());
										stroke->SetEnabledPen(kBlackPen);
									}
							
									++iter;
								} */
						
						XInterfacePtr(const null_t&)												{}
						/**< We can't use nil or NULL because of ambiguities, but we can use null. */

						XInterfacePtr(const XInterfacePtr<T>& rhs) : ZBaseInterfacePtr(rhs)			{}
						/**< Copy ctor (if this is omitted the compiler selects the template ctor instead
						of the compiler provided copy ctor). */
						
						template<class U>
			XInterfacePtr& operator=(const XInterfacePtr<U>& rhs)			{this->DoAssign(rhs, is_same<unqualified_type, U>::RET()); COMPILE_CHECK(!is_const<U>::value || is_const<T>::value); return *this;}	// if rhs is const lhs must also be const

			XInterfacePtr& operator=(const XInterfacePtr& rhs)				{Inherited::operator=(rhs); return *this;}

//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----			
			T*			operator->() const									{ASSERT(mPtr != nil); return (T*) mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const XInterfacePtr<const T>& rhs) const	{return this->Get() == rhs.Get();}
			bool 		operator!=(const XInterfacePtr<const T>& rhs) const	{return this->Get() != rhs.Get();}
			bool 		operator<(const XInterfacePtr<const T>& rhs) const	{return this->Get() < rhs.Get();}
			bool 		operator<=(const XInterfacePtr<const T>& rhs) const	{return this->Get() <= rhs.Get();}
			bool 		operator>(const XInterfacePtr<const T>& rhs) const	{return this->Get() > rhs.Get();}
			bool 		operator>=(const XInterfacePtr<const T>& rhs) const	{return this->Get() >= rhs.Get();}

			bool 		operator==(const T* rhs) const						{return (T*) mPtr == rhs;}
			bool 		operator!=(const T* rhs) const						{return (T*) mPtr != rhs;}
			bool 		operator<(const T* rhs) const						{return (T*) mPtr < rhs;}
			bool 		operator<=(const T* rhs) const						{return (T*) mPtr <= rhs;}
			bool 		operator>(const T* rhs) const						{return (T*) mPtr > rhs;}
			bool 		operator>=(const T* rhs) const						{return (T*) mPtr >= rhs;}

			bool 		operator==(const null_t&) const						{return this->Get() == nil;}
			bool 		operator!=(const null_t&) const						{return this->Get() != nil;}

	// ----- Misc -----
						operator const void*() const						{return mPtr;}
						/**< Used for validity tests (this is safer than using bool). */
			
			T*			Get() const											{return (T*) mPtr;}
						/**< In general there's no good reason to use this method. The
						only case I can think of is to construct an XCallback object
						pointing to a method on the interface. */
						
//-----------------------------------
//	Internal API
//
private:
			void 		DoAssign(const ZBaseInterfacePtr& rhs, char)		{XInterfacePtr<T> temp(rhs.GetPtr()); Inherited::operator=(temp);}
						// This function is used to assign unrelated pointers. The method
						// below is used when T and U are the same type, but T is const.
						// This results in more efficient code and avoids queries that may
						// fail if the interface is repeated.
						
			void 		DoAssign(const ZBaseInterfacePtr& rhs, int)			{Inherited::operator=(rhs);}
						// $$ Classier to use partial specialization here, but I couldn't
						// $$ get it to compile with CW 6.
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
						XInterfacePtr()																	{}

						XInterfacePtr(XUnknown* unknown) : ZBaseInterfacePtr(unknown)					{if (mPtr != nil) this->DoAddReference();}

						template<class U>
						XInterfacePtr(const XInterfacePtr<U>& unknown) : ZBaseInterfacePtr(unknown)		{if (mPtr != nil) this->DoAddReference(); COMPILE_CHECK(!is_const<U>::value);}
						// Every non-nil interface pointer can be successfully converted
						// into an XUnknown*.
						
	explicit			XInterfacePtr(const std::wstring& boss) : ZBaseInterfacePtr(boss)				{if (mPtr != nil) this->DoAddReference();}

						XInterfacePtr(XBoss* boss, const XBoss::iterator& iter) : ZBaseInterfacePtr(boss, iter)	{if (mPtr != nil) this->DoAddReference();}
						
						XInterfacePtr(const XInterfacePtr<XUnknown>& rhs) : ZBaseInterfacePtr(rhs)		{}

						template<class U>
			XInterfacePtr& operator=(const XInterfacePtr<U>& rhs)				{COMPILE_CHECK(!is_const<U>::value || is_const<T>::value); Inherited::operator=(rhs); return *this;}	// if rhs is const lhs must also be const

//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----			
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

			bool 		operator==(const null_t&) const							{return mPtr == nil;}
			bool 		operator!=(const null_t&) const							{return mPtr != nil;}

	// ----- Misc -----
						operator const void*() const							{return mPtr;}

			XUnknown*	Get() const												{return mPtr;}
};


// ===================================================================================
//	class XInterfacePtr<const XUnknown>
// ===================================================================================
class XInterfacePtr<const XUnknown> : public ZBaseInterfacePtr {

	typedef ZBaseInterfacePtr Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:	
						XInterfacePtr()																		{}

						XInterfacePtr(const XUnknown* unknown) : ZBaseInterfacePtr(unknown)					{if (mPtr != nil) this->DoAddReference();}

						template<class U>
						XInterfacePtr(const XInterfacePtr<U>& unknown) : ZBaseInterfacePtr(unknown)			{if (mPtr != nil) this->DoAddReference();}
						// Every non-nil interface pointer can be successfully converted
						// into an XUnknown*.
						
	explicit			XInterfacePtr(const std::wstring& boss) : ZBaseInterfacePtr(boss)					{if (mPtr != nil) this->DoAddReference();}

						XInterfacePtr(const XBoss* boss, const XBoss::const_iterator& iter) : ZBaseInterfacePtr(boss, iter)	{if (mPtr != nil) this->DoAddReference();}
						
						XInterfacePtr(const XInterfacePtr<XUnknown>& rhs) : ZBaseInterfacePtr(rhs)			{}

			XInterfacePtr& operator=(const ZBaseInterfacePtr& rhs)					{Inherited::operator=(rhs); return *this;}

//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----			
			const XUnknown*	operator->() const										{ASSERT(mPtr != nil); return mPtr;}
			
	// ----- Comparisons -----
			bool 		operator==(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr == rhs.mPtr;}
			bool 		operator!=(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr != rhs.mPtr;}
			bool 		operator<(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr < rhs.mPtr;}
			bool 		operator<=(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr <= rhs.mPtr;}
			bool 		operator>(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr > rhs.mPtr;}
			bool 		operator>=(const XInterfacePtr<const XUnknown>& rhs) const	{return mPtr >= rhs.mPtr;}

			bool 		operator==(const XUnknown* rhs) const						{return mPtr == rhs;}
			bool 		operator!=(const XUnknown* rhs) const						{return mPtr != rhs;}
			bool 		operator<(const XUnknown* rhs) const						{return mPtr < rhs;}
			bool 		operator<=(const XUnknown* rhs) const						{return mPtr <= rhs;}
			bool 		operator>(const XUnknown* rhs) const						{return mPtr > rhs;}
			bool 		operator>=(const XUnknown* rhs) const						{return mPtr >= rhs;}

			bool 		operator==(const null_t&) const								{return mPtr == nil;}
			bool 		operator!=(const null_t&) const								{return mPtr != nil;}

	// ----- Misc -----
						operator const void*() const								{return mPtr;}

			const XUnknown*	Get() const												{return mPtr;}
};

typedef XInterfacePtr<const XUnknown> XConstUnknownPtr;
typedef XInterfacePtr<XUnknown> XUnknownPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


