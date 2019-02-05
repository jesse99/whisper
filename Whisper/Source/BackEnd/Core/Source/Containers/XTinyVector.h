/*
 *  File:       XTinyVector.h
 *  Summary:   	Simple very lightweight vector class.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTinyVector.h,v $
 *		Revision 1.4  2001/03/05 05:44:03  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:56:44  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:56:34  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Iterator>

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class XTinyVector
//!		Simple very lightweight vector class.
// ===================================================================================
template <class T> class XTinyVector {

//-----------------------------------
//	Types
//
public:
	typedef T 				value_type;
	typedef T& 				reference;
    typedef const T& 		const_reference;
    typedef T*		 		pointer;
    typedef const T*		const_pointer;

	typedef uint32			size_type;
	typedef int32			difference_type;
    
    typedef T*		 		iterator;
    typedef const T*		const_iterator;

#if MSVC >= 1100
    typedef std::reverse_iterator<const_iterator, const T>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator, T>				reverse_iterator;
#else
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
    typedef std::reverse_iterator<iterator>			reverse_iterator;
#endif

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XTinyVector()							{delete [] mData;}
							
						XTinyVector()							{mSize = 0; mData = new T[mSize];}

	explicit			XTinyVector(uint32 size)				{mSize = size; mData = new T[mSize];}

						XTinyVector(const XTinyVector& rhs);
						
			XTinyVector& operator=(const XTinyVector& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Size -----
			bool 		empty() const							{return mSize == 0;}
			uint32 		size() const							{return mSize;}
						
	// ----- Access -----
			T&			operator[](uint32 index)				{ASSERT(index < mSize); return mData[index];}	
			const T& 	operator[](uint32 index) const			{ASSERT(index < mSize); return mData[index];}
			
			T&			front()									{ASSERT(mSize > 0); return mData[0];}
			const T& 	front() const							{ASSERT(mSize > 0); return mData[0];}

			T&			back()									{ASSERT(mSize > 0); return mData[mSize - 1];}
			const T& 	back() const							{ASSERT(mSize > 0); return mData[mSize - 1];}
			
			T* 			buffer() 								{return mData;}
			const T* 	buffer() const							{return mData;}

	// ----- Comparisons -----
			bool 		operator==(const XTinyVector& rhs) const;
			bool 		operator!=(const XTinyVector& rhs) const	{return !this->operator==(rhs);}

			bool 		operator<(const XTinyVector& rhs) const;
						
	// ----- Iterating -----
			iterator 				begin()						{return mData;}
			const_iterator			begin() const				{return mData;}
						
			iterator 				end()						{return this->begin() + this->size();}
			const_iterator 			end() const					{return this->begin() + this->size();}

			reverse_iterator 		rbegin()					{return reverse_iterator(this->end());}
			const_reverse_iterator	rbegin() const				{return const_reverse_iterator(this->end());}
						
			reverse_iterator 		rend()						{return reverse_iterator(this->begin());}
			const_reverse_iterator 	rend() const				{return const_reverse_iterator(this->begin());}

	// ----- Misc -----
			void 					swap(XTinyVector<T>& rhs);

//-----------------------------------
//	Member Data
//
protected:
	uint32	mSize;
	T*		mData;
};


}	// namespace Whisper


#include <XTinyVector.inc>

