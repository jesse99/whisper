/*
 *  File:       XTraceSink.cpp
 *  Summary:    Abstract base class for TRACE destinations.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTraceSink.cpp,v $
 *		Revision 1.3  2001/04/27 04:37:28  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:57:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */
 
#include <XWhisperHeader.h>
#include <XTraceSink.h>

#include <XDebug.h>

#if DEBUG
namespace Whisper {

using namespace std;


// ===================================================================================
//	class XTraceSink
// ===================================================================================

//---------------------------------------------------------------
//
// XTraceSink::~XTraceSink
//
//---------------------------------------------------------------
XTraceSink::~XTraceSink()
{
}


//---------------------------------------------------------------
//
// XTraceSink::XTraceSink
//
//---------------------------------------------------------------
XTraceSink::XTraceSink()
{
	mEnabled = true;
}


//---------------------------------------------------------------
//
// XTraceSink::HandlePrint
//
//---------------------------------------------------------------
void XTraceSink::HandlePrint(const wchar_t* str)		
{
	PRECONDITION(str != nil);
	
	if (mEnabled)
		this->OnPrint(str);
}


}		// namespace Whisper
#endif	// DEBUG