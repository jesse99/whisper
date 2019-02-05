/*
 *  File:       XSafePtr.h
 *  Summary:    Smart pointer class that is notified when it's target is destroyed.
 *  Written by: Jesse Jones
 *
 *	Classes:	XSafePtrMixin	- The object the safe pointer points to must descend from this.
 *				ZBaseSafePtr	- Base class for safe pointers (you shouldn't need to deal with this).
 *				XSafePtr		- The templatized smart pointer class.
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSafePtr.h,v $
 *		Revision 1.4  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:55:19  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:50:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	11/02/00	JDJ		Replaced IsValid with a const void* conversion operator.
 *		 <4>	 6/21/99	JDJ		Renamed XSafePtrMixin (from XSafePtrMixIn).
 *		 <3>	 6/11/99	JDJ		Tweaked the ZBaseSafePtr and XSafePtrMixin friend declarations so 
 *									they compile with Pro 5fc3. 
 *		 <2>	 3/15/99	JDJ		Made the XSafePtr ctor explicit (the ctor and dtor are non-trivial 
 *									so it seems like a good idea to eliminate implicit temporaries).
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <XDebug.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ========================================================================================
//	class XSafePtrMixin
//!		The object XSafePointer points to must descend from this.
// ========================================================================================
class CORE_EXPORT XSafePtrMixin  {	

	friend class CORE_EXPORT ZBaseSafePtr;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 		   ~XSafePtrMixin();
						/**< Tells any safe pointers that target is going away. */

						XSafePtrMixin();

//-----------------------------------
//	API
//
protected:
	static	void 		AddSafePtr(ZBaseSafePtr* ptr);

	static	void 		RemoveSafePtr(ZBaseSafePtr* ptr);
};


// ===================================================================================
//	class ZBaseSafePtr
//!		Base class for safe pointers (you shouldn't need to deal with this).
// ===================================================================================
class CORE_EXPORT ZBaseSafePtr {

	friend class CORE_EXPORT XSafePtrMixin;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~ZBaseSafePtr();

			 			ZBaseSafePtr(XSafePtrMixin* target);
			 			
						ZBaseSafePtr(const ZBaseSafePtr& rhs);
						
			ZBaseSafePtr& operator=(const ZBaseSafePtr& rhs);

//-----------------------------------
//	API
//
public:
			void		Set(XSafePtrMixin* target);

			const XSafePtrMixin* GetTarget() const 				{return mTarget;}
			
protected:
			void 		HandleLostTarget();

//-----------------------------------
//	Member Data
//
protected:
	XSafePtrMixin*	mTarget;
};


// ===================================================================================
//	class XSafePtr
//!		Smart pointer class that is notified when it's target is destroyed.
/*!		Use this class whenever you have an object containing a pointer to a second object 
 *		and cannot be sure that the second object will not be destroyed before the pointer 
 *		is used. For example, if you have an XCommand that operates on a control it's 
 *		possible that the control can be deleted before the callback executes. To fix this 
 *		you can use an XSafePtr: \code
 *			class CCommand : public XCommand {
 *			public:
 *								CCommand(XControl* control) : mControl(control) {}
 *			protected:
 *				virtual void 	OnExecute()	{if (mControl.TargetExists()) mControl->DoIt();}
 *			private:
 *				XSafePtr<XControl>	mControl;
 *			}; \endcode */
// ===================================================================================
template<class T> class XSafePtr : public ZBaseSafePtr {

	typedef ZBaseSafePtr Inherited;

//-----------------------------------
//	Types
//
public:
	typedef T	value_type;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XSafePtr()									{}

	explicit			XSafePtr(T* target) : ZBaseSafePtr(target)	{}
			
//-----------------------------------
//	API
//
public:
	// ----- Smart Pointer Operations -----
			T&			operator*() const							{ASSERT(mTarget != nil); return *((T*) mTarget);}
			T*			operator->() const							{ASSERT(mTarget != nil); return (T*) mTarget;}

			T*			Get() const									{return (T*) mTarget;}
			void		Set(T* target)								{Inherited::Set(target);}
			
	// ----- Comparisons -----
			bool 		operator==(const XSafePtr<T>& rhs) const	{return mTarget == rhs.mTarget;}
			bool 		operator!=(const XSafePtr<T>& rhs) const	{return mTarget != rhs.mTarget;}

			bool 		operator<(const XSafePtr<T>& rhs) const		{return mTarget < rhs.mTarget;}
			bool 		operator<=(const XSafePtr<T>& rhs) const	{return mTarget <= rhs.mTarget;}

			bool 		operator>(const XSafePtr<T>& rhs) const		{return mTarget > rhs.mTarget;}
			bool 		operator>=(const XSafePtr<T>& rhs) const	{return mTarget >= rhs.mTarget;}

	// ----- Misc -----
						operator const void*() const		{return mPtr;}
						/**< Used for validity tests (this is safer than using bool). */
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

