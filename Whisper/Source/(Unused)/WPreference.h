/*
 *  File:       WPreference.h
 *  Summary:    Low level Windows pref class that stores strings (XPreference is
 *				usually a better choice).
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WPreference.h,v $
 *		Revision 1.2  2000/11/09 12:10:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/12/00	JDJ		Created (from XPreference)
 */

#pragma once

#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class WPreference
// ===================================================================================
class CORE_EXPORT WPreference {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~WPreference();
	
						WPreference(const std::wstring& name, const std::wstring& defaultValue);
						// Key defaults to HKEY_LOCAL_MACHINE and subKey defaults to
						// WSystemInfo::GetPrefsName().

						WPreference(const std::wstring& name, const std::wstring& defaultValue, HKEY key, const std::wstring& subKey);
						
//-----------------------------------
//	API
//
public:
						  operator const std::wstring*() const		{return &mData;}
						  operator std::wstring*()					{return &mData;}
						
			const std::wstring& operator*() const					{return mData;}
			std::wstring&	   operator*()							{return mData;}
					
			const std::wstring* operator->() const					{return &mData;}
			std::wstring*	   operator->()							{return &mData;}
			
			void 		Flush();
	
//-----------------------------------
//	Member Data
//
private:
	std::wstring		mName;	
	HKEY				mKey;	
	std::wstring		mSubKey;
				
	std::wstring		mData;
	std::wstring		mOldData;
};


}	// namespace Whisper
