/*
 *  File:       XSingleton.h
 *  Summary:    Thread safe base class for singleton objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XSingleton.h,v $
 *		Revision 1.2  2000/11/09 12:51:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 1/23/00	JDJ		Created
 */

#pragma once

#include <XAutoPtr.h>
#include <XCriticalSection.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XSingleton
// ===================================================================================
template <class T>
class XSingleton {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XSingleton() = 0;

						XSingleton();

	static T* 			Instance();

private:
						XSingleton(const XSingleton& rhs);
	
			XSingleton& operator=(const XSingleton& rhs);

//-----------------------------------
//	Member Data
//
private:
	static XAutoPtr<T>			msInstance;			// $$$ when building DLL's each DLL gets a copy of this...
	static XCriticalSection		msCreateMutex;
};


// ===================================================================================
//	Outlined Methods
// ===================================================================================

template <class T> XAutoPtr<T>		XSingleton<T>::msInstance;		
template <class T> XCriticalSection XSingleton<T>::msCreateMutex;

template <class T>
XSingleton<T>::~XSingleton()
{
}


template <class T>
XSingleton<T>::XSingleton()
{
	ASSERT(msInstance.Get() == nil);
}


template <class T>
T* XSingleton<T>::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new T);
	}
	
	return msInstance.Get();
}


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

