/*
 *  File:		XFileFilter.h
 *  Summary:	Functor used for filtering files.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileFilter.h,v $
 *		Revision 1.4  2001/03/05 05:41:20  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:52:07  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:35:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XFileType.h>
#include <XReferenceCounted.h>
#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


// ===================================================================================
//	class XFileFilter
//!		Functor used for filtering files.		
// ===================================================================================
class FILES_EXPORT XFileFilter : public XReferenceCountedMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XFileFilter();
	
						XFileFilter();
						/**< Defaults to no filtering. */
 						
private:
						XFileFilter(const XFileFilter& rhs);
						
			XFileFilter& operator=(const XFileFilter& rhs);
												
//-----------------------------------
//	API
//
public:
	// ----- Types -----
			void 		ClearTypes()								{mTypes.clear();}
						/**< Base class won't filter out any files because of type. */
						
			void 		AddType(const XFileType& type)				{mTypes.push_back(type);}
			void 		AddTypes(const XFileType* begin, const XFileType* end);	// $$$ make this a template member
						/**< Base class will filter out any files with types differing
						from the specified type(s). */
												
	// ----- Filtering -----
	virtual bool 		operator()(const XFileSpec& spec) const;
						/**< Returns true if the file is OK. */
						
//-----------------------------------
//	Member Data
//
protected:
	std::list<XFileType>	mTypes;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
