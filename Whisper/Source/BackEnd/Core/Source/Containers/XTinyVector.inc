/*
 *  File:       XTinyVector.inc
 *  Summary:   	Simple very lightweight vector class.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTinyVector.inc,v $
 *		Revision 1.2  2000/11/09 12:56:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

namespace Whisper {


// ===================================================================================
//	class XTinyVector
// ===================================================================================

//---------------------------------------------------------------
//
// XTinyVector::XTinyVector (XTinyVector)
//
//---------------------------------------------------------------
template <class T>
XTinyVector<T>::XTinyVector(const XTinyVector& rhs)
{	
	mSize = rhs.mSize;
	mData = new T[mSize];
	
	STD::copy(rhs.begin(), rhs.end(), this->begin());
}


//---------------------------------------------------------------
//
// XTinyVector::operator=
//
//---------------------------------------------------------------
template <class T>
XTinyVector<T>& XTinyVector<T>::operator=(const XTinyVector& rhs)
{	
	if (mData != rhs.mData) {
		T* newData = new T[rhs.mSize];
		STD::copy(rhs.begin(), rhs.end(), newData);
		
		delete [] mData;

		mSize = rhs.mSize;
		mData = newData;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XTinyVector::operator==
//
//---------------------------------------------------------------
template <class T>
bool XTinyVector<T>::operator==(const XTinyVector& rhs) const
{	
	bool equal = mSize == rhs.mSize && STD::equal(this->begin(), this->end(), rhs.begin());

	return equal;
}


//---------------------------------------------------------------
//
// XTinyVector::operator<
//
// $$ This is defined out of line, because when it is inline CW Pro 2
// $$ requires T to have an operator< even when the vector's operator<
// $$ is never called.
//
//---------------------------------------------------------------
template <class T>
bool XTinyVector<T>::operator<(const XTinyVector& rhs) const	
{	
	return STD::lexicographical_compare(this->begin(), this->end(), rhs.begin(), rhs.end());
}


//---------------------------------------------------------------
//
// XTinyVector::swap
//
//---------------------------------------------------------------
template <class T>
void XTinyVector<T>::swap(XTinyVector<T>& rhs)
{	
	std::swap(mSize, rhs.mSize);
	std::swap(mData, rhs.mData);
}


}	// namespace Whisper

