/*
 *  File:		MAlias.h
 *  Summary:	AliasHandle wrapper.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAlias.h,v $
 *		Revision 1.1  2001/05/06 10:29:23  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XFileSystemSpec.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MAlias
//!		AliasHandle wrapper.
// ===================================================================================
class FILES_EXPORT MAlias {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~MAlias();
	
						MAlias();
						/**< Points to a missing file. */

						MAlias(const XFileSystemSpec& target);

						MAlias(const XFileSystemSpec& fromFile, const XFileSystemSpec& target);
						/**< Creates a relative alias. */

	explicit			MAlias(AliasHandle takeAlias);

	explicit			MAlias(ResID aliasID);
	
						MAlias(const MAlias& rhs);
						
			MAlias& 	operator=(const MAlias& rhs);
			
			MAlias& 	operator=(const XFileSystemSpec& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Resolution -----
			XFileSystemSpec GetTarget(uint32 mountFlags = 0) const;
						/**< Resolves the alias. Throws if the target cannot be found. 
						MountFlags can be 0 or a sum of kARMMountVol, kARMNoUI, kARMMultVols, 
						kARMSearch, kARMSearchMore, and kARMSearchRelFirst. */
						
			XFileSystemSpec GetTarget(const XFileSystemSpec& fromFile, uint32 mountFlags = 0) const;
						/**< Resolves the alias relative to fromFile. */
			
	// ----- Updating -----
			bool 		Update(const XFileSystemSpec& target);
						/**< Use this to point the alias at a new target. Returns true if 
						the alias changed. */ 
			
			bool 		Update(const XFileSystemSpec& fromFile, const XFileSystemSpec& target);
						/**< Use this to point the alias at a new relative target. */
			
	// ----- Info -----
			std::wstring GetInfo(int16 index) const;
						/**< Returns imformation about the alias without actually resolving
						it. Index can be asiZoneName, asiServerName, asiVolumeName, asiAliasName, 
						or asiParentName. */
						
			bool 		WasUpdated() const							{return mUpdated;}
						/**< Returns true if the alias information was updated during a 
						resolve. */
						
			AliasHandle GetAlias() const							{return mAlias;}
						/**< Note that this can be treated as an ordinary handle. */ 
			
//-----------------------------------
//	Member Data
//
private:
	AliasHandle		mAlias;
	mutable bool	mUpdated;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
