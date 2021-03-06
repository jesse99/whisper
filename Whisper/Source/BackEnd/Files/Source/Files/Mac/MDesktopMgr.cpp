/*
 *  File:		MDesktopMgr.cpp
 *  Summary:	Some handy functions for dealing with the Desktop Manager.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MDesktopMgr.cpp,v $
 *		Revision 1.5  2001/04/27 04:19:37  jesjones
 *		Updated for the XTranscode changes.
 *		
 *		Revision 1.4  2001/04/21 03:25:00  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.3  2000/12/10 04:03:14  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:12:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 2/06/00	JDJ		Moved LaunchApp code from MProcess to here.
 *		 <2>	 7/19/99	JDJ		Tweaked for Carbon.
 *		 <1>	 1/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <MDesktopMgr.h>

#include <Errors.h>

#include <MoreDesktopMgr.h>
#include <MProcess.h>
#include <MVolumeIterator.h>
#include <XExceptions.h>
#include <XFileSystem.h>
#include <XNumbers.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class MDesktopMgr
//		This code is based on that in MoreFiles by Jim Luther and Nitin Ganatra.
// ===================================================================================

//---------------------------------------------------------------
//
// MDesktopMgr::GetComment								[static]
//
//---------------------------------------------------------------
std::wstring MDesktopMgr::GetComment(const FSSpec& spec)
{	
	Str255 comment;
	
	OSErr err = FSpDTGetComment(&spec, comment);
	ThrowIfOSErr(err);
		
	return FromPascalStr(comment);
}


//---------------------------------------------------------------
//
// MDesktopMgr::SetComment								[static]
//
//---------------------------------------------------------------
void MDesktopMgr::SetComment(const FSSpec& spec, const std::wstring& comment)
{
	PRECONDITION(comment.length() <= 200);
	
	OSErr err = FSpDTSetComment(&spec, ToPascalStr(comment).c_str());
	ThrowIfOSErr(err);	
}


//---------------------------------------------------------------
//
// MDesktopMgr::GetApp (int16, OSType)					[static]
//
//---------------------------------------------------------------
FSSpec MDesktopMgr::GetApp(int16 vRefNum, OSType signature)
{
	FSSpec spec;
	
	OSErr err = FSpDTGetAPPL(nil, vRefNum, signature, &spec);
	ThrowIfOSErr(err);

	return spec;
}


//---------------------------------------------------------------
//
// MDesktopMgr::GetApp (OSType, bool)					[static]
//
//---------------------------------------------------------------
FSSpec MDesktopMgr::GetApp(OSType signature, bool allowRemoteVolumes)
{
	FSSpec spec;
	
	bool found = false;
	
	MVolumeIterator iter(allowRemoteVolumes);
	while (iter != MVolumeIterator() && !found) {
		int16 volume = *iter++;
		
		OSErr err = FSpDTGetAPPL(nil, volume, signature, &spec);
		if (err != noErr && err != afpItemNotFound)
			ThrowErr(err);
			
		found = err == noErr;
	}
	
	if (!found)
		ThrowErr(afpItemNotFound);
		
	return spec;
}


//---------------------------------------------------------------
//
// MDesktopMgr::GetIconData								[static]
//
//---------------------------------------------------------------
Handle MDesktopMgr::GetIconData(int16 vRefNum, OSType type, OSType creator, SignedByte iconType)
{	
	Handle hand = nil;
	
	OSErr err = DTGetIcon(nil, vRefNum, iconType, creator, type, &hand);
	ThrowIfOSErr(err);

	ASSERT(hand != nil);
	
	return hand;	
}


//---------------------------------------------------------------
//
// MDesktopMgr::GetIcon									[static]
//
// Adapted from the "icl8 To cicn" snippet by Edgar Lee.
//
//---------------------------------------------------------------
CIconHandle MDesktopMgr::GetIcon(int16 volume, OSType type, OSType creator)
{
	CIconHandle iconH = nil;
	Handle colorIcon  = nil;

	try {
		// Allocate the icon.
		iconH = (CIconHandle) NewHandleClear(sizeof(CIcon));
		ThrowIfMemFail(iconH);
		
		// Get the 'icl8' data and lock it down.
		colorIcon = MDesktopMgr::GetIconData(volume, type, creator, kLarge8BitIcon);
		HLock(colorIcon);
		HNoPurge(colorIcon);
		
		// Fill in the cicn's bitmap fields.	
		Rect bounds = {0, 0, 32, 32};
		
		(**iconH).iconBMap.baseAddr				= nil;
		(**iconH).iconBMap.rowBytes				= 4;
		(**iconH).iconBMap.bounds				= bounds;

		// Fill in the cicn's mask bitmap fields.	
		(**iconH).iconMask.baseAddr				= nil;
		(**iconH).iconMask.rowBytes				= 4;
		(**iconH).iconMask.bounds				= bounds;
		
		// Fill in the cicn's pixmap fields.	
		(**iconH).iconPMap.baseAddr				= nil;
		(**iconH).iconPMap.rowBytes				= numeric_cast<int16>(((8*(bounds.right - bounds.left))/8) | 0x8000);
		(**iconH).iconPMap.bounds				= bounds;
		(**iconH).iconPMap.pmVersion			= 0;
		(**iconH).iconPMap.packType				= 0;
		(**iconH).iconPMap.packSize				= 0;
		(**iconH).iconPMap.hRes					= 72;
		(**iconH).iconPMap.vRes					= 72;
		(**iconH).iconPMap.pixelType			= 0;
		(**iconH).iconPMap.pixelSize			= 8;
		(**iconH).iconPMap.cmpCount				= 1;
		(**iconH).iconPMap.cmpSize				= 8;
#if TARGET_CARBON
		(**iconH).iconPMap.pixelFormat			= k8IndexedPixelFormat;
		(**iconH).iconPMap.pmExt				= nil;
#else
		(**iconH).iconPMap.planeBytes			= 0;
		(**iconH).iconPMap.pmReserved			= 0;
#endif
		(**iconH).iconPMap.pmTable				= GetCTable(8);

		// Set the 'icl8' pixel image to the iconData field.
		(**iconH).iconData = (Handle) colorIcon;
		
		// Resize the 'cicn' for the bitmap image and mask.	
		long bitmapSize = 4*32;									// 4 bytesPerRow * 32 rows
		SetHandleSize((Handle) iconH, sizeof(CIcon) + 2*bitmapSize);
		ThrowIfMemError();
		
		// Copy the 'ICN#' data into the iconMaskData array.
		// Note1: This is an array of shorts, so divide bitmapSize by 2.
		// Note2: The mask comes before the image.  The is opposite of an 'ICN#'
		Handle bwIcon = MDesktopMgr::GetIconData(volume, type, creator, kLargeIcon);

		::BlockMoveData(*bwIcon, &(**iconH).iconMaskData[bitmapSize/2], bitmapSize);	// The 1bit image.
		::BlockMoveData(*bwIcon + bitmapSize, (**iconH).iconMaskData, bitmapSize);	// The mask.
	
		DisposeHandle(bwIcon);

	} catch (...) {
		if (iconH != nil)
			DisposeHandle((Handle) iconH);
			
		if (colorIcon != nil)
			DisposeHandle(colorIcon);
			
		throw;
	}
	
	return iconH;
}


//---------------------------------------------------------------
//
// MDesktopMgr::LaunchApp (OSType, bool, bool, bool)	[static]
//
// This stuff is adapted from TN 1002 by John Montbriand. 
//
//---------------------------------------------------------------
void MDesktopMgr::LaunchApp(OSType signature, bool keepRunning, bool doSwitch, bool allowRemote)
{
	PRECONDITION(keepRunning || doSwitch);		// can't both be false
	
	if (MProcess::IsRunning(signature)) {
		if (doSwitch) {
			ProcessSerialNumber psn = MProcess::GetSerialNumber(signature);
			MProcess::PullToFront(psn);
		}
		
		if (!keepRunning)
			MProcess::Kill();
		
	} else {
		FSSpec appSpec = MDesktopMgr::GetApp(signature, allowRemote);
		
		LaunchParamBlockRec pb;
		
		pb.launchBlockID       = extendedBlock;
		pb.launchEPBLength     = extendedBlockLen;
		pb.launchFileFlags     = 0;
		pb.launchControlFlags  = launchNoFileFlags;
		pb.launchAppSpec       = &appSpec;
		pb.launchAppParameters = nil;
	
		if (keepRunning)
			pb.launchControlFlags |= launchContinue;
	
		if (!doSwitch)
			pb.launchControlFlags |= launchDontSwitch;
			
		OSErr err = LaunchApplication(&pb);
		ThrowIfOSErr(err);
	}
}


//---------------------------------------------------------------
//
// MDesktopMgr::LaunchApp (OSType, XFileSpec, bool, bool, bool) [static]
//
//---------------------------------------------------------------
void MDesktopMgr::LaunchApp(OSType signature, const XFileSpec& spec, bool keepRunning, bool doSwitch, bool allowRemote)
{
	MDesktopMgr::LaunchApp(signature, true, doSwitch, allowRemote);		// apple events sent to self are executed immediately so force keepRunning to true
		
	if (MProcess::IsRunning(signature)) {
		AEDesc 		target = {typeNull, nil};
		AppleEvent	event  = {typeNull, nil};
		AEDescList 	files  = {typeNull, nil};
		AEDesc 		file   = {typeNull, nil};
		AliasHandle aliasH = nil;

		try {
			ProcessSerialNumber psn = MProcess::GetSerialNumber(signature);
			
			// Target the process we just launched.
			OSErr err = AECreateDesc(typeProcessSerialNumber, &psn, sizeof(ProcessSerialNumber), &target);
			ThrowIfOSErr(err);

			// Create the open document apple event.
			err = AECreateAppleEvent(kCoreEventClass, kAEOpenDocuments, &target, kAutoGenerateReturnID, kAnyTransactionID, &event);
			ThrowIfOSErr(err);
			
			// The open document apple event contains a list of aliases 
			// to the files that need to be opened. So we'll first create
			// the list:
			err = AECreateList(nil, 0, false, &files);
			ThrowIfOSErr(err);
			
			// and then add an alias:
			err = NewAlias(nil, &(spec.GetOSSpec()), &aliasH);
			ThrowIfOSErr(err);
			
			HLock((Handle) aliasH);
			err = AECreateDesc(typeAlias, *aliasH, GetHandleSize((Handle) aliasH), &file);
			HUnlock((Handle) aliasH);
			ThrowIfOSErr(err);

			err = AEPutDesc(&files, nil, &file);
			ThrowIfOSErr(err);		
			
			// Add the alias list to our apple event:
			err = AEPutParamDesc(&event, keyDirectObject, &files);
			ThrowIfOSErr(err);
			
			// Send the apple event!
			AEDesc reply  = {typeNull, nil};
			err = AESend(&event, &reply, kAENoReply, kAENormalPriority, kAEDefaultTimeout, nil, nil);
			ThrowIfOSErr(err);
			
			if (aliasH != nil)
				DisposeHandle((Handle) aliasH);
				
			AEDisposeDesc(&target);
			AEDisposeDesc(&event);
			AEDisposeDesc(&files);
			AEDisposeDesc(&file);
		
		} catch (...) {
			if (aliasH != nil)
				DisposeHandle((Handle) aliasH);
				
			AEDisposeDesc(&target);
			AEDisposeDesc(&event);
			AEDisposeDesc(&files);
			AEDisposeDesc(&file);
		
			throw;
		}

	} else
		DEBUGSTR("MProcess::Launch didn't work!");

	if (!keepRunning)
		MProcess::Kill();
}


}	// namespace Whisper
