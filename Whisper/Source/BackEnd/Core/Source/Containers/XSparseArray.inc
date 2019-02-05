/*
 *  File:		XSparseArray.inc
 *  Summary:	2D array class that's optimized for most elements being empty.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSparseArray.inc,v $
 *		Revision 1.2  2000/11/09 12:52:16  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 6/11/99	JDJ		Added typename to Find methods (for Pro 5fc3).
 *		 <2>	12/17/98	JDJ		Fixed some minor compile errors.
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

namespace Whisper {


// ===================================================================================
//	class XSparseArray
// ===================================================================================

//---------------------------------------------------------------
//
// XSparseArray::~XSparseArray
//
//---------------------------------------------------------------
template <class T>
XSparseArray<T>::~XSparseArray()
{
}


//---------------------------------------------------------------
//
// XSparseArray::XSparseArray (T)
//
//---------------------------------------------------------------
template <class T>
XSparseArray<T>::XSparseArray(const T& initial) : mDefaultValue(initial)
{
	mShape = kZeroRect;
}


//---------------------------------------------------------------
//
// XSparseArray::XSparseArray (XSize, T)
//
//---------------------------------------------------------------
template <class T>
XSparseArray<T>::XSparseArray(const XSize& size, const T& initial) : mDefaultValue(initial)
{
	mShape = XRect(kZeroPt, size);
}


//---------------------------------------------------------------
//
// XSparseArray::XSparseArray (XRect, T)
//
//---------------------------------------------------------------
template <class T>
XSparseArray<T>::XSparseArray(const XRect& shape, const T& initial) : mDefaultValue(initial)
{
	mShape = shape;
}


//---------------------------------------------------------------
//
// XSparseArray::SetShape
//
//---------------------------------------------------------------
template <class T>
void XSparseArray<T>::SetShape(const XRect& shape)
{
	if (shape != mShape) {
		if (!shape.Contains(mShape)) {						
			if (shape == kZeroRect)
				mElements.clear();
				
			else {
				ElementTable::iterator iter = mElements.begin();
				while (iter != mElements.end()) {
					ElementTable::iterator temp = iter;
					const ElementTable::value_type& value = *iter++;
					
					if (!shape.Contains(value.first) || value.second == mDefaultValue)	// may as well reduce the table while we're iterating over the elements
						mElements.erase(temp);
				}
			}
		}
		
		mShape = shape;
	}		
}


//---------------------------------------------------------------
//
// XSparseArray::operator() const
//
//---------------------------------------------------------------
template <class T>
const T& XSparseArray<T>::operator()(int32 col, int32 row) const
{
	XPoint cell(col, row);
	ASSERT(mShape.Contains(cell));
	
	ElementTable::const_iterator iter = mElements.find(cell);
	if (iter != mElements.end())
		return (*iter).second;
	else
		return mDefaultValue;
}


//---------------------------------------------------------------
//
// XSparseArray::operator()
//
//---------------------------------------------------------------
template <class T>
T& XSparseArray<T>::operator()(int32 col, int32 row)
{
	XPoint cell(col, row);
	ASSERT(mShape.Contains(cell));
	
	ElementTable::iterator iter = mElements.find(cell);
	if (iter != mElements.end())
		return (*iter).second;
		
	else {
		ElementTable::value_type value(cell, mDefaultValue);
		
		pair<ElementTable::iterator, bool> result = mElements.insert(value);
		ASSERT(result.second);
			
		return (*result.first).second;
	}
}


//---------------------------------------------------------------
//
// XSparseArray::Force
//
//---------------------------------------------------------------
template <class T>
void XSparseArray<T>::Force(int32 col, int32 row, const T& elem)
{
	XPoint cell(col, row);
	
	if (!mShape.Contains(cell)) {
		XRect newShape;
		newShape.left   = Min(mShape.left, col);
		newShape.right  = Max(mShape.right, col + 1L);
		newShape.top    = Min(mShape.top, row);
		newShape.bottom = Max(mShape.bottom, row + 1L);
		
		this->SetShape(newShape);
	}

	mElements[cell] = elem;
}


//---------------------------------------------------------------
//
// XSparseArray::Find const
//
//---------------------------------------------------------------
template <class T>
#if !MSVC
typename 
#endif
XSparseArray<T>::const_iterator XSparseArray<T>::Find(int32 col, int32 row) const
{
	const_iterator iter = this->end();
	
	XPoint cell(col, row);
	
	if (mShape.Contains(cell)) {
		ElementTable::const_iterator iter2 = mElements.find(cell);
		if (iter2 != mElements.end() && (*iter2).second != mDefaultValue)
			iter = const_iterator(&mElements, iter2, mDefaultValue);
	}
	
	return iter;
}


//---------------------------------------------------------------
//
// XSparseArray::Find
//
//---------------------------------------------------------------
template <class T>
#if !MSVC
typename 
#endif
XSparseArray<T>::iterator XSparseArray<T>::Find(int32 col, int32 row)
{
	iterator iter = this->end();
	
	XPoint cell(col, row);
	
	if (mShape.Contains(cell)) {
		ElementTable::iterator iter2 = mElements.find(cell);
		if (iter2 != mElements.end() && (*iter2).second != mDefaultValue)
			iter = iterator(&mElements, iter2, mDefaultValue);
	}
	
	return iter;
}


//---------------------------------------------------------------
//
// XSparseArray::Reduce
//
//---------------------------------------------------------------
template <class T>
void XSparseArray<T>::Reduce()
{
	ElementTable::iterator iter = mElements.begin();
	while (iter != mElements.end()) {
		ElementTable::iterator temp = iter;
		const ElementTable::value_type& value = *iter++;
		
		if (value.second == mDefaultValue)
			mElements.erase(temp);
	}
}


}	// namespace Whisper

