/*
 *  File:       X3DStorages.cpp
 *  Summary:	QuickDraw 3D storage objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStorages.cpp,v $
 *		Revision 1.4  2001/04/21 03:38:03  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:32:04  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:20:57  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DStorages.h>

#include <QuesaStorage.h>

#include <X3DUtils.h>
#include <XDebug.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CreateMemoryStorage
//
//---------------------------------------------------------------
static X3DObject* CreateMemoryStorage(TQ3Object inObject)
{
	X3DMemoryStorage* object = new X3DMemoryStorage(inObject);
	
	return object;
}

//---------------------------------------------------------------
//
// CreateHandleStorage
//
//---------------------------------------------------------------
#if MAC
static X3DObject* CreateHandleStorage(TQ3Object inObject)
{
	X3DHandleStorage* object = new X3DHandleStorage(inObject);
	
	return object;
}
#endif


//---------------------------------------------------------------
//
// CreateMacFileStorage
//
//---------------------------------------------------------------
#if MAC
static X3DObject* CreateMacFileStorage(TQ3Object inObject)
{
	X3DMacFileStorage* object = new X3DMacFileStorage(inObject);
	
	return object;
}
#endif


//---------------------------------------------------------------
//
// CreateFSSpecStorage
//
//---------------------------------------------------------------
#if MAC
static X3DObject* CreateFSSpecStorage(TQ3Object inObject)
{
	X3DFSSpecStorage* object = new X3DFSSpecStorage(inObject);
	
	return object;
}
#endif


//---------------------------------------------------------------
//
// CreateWin32FileStorage
//
//---------------------------------------------------------------
#if WIN		
static X3DObject* CreateWin32FileStorage(TQ3Object inObject)
{
	X3DWin32FileStorage* object = new X3DWin32FileStorage(inObject);
	
	return object;
}
#endif  

#pragma mark -

// ===================================================================================
//	class X3DMemoryStorage
// ===================================================================================

//---------------------------------------------------------------
//
// X3DMemoryStorage::~X3DMemoryStorage
//
//---------------------------------------------------------------
X3DMemoryStorage::~X3DMemoryStorage()
{
}

					
//---------------------------------------------------------------
//
// X3DMemoryStorage::X3DMemoryStorage (uint32, const void*)
//
//---------------------------------------------------------------
X3DMemoryStorage::X3DMemoryStorage(uint32 bytes, const void* buffer) : X3DStorage(Q3MemoryStorage_New((Byte*) buffer, bytes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMemoryStorage::X3DMemoryStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DMemoryStorage::X3DMemoryStorage(TQ3StorageObject object) : X3DStorage(object)
{
	this->ConfirmType(kQ3StorageTypeMemory);
}


//---------------------------------------------------------------
//
// X3DMemoryStorage::Clone
//
//---------------------------------------------------------------
X3DMemoryStorage* X3DMemoryStorage::Clone() const
{
	X3DMemoryStorage* object = new X3DMemoryStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMemoryStorage::Set
//
//---------------------------------------------------------------
void X3DMemoryStorage::Set(uint32 bytes, const void* buffer)
{
	PRECONDITION(bytes < 1024L*1024L);
	PRECONDITION(buffer != nil);
		
	TQ3Status status = Q3MemoryStorage_Set(mObject, (const Byte*) buffer, bytes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMemoryStorage::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DMemoryStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StorageTypeMemory);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DPtrStorage
// ===================================================================================

// $$$ÊNo type to register!

//---------------------------------------------------------------
//
// X3DPtrStorage::~X3DPtrStorage
//
//---------------------------------------------------------------
X3DPtrStorage::~X3DPtrStorage()
{
}

					
//---------------------------------------------------------------
//
// X3DPtrStorage::X3DPtrStorage (uint32, uint32, void*)
//
//---------------------------------------------------------------
X3DPtrStorage::X3DPtrStorage(uint32 validBytes, uint32 bufferSize, void* buffer) : X3DMemoryStorage(Q3MemoryStorage_NewBuffer((Byte*) buffer, validBytes, bufferSize))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPtrStorage::X3DPtrStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DPtrStorage::X3DPtrStorage(TQ3StorageObject object) : X3DMemoryStorage(object)
{
//	this->ConfirmType(xxx);			// $$$Êno type to check!
}


//---------------------------------------------------------------
//
// X3DPtrStorage::Clone
//
//---------------------------------------------------------------
X3DPtrStorage* X3DPtrStorage::Clone() const
{
	X3DPtrStorage* object = new X3DPtrStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DPtrStorage::GetBuffer
//
//---------------------------------------------------------------
void X3DPtrStorage::GetBuffer(uint32* validBytes, uint32* bufferSize, Byte** buffer) const
{
	PRECONDITION(validBytes != nil);
	PRECONDITION(bufferSize != nil);
	PRECONDITION(buffer != nil);
		
	TQ3Status status = Q3MemoryStorage_GetBuffer(mObject, buffer, validBytes, bufferSize);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPtrStorage::SetBuffer
//
//---------------------------------------------------------------
void X3DPtrStorage::SetBuffer(uint32 validBytes, uint32 bufferSize, void* buffer)
{
	PRECONDITION(validBytes <= bufferSize);
	PRECONDITION(bufferSize < 1024L*1024L);
	PRECONDITION(buffer != nil);
		
	TQ3Status status = Q3MemoryStorage_SetBuffer(mObject, (Byte*) buffer, validBytes, bufferSize);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DPtrStorage::IsCompatible 							[static]
//	
//---------------------------------------------------------------
bool X3DPtrStorage::IsCompatible(TQ3Object)
{
	bool compatible = false;
	
	// $$$Êfix this once Apple defines a constant for ptr storage
	throw std::range_error("Internal Error: X3DPtrStorage::IsCompatible isn't implemented (Apple needs to define a type constant).");
	
//	if (object != nil)
//		compatible = Q3Object_IsType(object, xxx);
	
	return compatible;
}

#pragma mark -

// ===================================================================================
//	class X3DHandleStorage
// ===================================================================================

#if MAC	

//---------------------------------------------------------------
//
// X3DHandleStorage::~X3DHandleStorage
//
//---------------------------------------------------------------
X3DHandleStorage::~X3DHandleStorage()
{
}

						
//---------------------------------------------------------------
//
// X3DHandleStorage::X3DHandleStorage (Handle)
//
//---------------------------------------------------------------
X3DHandleStorage::X3DHandleStorage(Handle takeHand) : X3DMemoryStorage(Q3HandleStorage_New(takeHand, 0))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::X3DHandleStorage (uint32)
//
//---------------------------------------------------------------
X3DHandleStorage::X3DHandleStorage(uint32 bytes) : X3DMemoryStorage(Q3HandleStorage_New(nil, bytes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::X3DHandleStorage (Handle, uint32)
//
//---------------------------------------------------------------
X3DHandleStorage::X3DHandleStorage(Handle takeHand, uint32 validBytes) : X3DMemoryStorage(Q3HandleStorage_New(takeHand, validBytes))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::X3DHandleStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DHandleStorage::X3DHandleStorage(TQ3StorageObject object) : X3DMemoryStorage(object)
{
	this->ConfirmType(kQ3MemoryStorageTypeHandle);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::Clone
//
//---------------------------------------------------------------
X3DHandleStorage* X3DHandleStorage::Clone() const
{
	X3DHandleStorage* object = new X3DHandleStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DHandleStorage::Get
//
//---------------------------------------------------------------
void X3DHandleStorage::Get(Handle* hand, uint32* validSize) const
{
	PRECONDITION(hand != nil);
	PRECONDITION(validSize != nil);
		
	TQ3Status status = Q3HandleStorage_Get(mObject, hand, validSize);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::Set
//
//---------------------------------------------------------------
void X3DHandleStorage::Set(Handle takeHand, uint32 validBytes)
{		
	TQ3Status status = Q3HandleStorage_Set(mObject, takeHand, validBytes);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DHandleStorage::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DHandleStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3MemoryStorageTypeHandle);
	
	return compatible;
}
#endif	// MAC	

#pragma mark -

// ===================================================================================
//	class X3DMacFileStorage
// ===================================================================================

#if MAC	

//---------------------------------------------------------------
//
// X3DMacFileStorage::~X3DMacFileStorage
//
//---------------------------------------------------------------
X3DMacFileStorage::~X3DMacFileStorage()
{
}

						
//---------------------------------------------------------------
//
// X3DMacFileStorage::X3DMacFileStorage (short)
//
//---------------------------------------------------------------
X3DMacFileStorage::X3DMacFileStorage(short refNum) : X3DStorage(Q3MacintoshStorage_New(refNum))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMacFileStorage::X3DMacFileStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DMacFileStorage::X3DMacFileStorage(TQ3StorageObject object) : X3DStorage(object)
{
	this->ConfirmType(kQ3StorageTypeMacintosh);
}


//---------------------------------------------------------------
//
// X3DMacFileStorage::Clone
//
//---------------------------------------------------------------
X3DMacFileStorage* X3DMacFileStorage::Clone() const
{
	X3DMacFileStorage* object = new X3DMacFileStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DMacFileStorage::Get
//
//---------------------------------------------------------------
short X3DMacFileStorage::Get() const
{
	short refNum;
	
	TQ3Status status = Q3MacintoshStorage_Get(mObject, &refNum);
	ThrowIf3DError(status);
	
	return refNum;
}


//---------------------------------------------------------------
//
// X3DMacFileStorage::Set
//
//---------------------------------------------------------------
void X3DMacFileStorage::Set(short refNum)
{
	TQ3Status status = Q3MacintoshStorage_Set(mObject, refNum);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DMacFileStorage::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DMacFileStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StorageTypeMacintosh);
	
	return compatible;
}
#endif	// MAC	

#pragma mark -

// ===================================================================================
//	class X3DFSSpecStorage
// ===================================================================================

#if MAC	

//---------------------------------------------------------------
//
// X3DFSSpecStorage::~X3DFSSpecStorage
//
//---------------------------------------------------------------
X3DFSSpecStorage::~X3DFSSpecStorage()
{
}

						
//---------------------------------------------------------------
//
// X3DFSSpecStorage::X3DFSSpecStorage (FSSpec)
//
//---------------------------------------------------------------
X3DFSSpecStorage::X3DFSSpecStorage(const FSSpec& spec) : X3DMacFileStorage(Q3FSSpecStorage_New(&spec))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFSSpecStorage::X3DFSSpecStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DFSSpecStorage::X3DFSSpecStorage(TQ3StorageObject object) : X3DMacFileStorage(object)
{
	this->ConfirmType(kQ3MacintoshStorageTypeFSSpec);
}


//---------------------------------------------------------------
//
// X3DFSSpecStorage::Clone
//
//---------------------------------------------------------------
X3DFSSpecStorage* X3DFSSpecStorage::Clone() const
{
	X3DFSSpecStorage* object = new X3DFSSpecStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DFSSpecStorage::Get
//
//---------------------------------------------------------------
FSSpec X3DFSSpecStorage::Get() const
{
	FSSpec spec;
	
	TQ3Status status = Q3FSSpecStorage_Get(mObject, &spec);
	ThrowIf3DError(status);
	
	return spec;
}


//---------------------------------------------------------------
//
// X3DFSSpecStorage::Set
//
//---------------------------------------------------------------
void X3DFSSpecStorage::Set(const FSSpec& spec)
{
	TQ3Status status = Q3FSSpecStorage_Set(mObject, &spec);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFSSpecStorage::IsCompatible 						[static]
//	
//---------------------------------------------------------------
bool X3DFSSpecStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3MacintoshStorageTypeFSSpec);
	
	return compatible;
}
#endif	// MAC	

#pragma mark -

// ===================================================================================
//	class X3DWin32FileStorage
// ===================================================================================

#if WIN		

//---------------------------------------------------------------
//
// X3DWin32FileStorage::X3DWin32FileStorage
//
//---------------------------------------------------------------
X3DWin32FileStorage::~X3DWin32FileStorage()
{
}

						
//---------------------------------------------------------------
//
// X3DWin32FileStorage::X3DWin32FileStorage (HANDLE)
//
//---------------------------------------------------------------
X3DWin32FileStorage::X3DWin32FileStorage(HANDLE hFile) : X3DStorage(Q3Win32Storage_New(hFile))
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWin32FileStorage::X3DWin32FileStorage (TQ3StorageObject)
//
//---------------------------------------------------------------
X3DWin32FileStorage::X3DWin32FileStorage(TQ3StorageObject object) : X3DStorage(object)
{
	this->ConfirmType(kQ3StorageTypeWin32);
}


//---------------------------------------------------------------
//
// X3DWin32FileStorage::Clone
//
//---------------------------------------------------------------
X3DWin32FileStorage* X3DWin32FileStorage::Clone() const
{
	X3DWin32FileStorage* object = new X3DWin32FileStorage(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DWin32FileStorage::Get
//
//---------------------------------------------------------------
HANDLE X3DWin32FileStorage::Get() const
{
	HANDLE hFile;
	
	TQ3Status status = Q3Win32Storage_Get(mObject, &hFile);
	ThrowIf3DError(status);
	
	return hFile;
}


//---------------------------------------------------------------
//
// X3DWin32FileStorage::Set
//
//---------------------------------------------------------------
void X3DWin32FileStorage::Set(HANDLE hFile)
{
	TQ3Status status = Q3Win32Storage_Set(mObject, hFile);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DWin32FileStorage::IsCompatible 					[static]
//	
//---------------------------------------------------------------
bool X3DWin32FileStorage::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3StorageTypeMacintosh);
	
	return compatible;
}
#endif  // WIN


}	// namespace Whisper
