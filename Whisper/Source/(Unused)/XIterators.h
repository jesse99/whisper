/*
 *  File:       XIterators.h
 *  Summary:   	Concrete iterator classes.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XIterators.h,v $
 *		Revision 1.2  2000/11/09 12:40:39  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/15/00	JDJ		Created
 */

#pragma once

#include <IIterators.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class IForwardIterator
// ===================================================================================
template <class T>
class IForwardIterator : public IForwardIterator<T> {

public:		 		
	virtual T& 			Get() const = 0;
	
	virtual void 		Advance() = 0;
	
	virtual int32 		Compare(const IForwardIterator<T>& rhs) const = 0;
	
private:
	std::vector<T>::iterator	mIter;
};



#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
