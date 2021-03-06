/*
 *  File:       XPreference.h
 *  Summary:    High level preference class that operates like a persistent smart pointer.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPreference.h,v $
 *		Revision 1.9  2001/04/27 04:27:58  jesjones
 *		Moved the specializations into a seperate file.
 *		
 *		Revision 1.8  2001/04/19 04:13:39  jesjones
 *		Switched to using CFPreference. Added specializations for std::vector and std::map. Windows code uses ToStr and FromStr instead of specializing on primitive types.
 *		
 *		Revision 1.7  2001/04/13 08:03:24  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.6  2001/03/17 07:42:16  jesjones
 *		Moved the template specializations into the cpp.
 *		
 *		Revision 1.5  2001/03/02 11:24:57  jesjones
 *		Added overloaded ToStr and FromStr functions that are used in place of iostreams (eliminates a *lot* of object file bloat, but not too much exe bloat).
 *		
 *		Revision 1.4  2000/12/15 09:07:47  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/10 08:54:42  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:47:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTextConversions.h>
#include <XTranscode.h>

#if  WIN
	#include <WRegistryKey.h>
	#include <WSystemInfo.h>
#endif

#if TARGET_CARBON
	#include <CFBase.h>						// $$ CoreFoundation's wacky includes mess CW up so we'll include everything ourselves
	#include <CFArray.h>
	#include <CFDictionary.h>
	#include <CFString.h>
	#include <CFPreferences.h>
	#include <MCFObject.h>
	#include <XLocker.h>
	#include <XPointerStream.h>
	#include <XSystemInfo.h>
#endif

namespace Whisper {


//-----------------------------------
//	Types
//
#if MAC
	typedef int32 PrefType;
#elif WIN
	struct PrefType {
		HKEY 		   key;
		const wchar_t* subKey;				// if nil subKey will be from WSystemInfo and look something like "SOFTWARE\\CompanyName\\ProductName\\Version"
	};
#endif


//-----------------------------------
//	Constants
//
#if MAC
	const PrefType kUserPref   	= 1;		// pref for the current user (eg show/hide a tool window)
	const PrefType kAppPref    	= 0;		// app pref that works with any user (eg location of a data folder)
	const PrefType kSystemPref	= 2;		// pref that applies to multiple apps (eg a pref used within Whisper)
#elif WIN
	const PrefType kUserPref 	= {HKEY_CURRENT_USER, nil};		
	const PrefType kAppPref  	= {HKEY_LOCAL_MACHINE, nil};
	const PrefType kSystemPref 	= {HKEY_LOCAL_MACHINE, L"SOFTWARE\\Whisper\\"};
#endif


// ===================================================================================
//	class XPreference
//!		High level preference class that operates like a persistent smart pointer. 
/*!		It's usually better to use multiple XPreference objects parameterized on simple 
 *		types like ints and strings rather than on a struct containing individual preferences. 
 *		Using simple types is less work because you don't have to write streaming and 
 *		comparison operators and they work better with the Windows registry. For example, 
 *		your app might have preferences like these: \code
 *			XPreference<XSize>	mSwitchResolutionTo;
 *			XPreference<int32>	mSwitchDepthTo; \endcode
 *		In the app's initializor list you would have something like: \code
 *			mSwitchResolutionTo("Resolution", XSize(640, 480)),
 *			mSwitchDepthTo("Depth", 16); \endcode
 *		When these ctors execute the preference will be read in. If the preference doesn't 
 *		exist or there's an error the default values will be used. Once the objects have 
 *		been constructed you can treat them just like a pointer to an XSize and an int32. 
 *		If the preference is changed it will be written out when Flush is called or when 
 *		the object is destroyed. */
// ===================================================================================
template <class DATA> class XPreference {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XPreference();
						/**< Calls Flush. */
	
						XPreference(const std::wstring& name, const DATA& defaultValue, PrefType type = kUserPref);
						/**< Reads mData in. Exceptions are trapped. */
						
//-----------------------------------
//	API
//
public:
						operator const DATA*() const			{return &mData;}
						operator DATA*()						{return &mData;}
						
			const DATA&	operator*() const						{return mData;}
			DATA&		operator*()								{return mData;}
						
			const DATA*	operator->() const						{return &mData;}
			DATA*		operator->()							{return &mData;}
			
			void 		Flush();
						/**< Writes mData out. Exceptions are trapped. */
	
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	DATA			mData;
	DATA			mOldData;
};


// ===================================================================================
//	Outlined Methods
// ===================================================================================
template <class DATA> 
XPreference<DATA>::~XPreference()
{
	this->Flush();
}


template <class DATA> 
XPreference<DATA>::XPreference(const std::wstring& name, const DATA& defaultValue, PrefType type) : mName(name), mData(defaultValue)
{	
	mType = type;
	
	try {
#if TARGET_CARBON
		MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
		
		CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
		CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
		CFTypeRef dataRef = CFPreferencesCopyValue(ToCarbonStr(mName), appRef, userRef, kCFPreferencesAnyHost);
		if (dataRef != nil) {
			MCFObject<CFStringRef> data((CFStringRef) dataRef);
		
			if (CFStringGetTypeID() == CFGetTypeID(*data)) {		// we only write out CFString's so this should match...
				std::wstring str = FromCarbonStr(*data);
				FromStr(str, mData);								
				
			} else
				DEBUGSTR("XPreference data wasn't CFString!");
		}
		
#elif WIN
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
		if (key.HasValue(mName)) {							
			std::wstring str = key.ReadString(mName);			
			FromStr(str, mData);								
		}														
#endif
		
		mOldData = mData;
			
	} catch (...) {
		// eat exceptions (we do this because preference data is non-essential and
		// because it's quite likely that if one preference fails to save they all
		// will and the user would be very annoyed by five dialogs in a row telling
		// him some bit of preference data couldn't be saved).
	}
}


template <class DATA> 
void XPreference<DATA>::Flush()
{	
	try {
		if (mData != mOldData) {
#if TARGET_CARBON
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			MCoreString str = ToCarbonStr(ToStr(mData));												
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			CFPreferencesSetValue(ToCarbonStr(mName), str, appRef, userRef, kCFPreferencesAnyHost);			
			VERIFY(CFPreferencesAppSynchronize(appRef));
			
#elif WIN
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
														
			std::wstring str = ToStr(mData);												
			key.WriteString(mName, str);					
#endif
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


}	// namespace Whisper


#define INCLUDE_PREF_SPECIALIZATIONS	1
#include <XPrefSpecializations.h>
#undef INCLUDE_PREF_SPECIALIZATIONS
