/*
 *  File:       XPrefSpecializations.h
 *  Summary:    XPreference specializations for some common types.
 *  Written by: Jesse Jones
 *
 *	Types:		XHandle, std::list, std::vector, std::map
 *				XFileSpec, XFolderSpec (these two are in the Files project)
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XPrefSpecializations.h,v $
 *		Revision 1.2  2001/04/27 09:17:37  jesjones
 *		Fixed some Windows compile errors.
 *		
 *		Revision 1.1  2001/04/27 04:15:14  jesjones
 *		Checked in
 */
 
#include <XHandle.h>
 
#if INCLUDE_PREF_SPECIALIZATIONS	// this file should only be included by <XPreference.h>

namespace Whisper {

// ===================================================================================
//	XHandle
//		Used for binary data. This should not be used for large amounts of data.
// ===================================================================================
template <> 
class CORE_EXPORT XPreference<XHandle> {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
							operator const XHandle*() const	{return &mData;}
							operator XHandle*()				{return &mData;}
						
			const XHandle&	operator*() const				{return mData;}
			XHandle&		operator*()						{return mData;}
						
			const XHandle*	operator->() const				{return &mData;}
			XHandle*		operator->()					{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	XHandle			mData;
	XHandle			mOldData;
};


// ===================================================================================
//	std::list
// ===================================================================================
template <class T> 
class XPreference<std::list<T> > {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
						operator const std::list<T>*() const	{return &mData;}
						operator std::list<T>*()				{return &mData;}
						
			const std::list<T>&	operator*() const				{return mData;}
			std::list<T>&		operator*()						{return mData;}
						
			const std::list<T>*	operator->() const				{return &mData;}
			std::list<T>*		operator->()					{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	std::list<T>	mData;
	std::list<T>	mOldData;
};


template <class T> 
XPreference<std::list<T> >::~XPreference()
{
	this->Flush();
}


template <class T> 
XPreference<std::list<T> >::XPreference(const std::wstring& name, PrefType type) : mName(name)
{	
	mType = type;
	
	try {
#if TARGET_CARBON
		MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
		
		CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
		CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
		CFTypeRef dataRef = CFPreferencesCopyValue(ToCarbonStr(mName), appRef, userRef, kCFPreferencesAnyHost);
		if (dataRef != nil) {
			MCFObject<CFArrayRef> data((CFArrayRef) dataRef);
		
			if (CFArrayGetTypeID() == CFGetTypeID(*data)) {		// we only write out CFArray's so this should match...
				int32 count = CFArrayGetCount(data.Get());
				
				mData.clear();
				
				for (int32 i = 0; i < count; ++i) {
					CFStringRef str = (CFStringRef) CFArrayGetValueAtIndex(data.Get(), i);
					
					T value;
					FromStr(FromCarbonStr(str), value);								
					mData.push_back(value);
				}
				
			} else
				DEBUGSTR("XPreference data wasn't CFArray!");
		}
		
#elif WIN
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());

		mData.clear();
				
		uint32 i = 0;
		while (true) {
			std::wstring name, str;			
			bool found = key.EnumStrings(i++, name, str);
			if (!found)
				break;
				
			mData.push_back(FromCarbonStr(str));
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


template <class T> 
void XPreference<std::list<T> >::Flush()
{	
	try {
		if (mData != mOldData) {
#if TARGET_CARBON
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			MCFObject<CFMutableArrayRef> data(CFArrayCreateMutable(kCFAllocatorSystemDefault, (int32) mData.size(), &kCFTypeArrayCallBacks));
			if (data) {
				for (std::list<T>::const_iterator iter = mData.begin(); iter != mData.end(); ++iter) {
					std::wstring str = ToStr(*iter);	
					CFArrayAppendValue(data.Get(), (CFStringRef) ToCarbonStr(str));											
				}
				
				CFPreferencesSetValue(ToCarbonStr(mName), data.Get(), appRef, userRef, kCFPreferencesAnyHost);			
				VERIFY(CFPreferencesAppSynchronize(appRef));

			} else
				DEBUGSTR("Couldn't create the CFMutableArray!");
			
#elif WIN
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
			
			uint32 i = 0;		
			for (std::list<T>::const_iterator iter = mData.begin(); iter != mData.end(); ++iter) {
				std::wstring name = mName + ToStr(i++);	
				std::wstring str = ToStr(*iter);	
				key.WriteString(name, str);					
			}
#endif
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


// ===================================================================================
//	std::vector
// ===================================================================================
template <class T> 
class XPreference<std::vector<T> > {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
						operator const std::vector<T>*() const	{return &mData;}
						operator std::vector<T>*()				{return &mData;}
						
			const std::vector<T>&	operator*() const			{return mData;}
			std::vector<T>&			operator*()					{return mData;}
						
			const std::vector<T>*	operator->() const			{return &mData;}
			std::vector<T>*			operator->()				{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	std::vector<T>	mData;
	std::vector<T>	mOldData;
};


template <class T> 
XPreference<std::vector<T> >::~XPreference()
{
	this->Flush();
}


template <class T> 
XPreference<std::vector<T> >::XPreference(const std::wstring& name, PrefType type) : mName(name)
{	
	mType = type;
	
	try {
#if TARGET_CARBON
		MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
		
		CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
		CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
		CFTypeRef dataRef = CFPreferencesCopyValue(ToCarbonStr(mName), appRef, userRef, kCFPreferencesAnyHost);
		if (dataRef != nil) {
			MCFObject<CFArrayRef> data((CFArrayRef) dataRef);
		
			if (CFArrayGetTypeID() == CFGetTypeID(*data)) {		// we only write out CFArray's so this should match...
				int32 count = CFArrayGetCount(data.Get());
				
				mData.resize(0);
				mData.reserve((uint32) count);
				
				for (int32 i = 0; i < count; ++i) {
					CFStringRef str = (CFStringRef) CFArrayGetValueAtIndex(data.Get(), i);
					
					T value;
					FromStr(FromCarbonStr(str), value);								
					mData.push_back(value);
				}
				
			} else
				DEBUGSTR("XPreference data wasn't CFArray!");
		}
		
#elif WIN
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());

		mData.resize(0);
				
		uint32 index = 0;
		while (true) {
			std::wstring name, str;			
			bool found = key.EnumStrings(index++, name, str);
			if (!found)
				break;
				
			mData.push_back(FromCarbonStr(str));
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


template <class T> 
void XPreference<std::vector<T> >::Flush()
{	
	try {
		if (mData != mOldData) {
#if TARGET_CARBON
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			MCFObject<CFMutableArrayRef> data(CFArrayCreateMutable(kCFAllocatorSystemDefault, (int32) mData.size(), &kCFTypeArrayCallBacks));
			if (data) {
				for (uint32 i = 0; i < mData.size(); ++i) {
					std::wstring str = ToStr(mData[i]);	
					CFArrayAppendValue(data.Get(), (CFStringRef) ToCarbonStr(str));											
				}
				
				CFPreferencesSetValue(ToCarbonStr(mName), data.Get(), appRef, userRef, kCFPreferencesAnyHost);			
				VERIFY(CFPreferencesAppSynchronize(appRef));

			} else
				DEBUGSTR("Couldn't create the CFMutableArray!");
			
#elif WIN
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
														
			for (uint32 i = 0; i < mData.size(); ++i) {
				std::wstring name = mName + ToStr(i);	
				std::wstring str = ToStr(mData[i]);	
				key.WriteString(name, str);					
			}
#endif
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


// ===================================================================================
//	std::map
// ===================================================================================
template <class K, class V> 
class XPreference<std::map<K, V> > {

public:
						~XPreference();
	
						XPreference(const std::wstring& name, PrefType type = kUserPref);
						
public:
						operator const std::map<K, V>*() const	{return &mData;}
						operator std::map<K, V>*()				{return &mData;}
						
			const std::map<K, V>&	operator*() const			{return mData;}
			std::map<K, V>&			operator*()					{return mData;}
						
			const std::map<K, V>*	operator->() const			{return &mData;}
			std::map<K, V>*			operator->()				{return &mData;}
			
			void 		Flush();
	
private:
	std::wstring	mName;	
	PrefType		mType;				
	
	std::map<K, V>	mData;
	std::map<K, V>	mOldData;
};


template <class K, class V> 
XPreference<std::map<K, V> >::~XPreference()
{
	this->Flush();
}


template <class K, class V> 
XPreference<std::map<K, V> >::XPreference(const std::wstring& name, PrefType type) : mName(name)
{	
	mType = type;
	
	try {
#if TARGET_CARBON
		MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
		
		CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
		CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
		CFTypeRef dataRef = CFPreferencesCopyValue(ToCarbonStr(mName), appRef, userRef, kCFPreferencesAnyHost);
		if (dataRef != nil) {
			MCFObject<CFDictionaryRef> data((CFDictionaryRef) dataRef);
		
			if (CFDictionaryGetTypeID() == CFGetTypeID(*data)) {		// we only write out CFDictionary's so this should match...
				uint32 count = (uint32) CFDictionaryGetCount(data.Get());
				
				std::vector<CFStringRef> keys(count);		// only other way to get this is with CFDictionaryApplyFunction...
				std::vector<CFStringRef> values(count);
				CFDictionaryGetKeysAndValues(data.Get(), (const void**) &keys[0], (const void**) &values[0]);
				
				mData.clear();
				for (uint32 i = 0; i < count; ++i) {					
					K key;
					FromStr(FromCarbonStr(keys[i]), key);								

					V value;
					FromStr(FromCarbonStr(values[i]), value);								

					mData.insert(std::map<K, V>::value_type(key, value));
				}
				
			} else
				DEBUGSTR("XPreference data wasn't CFDictionaryRef!");
		}
		
#elif WIN
		WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());

		mData.clear();
				
		uint32 index = 0;
		while (true) {
			std::wstring name, str;			
			bool found = key.EnumStrings(index++, name, str);
			if (!found)
				break;
				
			K k;
			FromStr(name, k);								

			V v;
			FromStr(str, v);								

			mData.insert(std::map<K, V>::value_type(k, v));
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


template <class K, class V> 
void XPreference<std::map<K, V> >::Flush()
{	
	try {
		if (mData != mOldData) {
#if TARGET_CARBON
			MCoreString appName = ToCarbonStr(XSystemInfo::GetAppName());
			
			CFStringRef appRef  = mType == kSystemPref ? kCFPreferencesAnyApplication : (CFStringRef) appName;
			CFStringRef userRef = mType == kUserPref ? kCFPreferencesCurrentUser : kCFPreferencesAnyUser;
			
			MCFObject<CFMutableDictionaryRef> data(CFDictionaryCreateMutable(kCFAllocatorSystemDefault, (int32) mData.size(), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
			if (data) {
				for (std::map<K, V>::const_iterator iter = mData.begin(); iter != mData.end(); ++iter) {
					const std::wstring key   = ToStr(iter->first);
					const std::wstring value = ToStr(iter->second);
					
					CFDictionaryAddValue(data.Get(), (CFStringRef) ToCarbonStr(key), (CFStringRef) ToCarbonStr(value));											
				}
				
				CFPreferencesSetValue(ToCarbonStr(mName), data.Get(), appRef, userRef, kCFPreferencesAnyHost);			
				VERIFY(CFPreferencesAppSynchronize(appRef));

			} else
				DEBUGSTR("Couldn't create the CFMutableDictionary!");
			
#elif WIN
			WRegistryKey key(mType.key, mType.subKey != nil ? mType.subKey : WSystemInfo::GetPrefsName().c_str());
														
			for (std::map<K, V>::const_iterator iter = mData.begin(); iter != mData.end(); ++iter) {
				std::wstring name = ToStr(iter->first);	
				std::wstring str = ToStr(iter->second);	
				key.WriteString(name, str);					
			}
#endif
			
			mOldData = mData;
		}
		
	} catch (...) {
		// eat exceptions (see above for rationale)
	}
}


}		// namespace Whisper
#endif	// INCLUDE_PREF_SPECIALIZATIONS