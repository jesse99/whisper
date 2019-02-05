/*
 *  File:		XStreaming.cpp
 *  Summary:	Streaming operators for some common types.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XStreaming.cpp,v $
 *		Revision 1.2  2000/11/09 12:52:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	12/11/99	JDJ		Added XFileSystemSpec streaming methods.
 *		 <2>	 2/19/99	JDJ		Added XDate streaming methods.
 *		 <1>	 1/11/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XStreaming.h>

#include <climits>

#include <XBinaryPersistent.h>
#include <XDate.h>
#include <XExceptions.h>
#include <XHandle.h>
#include <XHandleStream.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XObjectFactory.h>
#include <XPointer.h>
#include <XRect.h>
#include <XStringUtils.h>

namespace Whisper {


// ========================================================================================
//	Streaming Operators
// ========================================================================================

//---------------------------------------------------------------
//
// operator>> (char)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, char& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'CHAR');
		
	stream.ReadBytes(&value, sizeof(value));
	
	return stream;
}

				
//---------------------------------------------------------------
//
// operator>> (wchar_t)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, wchar_t& inValue)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'WIDC');
	
	int16 value;
	stream.ReadBytes(&value, sizeof(value));
	inValue = static_cast<wchar_t>(value);
	
	return stream;
}

				
//---------------------------------------------------------------
//
// operator>> (short)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, short& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'SHRT');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (int)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, int& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'INT ');
		
	long result;
	stream.ReadBytes(&result, sizeof(result));
	
	if (stream.NeedsByteSwap())
		ByteSwap(result);
	
	if (result > INT_MAX || result < INT_MIN)
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Read in an int that was too big.")));
	else
		value = (int) result;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (long)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, long& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'LONG');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (int64)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, int64& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'LLNG');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (float)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, float& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'FLT ');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (double)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, double& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'DBL ');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (unsigned char)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, unsigned char& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'UCHR');
		
	stream.ReadBytes(&value, sizeof(value));
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (unsigned short)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, unsigned short& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'USHT');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (unsigned int)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, unsigned int& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'UINT');
		
	uint32 result;
	stream.ReadBytes(&result, sizeof(result));
	
	if (stream.NeedsByteSwap())
		ByteSwap(result);
	
	if (result > UINT_MAX)
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Read in a unsigned int that was too big.")));
	else
		value = (unsigned int) result;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (unsigned long)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, unsigned long& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'ULNG');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (unint64)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, uint64& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'ULLG');
		
	stream.ReadBytes(&value, sizeof(value));
	
	if (stream.NeedsByteSwap())
		ByteSwap(value);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (bool)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, bool& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'BOOL');
		
	unsigned char result;
	stream.ReadBytes(&result, sizeof(result));
		
	if (result > 1)
		throw std::range_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Read in a bool that was too big.")));
	else
		value = (bool) result;
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (wstring)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, std::wstring& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'STR ');
		
	uint32 numBytes;
	stream.ReadBytes(&numBytes, sizeof(numBytes));
	
	if (stream.NeedsByteSwap())
		ByteSwap(numBytes);
	
	XPointer buffer(numBytes);
	stream.ReadBytes(buffer.GetPtr(), numBytes);

	wchar_t* text = (wchar_t*) buffer.GetPtr();
	uint32 numChars = numBytes/sizeof(wchar_t);
	
	if (stream.NeedsByteSwap()) {
		for (uint32 index = 0; index < numChars; ++index)
			ByteSwap(text[index]);
	}
	
	value.assign(text, numChars);
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XPoint)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XPoint& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'XPT ');
		
	stream.ReadBytes(&value.x, sizeof(value.x));
	stream.ReadBytes(&value.y, sizeof(value.y));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(value.x);
		ByteSwap(value.y);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XSize)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XSize& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'XSIZ');
		
	stream.ReadBytes(&value.width, sizeof(value.width));
	stream.ReadBytes(&value.height, sizeof(value.height));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(value.width);
		ByteSwap(value.height);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XRect)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XRect& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'XRCT');
		
	stream.ReadBytes(&value.top, sizeof(value.top));
	stream.ReadBytes(&value.left, sizeof(value.left));
	stream.ReadBytes(&value.bottom, sizeof(value.bottom));
	stream.ReadBytes(&value.right, sizeof(value.right));
	
	if (stream.NeedsByteSwap()) {
		ByteSwap(value.left);
		ByteSwap(value.right);
		ByteSwap(value.top);
		ByteSwap(value.bottom);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XHandle)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XHandle& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'HAND');
		
	uint32 size;
	stream.ReadBytes(&size, sizeof(size));
	
	if (stream.NeedsByteSwap())
		ByteSwap(size);

	value.SetSize(size);
	
	{
	XLocker lock(value);
		stream.ReadBytes(value.GetPtr(), size);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator>> (XDate)
//
//---------------------------------------------------------------
XInStream& operator>>(XInStream& stream, XDate& value)
{
	if (stream.IsTagged())
		VERIFY(stream.ReadTag() == 'Date');
		
	int64 time;
	stream.ReadBytes(&time, sizeof(time));
	
	if (stream.NeedsByteSwap())
		ByteSwap(time);
		
	XDate normalized(time);
	XDate base(1900, 1, 1);				
	value = base + normalized;
	
	return stream;
}

#if __MWERKS__
#pragma mark -
#endif

//---------------------------------------------------------------
//
// operator<< (char)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, char value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('CHAR');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}

				
//---------------------------------------------------------------
//
// operator<< (wchar_t)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, wchar_t value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('WIDC');
#endif

	int16 temp = static_cast<int16>(value);
	stream.WriteBytes(&temp, sizeof(temp));

	return stream;
}

				
//---------------------------------------------------------------
//
// operator<< (short)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, short value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('SHRT');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (int)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, int value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('INT ');
#endif

	long numb = value;
	stream.WriteBytes(&numb, sizeof(numb));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (long)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, long value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('LONG');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (int64)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, int64 value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('LLNG');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (float)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, float value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('FLT ');
#endif

	stream.WriteBytes(&value, sizeof(value));	

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (double)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, double value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('DBL ');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (unsigned char)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, unsigned char value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('UCHR');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (unsigned short)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, unsigned short value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('USRT');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (unsigned int)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, unsigned int value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('UINT');
#endif

	uint32 numb = value;
	stream.WriteBytes(&numb, sizeof(numb));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (unsigned long)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, unsigned long value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('ULNG');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (uint64)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, uint64 value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('ULLG');
#endif

	stream.WriteBytes(&value, sizeof(value));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (bool)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, bool value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('BOOL');
#endif

	unsigned char numb = value;							// sizeof(bool) is undefined so we'll stick it in a char
	stream.WriteBytes(&numb, sizeof(numb));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (wstring)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const std::wstring& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('STR ');
#endif

	uint32 size = value.length()*sizeof(wchar_t);
	stream.WriteBytes(&size, sizeof(size));
	
	stream.WriteBytes(value.c_str(), size);

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XPoint)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XPoint& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('XPT ');
#endif

	stream.WriteBytes(&value.x, sizeof(value.x));
	stream.WriteBytes(&value.y, sizeof(value.y));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XSize)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XSize& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('XSIZ');
#endif

	stream.WriteBytes(&value.width, sizeof(value.width));
	stream.WriteBytes(&value.height, sizeof(value.height));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XRect)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XRect& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('XRCT');
#endif

	stream.WriteBytes(&value.top, sizeof(value.top));
	stream.WriteBytes(&value.left, sizeof(value.left));
	stream.WriteBytes(&value.bottom, sizeof(value.bottom));
	stream.WriteBytes(&value.right, sizeof(value.right));

	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XHandle)
//
// $$$ Might be nice if XInStream and XOutStream had a ReserveBytes
// $$$Êmethod to prepare for a resize. This was we wouldn't be
// $$$ (potentially) allocating memory while the handle is locked
// $$$Êdown.
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XHandle& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('HAND');
#endif

	uint32 size = value.GetSize();
	stream.WriteBytes(&size, sizeof(size));
	
	{
	XLocker lock(value);
		stream.WriteBytes(value.GetPtr(), size);
	}
	
	return stream;
}


//---------------------------------------------------------------
//
// operator<< (XDate)
//
//---------------------------------------------------------------
XOutStream& operator<<(XOutStream& stream, const XDate& value)
{
#if DEBUG
	if (stream.IsTagged())
		stream.WriteTag('Date');
#endif

	XDate base(1900, 1, 1);				// convert to a common epoch so we stay cross platform
	XDate normalized = value - base;
	
	int64 time = normalized.GetTime();
	stream.WriteBytes(&time, sizeof(time));

	return stream;
}


}		// namespace Whisper
