/*
 *  File:		XSet.h
 *  Summary:	Wrapper around an STL set that provides a more natural set-like interface.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSet.h,v $
 *		Revision 1.4  2001/03/05 05:43:20  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:55:28  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:51:04  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 8/25/99	JDJ		Tweaked intersects for MSVC.
 *		 <5>	 7/09/99	JDJ		Added default compare template argument.
 *		 <4>	 6/21/99	JDJ		Tweaked insert for Pro 3.
 *		 <3>	 1/06/99	JDJ		Streaming operators and intersects use new template friend syntax.
 *		 <2>	12/17/98	JDJ		Fixed some minor compile errors. Added streaming operators.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <Set>

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;

template <class T, class C> class XSet;

template <class T, class C>  bool 		intersects(const XSet<T, C>& lhs, const XSet<T, C>& rhs);
template <class T, class C> XInStream& 	operator>>(XInStream& stream, XSet<T, C>& c);			
template <class T, class C> XOutStream&	operator<<(XOutStream& stream, const XSet<T, C>& c);


// ===================================================================================
//	class XSet
//!		Wrapper around an STL set that provides a more natural set-like interface.
// ===================================================================================
template <class T, class C = std::less<T> > class XSet {			
	
//-----------------------------------
//	Types
//
protected:
	typedef std::set<T, C> Set;

public:
	typedef T							value_type;
	typedef value_type& 				reference;
    typedef const value_type&			const_reference;    
    typedef value_type*					pointer;
    typedef const value_type*			const_pointer;

	typedef STD::size_t					size_type;
	typedef STD::ptrdiff_t				difference_type;

    typedef Set::const_iterator			iterator;				// sets are sorted so iterator can't change elements
    typedef Set::const_iterator			const_iterator;

    typedef Set::const_reverse_iterator	reverse_iterator;
    typedef Set::const_reverse_iterator	const_reverse_iterator;

	typedef std::pair<iterator, iterator> 	range;

//-----------------------------------
//	Initialization/Destruction
//
public:
							XSet()										{}

							XSet(const T& elem)							{mElements.insert(elem);}

							XSet(const Set& rhs)						{mElements = rhs;}
						
//							template <class InputIterator>
//							XSet(InputIterator first, InputIterator last, const Compare& comp = Compare());
							// $$$ implement when we have member templates
						
			XSet<T, C>& 	operator+=(const T& rhs)					{mElements.insert(rhs); return *this;}
			XSet<T, C>& 	operator+=(const XSet<T, C>& rhs);

			XSet<T, C>& 	operator-=(const T& rhs)					{mElements.erase(rhs); return *this;}
			XSet<T, C>& 	operator-=(const XSet<T, C>& rhs);

			XSet<T, C>& 	operator&=(const T& rhs);
			XSet<T, C>& 	operator&=(const XSet<T, C>& rhs);

			void 			swap(XSet<T, C>& rhs)						{mElements.swap(rhs.mElements);}
						
//-----------------------------------
//	API
//
public:
	// ----- Size -----
			uint32 			size() const										{return mElements.size();}
			
			bool 			empty() const										{return mElements.empty();}

	// ----- Set Operations -----
			XSet<T, C> 		operator+(const T& rhs) const						{XSet<T, C> result = *this; result += rhs; return result;}
			XSet<T, C> 		operator+(const XSet<T, C>& rhs) const				{XSet<T, C> result = *this; result += rhs; return result;}
	friend	XSet<T, C> 		operator+(const T& lhs, const XSet<T, C>& rhs);

			XSet<T, C> 		operator-(const T& rhs) const						{XSet<T, C> result = *this; result -= rhs; return result;}
			XSet<T, C> 		operator-(const XSet<T, C>& rhs) const				{XSet<T, C> result = *this; result -= rhs; return result;}
	friend	XSet<T, C> 		operator-(const T& lhs, const XSet<T, C>& rhs);

			XSet<T, C> 		operator&(const T& rhs) const						{XSet<T, C> result = *this; result &= rhs; return result;}
			XSet<T, C> 		operator&(const XSet<T, C>& rhs) const				{XSet<T, C> result = *this; result &= rhs; return result;}
	friend	XSet<T, C> 		operator&(const T& lhs, const XSet<T, C>& rhs);
	
			T 				min() const											{ASSERT(mElements.size() > 0); return *(mElements.begin());}
			T 				max() const											{ASSERT(mElements.size() > 0); return *(mElements.rbegin());}
			
			bool 			contains(const T& rhs) const						{return mElements.count(rhs) > 0;}
			bool 			contains(const XSet<T, C>& rhs) const;
							// Note that every set contains the empty set.

#if MSVC >= 1100
	friend	bool 			intersects(const XSet<T, C>& lhs, const XSet<T, C>& rhs);
#else
	friend	bool 			intersects<T, C>(const XSet<T, C>& lhs, const XSet<T, C>& rhs);
#endif
							// Returns true if there is an element in lhs that is also
							// in rhs. 

	// ----- Comparisons -----
			bool 			operator==(const T& rhs) const						{return mElements.size() == 1 && mElements.count(rhs) > 0;}
			bool 			operator==(const XSet<T, C>& rhs) const				{return mElements == rhs.mElements;}
	friend	bool 			operator==(const T& lhs, const XSet<T, C>& rhs);
	
			bool 			operator!=(const T& rhs) const						{return !(this->operator==(rhs));}
			bool 			operator!=(const XSet<T, C>& rhs) const				{return !(this->operator==(rhs));}
	friend	bool 			operator!=(const T& lhs, const XSet<T, C>& rhs);

			bool 			operator<(const XSet<T, C>& rhs) const				{return mElements < rhs.mElements;}

	// ----- STL Operations -----
			std::pair<Set::iterator,bool> insert(const T& elem)					{return mElements.insert(elem);}
							// Uses Set::iterator instead of iterator for Pro 3.

//							template <class InputIterator>
//			void 			insert(InputIterator first, InputIterator last);
							// $$$ implement when we have member templates

			void 			clear()												{mElements.clear();}
							// Empties the set.

			void 			erase(const const_iterator& iter)					{mElements.erase(iter);}

			void 			erase(const const_iterator& first, const const_iterator& last)	{mElements.erase(first, last);}

			const_iterator 	find(const T& elem) const							{return mElements.find(elem);}
			
			uint32 			count(const T& elem) const							{return mElements.count(elem);}
							// Returns 1 if elem is in the set or 0 if not.
			
			const_iterator 	lower_bound(const T& elem) const					{return mElements.lower_bound(elem);}
							// Returns iter such that all elements in [iter, end()] are
							// >= elem. Returns end() if all elements are less than elem.

			const_iterator 	upper_bound(const T& elem) const					{return mElements.upper_bound(elem);}
							// Returns iter such that all elements in [iter, end()] are
							// > elem. Returns end() if all elements are greater than elem.

			range 			equal_range(const T& elem) const					{return mElements.equal_range(elem);}

	// ----- Iterating -----
			const_iterator 	begin() const										{return mElements.begin();}
			
			const_iterator 	end() const											{return mElements.end();}
			
			const_reverse_iterator rbegin() const								{return mElements.rbegin();}
			
			const_reverse_iterator rend() const									{return mElements.rend();}

	// ----- Streaming -----
#if MSVC >= 1100
	friend 	XInStream& 		operator>>(XInStream& stream, XSet<T, C>& c);			// these are defined in XStreaming.h

	friend	XOutStream& 	operator<<(XOutStream& stream, const XSet<T, C>& c);
#else
	friend 	XInStream& 		operator>> <T, C>(XInStream& stream, XSet<T, C>& c);	// these are defined in XStreaming.h

	friend	XOutStream& 	operator<< <T, C>(XOutStream& stream, const XSet<T, C>& c);
#endif

//-----------------------------------
//	Member Data
//
protected:									
	Set		mElements;
};


// ===================================================================================
//	Inlines
//		$$$ These are defined down here because the 2.0.0a4 compiler is stupid.
// ===================================================================================
template <class T, class C>
inline XSet<T, C> operator+(const T& lhs, const XSet<T, C>& rhs)			
{
	XSet<T, C> result = rhs; result += lhs; return result;
}

template <class T, class C>
inline XSet<T, C> operator-(const T& lhs, const XSet<T, C>& rhs)			
{
	XSet<T, C> result = rhs; result -= lhs; return result;
}

template <class T, class C>
inline XSet<T, C> operator&(const T& lhs, const XSet<T, C>& rhs)			
{
	XSet<T, C> result = rhs; result &= lhs; return result;
}

template <class T, class C>
inline bool operator==(const T& lhs, const XSet<T, C>& rhs)		
{
	return rhs.mElements.size() == 1 && rhs.mElements.count(lhs) > 0;
}
	
template <class T, class C>
inline bool operator!=(const T& lhs, const XSet<T, C>& rhs)		
{
	return !(rhs.operator==(lhs));
}


}	// namespace Whisper


#include <XSet.inc>
