/*
 *  File:		MVolumeIterator.h
 *  Summary:	A class that iterates over volumes.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MVolumeIterator.h,v $
 *		Revision 1.2  2000/11/09 09:21:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <Iterator>

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MVolumeIterator
// ===================================================================================
class FILES_EXPORT MVolumeIterator : public std::iterator<std::forward_iterator_tag, int16> {

//-----------------------------------
//	Initialization/Destruction
//
public:
							MVolumeIterator();
							// For end.
					
							MVolumeIterator(bool allowRemote, bool onlineOnly = true);
							// Note that local volumes are always returned first.
							// If onlineOnly is set ejected and offline volumes are ignored.

//-----------------------------------
//	API
//
public:
			int16			operator*() const								{ASSERT(mIterating); return mVolume;}
	
			MVolumeIterator& operator++();
			MVolumeIterator operator++(int);
	
			bool 			operator==(const MVolumeIterator& rhs) const	{return mIndex == rhs.mIndex;}	
			bool 			operator!=(const MVolumeIterator& rhs) const	{return mIndex != rhs.mIndex;}

//-----------------------------------
//	Internal API
//
protected:
			void 			Advance();
		
//-----------------------------------
//	Member Data
//
private:
	bool		mAllowRemote;
	
	bool		mLocalOnly;
	bool		mRemoteOnly;
	bool		mOnlineOnly;
	
	int16		mIndex;
	bool		mIterating;
	
	int16		mVolume;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

