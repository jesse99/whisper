/*
 *  File:		XArray.inc
 *  Summary:	Templatized array class.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XArray.inc,v $
 *		Revision 1.3  2001/05/12 11:16:36  jesjones
 *		Added IsEmpty. SetSize and SetShape now take an optional initial value. Switched to allocating raw memory for slightly better performance.
 *		
 *		Revision 1.2  2000/11/09 12:25:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <cstdarg>

namespace Whisper {


// ===================================================================================
//	class XArray
// ===================================================================================

//---------------------------------------------------------------
//
// XArray::~XArray
//
//---------------------------------------------------------------
template <class T>
XArray<T>::~XArray()
{
	operator delete(mElements);
}


//---------------------------------------------------------------
//
// XArray::XArray (XSize)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XSize& dim) : mShape(kZeroPt, dim)
{
	mElements = this->DoAllocate(mShape, T());
}


//---------------------------------------------------------------
//
// XArray::XArray (XRect)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XRect& shape) : mShape(shape)
{		
	mElements = this->DoAllocate(mShape, T());
}


//---------------------------------------------------------------
//
// XArray::XArray (XSize, T)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XSize& dim, const T& initial) : mShape(kZeroPt, dim)
{		
	mElements = this->DoAllocate(mShape, initial);
}


//---------------------------------------------------------------
//
// XArray::XArray (XRect, T)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XRect& shape, const T& initial) : mShape(shape)
{
	mElements = this->DoAllocate(mShape, initial);
}


//---------------------------------------------------------------
//
// XArray::XArray (XSize, T, T, ...)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XSize& dim, T value0, T value1, ...) : mShape(kZeroPt, dim)
{
	ASSERT(dim.width >= 2);
	ASSERT(dim.height >= 1);
	
	mElements = nil;
			
	try {
		mElements = this->DoAllocate(mShape, T());	// need to do this to remain exception safe
		
		T* buffer = mElements;
		
		*buffer++ = value0;
		*buffer++ = value1;

		va_list ap;
		va_start(ap, value1);						// second argument is last fixed parameter

		for (int32 index = 2; index < mShape.GetArea(); index++) {
			T value = va_arg(ap, T);	

			*buffer++ = value;					
		}
		
		va_end(ap);
		
	} catch (...) {
		operator delete(mElements);
		throw;
	}
}

		
//---------------------------------------------------------------
//
// XArray::XArray (XArray)
//
//---------------------------------------------------------------
template <class T>
XArray<T>::XArray(const XArray& rhs) : mShape(shape)
{		
	mElements = (T*) operator new(mShape.GetArea()*sizeof(T));
	
	try {
		std::uninitialized_copy(rhs.mElements, rhs.mElements + mShape.GetArea(), mElements);
		
	} catch (...) {
		operator delete(mElements);
		throw;
	}
}


//---------------------------------------------------------------
//
// XArray::operator=
//
//---------------------------------------------------------------
template <class T>
XArray<T>& XArray<T>::operator=(const XArray& rhs)
{
	if (this != &rhs) {		
		T* elements = (T*) operator new(rhs.mShape.GetArea()*sizeof(T));
		
		try {
			std::uninitialized_copy(rhs.mElements, rhs.mElements + rhs.mShape.GetArea(), elements);
			
		} catch (...) {
			operator delete(elements);
			throw;
		}

		operator delete(mElements);

		mElements = elements;
		mShape = rhs.mShape;
	}
	
	return *this;
}

												
//---------------------------------------------------------------
//
// XArray::operator==
//
//---------------------------------------------------------------
template <class T>
bool XArray<T>::operator==(const XArray& rhs) const
{
	bool equal = mShape == rhs.mShape && ::equal(mElements, mElements + mShape.GetArea(), rhs.mElements);
	
	return equal;
}
			
			
//---------------------------------------------------------------
//
// XArray::SetShape (XRect)
//
//---------------------------------------------------------------
template <class T>
void XArray<T>::SetShape(const XRect& shape)
{
	if (shape != mShape) {
		T* elements = this->DoAllocate(shape, T()); 	// allocate a constructed array so we can destruct properly in the event of an exception
	
		this->DoSetShape(elements, shape);
	}
}


//---------------------------------------------------------------
//
// XArray::SetShape (XRect, T)
//
//---------------------------------------------------------------
template <class T>
void XArray<T>::SetShape(const XRect& shape, const T& newValue)
{
	if (shape != mShape) {
		T* elements = this->DoAllocate(shape, newValue);
	
		this->DoSetShape(elements, shape);
	}
}


//---------------------------------------------------------------
//
// XArray::operator() const
//
//---------------------------------------------------------------
template <class T>
const T& XArray<T>::operator()(int32 col, int32 row) const
{
	ASSERT(col >= mShape.left);
	ASSERT(row >= mShape.top);
	ASSERT(col < mShape.right);
	ASSERT(row < mShape.bottom);
	
	int32 h = col - mShape.left;
	int32 v = row - mShape.top;
	
	return mElements[h + v*mShape.GetWidth()];
}


//---------------------------------------------------------------
//
// XArray::operator()
//
//---------------------------------------------------------------
template <class T>
T& XArray<T>::operator()(int32 col, int32 row)
{
	ASSERT(col >= mShape.left);
	ASSERT(row >= mShape.top);
	ASSERT(col < mShape.right);
	ASSERT(row < mShape.bottom);
	
	int32 h = col - mShape.left;
	int32 v = row - mShape.top;
	
	return mElements[h + v*mShape.GetWidth()];
}


//---------------------------------------------------------------
//
// XArray::Force
//
//---------------------------------------------------------------
template <class T>
void XArray<T>::Force(int32 col, int32 row, const T& elem)
{
	XPoint cell(col, row);
	
	if (!mShape.Contains(cell)) {
		XRect newShape;
		newShape.left   = Min(mShape.left, col);
		newShape.right  = Max(mShape.right, col + 1);
		newShape.top    = Min(mShape.top, row);
		newShape.bottom = Max(mShape.bottom, row + 1);
		
		this->SetShape(newShape);
	}

	int32 h = col - mShape.left;
	int32 v = row - mShape.top;
	
	mElements[h + v*mShape.GetWidth()] = elem;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XArray::DoAllocate
//
//---------------------------------------------------------------
template <class T>
T* XArray<T>::DoAllocate(const XRect& shape, const T& initial)
{
	T* elements = (T*) operator new(shape.GetArea()*sizeof(T));

	try {
		std::uninitialized_fill_n(elements, shape.GetArea(), initial);
		
	} catch (...) {
		operator delete(elements);
		throw;
	}
	
	return elements;
}


//---------------------------------------------------------------
//
// XArray::DoSetShape
//
//---------------------------------------------------------------
template <class T>
void XArray<T>::DoSetShape(T* elements, const XRect& shape)
{
	ASSERT(shape != mShape);

	try {
		int32 startH = Max(shape.left, mShape.left);		
		int32 startV = Max(shape.top, mShape.top);		
		int32 lastH  = Min(shape.right, mShape.right);		
		int32 lastV  = Min(shape.bottom, mShape.bottom);		
		
		for (int32 v = startV; v < lastV; v++) {
			for (int32 h = startH; h < lastH; h++) {
				int32 oldIndex = (h - mShape.left) + (v - mShape.top)*mShape.GetWidth();
				int32 newIndex = (h - shape.left) + (v - shape.top)*shape.GetWidth();
				
				elements[newIndex] = mElements[oldIndex];
			}
		}
		
	} catch (...) {
		operator delete(elements);
		throw;
	}

	operator delete(mElements);

	mElements = elements;
	mShape = shape;
}


}	// namespace Whisper

