/*
 *  File:		XCreateByName.h
 *  Summary:	Mixin for objects that can be created from a string (using XObjectFactory).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XCreateByName.h,v $
 *		Revision 1.4  2001/03/05 05:40:58  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:51:06  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:29:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XCreateByNameMixin
//!		Mixin for objects that can be created from a string (using XObjectFactory).
// ===================================================================================
class CORE_EXPORT XCreateByNameMixin {
	
//-----------------------------------
//	Types
//
public:
	typedef XCreateByNameMixin* (*CreatorProcPtr)(XCreateByNameMixin* parent);

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XCreateByNameMixin();

						XCreateByNameMixin();
						
//-----------------------------------
//	API
//
public:
			 std::wstring GetClassName() const;
						/**< This is initialized using typeid (after the object is fully
						constructed). */
	
	virtual void 		SetClassName(const std::wstring& name);

//-----------------------------------
//	Member Data
//
private:
	mutable std::wstring	mClassName;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

