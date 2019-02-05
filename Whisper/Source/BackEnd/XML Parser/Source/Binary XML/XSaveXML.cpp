/*
 *  File:		XSaveXML.cpp
 *  Summary:	Streams an XML file into a binary XML file.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XSaveXML.cpp,v $
 *		Revision 1.2  2000/11/09 12:50:13  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 8/08/00	JDJ		Tweaked the warning reporting code.
 *		 <2>	 7/04/00	JDJ		Always writes out in big endian format. Stream
 *									is now cooked.
 *		 <1>	 3/28/00	JDJ		Created (from XXMLDoc)
 */

#include <XWhisperHeader.h>
#include <XSaveXML.h>

#include <XAutoPtr.h>
#include <XCompress.h>
#include <XFile.h>
#include <XFileSystem.h>
#include <XHandleStream.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XResource.h>
#include <XStreaming.h>
#include <XXMLCallbacks.h>
#include <XXMLParser.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Save
//
//---------------------------------------------------------------
bool Save(const XFileSpec& inSpec, const XFileSpec& outSpec, bool compress)
{	
	bool parsed = false;
	
	XAutoPtr<XResource> data(new XResource(inSpec));

	// Get the CRC for the XML data
	uint32 xmlCRC;
	{
	XLocker lock(data.Get());
		xmlCRC = ComputeCRC(data->GetPtr(), data->GetSize());
	}
			
	// Get the CRC of the saved XML data
	uint32 binCRC = xmlCRC + 1;
	if (XFileSystem::FileExists(outSpec)) {
		XFile file(outSpec);
		file.Open(kReadPermission);
		
		if (file.GetLength() > 0) {
			file.Read(&binCRC, sizeof(binCRC));
#if !BIG_ENDIAN
			Whisper::ByteSwap(binCRC);
#endif
		}
			
		file.Close();
	}
	
	// If they differ we need to:  
	if (xmlCRC != binCRC || true) {
	
		// re-parse the XML,
		XXMLDoc doc;
//		TRACE("Parsing '%ls'\n", inSpec.GetPath().c_str());
		
		{
		XXMLParser parser(data.Get());
		XXMLDocumentCallback builder(doc);
			parser.Parse(builder);
			parsed = true;

#if DEBUG
			uint32 count = parser.GetNumWarnings();
			if (count > 0) {
				if (count == 1)
					TRACE("There was one warning:\n");
				else
					TRACE("There were ", count, " warnings:\n");
				
				for (uint32 index = 0; index < count; ++index)
					TRACE(parser.GetWarning(index), "\n");
			}
#endif
		}
		
		// stream it into a handle,
		XHandle hand;
		{
		XOutHandleStream stream(data->GetSize(), kCooked);
			stream << doc;
			hand = stream.GetHandle();
		}
		
		// zip the handle (this can save over a 100K for something like a string table),
		if (hand.GetSize() < 2*1024)
			compress = false;
			
		XHandle zipped;
		if (compress) {
			zipped.SetSize(GetMaxZippedBytes(hand.GetSize()));

			XLocker lock1(hand);
			uint32 dstLen = zipped.GetSize();
			{
			XLocker lock2(zipped);
				Zip(hand.GetPtr(), hand.GetSize(), zipped.GetPtr(), &dstLen);
			}
			zipped.SetSize(dstLen);
			
		} else
			zipped = hand;

		// and write the zipped data out to disk.
		{
		XFile file(outSpec);
		XLocker lock3(zipped);
			file.Open('Whsp', 'bXML', kWritePermission);
			file.SetLength(3*sizeof(uint32) + zipped.GetSize());		// improves the chances of the file being contiguous and may wind up with a smaller file if the old one was larger			
			file.SeekToStart();
			
			uint32 srcLen = hand.GetSize();
			uint32 temp = compress;
			
#if !BIG_ENDIAN
			Whisper::ByteSwap(xmlCRC);
			Whisper::ByteSwap(temp);
			Whisper::ByteSwap(srcLen);
#endif

			file.Write(&xmlCRC, sizeof(uint32));
			file.Write(&temp, sizeof(uint32));
			file.Write(&srcLen, sizeof(uint32));
			file.Write(zipped.GetPtr(), zipped.GetSize());
			
			file.Close();
		}
	}
	
	return parsed;
}


}	// namespace Whisper

