/*
 *  File:       XQueue.h
 *  Summary:    Fast light weight queue class that's optimized for a relatively
 *				small number of elements.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XQueue.h,v $
 *		Revision 1.4  2001/03/05 05:43:07  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:49  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:47:59  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <algorithm>
#include <iterator>

#include <XDebug.h>

namespace Whisper {


// ===================================================================================
//	class XQueue
//!		Fast light weight queue class that's optimized for a relatively small number of elements.
//		Note that this class currently constructs all of the elements in the buffer:
//		if you're storing objects that are expensive to create you might want to
//		consider using vector or list instead.
// ===================================================================================
template<class TYPE, int INITIAL_SIZE = 256> class XQueue {

//-----------------------------------
//	Types
//
public:
#if 0
	typedef          TYPE			value_type;
	typedef typename TYPE*			pointer;
	typedef typename const TYPE*	const_pointer;
	typedef typename TYPE&			reference;
	typedef typename const TYPE&	const_reference;
	typedef uint32					size_type;
	typedef std::ptrdiff_t			difference_type;

    typedef typename TYPE*			iterator;
    typedef typename const TYPE*	const_iterator;
    
#else
	typedef TYPE			value_type;		// $$$ CW Pro 3 doesn't like typename
	typedef TYPE*			pointer;
	typedef const TYPE*		const_pointer;
	typedef TYPE&			reference;
	typedef const TYPE&		const_reference;
	typedef uint32			size_type;
	typedef int32			difference_type;

    typedef  TYPE*			iterator;
    typedef  const TYPE*	const_iterator;
#endif

#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const TYPE>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator, TYPE>				reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator>			reverse_iterator;
#endif

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XQueue();

			 			XQueue();
	
						XQueue(const XQueue<TYPE, INITIAL_SIZE>& rhs);
						
			XQueue<TYPE, INITIAL_SIZE>& operator=(const XQueue<TYPE, INITIAL_SIZE>& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Size -----
			bool 		empty() const				{return mCount == 0;}
			
			uint32 		size() const				{return mCount;}
			
	// ----- Element Access -----
			reference 		front()					{ASSERT(mCount > 0); return *mBuffer;}	
			reference 		front() const			{ASSERT(mCount > 0); return *mBuffer;}
			
			const_reference back()					{ASSERT(mCount > 0); return *(mBuffer + mCount - 1);}
			const_reference	back() const			{ASSERT(mCount > 0); return *(mBuffer + mCount - 1);}

	// ----- Modifiers -----
			void 		push_front(const TYPE& x);
			void 		push_back(const TYPE& x);
			
			void 		pop_front();
			void 		pop_back()					{ASSERT(mCount > 0); mCount--;}
			
			void 		clear()						{mCount = 0;}
			
	// ----- Comparisons -----
    		bool 		operator==(const XQueue<TYPE, INITIAL_SIZE>& rhs) const	{return this->size() == rhs.size() && ::equal(this->begin(), this->end(), rhs.begin());}

    		bool 		operator!=(const XQueue<TYPE, INITIAL_SIZE>& rhs) const	{return !this->operator==(rhs);}

    		bool 		operator<(const XQueue<TYPE, INITIAL_SIZE>& rhs) const	{return ::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());}			

	// ----- Iterators -----
			iterator 				begin()			{return mBuffer;}
			const_iterator 			begin() const	{return mBuffer;}

			iterator 				end()			{return mBuffer + mCount;}
			const_iterator 			end() const		{return mBuffer + mCount;}

			reverse_iterator 		rbegin()		{return reverse_iterator(mBuffer + mCount);}
			const_reverse_iterator 	rbegin() const	{return const_reverse_iterator(mBuffer + mCount);}

			reverse_iterator 		rend()			{return reverse_iterator(mBuffer);}
			const_reverse_iterator 	rend() const	{return const_reverse_iterator(mBuffer);}

//-----------------------------------
//	Internal API
//
protected:
			void 		DoExpandBuffer(uint32 newSize);

//-----------------------------------
//	Member Data
//
protected:
	TYPE*	mBuffer;						// points to initial or heap buffer
	uint32	mCount;
	uint32	mCapacity;	
	
	TYPE	mInitialBuffer[INITIAL_SIZE];		
	TYPE*	mHeapBuffer;					// only used when queue has more than INITIAL_SIZE items
};


}	// namespace Whisper

#include <XQueue.inc>

