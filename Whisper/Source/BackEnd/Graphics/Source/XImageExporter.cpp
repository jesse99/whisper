/*
 *  File:       XImageExporter.cpp
 *  Summary:	Abstract base class that can be used to write XBaseImage's out.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first): 
 *
 *		$Log: XImageExporter.cpp,v $
 *		Revision 1.2  2000/11/09 12:38:28  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	  9/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XImageExporter.h>

namespace Whisper {


// ===================================================================================
//	class XImageExporter
// ===================================================================================

//---------------------------------------------------------------
//
// XImageExporter::~XImageExporter
//
//---------------------------------------------------------------
XImageExporter::~XImageExporter()
{
}


//---------------------------------------------------------------
//
// XImageExporter::XImageExporter
//
//---------------------------------------------------------------
XImageExporter::XImageExporter()
{	
}


}	// namespace Whisper

