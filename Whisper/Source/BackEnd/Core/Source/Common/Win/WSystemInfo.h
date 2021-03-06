/*
 *  File:       WSystemInfo.h
 *	Summary:	Provides information about the system.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WSystemInfo.h,v $
 *		Revision 1.4  2001/03/05 05:40:22  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:55  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:12:22  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XDebug.h>
#include <XTypes.h>

namespace Whisper {


// ===================================================================================
//	class WSystemInfo		
//!		Provides information about the system.
// ===================================================================================
class CORE_EXPORT WSystemInfo {

//-----------------------------------
//	API
//
public:
	//! @name App Info
	//@{
	static	HINSTANCE	GetAppInstance();
						/**< Note that this can also be used if you need the process's
						module handle. */
						
	static 	std::wstring GetAppPath();

	static 	std::wstring GetAppName();
	//@}

	//! @name OS Version
	//@{
	static	bool		IsWin32s();

	static	bool		IsWin32();
						/**< Returns true if we're running NT or Win 95 or later. */

	static	bool		IsWin95OSR2();
						/**< Returns true if we're running Win 95 OSR2 or later. */

	static	bool		IsWin98();
						/**< Returns true if we're running Win 98 or later. */

	static	bool		IsWin2000();
						/**< Returns true if we're running NT 5 or later. */

	static	bool		IsNT();

	static	uint8		GetMajorOSVersion();

	static	uint8		GetMinorOSVersion();
	//@}

	//! @name System Info
	//@{
	static	bool		HasUnicode()							{return WSystemInfo::IsNT();}

	static	uint32		GetPageSize();
						/**< Returns the size of the pages used by VM. */
	//@}

	//! @name Preferences
	//@{
	static	std::wstring GetPrefsName()							{ASSERT(msPrefsName.size() > 0); return msPrefsName;}

	static	void		SetPrefsName(const std::wstring& name);
						/**< Call this when your app is being initialized. You'll usually 
						want to use something like: "SOFTWARE\\CompanyName\\ProductName\\Version". */
	//@}
						
//-----------------------------------
//	Member Data
//
protected:
	static std::wstring		msPrefsName;
};

}	// namespace Whisper
