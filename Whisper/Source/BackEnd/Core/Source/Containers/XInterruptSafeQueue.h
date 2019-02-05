/*
 *  File:       XInterruptSafeQueue.h
 *  Summary:   	Wrappers for (Mac) interrupt safe OS queues.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterruptSafeQueue.h,v $
 *		Revision 1.3  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:40:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 1/10/00	JDJ		Fixed const_iterator so it uses forward_iterator_tag
 *									instead of bidirectional_iterator_tag.
 *		 <1>	12/26/97	JDJ		Created (from Raven)
 */

#pragma once

#include <Iterator>

#include <XDebug.h>

#if MAC
	#include <OSUtils.h>

	#pragma profile off			

#elif WIN
	#include <list>	
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XInterruptSafeMixIn
//!		A mixin that allows object pointers to be placed in an OS Queue. 
/*!		Derived classes *must* inherit from XInterruptSafeMixIn first to ensure QElem 
 *		is placed first in the object.
 *		
 *		A XInterruptSafeQueue can be used whenever you need to use something like an STL 
 *		list at interrupt time. The template type you can use with XInterruptSafeQueue
 *		is restricted to types that can be implicitly converted to a QElem*. Typically 
 *		this is done by mixing XInterruptSafeMixIn into the object you want to place in 
 *		the queue. For example, XSoundCommand maintains an interrupt safe list of commands 
 *		that have been executed: \code
 *			class XSoundCommand : public XInterruptSafeMixIn {
 *				...
 *			};
 *
 *			typedef XInterruptSafeQueue<XSoundCommand*> ZSoundQueue;
 *			static ZSoundQueue sCompletedCommands; \endcode
 *
 *		At this point you can use a subset of the STL list API to operate on the queue. */
// ===================================================================================
#if MAC
	class CORE_EXPORT XInterruptSafeMixIn : public QElem {

	public:
		virtual				~XInterruptSafeMixIn()						{}
		
							XInterruptSafeMixIn()						{qLink = nil; qType = 0;}
							
	private:
							XInterruptSafeMixIn(const XInterruptSafeMixIn& rhs);
							
				XInterruptSafeMixIn& operator=(const XInterruptSafeMixIn& rhs);
	};
	
#elif WIN
	class XInterruptSafeMixIn {

	public:
		virtual				~XInterruptSafeMixIn()						{}
		
							XInterruptSafeMixIn()						{}
							
	private:
							XInterruptSafeMixIn(const XInterruptSafeMixIn& rhs);
							
				XInterruptSafeMixIn& operator=(const XInterruptSafeMixIn& rhs);
	};
#endif


// ===================================================================================
//	class XInterruptSafeQueue
//		T should be a pointer to a subclass of QElem (eg something descending from XInterruptSafeMixIn).
// ===================================================================================
#if MAC
	template <class T>
	class XInterruptSafeQueue : public QHdr {

	//-----------------------------------
	//	Types
	//
	public:
		typedef T 				value_type;
		typedef T& 				reference;
	    typedef const T& 		const_reference;		// const pointer, not const data
		typedef std::size_t		size_type;
		typedef std::ptrdiff_t	difference_type;

	//-----------------------------------
	//	Initialization/Destruction
	//
	public:
							~XInterruptSafeQueue()							{SAFE_ASSERT(qHead == nil); SAFE_ASSERT(qTail == nil);}
		
							XInterruptSafeQueue()							{qFlags = 0; qHead = qTail = nil;}
							
	private:
							XInterruptSafeQueue(const XInterruptSafeQueue& rhs);
							
				XInterruptSafeQueue& operator=(const XInterruptSafeQueue& rhs);

	//-----------------------------------
	//	Iterator
	//
	public:
		class const_iterator : public std::iterator<std::forward_iterator_tag, T> {
		
		public:
							const_iterator(QElem* element = nil)		{mElement = (value_type) element;}

			bool 			operator==(const const_iterator& rhs) const	{return mElement == rhs.mElement;}
			bool 			operator!=(const const_iterator& rhs) const	{return mElement != rhs.mElement;}

			const_reference	operator*() const							{return mElement;}
			const_iterator& operator++()								{if (mElement != nil) mElement = (value_type) mElement->qLink; return *this;}
			const_iterator 	operator++(int)								{const_iterator temp = *this; ++*this; return temp;}
			
		private:
			T	mElement;
		};

	//-----------------------------------
	//	API
	//
	public:
				const_iterator 	begin() const					{return qHead;}
				const_iterator 	end() const						{return nil;}
				bool 			empty() const					{return qHead == nil;}
				const_reference front() const					{return (T&) qHead;}
				const_reference back() const					{return (T&) qTail;}
				void 			push_back(T x)					{Enqueue(x, this);}
				OSErr 			erase(const_iterator position)	{return Dequeue(*position, this);}
				OSErr 			pop_front()						{return Dequeue(qHead, this);}
				OSErr 			pop_back()						{return Dequeue(qTail, this);}
				OSErr 			remove(T x)						{return Dequeue(x, this);}

				OSErr 			splice(XInterruptSafeQueue& x);
	};

#elif WIN
	template <class T>
	class XInterruptSafeQueue : public std::list<T> {

		typedef std::list<T> Inherited;

	public:
							~XInterruptSafeQueue()				{}
		
							XInterruptSafeQueue()				{}

				void		splice(XInterruptSafeQueue& rhs)	{Inherited::splice(this->end(), rhs);}

	private:
							XInterruptSafeQueue(const XInterruptSafeQueue& rhs);
							
				XInterruptSafeQueue& operator=(const XInterruptSafeQueue& rhs);
	};
#endif


// ===================================================================================
//	Outlined Functions
// ===================================================================================
#if MAC
	template <class T>
	OSErr XInterruptSafeQueue<T>::splice(XInterruptSafeQueue& x) 
	{
		OSErr err = noErr;
		
		while (err == noErr && !x.empty()) {
			value_type elem = x.front();
			err = x.pop_front();
			
			if (err == noErr)
				this->push_back(elem);
		}
		
		return err;
			
	}
#endif


#if MAC
	#pragma profile reset				
#endif

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
