/*
 *  File:		XArray.h
 *  Summary:	Templatized 2D array class.
 *  Written by:	Jesse Jones
 *
 *	Notes:		This is just a 2D collection. If you're storing floats you may want
 *				to look at XMatrix.
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XArray.h,v $
 *		Revision 1.5  2001/05/30 01:16:34  jesjones
 *		Added missing std::.
 *		
 *		Revision 1.4  2001/05/12 11:15:34  jesjones
 *		Added IsEmpty. SetSize and SetShape now take an optional initial value.
 *		
 *		Revision 1.3  2000/12/10 08:50:08  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:25:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XRect.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XInStream;
class XOutStream;

template <class T> class XArray;

template <class T> XInStream& 	operator>>(XInStream& stream, XArray<T>& c);	
template <class T> XOutStream& 	operator<<(XOutStream& stream, const XArray<T>& c);


// ===================================================================================
//	class XArray
//!		Templatized 2D array class.
// ===================================================================================
template <class T> class XArray {
	
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

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XArray();

						XArray(const XSize& dim = kZeroSize);

						XArray(const XRect& shape);
						/**< topLeft is taken to be the minimum elements indices.
						botRight is taken to be one past the maximum element indices. */

						XArray(const XSize& dim, const T& initial);
						
						XArray(const XRect& shape, const T& initial);
						
						XArray(const XSize& dim, T value0, T value1, ...);
						/**< Contents are initialized using the arguments. There must be
						exactly numCols*numRows T arguments (the first two are 
						explicit to avoid ambiguities with the other constructors). */

						XArray(const XArray& rhs);
						
			XArray& 	operator=(const XArray& rhs);
																		
//-----------------------------------
//	API
//
public:
	//! @name Dimensions
	//@{		
			XRect	 	GetShape() const						{return mShape;}
			
			XSize 		GetSize() const							{return mShape.GetSize();}
			int32 		GetWidth() const						{return mShape.GetWidth();}
			int32 		GetHeight() const						{return mShape.GetHeight();}
			
			bool 		IsEmpty() const							{return mShape == kZeroRect;}
			
			void 		SetShape(const XRect& shape);
			void 		SetShape(const XRect& shape, const T& newValue);
			
			void 		SetSize(const XSize& dim)						{this->SetShape(XRect(kZeroPt, dim));}
			void 		SetSize(const XSize& dim, const T& newValue)	{this->SetShape(XRect(kZeroPt, dim), newValue);}
	//@}

	//! @name Access
	//@{
			const T& 	operator()(int32 col, int32 row) const;
			T& 			operator()(int32 col, int32 row);
						
			const T& 	Get(int32 col, int32 row) const			{return this->operator()(col, row);}
			T& 			Get(int32 col, int32 row)				{return this->operator()(col, row);}
						
			void 		Set(const T& elem)						{std::fill_n(mElements, mShape.GetArea(), elem);}
			void 		Set(int32 col, int32 row, const T& elem){this->operator()(col, row) = elem;}

			void 		Force(int32 col, int32 row, const T& elem);
						/**< If the (col, row) is outside the array the array will be expanded. */
									
			void 		Clear()									{this->SetShape(kZeroRect);}
	//@}
			
	//! @name Comparison Operators
	//@{
			bool 		operator==(const XArray& rhs) const;
			bool 		operator!=(const XArray& rhs) const		{return !this->operator==(rhs);}
	//@}

	//! @name Streaming
	//@{
#if MSVC >= 1100
	friend 	XInStream& 	operator>>(XInStream& stream, XArray<T>& c);		// these are defined in XStreaming.h
	friend	XOutStream& operator<<(XOutStream& stream, const XArray<T>& c);
#else
	friend 	XInStream& 	operator>> <T>(XInStream& stream, XArray<T>& c);	// these are defined in XStreaming.h
	friend	XOutStream& operator<< <T>(XOutStream& stream, const XArray<T>& c);
#endif
	//@}

//-----------------------------------
//	Internal API
//
private:
			T* 			DoAllocate(const XRect& shape, const T& initial);
			void 		DoSetShape(T* elements, const XRect& shape);

//-----------------------------------
//	Member Data
//
private:
	XRect		mShape;	
	T*			mElements;			// can't use vector because it won't preserve col/row when resizing
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper


#include <XArray.inc>

