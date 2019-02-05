/*
 *  File:       WRegistryKey.h
 *  Summary:    A simple wrapper around the Windows Registry.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WRegistryKey.h,v $
 *		Revision 1.4  2001/03/05 05:40:15  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:51  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:11:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XHandle.h>

namespace Whisper {


// ===================================================================================
//	class WRegistryKey
//!		A simple wrapper around the Windows Registry.
// ===================================================================================
class CORE_EXPORT WRegistryKey {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~WRegistryKey();
						
						WRegistryKey(HKEY parent, const std::wstring& subKey, uint32 options = REG_OPTION_NON_VOLATILE, REGSAM securityAccess = KEY_ALL_ACCESS);
						/**< Opens (or creates) a subKey. parent will normally be
						HKEY_LOCAL_MACHINE for user independant prefs or HKEY_CURRENT_USER
						for user specific prefs. subKey will normally be something like:
						"SOFTWARE\\CompanyName\\ProductName\\Version". */
	
//-----------------------------------
//	API
//
public:
	//! @name Getters
	//@{
			bool 		HasValue(const std::wstring& name) const;

			uint32 		ReadInt(const std::wstring& name) const;

			std::wstring ReadString(const std::wstring& name) const;

			XHandle 	ReadHandle(const std::wstring& name) const;

			void 		ReadBinary(const std::wstring& name, void* buffer, uint32 bytes) const;
	//@}

	//! @name Setters
	//@{
			void 		WriteInt(const std::wstring& name, uint32 value);

			void 		WriteString(const std::wstring& name, const std::wstring& value);

			void 		WriteHandle(const std::wstring& name, const XHandle& value);

			void 		WriteBinary(const std::wstring& name, const void* buffer, uint32 bytes);
						/**< Note that large data should be stored in files and the path
						saved in the registry. */
	//@}
						
	//! @name Enumerating Values
	//@{
			bool 		EnumInts(uint32 index, std::wstring& name, uint32& value);
						/**< Returns all values with integer type in the key. Returns false
						when there's nothing left. */
						
			bool 		EnumStrings(uint32 index, std::wstring& name, std::wstring& value);
	//@}

	//! @name Misc
	//@{
			void 		DeleteValue(const std::wstring& name);

			void 		DeleteValues();
						/**< Deletes all the values in the key. */

			void 		Flush();
						/**< Flushs the key and it's subKeys out to disk. */
	//@}

//-----------------------------------
//	Member Data
//
private:
	HKEY			mParent;
	std::wstring	mSubKey;
	
	uint32			mOptions;
	REGSAM			mSecurityAccess;
};


}	// namespace Whisper
