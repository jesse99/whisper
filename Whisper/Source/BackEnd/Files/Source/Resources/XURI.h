/*
 *	File:		XURI.h
 *	Function:	Encapsulates a Uniform Resource Identifier.
 *	Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XURI.h,v $
 *		Revision 1.4  2000/12/10 08:57:20  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.3  2000/11/20 05:38:08  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 12:58:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	  2/06/99	JDJ		Created
 */

#pragma once

#include <string>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;
class XFolderSpec;


// ===================================================================================
//	class XURI
//!		Encapsulates a Uniform Resource Identifier.
// ===================================================================================
class FILES_EXPORT XURI {
			
//-----------------------------------
//	Initialization/Destruction
//
public:	
						~XURI();			

						XURI();			

	explicit			XURI(const std::wstring& uri);			
						/**< The uri must be an absolute URI: it should start with a scheme 
						(eg "file", "ftp", "http", "resource", etc) followed by "://" 
						and a path. For example, "file://Hendrix/Whisper/Documents/"
						and "ftp://ftp.is.co.za/rfc/rfc1808.txt". */
						
						XURI(const XURI& baseURI, const std::wstring& uri);
						/**< Uri is treated as an absolute URI if it starts with a scheme
						or a '/' (in which case the scheme is assumed to be that
						of the base URI). Otherwise the uri is relative to baseURI
						and looks something like: "To%20Do.txt", "./To%20Do.txt", 
						or "../Headers/XWhisperHeader.h". */
						
						XURI(const XFileSpec& spec);

						XURI(const XFolderSpec& spec);

//-----------------------------------
//	API 
//
public:
			std::wstring GetAddress() const;
			
			const std::wstring& GetScheme() const					{return mScheme;}
			
			const std::wstring& GetPath() const						{return mPath;}
						/**< Returns the URI sans the scheme and the "://". Note that this is 
						always an absolute path. */
	
			std::wstring GetFile() const;
						/**< Returns the text after the last '/' (may be empty). */
						
			bool 		operator==(const XURI& rhs) const		{return mScheme == rhs.mScheme && mPath == rhs.mPath;}
			bool 		operator!=(const XURI& rhs) const		{return !this->operator==(rhs);}
			bool 		operator<(const XURI& rhs) const		{return mScheme < rhs.mScheme || (mScheme == rhs.mScheme && mPath < rhs.mPath);}

//-----------------------------------
//	Internal Functions
//
protected:
			std::wstring DoUnescape(const std::wstring& inName);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring		mScheme;
	std::wstring		mPath;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
