/*
 *  File:		XFolderFilter.h
 *  Summary:	Functor used for filtering folders.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFolderFilter.cpp,v $
 *		Revision 1.2  2000/11/09 12:36:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 7/30/98	JDJ		Created (from Raven).
 */
 
#include <XWhisperHeader.h>
#include <XFolderFilter.h>

#include <XDebug.h>
#include <XFolderSpec.h>

namespace Whisper {


// ===================================================================================
//	class XFolderFilter
// ===================================================================================

//---------------------------------------------------------------
//
// XFolderFilter::~XFolderFilter
//
//---------------------------------------------------------------
XFolderFilter::~XFolderFilter()
{
}


//---------------------------------------------------------------
//
// XFolderFilter::XFolderFilter
//
//---------------------------------------------------------------
XFolderFilter::XFolderFilter()
{
}


//---------------------------------------------------------------
//
// XFolderFilter::operator()
//
//---------------------------------------------------------------
bool XFolderFilter::operator()(const XFolderSpec& spec) const
{
	UNUSED(spec);
	
	return true;
}


}	// namespace Whisper
