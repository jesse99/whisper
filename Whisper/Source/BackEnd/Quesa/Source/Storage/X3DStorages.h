/*
 *  File:       X3DStorages.h
 *  Summary:	QuickDraw 3D storage objects.
 *  Written by: Jesse Jones
 *
 *	Classes:	X3DMemoryStorage		- A storage object that works with blocks of memory.
 *					X3DPtrStorage   	- A storage object that works with pointers.
 *					X3DHandleStorage 	- A storage object that works with Mac Handle's.
 *				X3DMacFileStorage		- A storage object that works with file ref numbers.
 *					X3DFSSpecStorage  	- A storage object that works with FSSpec's.
 *				X3DWin32FileStorage		- A storage that works with Window's file handles.
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: X3DStorages.h,v $
 *		Revision 1.3  2000/12/13 08:54:19  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:21:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: X3DStorages.h,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/13 08:54:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <2>	  8/08/98	JDJ		Added X3DWin32FileStorage.
 *		 <1>	  3/01/97	JDJ		Created.
 */

#pragma once

#include <Files.h>

#include <X3DStorage.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class X3DMemoryStorage
//!		A storage object that works with blocks of memory.
// ===================================================================================
class QUESA_EXPORT X3DMemoryStorage : public X3DStorage {

	typedef X3DStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMemoryStorage();
												
						X3DMemoryStorage(uint32 bytes, const void* buffer);
						/**< Copies the contents of the buffer. */
						
						X3DMemoryStorage(TQ3StorageObject object);

	virtual X3DMemoryStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			void 		Set(uint32 bytes, const void* buffer);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMemoryStorage. */
};


// ===================================================================================
//	class X3DPtrStorage
//!		A storage object that works with pointers.
// ===================================================================================
class QUESA_EXPORT X3DPtrStorage : public X3DMemoryStorage {

	typedef X3DMemoryStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DPtrStorage();
												
						X3DPtrStorage(uint32 validBytes, uint32 bufferSize, void* buffer);
						/**< Stores the pointer. Note that you are responsible for disposing
						of the pointer (after the storage object is destroyed). */
						
						X3DPtrStorage(TQ3StorageObject object);

	virtual X3DPtrStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			void 		GetBuffer(uint32* validBytes, uint32* bufferSize, Byte** buffer) const;

			void 		SetBuffer(uint32 validBytes, uint32 bufferSize, void* buffer);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DPtrStorage. */
};


// ===================================================================================
//	class X3DHandleStorage
//!		A storage object that works with Mac Handle's.
// ===================================================================================
#if MAC	
class QUESA_EXPORT X3DHandleStorage : public X3DMemoryStorage {

	typedef X3DMemoryStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DHandleStorage();
												
	explicit			X3DHandleStorage(Handle takeHand);

	explicit			X3DHandleStorage(uint32 bytes);

						X3DHandleStorage(Handle takeHand, uint32 validBytes);
						
						X3DHandleStorage(TQ3StorageObject object);

	virtual X3DHandleStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			void 		Get(Handle* hand, uint32* validSize) const;

			void 		Set(Handle takeHand, uint32 validBytes);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DHandleStorage. */
};
#endif


// ===================================================================================
//	class X3DMacFileStorage
//!		A storage object that works with Mac file ref's.
// ===================================================================================
#if MAC	
class QUESA_EXPORT X3DMacFileStorage : public X3DStorage {

	typedef X3DStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DMacFileStorage();
												
						X3DMacFileStorage(short refNum);
						/**< File should be open. */
						
						X3DMacFileStorage(TQ3StorageObject object);

	virtual X3DMacFileStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			short 		Get() const;

			void 		Set(short refNum);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMacFileStorage. */
};
#endif	// TARGET_OS_MAC	


// ===================================================================================
//	class X3DFSSpecStorage
//!		A storage object that works with FSSpec's.
// ===================================================================================
#if MAC	
class QUESA_EXPORT X3DFSSpecStorage : public X3DMacFileStorage {

	typedef X3DMacFileStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DFSSpecStorage();
												
						X3DFSSpecStorage(const FSSpec& spec);
						/**< File should be closed. */
						
						X3DFSSpecStorage(TQ3StorageObject object);

	virtual X3DFSSpecStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			FSSpec 		Get() const;

			void 		Set(const FSSpec& spec);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DFSSpecStorage. */
};
#endif	// TARGET_OS_MAC	


// ===================================================================================
//	class X3DWin32FileStorage
//!		A storage that works with Window's file handles.
// ===================================================================================
#if WIN	
class QUESA_EXPORT X3DWin32FileStorage : public X3DStorage {

	typedef X3DStorage Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~X3DWin32FileStorage();
												
						X3DWin32FileStorage(HANDLE hFile);
						/**< File should be open. */
						
						X3DWin32FileStorage(TQ3StorageObject object);

	virtual X3DWin32FileStorage* Clone() const;
		
//-----------------------------------
//	New API
//
public:
			HANDLE 		Get() const;

			void 		Set(HANDLE hFile);

	static	bool 		IsCompatible(TQ3Object object);
						/**< Returns true if the QD 3D object can be used to construct a X3DMacFileStorage. */
};
#endif	


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
