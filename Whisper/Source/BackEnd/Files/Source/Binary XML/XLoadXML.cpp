/*
 *  File:		XLoadXML.cpp
 *  Summary:	Streams in an XXMLDoc from an XURI.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLoadXML.cpp,v $
 *		Revision 1.3  2001/02/17 10:24:23  jesjones
 *		Added an overload that takes an XResource.
 *		
 *		Revision 1.2  2000/11/09 12:41:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 7/04/00	JDJ		Save always writes out in big endian format. Stream
 *									is now cooked.
 *		 <1>	 3/28/00	JDJ		Created (from XXMLDoc)
 */

#include <XWhisperHeader.h>
#include <XLoadXML.h>

#include <XAutoPtr.h>
#include <XCompress.h> 
#include <XHandleStream.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XResource.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <XURIResolver.h>
#include <XXMLDoc.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Load (XXMLDoc, XResource)
//
//---------------------------------------------------------------
void Load(XXMLDoc& xml, const XResource& data)
{
	XXMLDoc doc;
	
	{
	XLocker lock(data);	
		uint32* temp = (uint32*) data.GetPtr();
		XHandle unzipped;
		
#if !BIG_ENDIAN
		Whisper::ByteSwap(temp[0]);
		Whisper::ByteSwap(temp[1]);
		Whisper::ByteSwap(temp[2]);
#endif

		uint32 offset = 0;
		if (temp[1]) {
			unzipped.SetSize(temp[2]);
			XLocker lock(unzipped);
			
			uint32 dstLen = unzipped.GetSize();
			Unzip(data.GetPtr() + 3*sizeof(uint32), data.GetSize() - 3*sizeof(uint32), unzipped.GetPtr(), &dstLen);
			ASSERT(dstLen == unzipped.GetSize());

		} else {
			unzipped = data.GetHandle();
			offset = 3*sizeof(uint32);
		}
		
		{
		XInHandleStream stream(unzipped, offset, kCooked);	
			stream >> xml;
		}
	}
}



//---------------------------------------------------------------
//
// Load (XXMLDoc, XURI)
//
//---------------------------------------------------------------
void Load(XXMLDoc& xml, const XURI& uri)
{	
	XAutoPtr<XResource> data(XURIResolver::Instance()->Resolve(uri));
	if (data.Get() == nil)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the '#1' resource.", uri.GetPath())));

	Whisper::Load(xml, *data);
}


}	// namespace Whisper

