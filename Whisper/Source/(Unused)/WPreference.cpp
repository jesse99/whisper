/*
 *  File:       WPreference.cpp
 *  Summary:    Low level Windows pref class that stores strings (XPreference is
 *				usually a better choice).
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WPreference.cpp,v $
 *		Revision 1.2  2000/11/09 12:10:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/12/00	JDJ		Created (from XPreference)
 */

#include <XWhisperHeader.h>
#include <WPreference.h>

#include <WRegistryKey.h>
#include <WSystemInfo.h>

namespace Whisper {


// ===================================================================================
//	class WPreference
// ===================================================================================

//---------------------------------------------------------------
//
// WPreference::~WPreference
//
//---------------------------------------------------------------
WPreference::~WPreference()
{
	this->Flush();
}


//---------------------------------------------------------------
//
// WPreference::WPreference (wstring, wstring)
//
//---------------------------------------------------------------
WPreference::WPreference(const std::wstring& name, const std::wstring& defaultValue) : mName(name), mKey(HKEY_LOCAL_MACHINE), mSubKey(WSystemInfo::GetPrefsName()), mData(defaultValue)
{	
	try {
		WRegistryKey key(mKey, mSubKey);
		if (key.HasValue(mName))
			mData = key.ReadString(mName);
		
		mOldData = mData;
			
	} catch (...) {
	}
}


//---------------------------------------------------------------
//
// WPreference::WPreference (wstring, wstring, HKEY, wstring)
//
//---------------------------------------------------------------
WPreference::WPreference(const std::wstring& name, const std::wstring& defaultValue, HKEY key, const std::wstring& subKey) : mName(name), mKey(key), mSubKey(subKey), mData(defaultValue)
{	
	try {
		WRegistryKey key(mKey, mSubKey);
		if (key.HasValue(mName))
			mData = key.ReadString(mName);
		
		mOldData = mData;
			
	} catch (...) {
	}
}


//---------------------------------------------------------------
//
// WPreference::Flush
//
//---------------------------------------------------------------
void WPreference::Flush()
{	
	try {
		if (mData != mOldData) {
			WRegistryKey key(mKey, mSubKey);			
			key.WriteString(mName, mData);
			
			mOldData = mData;
		}
		
	} catch (...) {
	}
}


}	// namespace Whisper
