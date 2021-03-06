/*
 *  File:		XSet.inc
 *  Summary:	Wrapper around an STL set that provides a more natural set-like interface.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSet.inc,v $
 *		Revision 1.2  2000/11/09 12:51:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	11/27/97	JDJ		Created (from Raven)
 */

namespace Whisper {


// ===================================================================================
//	class XSet
// ===================================================================================

//---------------------------------------------------------------
//
// XSet::operator+= (XSet)
//
//---------------------------------------------------------------
template <class T, class C>
XSet<T, C>& XSet<T, C>::operator+=(const XSet<T, C>& rhs)
{
	XSet<T, C>::const_iterator iter = rhs.begin();
	while (iter != rhs.end()) {
		const T& elem = *iter;
		
		mElements.insert(elem);
		
		++iter;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XSet::operator-= (XSet)
//
//---------------------------------------------------------------
template <class T, class C>
XSet<T, C>& XSet<T, C>::operator-=(const XSet<T, C>& rhs)
{
	if (mElements.size() > 0) {
		XSet<T, C>::const_iterator iter = rhs.begin();
		while (iter != rhs.end()) {
			const T& elem = *iter;
			
			if (mElements.count(elem) > 0)
				mElements.erase(elem);
			
			++iter;
		}
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XSet::operator&= (T)
//
//---------------------------------------------------------------
template <class T, class C>
XSet<T, C>& XSet<T, C>::operator&=(const T& rhs)
{
	if (mElements.size() > 0) {
		bool has = mElements.count(rhs) > 0;
		
		mElements.clear();
		mElements.insert(rhs);
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XSet::operator&= (XSet)
//
//---------------------------------------------------------------
template <class T, class C>
XSet<T, C>& XSet<T, C>::operator&=(const XSet<T, C>& rhs)
{
	if (mElements.size() > 0 && rhs.mElements.size() > 0) {
		std::set<T, C>::iterator iter = mElements.begin();
		while (iter != mElements.end()) {
			std::set<T, C>::iterator temp = iter;
			const T& elem = *iter++;
			
			if (rhs.count(elem) == 0)
				mElements.erase(temp);
		}
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XSet::contains (XSet)
//
//---------------------------------------------------------------
template <class T, class C>
bool XSet<T, C>::contains(const XSet<T, C>& rhs) const
{
	bool has = mElements.size() >= rhs.mElements.size();
	
	XSet<T, C>::const_iterator iter = rhs.begin();
	while (iter != rhs.end() && has) {
		const T& elem = *iter;
		
		if (mElements.count(elem) == 0)
			has = false;
		
		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// intersects
//
//---------------------------------------------------------------
template <class T, class C>
bool intersects(const XSet<T, C>& lhs, const XSet<T, C>& rhs)
{
	bool match = false;

	XSet<T, C>::const_iterator iter = rhs.begin();
	while (iter != rhs.end() && !match) {
		const T& elem = *iter;
		
		if (lhs.contains(elem))
			match = true;
		
		++iter;
	}
	
	return match;
}


}	// namespace Whisper

