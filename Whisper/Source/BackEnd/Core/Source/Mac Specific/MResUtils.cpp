/*
 *  File:       MResUtils.cpp
 *  Summary:    Resource manager utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MResUtils.cpp,v $
 *		Revision 1.4  2001/04/27 04:22:14  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.3  2001/04/21 03:28:18  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:18:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MResUtils.h>

#include <Errors.h>
#include <LowMem.h>
#include <Resources.h>
#include <Script.h>

#include <SLFunctions.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XLocker.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const uint32 kMaxBufferSize = 4*1024L*1024L;	// for sanity checking


// ===================================================================================
//	Helper Functions
// ===================================================================================

//---------------------------------------------------------------
//
// IsPurgeableHand
//
//---------------------------------------------------------------
static bool IsPurgeableHand(Handle hand)
{
	PRECONDITION(hand != nil);
	
	char state = HGetState(hand);
	OSErr err = MemError();
	
	if (err == noErr)
		return (state & 0x40) != 0;
	else if (err == nilHandleErr)
		return true;								// Handle has been purged.
	else
		DEBUGSTR("Error ", err, " in IsPurgeable.");
		
	return false;
}


//---------------------------------------------------------------
//
// IsResourceHand
//
//---------------------------------------------------------------
static bool IsResourceHand(Handle hand)
{
	PRECONDITION(hand != nil);				// Might want to treat this like IsPurgeable.

	char state = HGetState(hand);
	ASSERT(MemError() == noErr);
	return (state & 0x20) != 0;
}


//---------------------------------------------------------------
//
// GetResourceNoLoad (ResType, ResID, bool)
//
//---------------------------------------------------------------
static Handle GetResourceNoLoad(ResType type, ResID id, bool get1)
{
	Handle rsrc = nil;
	
	SetResLoad(false);
	if (get1)
		rsrc = Get1Resource(type, id);
	else
		rsrc = GetResource(type, id);
	OSErr err = ResError();
	SetResLoad(true);
	
	return rsrc;
}


//---------------------------------------------------------------
//
// GetResourceNoLoad (ResType, string, bool)
//
//---------------------------------------------------------------
static Handle GetResourceNoLoad(ResType type, const std::wstring& inName, bool get1)
{
	Handle rsrc = nil;
		
	SetResLoad(false);
	if (get1)
		rsrc = Get1NamedResource(type, ToPascalStr(inName).c_str());
	else
		rsrc = GetNamedResource(type, ToPascalStr(inName).c_str());
	OSErr err = ResError();
	SetResLoad(true);
	
	return rsrc;
}


//---------------------------------------------------------------
//
// CreateResource
//
// Creates a resource of the specified size or resizes an existing
// resource.
//
//---------------------------------------------------------------
static Handle CreateResource(ResType type, ResID id, const std::wstring& name, uint32 size, int16 attrs)
{
	Handle handle = nil;
	OSErr err = noErr;

	SLDisable();					// Spotlight complains if the resource doesn't exist
	if (name.length() > 0)
		handle = Get1NamedResource(type, ToPascalStr(name).c_str());
	else
		handle = Get1Resource(type, id);
	SLEnable();

	if (handle == nil) {
		handle = NewHandle(size);
		ThrowIfMemFail(handle);
		
		if (name.length() > 0)
			id = UniqueID(type);
		
		AddResource(handle, type, id, ToPascalStr(name).c_str());
		err = ResError();
		if (err == noErr) {
			if (attrs != 0)
				SetResAttrs(handle, (int16) (attrs | resChanged));		// preserve resChanged (it is set by AddResource())
		
		} else {
			DisposeHandle(handle);
			ThrowIfOSErr(err);
		}
					
	} else if (GetHandleSize(handle) != size) {
		SetHandleSize(handle, size);
		err = MemError();
		if (err == noErr) {
			ChangedResource(handle);
			err = ResError();
			
			if (err != noErr) {
				ReleaseIfResource(handle);
				ThrowIfOSErr(err);
			}
		}
		
	} else {
		ChangedResource(handle);						// Make sure resChanged bit is set.
		ThrowIfResError();
	}

	return handle;
}

#pragma mark -

// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// ReadResource (ResType, ResID)
//
//---------------------------------------------------------------
XHandle ReadResource(ResType type, ResID id)
{	
	XHandle hand;

	Handle rsrc = GetResourceNoLoad(type, id, false);
	ThrowIfResFail(rsrc);
		
	try {
		uint32 handleSize = (uint32) GetResourceSizeOnDisk(rsrc);		
		hand.SetSize(handleSize);
		
		{
		XLocker lock(hand);
			ReadPartialResource(rsrc, 0, hand.GetPtr(), (int32) handleSize);
			ThrowIfResError();
		}
		
		ReleaseResource(rsrc);
	
	} catch (...) {
		ReleaseResource(rsrc);
		throw;
	}
		
	return hand;
}


//---------------------------------------------------------------
//
// ReadResource (ResType, ResID, void*, uint32)
//
//---------------------------------------------------------------
void ReadResource(ResType type, ResID id, void* buffer, uint32 bytes)
{	
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	Handle rsrc = GetResourceNoLoad(type, id, false);
	ThrowIfResFail(rsrc);
	
	int32 handleSize = GetResourceSizeOnDisk(rsrc);
	ASSERT(ResError() == noErr && handleSize >= bytes);

	ReadPartialResource(rsrc, 0, buffer, (int32) bytes);
	OSErr err = ResError();

	ReleaseResource(rsrc);

	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// ReadResource (ResType, string)
//
//---------------------------------------------------------------
XHandle ReadResource(ResType type, const std::wstring& name)
{	
	XHandle hand;

	Handle rsrc = GetResourceNoLoad(type, name, false);
	ThrowIfResFail(rsrc);
		
	try {
		uint32 handleSize = (uint32) GetResourceSizeOnDisk(rsrc);		
		hand.SetSize(handleSize);
		
		{
		XLocker lock(hand);
			ReadPartialResource(rsrc, 0, hand.GetPtr(), (int32) handleSize);
			ThrowIfResError();
		}
		
		ReleaseResource(rsrc);
	
	} catch (...) {
		ReleaseResource(rsrc);
		throw;
	}
		
	return hand;
}


//---------------------------------------------------------------
//
// ReadResource (ResType, string, void*, uint32)
//
//---------------------------------------------------------------
void ReadResource(ResType type, const std::wstring& name, void* buffer, uint32 bytes)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	Handle rsrc = GetResourceNoLoad(type, name, false);
	ThrowIfResFail(rsrc);
	
	int32 handleSize = GetResourceSizeOnDisk(rsrc);
	ASSERT(ResError() == noErr && handleSize >= bytes);

	ReadPartialResource(rsrc, 0, buffer, (int32) bytes);
	OSErr err = ResError();

	ReleaseResource(rsrc);

	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// Read1Resource (ResType, ResID, void*, uint32)
//
//---------------------------------------------------------------
void Read1Resource(ResType type, ResID id, void* buffer, uint32 bytes)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	Handle rsrc = GetResourceNoLoad(type, id, true);
	ThrowIfResFail(rsrc);
	
	int32 handleSize = GetResourceSizeOnDisk(rsrc);
	ASSERT(ResError() == noErr && handleSize >= bytes);

	ReadPartialResource(rsrc, 0, buffer, (int32) bytes);
	OSErr err = ResError();

	ReleaseResource(rsrc);

	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// Read1Resource (ResType, string, void*, uint32)
//
//---------------------------------------------------------------
void Read1Resource(ResType type, const std::wstring& name, void* buffer, uint32 bytes)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	Handle rsrc = GetResourceNoLoad(type, name, true);
	ThrowIfResFail(rsrc);
	
	int32 handleSize = GetResourceSizeOnDisk(rsrc);
	ASSERT(ResError() == noErr && handleSize >= bytes);

	ReadPartialResource(rsrc, 0, buffer, (int32) bytes);
	OSErr err = ResError();

	ReleaseResource(rsrc);

	ThrowIfOSErr(err);
}


//---------------------------------------------------------------
//
// WriteResource (ResType, ResID, Handle, bool)
//
//---------------------------------------------------------------
void WriteResource(ResType type, ResID id, Handle data, bool purgeable)
{
	PRECONDITION(data != nil && *data != nil);
	PRECONDITION(!IsPurgeableHand(data));
	
	int16 attrs = 0;
	if (purgeable)
		attrs += resPurgeable;

	uint32 size = (uint32) GetHandleSize(data);
	Handle rsrc = CreateResource(type, id, L"", size, attrs);

	BlockMoveData(*data, *rsrc, size);
	::WriteResource(rsrc);
	ThrowIfResError();

	ReleaseIfResource(rsrc);
}


//---------------------------------------------------------------
//
// WriteResource (ResType, string, Handle, bool)
//
//---------------------------------------------------------------
void WriteResource(ResType type, const std::wstring& name, Handle data, bool purgeable)
{
	PRECONDITION(data != nil && *data != nil);
	PRECONDITION(!IsPurgeableHand(data));
	
	int16 attrs = 0;
	if (purgeable)
		attrs += resPurgeable;

	uint32 size = (uint32) GetHandleSize(data);
	Handle rsrc = CreateResource(type, 0, name, size, attrs);

	BlockMoveData(*data, *rsrc, size);
	::WriteResource(rsrc);
	ThrowIfResError();

	ReleaseIfResource(rsrc);
}


//---------------------------------------------------------------
//
// WriteResource (ResType, ResID, const void*, uint32, bool)
//
//---------------------------------------------------------------
void WriteResource(ResType type, ResID id, const void* buffer, uint32 bytes, bool purgeable)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	int16 attrs = 0;
	if (purgeable)
		attrs += resPurgeable;

	Handle rsrc = CreateResource(type, id, L"", bytes, attrs);

	BlockMoveData(buffer, *rsrc, bytes);
	::WriteResource(rsrc);
	ThrowIfResError();

	ReleaseIfResource(rsrc);
}


//---------------------------------------------------------------
//
// WriteResource (ResType, string, const void*, uint32, bool)
//
//---------------------------------------------------------------
void WriteResource(ResType type, const std::wstring& name, const void* buffer, uint32 bytes, bool purgeable)
{
	PRECONDITION(buffer != nil);
	PRECONDITION(bytes < kMaxBufferSize);
	
	int16 attrs = 0;
	if (purgeable)
		attrs += resPurgeable;

	Handle rsrc = CreateResource(type, 0, name, bytes, attrs);

	BlockMoveData(buffer, *rsrc, bytes);
	::WriteResource(rsrc);
	ThrowIfResError();

	ReleaseIfResource(rsrc);
}


//---------------------------------------------------------------
//
// Delete1Resource (ResType)
//
//---------------------------------------------------------------
void Delete1Resource(ResType type)
{
	int16 num = ::Count1Resources(type);
	for (int16 index = 1; index <= num; index++) {
		Handle hand;
		
		{
		MNoResLoad noLoad;
			hand = ::Get1IndResource(type, 1);
			ThrowIfResFail(hand);
		}
		
		::RemoveResource(hand);
		ThrowIfResError();
		
		::DisposeHandle(hand);
	}
}


//---------------------------------------------------------------
//
// DeleteResource (ResType)
//
//---------------------------------------------------------------
void DeleteResource(ResType type)
{
	int16 num = ::CountResources(type);
	for (int16 index = 1; index <= num; index++) {
		Handle hand;
		
		{
		MNoResLoad noLoad;
			hand = ::GetIndResource(type, 1);
			ThrowIfResFail(hand);
		}
		
		::RemoveResource(hand);
		ThrowIfResError();
		
		::DisposeHandle(hand);
	}
}


//---------------------------------------------------------------
//
// DeleteResource (ResType, ResID)
//
//---------------------------------------------------------------
void DeleteResource(ResType type, ResID id)
{
	Handle handle = GetResourceNoLoad(type, id, true);
	ASSERT(handle != nil);

	if (handle != nil) {
		int16 refNum = HomeResFile(handle);
		RemoveResource(handle);
		ThrowIfResError();
		if (refNum != -1)
			UpdateResFile(refNum);
		DisposeHandle(handle);
	}
}


//---------------------------------------------------------------
//
// DeleteResource (ResType, string)
//
//---------------------------------------------------------------
void DeleteResource(ResType type, const std::wstring& name)
{
	Handle handle = GetResourceNoLoad(type, name, true);
	ASSERT(handle != nil);

	if (handle != nil) {
		int16 refNum = HomeResFile(handle);
		RemoveResource(handle);
		ThrowIfResError();
		if (refNum != -1)
			UpdateResFile(refNum);
		DisposeHandle(handle);
	}
}


//---------------------------------------------------------------
//
// Has1Resource (ResType, ResID)
//
//---------------------------------------------------------------
bool Has1Resource(ResType type, ResID id)
{
	SLDisable();
	Handle rsrc = GetResourceNoLoad(type, id, true);
	
	bool has = rsrc != nil;
	
	if (has)
		ReleaseResource(rsrc);
	SLEnable();
	
	return has;
}


//---------------------------------------------------------------
//
// Has1Resource (ResType, string)
//
//---------------------------------------------------------------
bool Has1Resource(ResType type, const std::wstring& name)
{
	SLDisable();
	Handle rsrc = GetResourceNoLoad(type, name, true);
	
	bool has = rsrc != nil;
	
	if (has)
		ReleaseResource(rsrc);
	SLEnable();
	
	return has;
}


//---------------------------------------------------------------
//
// HasResource (ResType, ResID)
//
//---------------------------------------------------------------
bool HasResource(ResType type, ResID id)
{
	SLDisable();
	Handle rsrc = GetResourceNoLoad(type, id, false);
	
	bool has = rsrc != nil;
	
	if (has)
		ReleaseResource(rsrc);
	SLEnable();
	
	return has;
}


//---------------------------------------------------------------
//
// HasResource (ResType, string)
//
//---------------------------------------------------------------
bool HasResource(ResType type, const std::wstring& name)
{
	SLDisable();
	Handle rsrc = GetResourceNoLoad(type, name, false);
	
	bool has = rsrc != nil;
	
	if (has)
		ReleaseResource(rsrc);
	SLEnable();
	
	return has;
}


//---------------------------------------------------------------
//
// ChangeResourceName
//
//---------------------------------------------------------------
void ChangeResourceName(Handle handle, const std::wstring& name)
{
	int16	id;
	ResType	type;
	Str255	currentName;

	GetResInfo(handle, &id, &type, currentName);
	ThrowIfResError();

	if (name != FromPascalStr(currentName)) {
		SetResInfo(handle, id, ToPascalStr(name).c_str());
		ThrowIfResError();
	}
}


//---------------------------------------------------------------
//
// ReleaseIfResource
//
//---------------------------------------------------------------
Handle ReleaseIfResource(Handle handle)
{
	if (handle != nil) {
		PRECONDITION(IsResourceHand(handle));
		ReleaseResource(handle);
	}

	return nil;
}


//---------------------------------------------------------------
//
// GetResourceInfo
//
//---------------------------------------------------------------
SResourceInfo GetResourceInfo(Handle hand)
{
	int16	id;
	ResType	type;
	Str255	name;

	GetResInfo(hand, &id, &type, name);
	ThrowIfResError();

	return SResourceInfo(type, id, FromPascalStr(name));
}

#pragma mark -

// ===================================================================================
//	class MOpenResFile
// ===================================================================================

//---------------------------------------------------------------
//
// MOpenResFile::~MOpenResFile
//
//---------------------------------------------------------------
MOpenResFile::~MOpenResFile()
{
	if (mRefNum != -1 && mRefNum != 0) 		// zero closes all open resource files!
		CloseResFile(mRefNum);
}

		
//---------------------------------------------------------------
//
// MOpenResFile::MOpenResFile
//
//---------------------------------------------------------------
MOpenResFile::MOpenResFile(const FSSpec& spec, SignedByte permission)
{
	SLDisable();
	mRefNum = FSpOpenResFile(&spec, permission);
	OSErr err = ResError();
	SLEnable();
	
	if (err == eofErr) {
		FInfo info;		
		OSErr err = HGetFInfo(spec.vRefNum, spec.parID, spec.name, &info);
		ThrowIfOSErr(err);
		
		FSpCreateResFile(&spec, info.fdCreator, info.fdType, smSystemScript);
		ThrowIfResError();

		mRefNum = FSpOpenResFile(&spec, permission);
		ThrowIfResError();
		
	} else
		ThrowIfOSErr(err);
}

#pragma mark -

// ===================================================================================
//	class MSaveResFile
// ===================================================================================

//---------------------------------------------------------------
//
// MSaveResFile::~MSaveResFile
//
//---------------------------------------------------------------
#pragma segment ZTools
MSaveResFile::~MSaveResFile()
{
	UseResFile(mSavedResFile);
}

	
//---------------------------------------------------------------
//
// MSaveResFile::MSaveResFile ()
//
//---------------------------------------------------------------
#pragma segment ZTools
MSaveResFile::MSaveResFile()
{
	mSavedResFile = CurResFile();
}


//---------------------------------------------------------------
//
// MSaveResFile::MSaveResFile (int16)
//
//---------------------------------------------------------------
MSaveResFile::MSaveResFile(int16 newResFile)
{
	mSavedResFile = CurResFile();

	if (newResFile != kNoFileRefNum) {
		UseResFile(newResFile);
		ThrowIfResError();
	}
}

#pragma mark -

// ===================================================================================
//	class MNoResLoad
// ===================================================================================

//---------------------------------------------------------------
//
// MNoResLoad::~MNoResLoad
//
//---------------------------------------------------------------
MNoResLoad::~MNoResLoad()
{
	if (mDisabled) 
		SetResLoad(true);
}

	
//---------------------------------------------------------------
//
// MNoResLoad::MNoResLoad
//
//---------------------------------------------------------------
MNoResLoad::MNoResLoad()
{
	mDisabled = LMGetResLoad() != 0; 
	
	if (mDisabled) 
		SetResLoad(false);
}


}	// namespace Whisper
