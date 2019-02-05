/*
 *  File:       X3DFile.cpp
 *  Summary:	QuickDraw 3D file object.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DFile.cpp,v $
 *		Revision 1.5  2001/04/27 04:22:52  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:35:52  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2001/04/14 07:28:29  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:15:38  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <X3DFile.h>

#include <X3DUtils.h>
#include <XExceptions.h>

namespace Whisper {


// ===================================================================================
//	class X3DFile
// ===================================================================================

//---------------------------------------------------------------
//
// X3DFile::~X3DFile
//
//---------------------------------------------------------------
X3DFile::~X3DFile()
{
	PRECONDITION(!this->IsOpen());		// use Close to close the file (this may fire if an exception is thrown)

	if (this->IsOpen()) {
		TQ3Status status = Q3File_Cancel(mObject);
		ASSERT(status == kQ3Success);
	}
}


//---------------------------------------------------------------
//
// X3DFile::X3DFile ()
//
//---------------------------------------------------------------
X3DFile::X3DFile() : X3DSharedObject(Q3File_New())
{
	TQ3Status status = Q3Object_Dispose(mObject);		// get rid of the new reference added by the X3DSharedObject ctor
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::X3DFile (TQ3FileObject)
//
//---------------------------------------------------------------
X3DFile::X3DFile(TQ3FileObject object) : X3DSharedObject(object)
{
	this->ConfirmType(kQ3SharedTypeFile);
}


//---------------------------------------------------------------
//
// X3DFile::Clone
//
//---------------------------------------------------------------
X3DFile* X3DFile::Clone() const
{
	X3DFile* object = new X3DFile(mObject);
	object->Detach();
	
	return object;
}


//---------------------------------------------------------------
//
// X3DFile::GetStorage
//
//---------------------------------------------------------------
X3DStorage X3DFile::GetStorage() const
{
	TQ3StorageObject object = nil;
	
	TQ3Status status = Q3File_GetStorage(mObject, &object);
	ThrowIf3DError(status);

	X3DStorage obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DFile::SetStorage
//
//---------------------------------------------------------------
void X3DFile::SetStorage(const X3DStorage& storage)
{
	TQ3Status status = Q3File_SetStorage(mObject, storage);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::IsOpen
//
//---------------------------------------------------------------
bool X3DFile::IsOpen() const
{
	TQ3Boolean open;

	TQ3Status status = Q3File_IsOpen(mObject, &open);
	ThrowIf3DError(status);
	
	return open;
}


//---------------------------------------------------------------
//
// X3DFile::OpenRead
//
//---------------------------------------------------------------
TQ3FileMode X3DFile::OpenRead()
{
	TQ3FileMode mode;
	
	TQ3Status status = Q3File_OpenRead(mObject, &mode);
	ThrowIf3DError(status);
	
	return mode;
}


//---------------------------------------------------------------
//
// X3DFile::OpenWrite
//
//---------------------------------------------------------------
void X3DFile::OpenWrite(TQ3FileMode mode)
{
	TQ3Status status = Q3File_OpenWrite(mObject, mode);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::GetMode
//
//---------------------------------------------------------------
TQ3FileMode X3DFile::GetMode() const
{
	TQ3FileMode mode;
	
	TQ3Status status = Q3File_GetMode(mObject, &mode);
	ThrowIf3DError(status);
	
	return mode;
}


//---------------------------------------------------------------
//
// X3DFile::GetVersion
//
//---------------------------------------------------------------
TQ3FileVersion X3DFile::GetVersion() const
{
	TQ3FileVersion version;
	
	TQ3Status status = Q3File_GetMode(mObject, &version);
	ThrowIf3DError(status);
	
	return version;
}


//---------------------------------------------------------------
//
// X3DFile::Close
//
//---------------------------------------------------------------
void X3DFile::Close()
{
	TQ3Status status = Q3File_Close(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::GetNextObjectType
//
//---------------------------------------------------------------
TQ3ObjectType X3DFile::GetNextObjectType() const
{
	TQ3ObjectType type = Q3File_GetNextObjectType(mObject);
	
	return type;
}


//---------------------------------------------------------------
//
// X3DFile::IsNextObjectType
//
//---------------------------------------------------------------
bool X3DFile::IsNextObjectType(TQ3ObjectType type) const
{
	TQ3Boolean isNext = Q3File_IsNextObjectOfType(mObject, type);
	
	return isNext;
}


//---------------------------------------------------------------
//
// X3DFile::ReadObject
//
//---------------------------------------------------------------
X3DSharedObject X3DFile::ReadObject()
{
	TQ3Object object = Q3File_ReadObject(mObject);
	ThrowIfNil(object);
	
	X3DSharedObject obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DFile::SkipObject
//
//---------------------------------------------------------------
void X3DFile::SkipObject()
{
	TQ3Status status = Q3File_SkipObject(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::WriteNewLine
//
//---------------------------------------------------------------
void X3DFile::WriteNewLine()
{
	TQ3Status status = Q3NewLine_Write(mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::WriteComment
//
//---------------------------------------------------------------
void X3DFile::WriteComment(const std::wstring& comment)
{
	TQ3Status status = Q3Comment_Write((char*) ToPlatformStr(comment).c_str(), mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::ReadBytes
//
//---------------------------------------------------------------
void X3DFile::ReadBytes(void* data, uint32 bytes)
{
	PRECONDITION(data != nil);
	PRECONDITION(bytes >= 0);
	
	TQ3Status status = Q3RawData_Read((uint8*) data, bytes, mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::WriteBytes
//
//---------------------------------------------------------------
void X3DFile::WriteBytes(const void* data, uint32 bytes)
{
	PRECONDITION(data != nil);
	PRECONDITION(bytes >= 0);
	
	TQ3Status status = Q3RawData_Write((const uint8*) data, bytes, mObject);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::IsEndOfData
//
//---------------------------------------------------------------
bool X3DFile::IsEndOfData() const
{
	TQ3Boolean isEnd = Q3File_IsEndOfData(mObject);
	
	return isEnd;
}


//---------------------------------------------------------------
//
// X3DFile::IsEndOfContainer
//
//---------------------------------------------------------------
bool X3DFile::IsEndOfContainer(const X3DObject& rootObject) const
{
	TQ3Boolean isEnd = Q3File_IsEndOfContainer(mObject, rootObject);
	
	return isEnd;
}


//---------------------------------------------------------------
//
// X3DFile::IsEndOfFile
//
//---------------------------------------------------------------
bool X3DFile::IsEndOfFile() const
{
	TQ3Boolean isEnd = Q3File_IsEndOfFile(mObject);
	
	return isEnd;
}


//---------------------------------------------------------------
//
// X3DFile::MarkAsExternalReference
//
//---------------------------------------------------------------
void X3DFile::MarkAsExternalReference(const X3DSharedObject& object)
{
	TQ3Status status = Q3File_MarkAsExternalReference(mObject, object);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::GetExternalReferences
//
//---------------------------------------------------------------
X3DGroup X3DFile::GetExternalReferences() const
{
	TQ3GroupObject object = Q3File_GetExternalReferences(mObject);
	ThrowIfNil(object);

	X3DGroup obj(object);
	(void) Q3Object_Dispose(object);
	
	return obj;
}


//---------------------------------------------------------------
//
// X3DFile::SetReadInGroup
//
//---------------------------------------------------------------
void X3DFile::SetReadInGroup(TQ3FileReadGroupState state)
{
	TQ3Status status = Q3File_SetReadInGroup(mObject, state);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::GetReadInGroup
//
//---------------------------------------------------------------
TQ3FileReadGroupState X3DFile::GetReadInGroup() const
{
	TQ3FileReadGroupState state;
	
	TQ3Status status = Q3File_GetReadInGroup(mObject, &state);
	ThrowIf3DError(status);
	
	return state;
}


//---------------------------------------------------------------
//
// X3DFile::SetIdleMethod
//
//---------------------------------------------------------------
void X3DFile::SetIdleMethod(TQ3FileIdleMethod method, const void* data)
{
	TQ3Status status = Q3File_SetIdleMethod(mObject, method, data);
	ThrowIf3DError(status);
}


//---------------------------------------------------------------
//
// X3DFile::IsCompatible 								[static]
//	
//---------------------------------------------------------------
bool X3DFile::IsCompatible(TQ3Object object)
{
	bool compatible = false;
	
	if (object != nil)
		compatible = Q3Object_IsType(object, kQ3SharedTypeFile);
	
	return compatible;
}

#pragma mark -

// ========================================================================================
//	Input Streaming Operators
// ========================================================================================
X3DFile& operator>>(X3DFile& file, bool& value)
{
	uint8 temp;
	
	TQ3Status status = Q3Uns8_Read(&temp, file);
	ThrowIf3DError(status);
	
	value = temp;
	
	return file;
}
		
X3DFile& operator>>(X3DFile& file, char& value)
{
	uint8 temp;
	
	TQ3Status status = Q3Uns8_Read(&temp, file);
	ThrowIf3DError(status);
	
	value = (char) temp;
	
	return file;
}
				
X3DFile& operator>>(X3DFile& file, wchar_t& value)
{
	uint16 temp;
	
	TQ3Status status = Q3Uns16_Read(&temp, file);
	ThrowIf3DError(status);
	
	value = (wchar_t) temp;
	
	return file;
}
				
X3DFile& operator>>(X3DFile& file, short& value)
{
	TQ3Status status = Q3Int16_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, int& value)
{
	long temp;
	
	TQ3Status status = Q3Int32_Read(&temp, file);
	ThrowIf3DError(status);
	
	value = temp;
	
	return file;
}

X3DFile& operator>>(X3DFile& file, long& value)
{
	TQ3Status status = Q3Int32_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, float& value)
{
	TQ3Status status = Q3Float32_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, double& value)
{
	TQ3Status status = Q3Float64_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, unsigned char& value)
{
	TQ3Status status = Q3Uns8_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, unsigned short& value)
{
	TQ3Status status = Q3Uns16_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, unsigned int& value)
{
	uint32 temp;
	
	TQ3Status status = Q3Uns32_Read(&temp, file);
	ThrowIf3DError(status);
	
	value = temp;
	
	return file;
}

X3DFile& operator>>(X3DFile& file, unsigned long& value)
{
	TQ3Status status = Q3Uns32_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, std::wstring& value)
{
	char buffer[kQ3StringMaximumLength];
	
	uint32 length;
	
	TQ3Status status = Q3String_Read(buffer, &length, file);
	ThrowIf3DError(status);
	
	value = FromPlatformStr(buffer, length);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Point2D& value)
{
	TQ3Status status = Q3Point2D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Point3D& value)
{
	TQ3Status status = Q3Point3D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3RationalPoint3D& value)
{
	TQ3Status status = Q3RationalPoint3D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3RationalPoint4D& value)
{
	TQ3Status status = Q3RationalPoint4D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Vector2D& value)
{
	TQ3Status status = Q3Vector2D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Vector3D& value)
{
	TQ3Status status = Q3Vector3D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Matrix4x4& value)
{
	TQ3Status status = Q3Matrix4x4_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Tangent2D& value)
{
	TQ3Status status = Q3Tangent2D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

X3DFile& operator>>(X3DFile& file, TQ3Tangent3D& value)
{
	TQ3Status status = Q3Tangent3D_Read(&value, file);
	ThrowIf3DError(status);
	
	return file;
}

#pragma mark -

// ========================================================================================
//	Output Streaming Operators
// ========================================================================================
X3DFile& operator<<(X3DFile& file, bool value)
{
	TQ3Status status = Q3Uns8_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, char value)
{
	TQ3Status status = Q3Uns8_Write((unsigned char) value, file);
	ThrowIf3DError(status);
		
	return file;
}
				
X3DFile& operator<<(X3DFile& file, wchar_t value)
{
	TQ3Status status = Q3Uns16_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}
				
X3DFile& operator<<(X3DFile& file, short value)
{
	TQ3Status status = Q3Int16_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, int value)
{
	TQ3Status status = Q3Int32_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, long value)
{
	TQ3Status status = Q3Int32_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, float value)
{
	TQ3Status status = Q3Float32_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, double value)
{
	TQ3Status status = Q3Float64_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, unsigned char value)
{
	TQ3Status status = Q3Uns8_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, unsigned short value)
{
	TQ3Status status = Q3Uns16_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, unsigned int value)
{
	TQ3Status status = Q3Uns32_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, unsigned long value)
{
	TQ3Status status = Q3Uns32_Write(value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const std::wstring& value)
{
	TQ3Status status = Q3String_Write(ToPlatformStr(value).c_str(), file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Point2D& value)
{
	TQ3Status status = Q3Point2D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Point3D& value)
{
	TQ3Status status = Q3Point3D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3RationalPoint3D& value)
{
	TQ3Status status = Q3RationalPoint3D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3RationalPoint4D& value)
{
	TQ3Status status = Q3RationalPoint4D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Vector2D& value)
{
	TQ3Status status = Q3Vector2D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Vector3D& value)
{
	TQ3Status status = Q3Vector3D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Matrix4x4& value)
{
	TQ3Status status = Q3Matrix4x4_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Tangent2D& value)
{
	TQ3Status status = Q3Tangent2D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}

X3DFile& operator<<(X3DFile& file, const TQ3Tangent3D& value)
{
	TQ3Status status = Q3Tangent3D_Write(&value, file);
	ThrowIf3DError(status);
		
	return file;
}


}	// namespace Whisper

