/*
 *  File:		XSparseArray.h
 *  Summary:	2D array class that's optimized for most elements being empty.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSparseArray.h,v $
 *		Revision 1.3  2000/12/10 08:55:50  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:52:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XSparseArray.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/10 08:55:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Converted header comments to Doxygen format.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <4>	 6/11/99	JDJ		Tweaked friend declarations for CW Pro 5fc3.
 *		 <3>	 1/06/99	JDJ		Streaming operators use new template friend syntax.
 *		 <2>	12/17/98	JDJ		Fixed some minor compile errors.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#include <map>

#include <XRect.h>

namespace Whisper {


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;

template <class T> class XSparseArray;

template <class T> XInStream& 	operator>>(XInStream& stream, XSparseArray<T>& c);	
template <class T> XOutStream& 	operator<<(XOutStream& stream, const XSparseArray<T>& c);


// ===================================================================================
//	class XSparseArray
//!		2D array class that's optimized for most elements being empty.
// ===================================================================================
template <class T> class XSparseArray {
	
//-----------------------------------
//	Types
//
public:
	typedef T					value_type;
	typedef value_type& 		reference;
    typedef const value_type&	const_reference;    
    typedef value_type*			pointer;
    typedef const value_type*	const_pointer;

	typedef STD::size_t			size_type;
	typedef STD::ptrdiff_t		difference_type;

protected:
	typedef std::map<XPoint, T>	ElementTable;

//-----------------------------------
//	Iterators
//
public:
	// ----- iterator -----
    class const_iterator;
	class iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
	
		friend class XSparseArray;
		friend class const_iterator;

	protected:
					iterator(ElementTable* elements, ElementTable::iterator iter, const T& def) : mElements(elements), mIter(iter), mDefaultValue(def) {}
							
	public:
					iterator() : mElements(nil) 				{}

		reference 	operator*() const							{ASSERT(mIter != mElements->end()); return (*mIter).second;}

		iterator& 	operator++()
			{						// $$ These should be outlined, but I can't figure out how to get it to compile!
				ASSERT(mIter != mElements->end());
				
				while (++mIter != mElements->end() && (*mIter).second == mDefaultValue)
					(void) 0;
				
				return *this;
			}
		iterator 	operator++(int)								{iterator temp = *this; ++*this; return temp;}

		iterator& 	operator--()
			{
				ASSERT(mIter != mElements->begin());
				
				while (--mIter != mElements->begin() && (*mIter).second == mDefaultValue)
					(void) 0;
				
				return *this;
			}
		iterator 	operator--(int)								{iterator temp = *this; --*this; return temp;}
		
		int32 		GetCol() const								{ASSERT(mIter != mElements->end()); return (*mIter).first.x;}
		int32 		GetRow() const								{ASSERT(mIter != mElements->end()); return (*mIter).first.y;}
		
		bool 		operator==(const iterator& rhs) const		{ASSERT(mElements == rhs.mElements); return mIter == rhs.mIter;}	
		bool 		operator!=(const iterator& rhs) const		{ASSERT(mElements == rhs.mElements); return mIter != rhs.mIter;}

		bool 		operator==(const const_iterator& rhs) const	{ASSERT(mElements == rhs.mElements); return mIter == rhs.mIter;}	
		bool 		operator!=(const const_iterator& rhs) const	{ASSERT(mElements == rhs.mElements); return !(mIter == rhs.mIter);}

	protected:
		ElementTable*			mElements;
		T						mDefaultValue;
		ElementTable::iterator	mIter;
	};

	// ----- const_iterator -----
	class const_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {

		friend class XSparseArray;
#if __MWERKS__ > 0 && __MWERKS__ < 0x2300
		friend iterator;
#else
		friend class iterator;
#endif

	protected:
						const_iterator(const ElementTable* elements, ElementTable::const_iterator iter, const T& def) : mElements(elements), mIter(iter), mDefaultValue(def) {}
							
	public:
						const_iterator() : mElements(nil) 			{}
#if CODE_WARRIOR
						const_iterator(const XSparseArray::iterator& rhs) : mElements(rhs.mElements), mIter(rhs.mIter) {}	// Pro 4 won't compile if iterator isn't qualified...
#else
						const_iterator(const iterator& rhs) : mElements(rhs.mElements), mIter(rhs.mIter) {}	// Pro 4 won't compile if iterator isn't qualified...
#endif

		const_reference operator*() const							{ASSERT(mIter != mElements->end()); return (*mIter).second;}

		const_iterator& operator++()
			{
				ASSERT(mIter != mElements->end());
				
				while (++mIter != mElements->end() && (*mIter).second == mDefaultValue)
					(void) 0;
				
				return *this;
			}
		const_iterator 	operator++(int)								{const_iterator temp = *this; ++*this; return temp;}

		const_iterator& operator--()
			{
				ASSERT(mIter != mElements->begin());

				while (--mIter != mElements->begin() && (*mIter).second == mDefaultValue)
					(void) 0;
				
				return *this;
			}
		const_iterator 	operator--(int)								{const_iterator temp = *this; --*this; return temp;}
		
		int32 			GetCol() const								{ASSERT(mIter != mElements->end()); return (*mIter).first.x;}
		int32 			GetRow() const								{ASSERT(mIter != mElements->end()); return (*mIter).first.y;}
		
		bool 			operator==(const const_iterator& rhs) const	{ASSERT(mElements == rhs.mElements); return mIter == rhs.mIter;}	
		bool 			operator!=(const const_iterator& rhs) const	{ASSERT(mElements == rhs.mElements); return mIter != rhs.mIter;}

	public:				// $$$ iterator::operator==(const_iterator) won't compile if this is protected (CW Pro 5fc)
		const ElementTable*				mElements;
		T								mDefaultValue;
		ElementTable::const_iterator	mIter;
	};

	// ----- reverse iterators -----
#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const T>	const_reverse_iterator;
                                           
    typedef std::reverse_iterator<iterator, T> 		reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
                                           
    typedef std::reverse_iterator<iterator> 		reverse_iterator;
#endif

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XSparseArray();

						XSparseArray(const T& initial = T());

						XSparseArray(const XSize& dim, const T& initial = T());
						// Sets mDefaultValue to initial.
						
						XSparseArray(const XRect& shape, const T& initial = T());
						// topLeft is taken to be the minimum elements indices.
						// botRight is taken to be one past the maximum element indices.
						
//-----------------------------------
//	API
//
public:
	//! @name Dimensions
	//@{
			XRect	 	GetShape() const						{return mShape;}
			
			XSize 		GetDim() const							{return mShape.GetSize();}
			
			void 		SetShape(const XRect& shape);
			
			void 		SetDim(const XSize& dim)				{this->SetShape(XRect(kLongZeroPt, dim));}
	//@}

	//! @name Access
	//@{
			const T& 	operator()(int32 col, int32 row) const;

			T& 			operator()(int32 col, int32 row);
						/**< May create a new element. */
						
			void 		Force(int32 col, int32 row, const T& elem);
						/**< If the (col, row) is outside the array the array will be expanded. */
						
			void 		Erase(const iterator& iter)				{mElements.erase(iter.mIter);}
			
			void 		Clear()									{mElements.clear();}
	//@}
			
	//! @name Misc
	//@{
			bool 		IsSparse(int32 col, int32 row) const	{return this->Find(col, row) == this->end();}
						/**< Returns true if the element equals mDefaultValue. */
						
			iterator 	Find(int32 col, int32 row);
						/**< If the element isn't sparse the returned iterator points to it.
						Otherwise the iterator == end(). */

			const_iterator Find(int32 col, int32 row) const;

			void 		Reduce();
						/**< Delete any elements that equal mDefaultValue. */
	//@}

	//! @name Iterating
	//@{
			iterator 		begin()								{return iterator(&mElements, mElements.begin(), mDefaultValue);}
							/**< Note that these only return elements that do not equal mDefaultValue. */

			iterator 		end()								{return iterator(&mElements, mElements.end(), mDefaultValue);}

			const_iterator 	begin() const						{return const_iterator(&mElements, mElements.begin(), mDefaultValue);}

			const_iterator 	end() const							{return const_iterator(&mElements, mElements.end(), mDefaultValue);}

			reverse_iterator       	rbegin()					{return reverse_iterator(this->end());}

			reverse_iterator       	rend()						{return reverse_iterator(this->begin());}

			const_reverse_iterator	rbegin() const				{return const_reverse_iterator(this->end());}

			const_reverse_iterator 	rend() const				{return const_reverse_iterator(this->begin());}
	//@}

	//! @name Streaming
	//@{
#if MSVC >= 1100
	friend 	XInStream& 	operator>>(XInStream& stream, XSparseArray<T>& c);	// these are defined in XStreaming.h

	friend	XOutStream& operator<<(XOutStream& stream, const XSparseArray<T>& c);
#else
	friend 	XInStream& 	operator>> <T>(XInStream& stream, XSparseArray<T>& c);	// these are defined in XStreaming.h

	friend	XOutStream& operator<< <T>(XOutStream& stream, const XSparseArray<T>& c);
#endif
	//@}

//-----------------------------------
//	Member Data
//
protected:									
	XRect			mShape;	
	ElementTable	mElements;
	T				mDefaultValue;
};


}	// namespace Whisper


#include <XSparseArray.inc>

