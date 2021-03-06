/*
 *  File:       MResUtils.h
 *  Summary:    Resource manager utilities.
 *  Written by: Jesse Jones
 *
 *	Classes:	MOpenResFile	- Stack based class used to open a file's resource fork.
 *				MSaveResFile	- Stack based class than can be used to change the topmost file in the resource chain.
 *				MNoResLoad		- Stack based class that disables resource loading.
 *
 *  Copyright � 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MResUtils.h,v $
 *		Revision 1.4  2001/03/05 05:39:40  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:49:11  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 09:19:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <Files.h>
#include <Resources.h>

#include <XHandle.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	struct SResourceInfo
// ===================================================================================
struct SResourceInfo {
	ResType			type;
	ResID			id;
	std::wstring	name;
	
			SResourceInfo()												{type = '????'; id = 0;}
			
			SResourceInfo(ResType tp, ResID i, const std::wstring& nm)	{type = tp; id = i; name = nm;}
};


// ===================================================================================
//	Reading
// ===================================================================================
XHandle 	ReadResource(ResType type, ResID id);

void 		ReadResource(ResType type, ResID id, void* buffer, uint32 bytes);

void 		Read1Resource(ResType type, ResID id, void* buffer, uint32 bytes);


XHandle 	ReadResource(ResType type, const std::wstring& name);

void 		ReadResource(ResType type, const std::wstring& name, void* buffer, uint32 bytes);

void 		Read1Resource(ResType type, const std::wstring& name, void* buffer, uint32 bytes);


// ===================================================================================
//	Writing
// ===================================================================================
void 		WriteResource(ResType type, ResID id, Handle data, bool purgeable = kNonPurgeable);
			// Adds a new resource or overwrites an existing one.

void 		WriteResource(ResType type, ResID id, const void* buffer, uint32 bytes, bool purgeable = kNonPurgeable);


void 		WriteResource(ResType type, const std::wstring& name, Handle data, bool purgeable = kNonPurgeable);

void 		WriteResource(ResType type, const std::wstring& name, const void* buffer, uint32 bytes, bool purgeable = kNonPurgeable);


// ===================================================================================
//	Deleting
// ===================================================================================
void 		Delete1Resource(ResType type);

void 		DeleteResource(ResType type);

void 		DeleteResource(ResType type, ResID id);

void 		DeleteResource(ResType type, const std::wstring& name);


// ===================================================================================
//	Misc
// ===================================================================================
bool 		Has1Resource(ResType type, ResID id);

bool 		HasResource(ResType type, ResID id);

bool 		Has1Resource(ResType type, const std::wstring& name);

bool 		HasResource(ResType type, const std::wstring& name);

void 		ChangeResourceName(Handle handle, const std::wstring& name);

Handle 		ReleaseIfResource(Handle handle);

SResourceInfo GetResourceInfo(Handle hand);

	
// ===================================================================================
//	class MOpenResFile
//!		Stack based class used to open a file's resource fork.
// ===================================================================================
class MOpenResFile {

public:
						~MOpenResFile();
							
						MOpenResFile(const FSSpec& spec, SignedByte Perm);
						/**< Perm can be fsRdPerm, fsWrPerm, fsRdWrPerm, fsRdWrShPerm, or fsCurPerm. */

			int16 		GetRefNum() const				{return mRefNum;}

private:
	int16	mRefNum;
};


// ===================================================================================
//	class MSaveResFile
//!		Stack based class than can be used to change the topmost file in the resource chain.
// ===================================================================================
class MSaveResFile {

public:
						~MSaveResFile();
						/**< Restores the ResFile. */
							
						MSaveResFile();
						/**< Saves the ResFile. */

	explicit			MSaveResFile(int16 newResFile);
						/**< Will save the current ResFile and set newResFile as the new current ResFile. */

private:
	int16	mSavedResFile;
};


// ===================================================================================
//	class MNoResLoad
//!		Stack based class that disables resource loading.
// ===================================================================================
class MNoResLoad {

public:
						~MNoResLoad();
		
						MNoResLoad();
						/**< Disables resource loading. Note that these may be nested. */
		
private:
	bool mDisabled;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
