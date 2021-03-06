/*
 *  File:		MAlias.cpp
 *  Summary:	AliasHandle wrapper.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MAlias.cpp,v $
 *		Revision 1.1  2001/05/06 10:29:20  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <MAlias.h>

#include <XExceptions.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class MAlias
// ===================================================================================

//---------------------------------------------------------------
//
// MAlias::~MAlias
//
//---------------------------------------------------------------
MAlias::~MAlias()
{
	if (mAlias != nil)
		::DisposeHandle((Handle) mAlias);
}


//---------------------------------------------------------------
//
// MAlias::MAlias ()
//
//---------------------------------------------------------------
MAlias::MAlias()
{
	mAlias = nil;
	
	XFileSystemSpec target;
	OSErr err = NewAlias(nil, &target.GetOSSpec(), &mAlias);
	ThrowIfOSErr(err);
	
	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::MAlias (XFileSystemSpec)
//
//---------------------------------------------------------------
MAlias::MAlias(const XFileSystemSpec& target)
{
	mAlias = nil;
	
	OSErr err = NewAlias(nil, &target.GetOSSpec(), &mAlias);
	ThrowIfOSErr(err);
	
	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::MAlias (XFileSystemSpec, XFileSystemSpec)
//
//---------------------------------------------------------------
MAlias::MAlias(const XFileSystemSpec& fromFile, const XFileSystemSpec& target)
{
	mAlias = nil;
	
	OSErr err = NewAlias(&fromFile.GetOSSpec(), &target.GetOSSpec(), &mAlias);
	ThrowIfOSErr(err);
	
	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::MAlias (AliasHandle)
//
//---------------------------------------------------------------
MAlias::MAlias(AliasHandle takeAlias)
{
	PRECONDITION(takeAlias != nil);
	
	mAlias = takeAlias;
	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::MAlias (ResID)
//
//---------------------------------------------------------------
MAlias::MAlias(ResID aliasID)
{
	mAlias = (AliasHandle) ::GetResource('alis', aliasID);
	ThrowIfResFail(mAlias);
	
	HNoPurge((Handle) mAlias);
	ThrowIfMemError();

	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::MAlias (MAlias)
//
//---------------------------------------------------------------
MAlias::MAlias(const MAlias& rhs)
{
	Handle temp = (Handle) rhs.mAlias;
	OSErr err = HandToHand(&temp);
	ThrowIfOSErr(err);

	mAlias = (AliasHandle) temp;
	mUpdated = false;
}


//---------------------------------------------------------------
//
// MAlias::operator= (MAlias)
//
//---------------------------------------------------------------
MAlias& MAlias::operator=(const MAlias& rhs)
{
	if (this != &rhs) {
		Handle temp = (Handle) rhs.mAlias;
		OSErr err = HandToHand(&temp);
		ThrowIfOSErr(err);

		if (mAlias != nil)
			::DisposeHandle((Handle) mAlias);

		mAlias = (AliasHandle) temp;
		mUpdated = false;
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// MAlias::operator= (XFileSystemSpec)
//
//---------------------------------------------------------------
MAlias& MAlias::operator=(const XFileSystemSpec& rhs)
{
	AliasHandle temp = nil;
	OSErr err = NewAlias(nil, &rhs.GetOSSpec(), &temp);
	ThrowIfOSErr(err);

	if (mAlias != nil)
		::DisposeHandle((Handle) mAlias);

	mAlias = temp;
	mUpdated = false;
	
	return *this;
}


//---------------------------------------------------------------
//
// MAlias::GetTarget (uint32)
//
//---------------------------------------------------------------
XFileSystemSpec MAlias::GetTarget(uint32 mountFlags) const
{
	FSSpec target;
	Boolean wasChanged = false;
	
	OSErr err = noErr;
	if (mountFlags == 0)
		err = ResolveAlias(nil, mAlias, &target, &wasChanged);
	else
		err = ResolveAliasWithMountFlags(nil, mAlias, &target, &wasChanged, mountFlags);	
	ThrowIfOSErr(err);
	
	if (wasChanged)
		mUpdated = true;
	
	return target;
}


//---------------------------------------------------------------
//
// MAlias::GetTarget (XFileSystemSpec)
//
//---------------------------------------------------------------
XFileSystemSpec MAlias::GetTarget(const XFileSystemSpec& fromFile, uint32 mountFlags) const
{
	FSSpec target;
	Boolean wasChanged = false;
	
	OSErr err = noErr;
	if (mountFlags == 0)
		err = ResolveAlias(&fromFile.GetOSSpec(), mAlias, &target, &wasChanged);
	else
		err = ResolveAliasWithMountFlags(&fromFile.GetOSSpec(), mAlias, &target, &wasChanged, mountFlags);	
	ThrowIfOSErr(err);
	
	if (wasChanged)
		mUpdated = true;
	
	return target;
}


//---------------------------------------------------------------
//
// MAlias::Update (XFileSystemSpec)
//
//---------------------------------------------------------------
bool MAlias::Update(const XFileSystemSpec& target)
{
	Boolean wasChanged = false;

	OSErr err = UpdateAlias(nil, &target.GetOSSpec(), mAlias, &wasChanged);
	ThrowIfOSErr(err);
	
	mUpdated = false;

	return wasChanged;
}


//---------------------------------------------------------------
//
// MAlias::Update (XFileSystemSpec, XFileSystemSpec)
//
//---------------------------------------------------------------
bool MAlias::Update(const XFileSystemSpec& fromFile, const XFileSystemSpec& target)
{
	Boolean wasChanged = false;

	OSErr err = UpdateAlias(&fromFile.GetOSSpec(), &target.GetOSSpec(), mAlias, &wasChanged);
	ThrowIfOSErr(err);
	
	mUpdated = false;

	return wasChanged;
}


//---------------------------------------------------------------
//
// MAlias::GetInfo
//
//---------------------------------------------------------------
std::wstring MAlias::GetInfo(int16 index) const
{
	Str63 str;
	OSErr err = GetAliasInfo(mAlias, index, str);
	ThrowIfOSErr(err);
	
	return FromPascalStr(str);
}


}	// namespace Whisper
