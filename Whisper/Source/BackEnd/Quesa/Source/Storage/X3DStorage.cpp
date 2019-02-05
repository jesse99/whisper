/*
 *  File:       X3DStorage.cpp
 *  Summary:	Base class for QuickDraw 3D storage objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStorage.cpp,v $
 *		Revision 1.4  2001/04/21 03:37:59  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:31:59  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DStorage.h>

#include <QuesaStorage.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DStorage
// ===================================================================================

//---------------------------------------------------------------
//
// X3DStorage::~X3DStorage
//
//---------------------------------------------------------------
X3DStorage::~X3DStorage()
{
}

					
//---------------------------------------------------------------
//
// X3DStorage::X3DStorage
//
//---------------------------------------------------------------
X3DStorage::X3DStorage(TQ3StorageObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeStorage);
}


//---------------------------------------------------------------
//
// X3DStorage::Clone
//
//---------------------------------------------------------------
X3DStorage* X3DStorage::Clone() const
{
	X3DStorage* object = new X3DStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DStorage::GetSize
//
//---------------------------------------------------------------
uint32 X3DStorage::GetSize() const
{
	uint32 size;
	
	TQ3Status status = Q3Storage_GetSize(mObject, &size);
	ThrowIf3DError(status);
	
	return size;
}


//---------------------------------------------------------------
//
// X3DStorage::GetData
//
//---------------------------------------------------------------
void X3DStorage::GetData(uint32 offset, uint32 bytes, void* buffer) const
{
	PRECONDITION(offset < 1024L*1024L);
	PRECONDITION(bytes < 1024L*1024L);
	PRECONDITION(buffer != nil);
	
	uint32 bytesRead;
	
	TQ3Status status = Q3Storage_GetData(mObject, offset, bytes, (uint8*) buffer, &bytesRead);
	ThrowIf3DError(status);
	
	if (bytesRead < bytes)
		throw std::range_error("Internal Error: X3DStorage::GetData couldn't read all the data.");
}


//---------------------------------------------------------------
//
// X3DStorage::SetData
//
//---------------------------------------------------------------
void X3DStorage::SetData(uint32 offset, uint32 bytes, const void* buffer)
{
	PRECONDITION(offset < 1024L*1024L);
	PRECONDITION(bytes < 1024L*1024L);
	PRECONDITION(buffer != nil);
	
	uint32 bytesWritten;
	
	TQ3Status status = Q3Storage_SetData(mObject, offset, bytes, (uint8*) buffer, &bytesWritten);
	ThrowIf3DError(status);
	
	if (bytesWritten < bytes)
		throw std::range_error("Internal Error: X3DStorage::SetData couldn't write all the data.");
}


//---------------------------------------------------------------
//
// X3DStorage::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeStorage);
	
	return compatible;
}


}	// namespace Whisper
