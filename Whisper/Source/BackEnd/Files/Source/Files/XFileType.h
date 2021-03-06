/*
 *  File:       XFileType.h
 *  Summary:   	A class used to encapsulate the notion of a "file type". 
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XFileType.h,v $
 *		Revision 1.5  2001/05/06 10:42:25  jesjones
 *		Tweaked comments.
 *		
 *		Revision 1.4  2001/03/05 05:41:38  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:35  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:36:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XFileType
//!		A class used to encapsulate the notion of a "file type". 
// ===================================================================================
class FILES_EXPORT XFileType {
	
//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XFileType();
	
  						XFileType();
  						/**< Matches nothing. */

  						XFileType(const std::wstring& name, const std::wstring& extension);
  						/**< For example, L"Header Files", L"h;hpp;inl" or L"All Files", L"*". */

  						XFileType(const std::wstring& name, const std::wstring& extension, OSType creator, OSType type);
  						
#if MAC
  						XFileType(OSType creator, OSType type);
#endif

  						XFileType(const XFileType& rhs);

//-----------------------------------
//	Inherited API
//
public:
			const std::wstring& GetName() const						{return mName;}
			const std::wstring& GetExtension() const				{return mExtension;}

			OSType 		GetCreator() const							{return mCreator;}
			OSType 		GetType() const								{return mType;}
			
			uint32 		Match(const XFileType& rhs) const			{return this->Match(rhs.mExtension, rhs.mType);}
			uint32 		Match(const std::wstring& extension, OSType type) const;
						/**< Returns 0 for no match, 1 for extension or type match,
						and 2 if both match. */
			
//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mName;
	std::wstring	mExtension;

	OSType			mCreator;
	OSType			mType;	
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
