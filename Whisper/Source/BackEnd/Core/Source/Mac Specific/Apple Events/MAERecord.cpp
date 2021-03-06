/*
 *  File:       MAERecord.cpp
 *  Summary:   	An AEList with each item being tagged with a keyword.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAERecord.cpp,v $
 *		Revision 1.3  2001/04/21 03:23:37  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:09:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAERecord.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class MAERecord
// ===================================================================================

//---------------------------------------------------------------
//
// MAERecord::~MAERecord
//
//---------------------------------------------------------------
MAERecord::~MAERecord()
{
}


//---------------------------------------------------------------
//
// MAERecord::MAERecord ()
//
//---------------------------------------------------------------
MAERecord::MAERecord() : MAEList(true)
{
	PRECONDITION(descriptorType == typeAERecord);
}


//---------------------------------------------------------------
//
// MAERecord::MAERecord (DescType, Handle)
//
//---------------------------------------------------------------
MAERecord::MAERecord(DescType type, const void* data, uint32 bytes) : MAEList(type, data, bytes)
{
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);
}


//---------------------------------------------------------------
//
// MAERecord::MAERecord (AEDescList)
//
//---------------------------------------------------------------
MAERecord::MAERecord(const AEDescList& desc, bool copyDesc) : MAEList(desc, copyDesc)
{
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);
}


//---------------------------------------------------------------
//
// MAERecord::Append (AEKeyword, DescType, const void*, uint32)
//
//---------------------------------------------------------------
void MAERecord::Append(AEKeyword keyword, DescType typeCode, const void* data, uint32 bytes)
{
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);

	OSErr err = AEPutKeyPtr(this, keyword, typeCode, data, (int32) bytes);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAERecord::Append (AEKeyword, AEDesc)
//
//---------------------------------------------------------------
void MAERecord::Append(AEKeyword keyword, const AEDesc& desc)
{
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);
	
	OSErr err = AEPutKeyDesc(this, keyword, &desc);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAERecord::HasKey
//
//---------------------------------------------------------------
bool MAERecord::HasKey(AEKeyword keyword, DescType desiredType) const
{
	DescType typeCode;
	int32 data;
	int32 actualBytes;
	
	OSErr err = AEGetKeyPtr(this, keyword, desiredType, &typeCode, &data, sizeof(data), &actualBytes);
	return err == noErr;
}


//---------------------------------------------------------------
//
// MAERecord::GetKeyType
//
//---------------------------------------------------------------
DescType MAERecord::GetKeyType(AEKeyword keyword) const
{
	MAEDesc temp;

	OSErr err = AEGetKeyDesc(this, keyword, typeWildCard, &temp);
	ThrowIfOSErr(err);
	
	DescType type = temp.GetDescriptorType();

	return type;
}


//---------------------------------------------------------------
//
// MAERecord::GetKeyBytes
//
//---------------------------------------------------------------
uint32 MAERecord::GetKeyBytes(AEKeyword keyword) const
{
	MAEDesc temp;

	OSErr err = AEGetKeyDesc(this, keyword, typeWildCard, &temp);
	ThrowIfOSErr(err);
	
	uint32 bytes = temp.GetDataSize();

	return bytes;
}


//---------------------------------------------------------------
//
// MAERecord::GetItem (uint32, DescType, void*, uint32, DescType, DescType, uint32*)
//
//---------------------------------------------------------------
OSErr MAERecord::GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes, DescType* keyword, DescType* typeCode, uint32* actualBytes) const
{
	PRECONDITION(data != nil);
	PRECONDITION(keyword != nil);
	PRECONDITION(typeCode != nil);
	PRECONDITION(actualBytes != nil);
	
	OSErr err = AEGetNthPtr(this, (int32) (index + 1), desiredType, keyword, typeCode, data, (int32) bytes, (int32*) &actualBytes);
	
	return err;
}


//---------------------------------------------------------------
//
// MAERecord::GetKey (AEKeyword, DescType, AEDesc*)
//
//---------------------------------------------------------------
OSErr MAERecord::GetKey(AEKeyword keyword, DescType desiredType, AEDesc* desc) const
{
	PRECONDITION(desc != nil);
	
	OSErr err = AEGetKeyDesc(this, keyword, desiredType, desc);

	return err;
}


//---------------------------------------------------------------
//
// MAERecord::GetKey (AEKeyword, DescType, void*, uint32, uint32*)
//
//---------------------------------------------------------------
OSErr MAERecord::GetKey(AEKeyword keyword, DescType desiredType, void* data, uint32 bytes) const
{
	PRECONDITION(data != nil || bytes == 0);
	
	int32 actualBytes;
	DescType typeCode;
	OSErr err = AEGetKeyPtr(this, keyword, desiredType, &typeCode, data, (int32) bytes, &actualBytes);

	return err;
}


//---------------------------------------------------------------
//
// MAERecord::GetKey (AEKeyword, DescType, DescType*, void*, uint32, uint32*)
//
//---------------------------------------------------------------
OSErr MAERecord::GetKey(AEKeyword keyword, DescType desiredType, DescType* typeCode, void* data, uint32 bytes, uint32* actualBytes) const
{
	PRECONDITION(typeCode != nil);
	PRECONDITION(data != nil || bytes == 0);
	PRECONDITION(actualBytes != nil);
	
	OSErr err = AEGetKeyPtr(this, keyword, desiredType, typeCode, data, (int32) bytes, (int32*) actualBytes);

	return err;
}


//---------------------------------------------------------------
//
// MAERecord::SetKey (AEKeyword, DescType, const void*, uint32)
//
//---------------------------------------------------------------
void MAERecord::SetKey(AEKeyword keyword, DescType typeCode, const void* data, uint32 bytes)
{
	PRECONDITION(data != nil || bytes == 0);
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);

	OSErr err = AEPutKeyPtr(this, keyword, typeCode, data, (int32) bytes);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAERecord::SetKey (AEKeyword, AEDesc)
//
//---------------------------------------------------------------
void MAERecord::SetKey(AEKeyword keyword, const AEDesc& desc)
{
	PRECONDITION(descriptorType == typeAERecord || descriptorType == typeAppleEvent);

	OSErr err = AEPutKeyDesc(this, keyword, &desc);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAERecord::DeleteKey
//
//---------------------------------------------------------------
void MAERecord::DeleteKey(AEKeyword keyword)
{
	OSErr err = AEDeleteKeyDesc(this, keyword);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAERecord::GetBoolean
//
//---------------------------------------------------------------
bool MAERecord::GetBoolean(AEKeyword key) const
{	
	DescType actualType;
	int32 actualSize;
	
	Boolean b;
	OSErr err = AEGetKeyPtr(this, key, typeBoolean, &actualType, &b, sizeof(b), &actualSize);
	ThrowIfOSErr(err);

	return b;
}


//---------------------------------------------------------------
//
// MAERecord::SetBoolean
//
//---------------------------------------------------------------
void MAERecord::SetBoolean(AEKeyword key, bool value)
{
	Boolean b = value;
	this->SetKey(key, typeBoolean, &b, sizeof(b));
}


//---------------------------------------------------------------
//
// MAERecord::GetShort
//
//---------------------------------------------------------------
int16 MAERecord::GetShort(AEKeyword key) const
{
	int16 value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeShortInteger, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetLong
//
//---------------------------------------------------------------
int32 MAERecord::GetLong(AEKeyword key) const
{
	int32 value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeLongInteger, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetFloat
//
//---------------------------------------------------------------
float MAERecord::GetFloat(AEKeyword key) const
{
	float value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeIEEE32BitFloatingPoint, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetDouble
//
//---------------------------------------------------------------
double MAERecord::GetDouble(AEKeyword key) const
{
	double value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeIEEE64BitFloatingPoint, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetRect
//
//---------------------------------------------------------------
Rect MAERecord::GetRect(AEKeyword key) const
{
	Rect value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeQDRectangle, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetString
//
//---------------------------------------------------------------
std::wstring MAERecord::GetString(AEKeyword key) const
{	
	DescType actualType;
	int32 actualSize;
	char ch;
	
	OSErr err = AEGetKeyPtr(this, key, typeChar, &actualType, &ch, sizeof(ch), &actualSize);
	ThrowIfOSErr(err);

	XTinyVector<char> buffer((uint32) actualSize);
		
	err = AEGetKeyPtr(this, key, typeChar, &actualType, buffer.buffer(), actualSize, &actualSize);
	ThrowIfOSErr(err);
		
	std::wstring value = FromPlatformStr(buffer.buffer(), (uint32) actualSize);	// typeChar is unterminated

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetDateTime
//
//---------------------------------------------------------------
uint32 MAERecord::GetDateTime(AEKeyword key) const
{	
	DescType actualType;
	int32 actualSize;
	
	LongDateTime date;
	((LongDateCvt *) & date)->hl.lHigh = 0;
	((LongDateCvt *) & date)->hl.lLow  = 0;

	OSErr err = AEGetKeyPtr(this, key, typeLongDateTime, &actualType, &date, sizeof(date), &actualSize);
	ThrowIfOSErr(err);

	uint32 value = ((LongDateCvt *) & date)->hl.lLow;

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::SetDateTime
//
//---------------------------------------------------------------
void MAERecord::SetDateTime(AEKeyword key, uint32 value)
{
	LongDateTime date;
	((LongDateCvt *) & date)->hl.lHigh = 0;
	((LongDateCvt *) & date)->hl.lLow  = value;
	
	this->SetKey(key, typeLongDateTime, &date, sizeof(date));
}


//---------------------------------------------------------------
//
// MAERecord::GetColor
//
//---------------------------------------------------------------
RGBColor MAERecord::GetColor(AEKeyword key) const
{
	RGBColor value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeRGBColor, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetPSN
//
//---------------------------------------------------------------
ProcessSerialNumber MAERecord::GetPSN(AEKeyword key) const
{
	ProcessSerialNumber value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeProcessSerialNumber, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetFSSpec
//
//---------------------------------------------------------------
FSSpec MAERecord::GetFSSpec(AEKeyword key) const
{
	FSSpec value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeFSS, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetAlias
//
//---------------------------------------------------------------
AliasHandle MAERecord::GetAlias(AEKeyword key) const
{
	AliasHandle value = nil;
	
	DescType actualType;
	int32 actualSize;	
	char ch;
	
	OSErr err = AEGetKeyPtr(this, key, typeAlias, &actualType, &ch, sizeof(ch), &actualSize);
	ThrowIfOSErr(err);

	Handle hand = NewHandle((uint32) actualSize);
	ThrowIfNil(hand);
		
	{
	XLocker lock(hand);
		err = AEGetKeyPtr(this, key, typeAlias, &actualType, *hand, actualSize, &actualSize);
		ThrowIfOSErr(err);

		value = (AliasHandle) hand;
	}
	
	return value;
}


//---------------------------------------------------------------
//
// MAERecord::SetAlias
//
//---------------------------------------------------------------
void MAERecord::SetAlias(AEKeyword key, AliasHandle value)
{
	Handle hand = (Handle) value;
	
	XLocker lock(hand);
	
	this->SetKey(key, typeAlias, *hand, (uint32) GetHandleSize(hand));
}


//---------------------------------------------------------------
//
// MAERecord::GetEnum
//
//---------------------------------------------------------------
DescType MAERecord::GetEnum(AEKeyword key) const
{
	DescType value;
	
	DescType actualType;
	int32 actualSize;
	
	OSErr err = AEGetKeyPtr(this, key, typeEnumeration, &actualType, &value, sizeof(value), &actualSize);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::GetDesc
//
//---------------------------------------------------------------
MAEDesc MAERecord::GetDesc(AEKeyword key, DescType desiredType) const
{
	MAEDesc value;
		
	OSErr err = AEGetKeyDesc(this, key, desiredType, &value);
	ThrowIfOSErr(err);

	return value;
}


//---------------------------------------------------------------
//
// MAERecord::SetDesc
//
//---------------------------------------------------------------
void MAERecord::SetDesc(AEKeyword key, const MAEDesc& value)
{
	OSErr err = AEPutKeyDesc(this, key, &value);
	ThrowIfOSErr(err);
}
	

}	// namespace Whisper
