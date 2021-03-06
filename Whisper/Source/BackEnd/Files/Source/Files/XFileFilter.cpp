/*
 *  File:		XFileFilter.cpp
 *  Summary:	Functor used for filtering files.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFileFilter.cpp,v $
 *		Revision 1.4  2001/04/27 04:32:36  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/06 07:32:44  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/09 12:35:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XFileFilter.h>

#include <XDebug.h>
#include <XFileSpec.h>

namespace Whisper {


// ===================================================================================
//	class XFileFilter
// ===================================================================================

//---------------------------------------------------------------
//
// XFileFilter::~XFileFilter
//
//---------------------------------------------------------------
XFileFilter::~XFileFilter()
{
}


//---------------------------------------------------------------
//
// XFileFilter::XFileFilter 
//
//---------------------------------------------------------------
XFileFilter::XFileFilter()
{
}


//---------------------------------------------------------------
//
// XFileFilter::AddTypes
//
//---------------------------------------------------------------
void XFileFilter::AddTypes(const XFileType* begin, const XFileType* end)
{
	PRECONDITION(begin != nil);
	PRECONDITION(end != nil);
	PRECONDITION(begin <= end);
	
	const XFileType* iter = begin;
	while (iter != end) {
		mTypes.push_back(*iter);
		++iter;
	}
}


//---------------------------------------------------------------
//
// XFileFilter::operator()
//
//---------------------------------------------------------------
bool XFileFilter::operator()(const XFileSpec& spec) const
{
	bool display = true;
	
	if (!mTypes.empty()) {
		display = false;
	
		OSType type = 0;
		std::wstring extension = spec.GetExtension();
		
#if MAC
		type = spec.GetType();
#endif

		std::list<XFileType>::const_iterator iter = mTypes.begin();
		while (iter != mTypes.end() && !display) {
			const XFileType& fileType = *iter;
			++iter;
			
			display = fileType.Match(extension, type) != 0;
		}
	}
				
	return display;
}


}	// namespace Whisper
