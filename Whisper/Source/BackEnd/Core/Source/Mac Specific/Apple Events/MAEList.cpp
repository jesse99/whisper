/*
 *  File:       MAEList.cpp
 *  Summary:   	An AEDesc that contains a sequence of values.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAEList.cpp,v $
 *		Revision 1.3  2001/04/21 03:23:33  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:08:56  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MAEList.h>

#include <XDebug.h>
#include <XExceptions.h>
#include <XLocker.h>

namespace Whisper {


// ===================================================================================
//	class MAEList
// ===================================================================================

//---------------------------------------------------------------
//
// MAEList::~MAEList
//
//---------------------------------------------------------------
MAEList::~MAEList()
{
}


//---------------------------------------------------------------
//
// MAEList::MAEList (bool)
//
//---------------------------------------------------------------
MAEList::MAEList(bool isRecord)
{
	PRECONDITION(dataHandle == nil);
	
	OSErr err = AECreateList(nil, 0, isRecord, this);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEList::MAEList (DescType, const void*, uint32)
//
//---------------------------------------------------------------
MAEList::MAEList(DescType type, const void* data, uint32 bytes) : MAEDesc(type, data, bytes)
{
	PRECONDITION(descriptorType == typeAEList || descriptorType == typeAERecord || descriptorType == typeAppleEvent);
}


//---------------------------------------------------------------
//
// MAEList::MAEList (AEDescList)
//
//---------------------------------------------------------------
MAEList::MAEList(const AEDescList& desc, bool copyDesc) : MAEDesc(desc, copyDesc)
{
	PRECONDITION(descriptorType == typeAEList || descriptorType == typeAERecord || descriptorType == typeAppleEvent);
}


//---------------------------------------------------------------
//
// MAEList::Append (DescType, const void*, uint32)
//
//---------------------------------------------------------------
void MAEList::Append(DescType typeCode, const void* data, uint32 bytes)
{
	PRECONDITION(descriptorType == typeAEList);
	
	OSErr err = AEPutPtr(this, 0, typeCode, data, (int32) bytes);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEList::Append (AEDesc)
//
//---------------------------------------------------------------
void MAEList::Append(const AEDesc& desc)
{
	PRECONDITION(descriptorType == typeAEList);
	
	OSErr err = AEPutDesc(this, 0, &desc);
	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// MAEList::GetCount
//
//---------------------------------------------------------------
uint32 MAEList::GetCount() const
{
	int32 count = 0;
	
	OSErr err = AECountItems(this, &count);
	ThrowIfOSErr(err);

	return (uint32) count;
}


//---------------------------------------------------------------
//
// MAEList::GetItemType
//
//---------------------------------------------------------------
DescType MAEList::GetItemType(uint32 index) const
{
	MAEDesc temp;

	DescType keyword;
	OSErr err = AEGetNthDesc(this, (int32) (index + 1), typeWildCard, &keyword, &temp);
	ThrowIfOSErr(err);
	
	DescType type = temp.GetDescriptorType();

	return type;
}


//---------------------------------------------------------------
//
// MAEList::GetItemBytes
//
//---------------------------------------------------------------
uint32 MAEList::GetItemBytes(uint32 index) const
{
	MAEDesc temp;

	DescType keyword;
	OSErr err = AEGetNthDesc(this, (int32) (index + 1), typeWildCard, &keyword, &temp);
	ThrowIfOSErr(err);
	
	uint32 bytes = temp.GetDataSize();

	return bytes;
}


//---------------------------------------------------------------
//
// MAEList::GetItem (uint32, DescType, AEDesc*)
//
//---------------------------------------------------------------
OSErr MAEList::GetItem(uint32 index, DescType desiredType, AEDesc* desc) const
{
	PRECONDITION(desc != nil);
	
	DescType keyword;
	OSErr err = AEGetNthDesc(this, (int32) (index + 1), desiredType, &keyword, desc);
	
	return err;
}


//---------------------------------------------------------------
//
// MAEList::GetItem (uint32, DescType, void*, uint32)
//
//---------------------------------------------------------------
OSErr MAEList::GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes) const
{
	PRECONDITION(data != nil);
	
	int32 actualSize;
	DescType keyword, typeCode;
	OSErr err = AEGetNthPtr(this, (int32) (index + 1), desiredType, &keyword, &typeCode, data, (int32) bytes, &actualSize);
	
	return err;
}


//---------------------------------------------------------------
//
// MAEList::GetItem (uint32, DescType, void*, uint32, DescType, uint32*)
//
//---------------------------------------------------------------
OSErr MAEList::GetItem(uint32 index, DescType desiredType, void* data, uint32 bytes, DescType* typeCode, uint32* actualBytes) const
{
	PRECONDITION(data != nil);
	PRECONDITION(typeCode != nil);
	PRECONDITION(actualBytes != nil);
	
	DescType keyword;
	OSErr err = AEGetNthPtr(this, (int32) (index + 1), desiredType, &keyword, typeCode, data, (int32) bytes, (int32*) &actualBytes);
	
	return err;
}


//---------------------------------------------------------------
//
// MAEList::SetItem (uint32, DescType, const void*, uint32)
//
//---------------------------------------------------------------
void MAEList::SetItem(uint32 index, DescType typeCode, const void* data, uint32 bytes)
{
	PRECONDITION(descriptorType == typeAEList);
	
	OSErr err = AEPutPtr(this, (int32) (index + 1), typeCode, data, (int32) bytes);
	ThrowIfOSErr(err);
}
	

//---------------------------------------------------------------
//
// MAEList::SetItem (index, AEDesc)
//
//---------------------------------------------------------------
void MAEList::SetItem(uint32 index, const AEDesc& desc)
{
	PRECONDITION(descriptorType == typeAEList);
	
	OSErr err = AEPutDesc(this, (int32) (index + 1), &desc);
	ThrowIfOSErr(err);
}
	

//---------------------------------------------------------------
//
// MAEList::DeleteItem
//
//---------------------------------------------------------------
void MAEList::DeleteItem(uint32 index)
{
	OSErr err = AEDeleteItem(this, (int32) (index + 1));
	ThrowIfOSErr(err);
}
	

}	// namespace Whisper
