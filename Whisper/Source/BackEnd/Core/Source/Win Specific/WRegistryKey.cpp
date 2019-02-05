/*
 *  File:       WRegistryKey.cpp
 *  Summary:    A simple wrapper around the Windows Registry.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WRegistryKey.cpp,v $
 *		Revision 1.6  2001/11/25 23:38:01  jesjones
 *		Fixed a silly bug in ReadBinary.
 *		
 *		Revision 1.5  2001/04/27 09:19:28  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:33:30  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/03/03 23:58:25  jesjones
 *		Added calls to DEBUGSTR in catch (...) blocks.
 *		
 *		Revision 1.2  2000/11/09 12:11:23  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 4/06/99	JDJ		Fixed a bad bug in the Unicode case of WriteString.
 *		 <2>	 4/06/99	JDJ		Fixed a bug in ReadHandle that was introduced after
 *									adding code to use the wide interfaces under NT.
 *		 <1>	 1/12/98	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <WRegistryKey.h>

#include <WSystemInfo.h>
#include <XExceptions.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class ZOpenKey
// ===================================================================================
class ZOpenKey {

public:
						~ZOpenKey();
						
						ZOpenKey(HKEY parent, const std::wstring& subKey, uint32 options = REG_OPTION_NON_VOLATILE, REGSAM securityAccess = KEY_ALL_ACCESS);
	
						operator HKEY()								{return mKey;}

private:
	HKEY	mKey;
};


//---------------------------------------------------------------
//
// ZOpenKey::~ZOpenKey
//
//---------------------------------------------------------------
ZOpenKey::~ZOpenKey()
{
	(void) RegCloseKey(mKey);
	mKey = nil;
}


//---------------------------------------------------------------
//
// ZOpenKey::ZOpenKey
//
//---------------------------------------------------------------
ZOpenKey::ZOpenKey(HKEY parent, const std::wstring& subKey, uint32 options, REGSAM securityAccess)
{
	PRECONDITION(parent != nil);
	PRECONDITION(subKey.size() > 0);
	
	mKey = nil;
	
	uint32 disposition;
	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegCreateKeyExW(parent, subKey.c_str(), nil, const_cast<wchar_t*>(L""), options, securityAccess, nil, &mKey, &disposition);
	else
		err = RegCreateKeyExA(parent, ToPlatformStr(subKey).c_str(), nil, const_cast<char*>(""), options, securityAccess, nil, &mKey, &disposition);
	ThrowIf(err != ERROR_SUCCESS);
	
	POSTCONDITION(mKey != nil);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class WRegistryKey
// ===================================================================================

//---------------------------------------------------------------
//
// WRegistryKey::~WRegistryKey
//
//---------------------------------------------------------------
WRegistryKey::~WRegistryKey()
{
	mParent = nil;
}


//---------------------------------------------------------------
//
// WRegistryKey::WRegistryKey
//
//---------------------------------------------------------------
WRegistryKey::WRegistryKey(HKEY parent, const std::wstring& subKey, uint32 options, REGSAM securityAccess)
{
	PRECONDITION(parent != nil);
	PRECONDITION(subKey.size() > 0);
	
	mParent = parent;
	mSubKey = subKey;
	
	mOptions = options;
	mSecurityAccess = securityAccess;
}


//---------------------------------------------------------------
//
// WRegistryKey::HasValue
//
//---------------------------------------------------------------
bool WRegistryKey::HasValue(const std::wstring& name) const
{
	PRECONDITION(name.size() > 0);
	
	uint32 type = REG_NONE;
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegQueryValueExW(key, name.c_str(), nil, &type, nil, 0);
	else
		err = RegQueryValueExA(key, ToPlatformStr(name).c_str(), nil, &type, nil, 0);
	bool has = err == ERROR_SUCCESS;
	
	return has;
}


//---------------------------------------------------------------
//
// WRegistryKey::ReadInt
//
//---------------------------------------------------------------
uint32 WRegistryKey::ReadInt(const std::wstring& name) const
{	
	PRECONDITION(name.size() > 0);
	
	uint32 value;

	uint32 type  = REG_DWORD;
	uint32 bytes = sizeof(value);
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegQueryValueExW(key, name.c_str(), nil, &type, (uint8*) &value, &bytes);
	else
		err = RegQueryValueExA(key, ToPlatformStr(name).c_str(), nil, &type, (uint8*) &value, &bytes);
	ThrowIf(err != ERROR_SUCCESS || bytes != sizeof(value));
	ASSERT(type == REG_DWORD);
	
	return value;
}


//---------------------------------------------------------------
//
// WRegistryKey::ReadString
//
//---------------------------------------------------------------
std::wstring WRegistryKey::ReadString(const std::wstring& name) const
{
	PRECONDITION(name.size() > 0);
	
	std::wstring str;

	uint32 type = REG_SZ;
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode()) {
		wchar_t buffer[1024];
		uint32 bytes = sizeof(buffer);		

		err = RegQueryValueExW(key, name.c_str(), nil, &type, (uint8*) &buffer, &bytes);
		ThrowIf(err != ERROR_SUCCESS);

		str = buffer;
	
	} else {
		char buffer[1024];
		uint32 bytes = sizeof(buffer);		

		err = RegQueryValueExA(key, ToPlatformStr(name).c_str(), nil, &type, (uint8*) &buffer, &bytes);
		ThrowIf(err != ERROR_SUCCESS);

		str = FromPlatformStr(buffer);
	}
	
	ASSERT(type == REG_SZ);
	
	return str;
}


//---------------------------------------------------------------
//
// WRegistryKey::ReadHandle
//
//---------------------------------------------------------------
XHandle WRegistryKey::ReadHandle(const std::wstring& name) const
{
	PRECONDITION(name.size() > 0);
	COMPILE_CHECK(sizeof(VALENTA) == sizeof(VALENTW));
	
	XHandle value;
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	PlatformString str = ToPlatformStr(name);

	VALENTW values[1];
	values[0].ve_valuename = WSystemInfo::HasUnicode() ? (wchar_t*) name.c_str() : (wchar_t*) str.c_str();
	values[0].ve_valuelen  = 0;
	values[0].ve_valueptr  = nil;
	values[0].ve_type      = REG_NONE;
	
	uint32 type      = REG_BINARY;
	uint32 totalSize = 0;
	uint32 outBytes;
	
	if (WSystemInfo::HasUnicode()) {
		int32 err = RegQueryMultipleValuesW(key, values, 1, nil, &totalSize);
		ThrowIf(err != ERROR_SUCCESS && err != ERROR_MORE_DATA);
		
		value.SetSize(totalSize);
		outBytes  = totalSize;

		err = RegQueryValueExW(key, name.c_str(), nil, &type, value.GetUnsafePtr(), &outBytes);
		ThrowIf(err != ERROR_SUCCESS || outBytes != totalSize);
	
	} else {
		int32 err = RegQueryMultipleValuesA(key, (VALENTA*) values, 1, nil, &totalSize);
		ThrowIf(err != ERROR_SUCCESS && err != ERROR_MORE_DATA);
		
		value.SetSize(totalSize);
		outBytes  = totalSize;

		err = RegQueryValueExA(key, ToPlatformStr(name).c_str(), nil, &type, value.GetUnsafePtr(), &outBytes);
		ThrowIf(err != ERROR_SUCCESS || outBytes != totalSize);
	}
	
	ASSERT(type == REG_BINARY);

	return value;
}


//---------------------------------------------------------------
//
// WRegistryKey::ReadBinary
//
//---------------------------------------------------------------
void WRegistryKey::ReadBinary(const std::wstring& name, void* buffer, uint32 inBytes) const
{
	PRECONDITION(name.size() > 0);
	PRECONDITION(buffer != nil);
	
	uint32 type     = REG_BINARY;
	uint32 outBytes = inBytes;
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegQueryValueExW(key, name.c_str(), nil, &type, (uint8*) buffer, &outBytes);
	else
		err = RegQueryValueExA(key, ToPlatformStr(name).c_str(), nil, &type, (uint8*) buffer, &outBytes);
	ThrowIf(err != ERROR_SUCCESS || outBytes != inBytes);
	ASSERT(type == REG_BINARY);
}


//---------------------------------------------------------------
//
// WRegistryKey::DeleteValue
//
//---------------------------------------------------------------
void WRegistryKey::DeleteValue(const std::wstring& name)
{
	PRECONDITION(name.size() > 0);
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);
	
	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegDeleteValueW(key, name.c_str());
	else
		err = RegDeleteValueA(key, ToPlatformStr(name).c_str());
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::WriteInt
//
//---------------------------------------------------------------
void WRegistryKey::WriteInt(const std::wstring& name, uint32 value)
{
	PRECONDITION(name.size() > 0);
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegSetValueExW(key, name.c_str(), nil, REG_DWORD, (uint8*) &value, sizeof(value));
	else
		err = RegSetValueExA(key, ToPlatformStr(name).c_str(), nil, REG_DWORD, (uint8*) &value, sizeof(value));
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::WriteString
//
//---------------------------------------------------------------
void WRegistryKey::WriteString(const std::wstring& name, const std::wstring& value)
{
	PRECONDITION(name.size() > 0);
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegSetValueExW(key, name.c_str(), nil, REG_SZ, (uint8*) value.c_str(), (value.length() + 1)*sizeof(wchar_t));
	else
		err = RegSetValueExA(key, ToPlatformStr(name).c_str(), nil, REG_SZ, (uint8*) ToPlatformStr(value).c_str(), value.length() + 1);
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::WriteHandle
//
//---------------------------------------------------------------
void WRegistryKey::WriteHandle(const std::wstring& name, const XHandle& value)
{
	PRECONDITION(name.size() > 0);
	PRECONDITION(value.GetSize() < 4*1024);		// large data shouldn't be stored in the registry
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegSetValueExW(key, name.c_str(), nil, REG_BINARY, value.GetUnsafePtr(), value.GetSize());
	else
		err = RegSetValueExA(key, ToPlatformStr(name).c_str(), nil, REG_BINARY, value.GetUnsafePtr(), value.GetSize());
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::WriteBinary
//
//---------------------------------------------------------------
void WRegistryKey::WriteBinary(const std::wstring& name, const void* buffer, uint32 bytes)
{
	PRECONDITION(name.size() > 0);
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < 4*1024);					// large data shouldn't be stored in the registry
	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);

	int32 err;
	if (WSystemInfo::HasUnicode())
		err = RegSetValueExW(key, name.c_str(), nil, REG_BINARY, (uint8*) buffer, bytes);
	else
		err = RegSetValueExA(key, ToPlatformStr(name).c_str(), nil, REG_BINARY, (uint8*) buffer, bytes);
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::Flush
//
//---------------------------------------------------------------
void WRegistryKey::Flush()
{	
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);
	
	int32 err = RegFlushKey(key);
	ThrowIf(err != ERROR_SUCCESS);
}


//---------------------------------------------------------------
//
// WRegistryKey::EnumInts
//
//---------------------------------------------------------------
bool WRegistryKey::EnumInts(uint32 index, std::wstring& name, uint32& value)
{
	bool found = false;
	
	try {
		ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);
		
		uint32 type, temp;
		if (WSystemInfo::HasUnicode()) {
			wchar_t nameBuffer[256];
	
			uint32 nameBytes  = sizeof(nameBuffer);		
			uint32 valueBytes = sizeof(temp);
	
			int32 result = ::RegEnumValueW(key, index, nameBuffer, &nameBytes, nil, &type, (uint8*) &temp, &valueBytes);
			
			if (result == ERROR_SUCCESS && type == REG_DWORD) {
				name  = nameBuffer;
				value = temp;
				found = true;
			}

		} else {
			char nameBuffer[256];
	
			uint32 nameBytes  = sizeof(nameBuffer);		
			uint32 valueBytes = sizeof(temp);
	
			int32 result = ::RegEnumValueA(key, index, nameBuffer, &nameBytes, nil, &type, (uint8*) &temp, &valueBytes);
			
			if (result == ERROR_SUCCESS && type == REG_DWORD) {
				name = FromPlatformStr(nameBuffer);
				value = temp;
				found = true;
			}
		}
				
	} catch (...) {
		DEBUGSTR("Got an exception in WRegistryKey::EnumInts");		// trap exceptions
	}
	
	return found;
}

			
//---------------------------------------------------------------
//
// WRegistryKey::EnumStrings
//
//---------------------------------------------------------------
bool WRegistryKey::EnumStrings(uint32 index, std::wstring& name, std::wstring& value)
{
	bool found = false;
	
	try {
		ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);
		
		uint32 type;
		if (WSystemInfo::HasUnicode()) {
			wchar_t nameBuffer[256];
			wchar_t valueBuffer[256];
	
			uint32 nameBytes  = sizeof(nameBuffer);
			uint32 valueBytes = sizeof(valueBuffer);
			
			int32 result = ::RegEnumValueW(key, index, nameBuffer, &nameBytes, nil, &type, (uint8*) valueBuffer, &valueBytes);
			
			if (result == ERROR_SUCCESS && type == REG_SZ) {
				name  = nameBuffer;
				value = valueBuffer;
				found = true;
			}

		} else {
			char nameBuffer[256];
			char valueBuffer[256];
	
			uint32 nameBytes  = sizeof(nameBuffer);
			uint32 valueBytes = sizeof(valueBuffer);
			
			int32 result = ::RegEnumValueA(key, index, nameBuffer, &nameBytes, nil, &type, (uint8*) valueBuffer, &valueBytes);
			
			if (result == ERROR_SUCCESS && type == REG_SZ) {
				name = FromPlatformStr(nameBuffer);
				value = FromPlatformStr(valueBuffer);
				found = true;
			}
		}
				
	} catch (...) {
		DEBUGSTR("Got an exception in WRegistryKey::EnumStrings");	// trap exceptions
	}
	
	return found;
}


//---------------------------------------------------------------
//
// WRegistryKey::DeleteValues
//
//---------------------------------------------------------------
void WRegistryKey::DeleteValues()
{
	ZOpenKey key(mParent, mSubKey, mOptions, mSecurityAccess);
	
	while (true) {
		if (WSystemInfo::HasUnicode()) {
			wchar_t name[256];
			uint32 bytes = sizeof(name);
			uint32 type;
			
			int32 result = ::RegEnumValueW(key, 0, name, &bytes, nil, &type, nil, nil);
			if (result == ERROR_SUCCESS) {
				int32 err = ::RegDeleteValueW(key, name);
				ThrowIf(err != ERROR_SUCCESS);
				
			} else if (result == ERROR_NO_MORE_ITEMS)
				break;
				
			else
				ThrowIfErr();
		
		} else {
			char name[256];
			uint32 bytes = sizeof(name);
			uint32 type;
			
			int32 result = ::RegEnumValueA(key, 0, name, &bytes, nil, &type, nil, nil);
			if (result == ERROR_SUCCESS) {
				int32 err = ::RegDeleteValueA(key, name);
				ThrowIf(err != ERROR_SUCCESS);
				
			} else if (result == ERROR_NO_MORE_ITEMS)
				break;
				
			else
				ThrowIfErr();
		}
	}
}


}	// namespace Whisper
