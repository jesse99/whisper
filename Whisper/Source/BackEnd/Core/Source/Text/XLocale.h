/*
 *  File:		XLocale.h
 *  Summary:	Encapsulates a region and a language (eg US English).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLocale.h,v $
 *		Revision 1.4  2000/12/10 08:53:47  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:35:32  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 12:42:08  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 2/14/99	JDJ		Created
 */

#pragma once

#include <string>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Types
//
#if MAC
	typedef int32 OSLocale;			//!< region code
#elif WIN
	typedef LCID OSLocale;
#endif


// ========================================================================================
//	class XLocale
//!		Encapsulates a region and a language (eg US English).
// ========================================================================================
class CORE_EXPORT XLocale {

//-----------------------------------
//	Initialization/Destruction
//
public:
						XLocale();
						/**< Initialized to the current user's OS locale. */
						
						XLocale(OSLocale locale);

						XLocale(const std::wstring& language, const std::wstring& region);

//-----------------------------------
//	API
//
public:
			const std::wstring& GetLanguage() const					{return mLanguage;}
			const std::wstring& GetRegion() const					{return mRegion;}
			
			void 		SetLanguage(const std::wstring& language)	{mLanguage = language;}
			void 		SetRegion(const std::wstring& region)		{mRegion = region;}
			
			bool 		operator==(const XLocale& rhs) const	{return mLanguage == rhs.mLanguage && mRegion == rhs.mRegion;}
			bool 		operator!=(const XLocale& rhs) const	{return !this->operator==(rhs);}
						
			bool 		operator<(const XLocale& rhs) const		{return mLanguage < rhs.mLanguage || (mLanguage == rhs.mLanguage && mRegion < rhs.mRegion);}

//-----------------------------------
//	Internal API
//
protected:
			std::wstring DoGetRegion(OSLocale locale);
			std::wstring DoGetLanguage(OSLocale locale);
	
//-----------------------------------
//	Member Data
//
protected:
	std::wstring		mLanguage;
	std::wstring		mRegion;
};
	
	
#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
