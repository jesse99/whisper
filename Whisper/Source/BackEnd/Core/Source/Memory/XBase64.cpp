/*
 *  File:       XBase64.cpp
 *  Summary:   	Routines to convert binary data to and from 7-bit ASCII strings.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBase64.cpp,v $
 *		Revision 1.1  2001/04/27 04:15:03  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <XBase64.h>

#include <XNumbers.h>

namespace Whisper {


//-----------------------------------
//	Internal Variables
//
static char encodingTable[64] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};


// ===================================================================================
//	Internal Functions
//		This code is based on Dave Winer's code from "www.scripting.com/midas/base64".
// ===================================================================================

//---------------------------------------------------------------
//
// Encode
//
//---------------------------------------------------------------
static std::string Encode(const void* buffer, uint32 bytes, int16 linelength) 
{ 
	PRECONDITION(linelength <= 76);		// JDJ required by RFC2045 

	std::string text;
	
	uint32 inbuf[3], outbuf[4];
	int16 charsonline = 0;
	uint32 ixtext = 0;
	uint32 lentext = bytes;
	
	const unsigned char* data = static_cast<const unsigned char*>(buffer);

	while (true) {
		if (lentext <= ixtext)
			break;

		for (uint32 i = 0; i < 3; i++) { 
			uint32 ix = ixtext + i;

			if (ix < lentext)
				inbuf [i] = data[ix];
			else
				inbuf [i] = 0;
		}

		outbuf[0] = (inbuf[0] & 0xFC) >> 2;
		outbuf[1] = ((inbuf[0] & 0x03) << 4) | ((inbuf[1] & 0xF0) >> 4);
		outbuf[2] = ((inbuf[1] & 0x0F) << 2) | ((inbuf[2] & 0xC0) >> 6);
		outbuf[3] = inbuf[2] & 0x3F;

		uint32 origsize = text.length();
		text.resize(origsize + 4);				// $$ this looks inefficient

		uint32 ctremaining = lentext - ixtext;

		int16 ctcopy = 4;
		switch (ctremaining) {
			case 1: 
				ctcopy = 2; 
				break;

			case 2: 
				ctcopy = 3; 
				break;
		} 

		for (uint32 i = 0; i < ctcopy; i++)	
			text[origsize + i] = encodingTable[outbuf[i]];

		for (uint32 i = ctcopy; i < 4; i++)
			text[origsize + i] = '=';

		ixtext += 3;
		charsonline += 4;

		if (linelength > 0) { 					// DW 4/8/97 -- 0 means no line breaks
			if (charsonline >= linelength) {
				charsonline = 0;

				origsize = text.length();
				text.resize(origsize + 1);

				text[origsize] = '\n';
			}
		}
	} 

	return text;
}


//---------------------------------------------------------------
//
// Decode
//
//---------------------------------------------------------------
static XHandle Decode(const std::string& text) 
{ 
	XHandle data;
	
	uint32 inbuf[4], outbuf[3];
	bool flendtext = false;

	uint32 ixtext = 0;
	uint32 lentext = text.length();
	int16 ixinbuf = 0;

	while (true) {
		if (ixtext >= lentext)
			break;

		bool flignore = false;

		char ch = text[ixtext++];
		if (ch >= 'A' && ch <= 'Z')
			ch = numeric_cast<char>(ch - 'A');

		else if (ch >= 'a' && ch <= 'z')
			ch = numeric_cast<char>(ch - 'a' + 26);

		else if (ch >= '0' && ch <= '9')
			ch = numeric_cast<char>(ch - '0' + 52);

		else if (ch == '+')
			ch = 62;

		else if (ch == '=') 		// no op -- can't ignore this one
			flendtext = true;

		else if (ch == '/')
			ch = 63;

		else
			flignore = true; 		// JDJ seems lame, but this is required by RFC2045

		if (!flignore) {
			int16 ctcharsinbuf = 3;
			bool flbreak = false;

			if (flendtext) {
				if (ixinbuf == 0)
					break;

				if ((ixinbuf == 1) || (ixinbuf == 2))
					ctcharsinbuf = 1;
				else
					ctcharsinbuf = 2;

				ixinbuf = 3;

				flbreak = true;
			}

			inbuf[ixinbuf++] = ch;

			if (ixinbuf == 4) {
				ixinbuf = 0;

				outbuf[0] = (inbuf[0] << 2) | ((inbuf[1] & 0x30) >> 4);
				outbuf[1] = ((inbuf[1] & 0x0F) << 4) | ((inbuf[2] & 0x3C) >> 2);
				outbuf[2] = ((inbuf[2] & 0x03) << 6) | (inbuf[3] & 0x3F);

				uint32 origsize = data.GetSize();
				data.SetSize(origsize + ctcharsinbuf);		// $$ this looks inefficient

				for (uint32 i = 0; i < ctcharsinbuf; i++) 
					data.GetUnsafePtr()[origsize + i] = numeric_cast<unsigned char>(outbuf[i]);
			}

			if (flbreak)
				break;
		}
	} 

	return data;
} 

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// EncodeBase64
//
//---------------------------------------------------------------
std::string EncodeBase64(const void* buffer, uint32 bytes)
{
	PRECONDITION(buffer != nil || bytes == 0);
	
	std::string text = Encode(buffer, bytes, 60);
		
	return text;
}


//---------------------------------------------------------------
//
// DecodeBase64
//
//---------------------------------------------------------------
XHandle DecodeBase64(const std::string& text)
{
	XHandle data = Decode(text);
	
	return data;
}


}	// namespace Whisper

