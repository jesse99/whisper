/*
 *  File:       XPrefSpecializations.cpp
 *  Summary:    XPreference specializations for some common types.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XPrefSpecializations.cpp,v $
 *		Revision 1.1  2001/04/27 04:15:09  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <XPreference.h>

#include <XBase64.h>
#include <XLocker.h>

namespace Whisper {


// ===================================================================================
//	class XPreference<XHandle>
// ===================================================================================

//---------------------------------------------------------------
//
// XPreference<XHandle>::~XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XHandle>::~XPreference()
{
	this->Flush();
}


//---------------------------------------------------------------
//
// XPreference<XHandle>::XPreference
//
//---------------------------------------------------------------
template <> 
XPreference<XHandle>::XPreference(const std::wstring& name, PrefType type) : mName(name)
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
				mData = DecodeBase64(ToAsciiStr(str));
				
			} else
				DEBUGSTR("XPreference data wasn't CFString!");
		}
		
#elif WIN
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
		if (key.HasValue(mName)) {							
			std::wstring str = key.ReadString(mName);	
			mData = DecodeBase64(ToAsciiStr(str));
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


//---------------------------------------------------------------
//
// XPreference<XHandle>::Flush
//
//---------------------------------------------------------------
template <> 
void XPreference<XHandle>::Flush()
{	
	try {
		if (mData != mOldData) {
			XLocker lock(mData);
							
#if TARGET_CARBON
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			MCoreString str = ToCarbonStr(FromAsciiStr(EncodeBase64(mData.GetPtr(), mData.GetSize()).c_str()));												
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			CFPreferencesSetValue(ToCarbonStr(mName), str, appRef, userRef, kCFPreferencesAnyHost);			
			VERIFY(CFPreferencesAppSynchronize(appRef));
			
#elif WIN
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
										
			std::string str = EncodeBase64(mData.GetPtr(), mData.GetSize());												
			key.WriteString(mName, FromAsciiStr(str.c_str()));					
#endif
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


}	// namespace Whisper


