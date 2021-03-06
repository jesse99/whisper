/*
 *  File:       X3DFile.h
 *  Summary:	QuickDraw 3D file object.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DFile.h,v $
 *		Revision 1.4  2001/04/14 07:28:34  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.3  2000/12/13 08:51:34  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:15:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <QuesaIO.h>

#include <X3DGroup.h>
#include <X3DStorage.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DFile
//!		Quesa file object.
// ===================================================================================
class QUESA_EXPORT X3DFile : public X3DSharedObject {

	typedef X3DSharedObject Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DFile();
						/**< If the file is open Q3File_Cancel will be called (so changes
						will be thrown away). */
						
						X3DFile();

						X3DFile(TQ3FileObject object);
						
	virtual X3DFile* 	Clone() const;
		
//-----------------------------------
//	New API
//
public:
	//! @name Conversion Operator
	//@{
						operator TQ3FileObject() const		{return mObject;}
	//@}

	//! @name Storage
	//@{
			X3DStorage 	GetStorage() const;
			
			void 		SetStorage(const X3DStorage& storage);
	//@}

	//! @name Open/Close
	//@{
			bool 		IsOpen() const;
			
			TQ3FileMode OpenRead();
			
			void 		OpenWrite(TQ3FileMode mode);
						/**< Mode can be kQ3FileModeNormal, kQ3FileModeStream, kQ3FileModeDatabase, 
						or kQ3FileModeText. */
			
			TQ3FileMode GetMode() const;
			
			TQ3FileVersion GetVersion() const;
			
			void 		Close();
	//@}

	//! @name Reading (also see the stream operators below)
	//@{
			TQ3ObjectType GetNextObjectType() const;
			
			bool 		IsNextObjectType(TQ3ObjectType type) const;
			
			X3DSharedObject ReadObject();
			
			void 		SkipObject();
			
			void 		ReadBytes(void* data, uint32 bytes);

			bool 		IsEndOfData() const;
			
			bool 		IsEndOfContainer(const X3DObject& rootObject) const;
			
			bool 		IsEndOfFile() const;
	//@}

	//! @name Writing (also see X3DWriteLoop and the stream operators below)
	//@{
			void 		WriteNewLine();
			
			void 		WriteComment(const std::wstring& comment);
						/**< Only affects text files -- no-op in binary files. */
						
			void 		WriteBytes(const void* data, uint32 bytes);
	//@}

	//! @name External File References
	//@{
			void 		MarkAsExternalReference(const X3DSharedObject& object);
			
			X3DGroup 	GetExternalReferences() const;
	//@}

	//! @name Reading objects inside a group one-by-one
	//@{
			void 		SetReadInGroup(TQ3FileReadGroupState state);
						/**< State can be sum of kQ3FileReadWholeGroup, kQ3FileReadObjectsInGroup, 
						and/or kQ3FileCurrentlyInsideGroup. */
						
			TQ3FileReadGroupState GetReadInGroup() const;
	//@}

	//! @name Idling
	//@{
			void 		SetIdleMethod(TQ3FileIdleMethod method, const void* data);
	//@}

	//! @name Misc
	//@{
	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DFile. */
	//@}
};


// ========================================================================================
//	Input Streaming Operators (usage is like this: file >> ch)
// ========================================================================================
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, bool& value);		
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, char& value);				
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, wchar_t& value);				
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, short& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, int& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, long& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, float& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, double& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, unsigned char& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, unsigned short& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, unsigned int& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, unsigned long& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, std::wstring& value);

QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Point2D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Point3D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3RationalPoint3D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3RationalPoint4D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Vector2D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Vector3D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Matrix4x4& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Tangent2D& value);
QUESA_EXPORT X3DFile& operator>>(X3DFile& file, TQ3Tangent3D& value);


// ========================================================================================
//	Output Streaming Operators (usage is like this: file << ch)
// ========================================================================================
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, bool value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, char value);				
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, wchar_t value);				
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, short value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, int value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, long value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, float value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, double value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, unsigned char value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, unsigned short value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, unsigned int value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, unsigned long value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const std::wstring& value);

QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Point2D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Point3D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3RationalPoint3D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3RationalPoint4D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Vector2D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Vector3D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Matrix4x4& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Tangent2D& value);
QUESA_EXPORT X3DFile& operator<<(X3DFile& file, const TQ3Tangent3D& value);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
