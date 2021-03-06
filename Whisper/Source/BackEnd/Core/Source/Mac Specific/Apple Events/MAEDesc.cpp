/*
 *  File:       MAEDesc.cpp
 *  Summary:   	Wrapper around an AEDesc.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: MAEDesc.cpp,v $
 *		Revision 1.4  2001/04/21 02:25:39  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/13 07:48:27  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:08:47  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAEDesc.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XTinyVector.h>

namespace Whisper {


// ===================================================================================
//	class MAEDesc
// ===================================================================================

//---------------------------------------------------------------
//
// MAEDesc::~MAEDesc
//
//---------------------------------------------------------------
MAEDesc::~MAEDesc()
{
	OSErr err = AEDisposeDesc(this);
	ASSERT(err == noErr);				// don't throw from dtor!
}


//---------------------------------------------------------------
//
// MAEDesc::MAEDesc ()
//
//---------------------------------------------------------------
MAEDesc::MAEDesc()
{
	descriptorType = typeNull;
	dataHandle     = nil;
}


//---------------------------------------------------------------
//
// MAEDesc::MAEDesc (DescType, const void*, uint32)
//
//---------------------------------------------------------------
MAEDesc::MAEDesc(DescType type, const void* data, uint32 bytes) 
{
	PRECONDITION(data != nil || bytes == 0);
	
	OSErr err = ::AECreateDesc(type, data, (int32) bytes, this);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEDesc::MAEDesc (DescType, Handle)
//
//---------------------------------------------------------------
#if 0
MAEDesc::MAEDesc(DescType type, Handle takeHand)
{
#if TARGET_CARBON
	HLock(takeHand);
	OSErr err = ::AECreateDesc(type, *takeHand, GetHandleSize(takeHand), this);
	DisposeHandle(takeHand);
	ThrowIfOSErr(err);
	
#else
	descriptorType = type;
	dataHandle     = takeHand;
#endif
}
#endif


//---------------------------------------------------------------
//
// MAEDesc::MAEDesc (AEDesc, bool)
//
//---------------------------------------------------------------
MAEDesc::MAEDesc(const AEDesc& desc, bool copyDesc)
{
	if (copyDesc) {
		descriptorType = typeNull;
		dataHandle     = nil;

		OSErr err = AEDuplicateDesc(&desc, this);
		ThrowIfOSErr(err);
	
	} else {
		descriptorType = desc.descriptorType;
		dataHandle     = desc.dataHandle;
	}
}


//---------------------------------------------------------------
//
// MAEDesc::MAEDesc (MAEDesc)
//
//---------------------------------------------------------------
MAEDesc::MAEDesc(const MAEDesc& desc)
{
	descriptorType = typeNull;
	dataHandle     = nil;

	OSErr err = AEDuplicateDesc(&desc, this);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEDesc::operator=
//
//---------------------------------------------------------------
MAEDesc& MAEDesc::operator=(const MAEDesc& rhs)
{
	if (dataHandle != rhs.dataHandle) {
		AEDesc temp = {typeNull, nil};

		OSErr err = AEDuplicateDesc(&rhs, &temp);
		ThrowIfOSErr(err);

		err = AEDisposeDesc(this);
		ASSERT(err == noErr);					// ASSERT since we want to keep the object in a good state
		
		descriptorType = temp.descriptorType;
		dataHandle     = temp.dataHandle;
	
	} else if (descriptorType != rhs.descriptorType) {
		descriptorType = rhs.descriptorType;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// MAEDesc::CanCoerceTo
//
//---------------------------------------------------------------
bool MAEDesc::CanCoerceTo(DescType toType) const
{
	bool can = descriptorType == toType;
	
	if (!can) {
		MAEDesc temp;
		OSErr err = AECoerceDesc(this, toType, &temp);
		can = err == noErr;
	}
	
	return can;
}


//---------------------------------------------------------------
//
// MAEDesc::CoerceTo (DescType)
//
//---------------------------------------------------------------
MAEDesc MAEDesc::CoerceTo(DescType toType) const
{
	MAEDesc desc;
	OSErr err = AECoerceDesc(this, toType, &desc);
	ThrowIfOSErr(err);
	
	return desc;
}


//---------------------------------------------------------------
//
// MAEDesc::CoerceTo (DescType, AEDesc*)
//
//---------------------------------------------------------------
OSErr MAEDesc::CoerceTo(DescType toType, AEDesc* desc) const
{
	PRECONDITION(desc != nil);

	OSErr err = AECoerceDesc(this, toType, desc);
	
	return err;
}


//---------------------------------------------------------------
//
// MAEDesc::GetBoolean
//
//---------------------------------------------------------------
bool MAEDesc::GetBoolean() const
{
	Boolean value;

	if (descriptorType == typeBoolean) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
	
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeBoolean, &desc);
		ThrowIfOSErr(err);
		
		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}			

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetShort
//
//---------------------------------------------------------------
int16 MAEDesc::GetShort() const
{
	int16 value;

	if (descriptorType == typeShortInteger) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeShortInteger, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetLong
//
//---------------------------------------------------------------
int32 MAEDesc::GetLong() const
{
	int32 value;

	if (descriptorType == typeLongInteger) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeLongInteger, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetFloat
//
//---------------------------------------------------------------
float MAEDesc::GetFloat() const
{
	float value;

	if (descriptorType == typeIEEE32BitFloatingPoint) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeIEEE32BitFloatingPoint, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetDouble
//
//---------------------------------------------------------------
double MAEDesc::GetDouble() const
{
	double value;

	if (descriptorType == typeIEEE64BitFloatingPoint) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeIEEE64BitFloatingPoint, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetPSN
//
//---------------------------------------------------------------
ProcessSerialNumber MAEDesc::GetPSN() const
{
	ProcessSerialNumber value;

	if (descriptorType == typeProcessSerialNumber) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeProcessSerialNumber, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetFSSpec
//
//---------------------------------------------------------------
FSSpec MAEDesc::GetFSSpec() const
{
	FSSpec value;

	if (descriptorType == typeFSS) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeFSS, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetAlias
//
//---------------------------------------------------------------
AliasHandle MAEDesc::GetAlias() const
{
	AliasHandle value = nil;
	OSErr err = noErr;
	
	if (descriptorType == typeAlias) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);

		err = HandToHand((Handle*) &value);
		ThrowIfOSErr(err);
	
	} else {
		MAEDesc temp;
		err = this->CoerceTo(typeAlias, &temp);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&temp, &value, sizeof(value));
		ThrowIfOSErr(err);

		err = HandToHand((Handle*) &value);
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetRect
//
//---------------------------------------------------------------
Rect MAEDesc::GetRect() const
{
	Rect value;

	if (descriptorType == typeQDRectangle) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeQDRectangle, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetType
//
//---------------------------------------------------------------
DescType MAEDesc::GetType() const
{
	DescType value;

	if (descriptorType == typeType) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeType, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetString
//
//---------------------------------------------------------------
std::wstring MAEDesc::GetString() const
{	
	std::wstring value;

	if (descriptorType == typeChar) {
		uint32 size = this->GetDataSize();
		XTinyVector<char> buffer(size);
		
		OSErr err = ::AEGetDescData(this, buffer.buffer(), (int32) buffer.size());
		ThrowIfOSErr(err);
		
		value = FromPlatformStr(buffer.buffer(), size);		// typeChar is unterminated
	
	} else {
		MAEDesc temp;
		OSErr err = this->CoerceTo(typeChar, &temp);
		ThrowIfOSErr(err);

		uint32 size = temp.GetDataSize();
		XTinyVector<char> buffer(size);
	
		err = ::AEGetDescData(&temp, buffer.buffer(), (int32) buffer.size());
		ThrowIfOSErr(err);
	
		value = FromPlatformStr(buffer.buffer(), size);	
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetEnum
//
//---------------------------------------------------------------
DescType MAEDesc::GetEnum() const
{
	DescType value;

	if (descriptorType == typeEnumeration) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeEnumeration, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetDateTime
//
//---------------------------------------------------------------
uint32 MAEDesc::GetDateTime() const
{
	LongDateTime date;
	((LongDateCvt *) & date)->hl.lHigh = 0;
	((LongDateCvt *) & date)->hl.lLow  = 0;
	
	if (descriptorType == typeLongDateTime) {
		OSErr err = ::AEGetDescData(this, &date, sizeof(date));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeLongDateTime, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &date, sizeof(date));
		ThrowIfOSErr(err);
	}

	uint32 value = ((LongDateCvt *) &date)->hl.lLow;

	return value;
}


//---------------------------------------------------------------
//
// MAEDesc::GetRGBColor
//
//---------------------------------------------------------------
RGBColor MAEDesc::GetRGBColor() const
{
	RGBColor value;

	if (descriptorType == typeRGBColor) {
		OSErr err = ::AEGetDescData(this, &value, sizeof(value));
		ThrowIfOSErr(err);
		
	} else {
		MAEDesc desc;
		OSErr err = this->CoerceTo(typeRGBColor, &desc);
		ThrowIfOSErr(err);

		err = ::AEGetDescData(&desc, &value, sizeof(value));
		ThrowIfOSErr(err);
	}

	return value;
}


}	// namespace Whisper
