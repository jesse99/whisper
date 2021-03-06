/*
 *  File:       X3DUtils.cpp
 *  Summary:   	Misc QuickDraw 3D utilities.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: X3DUtils.cpp,v $
 *		Revision 1.4  2001/04/27 09:16:02  jesjones
 *		Windows compile time fixes.
 *		
 *		Revision 1.3  2001/04/14 07:33:25  jesjones
 *		Switched to using the Quesa includes (instead of the QD3D includes).
 *		
 *		Revision 1.2  2000/11/09 12:23:54  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	  8/08/98	JDJ		Tweaked for Windows.
 *		 <3>	 3/09/98	JDJ		CompilerCheck uses new COMPILE_CHECK macro and is 
 *									also now done in releae builds.
 *		 <2>	 1/27/98	JDJ		Added a missing comma to the string table.
 *		 <1>	 1/11/97	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <X3DUtils.h>

#include <QuesaErrors.h>

#include <XDebug.h>
#include <XExceptions.h>

#if MAC
	#include <MSystemInfo.h>
#endif

namespace Whisper {

using namespace std;


// ===================================================================================
//	Internal Types and Variables (from MyErrorHandler.c by Nick Thompson in the QD 3D SDK)
// ===================================================================================

#if DEBUG
enum {
	kNoticeString,
	kWarningString,
	kErrorString,
	kFirstErrorString,
	kLastErrorString,
	kSpaceString,
	kOpenParen,
	kCloseParen,
	kUnknownErrorString
};


// use these to construct intelligable messages
static struct {
	const char*		errorString;
} stringTable[] = {
	"QuickDraw 3D notice  ", 
	"QuickDraw 3D warning  ", 
	"QuickDraw 3D Error  ",
	" First Error: ",
	" Last Error: ",
	" ",
	"(",
	")",
	" Unknown Message\n"
};


/*
	To generate these tables, copy/paste the error codes from QD3DErrors.h
	and use this regexp:
	
	Find:		kQ3Error([0-9A-z]*),
	Replace:	kQ3Error\1, "\kQ3Error\1",
	
	Find:		kQ3Warning([0-9A-z]*),
	Replace:	kQ3Warning, "kQ3Warning"\1, "\kQ3Warning, "kQ3Warning"\1",
	
	Find:		kQ3Notice([0-9A-z]*),
	Replace:	kQ3Notice\1, "\kQ3Notice\1",
*/


// define a table for all of the QuickDraw 3D error codes
struct {
	TQ3Notice	theNotice;
	const char*	theString;
} noticeTable[] = {
	kQ3NoticeDataAlreadyEmpty, "kQ3NoticeDataAlreadyEmpty",
	kQ3NoticeMethodNotSupported, "kQ3NoticeMethodNotSupported",
	kQ3NoticeObjectAlreadySet, "kQ3NoticeObjectAlreadySet",
	kQ3NoticeParameterOutOfRange, "kQ3NoticeParameterOutOfRange",
	kQ3NoticeFileAliasWasChanged, "kQ3NoticeFileAliasWasChanged",
	kQ3NoticeMeshVertexHasNoComponent, "kQ3NoticeMeshVertexHasNoComponent",
	kQ3NoticeMeshInvalidVertexFacePair, "kQ3NoticeMeshInvalidVertexFacePair",
	kQ3NoticeMeshEdgeVertexDoNotCorrespond, "kQ3NoticeMeshEdgeVertexDoNotCorrespond",
	kQ3NoticeMeshEdgeIsNotBoundary, "kQ3NoticeMeshEdgeIsNotBoundary",
	kQ3NoticeDrawContextNotSetUsingInternalDefaults, "kQ3NoticeDrawContextNotSetUsingInternalDefaults",
	kQ3NoticeInvalidAttenuationTypeUsingInternalDefaults, "kQ3NoticeInvalidAttenuationTypeUsingInternalDefaults",
	kQ3NoticeBrightnessGreaterThanOne, "kQ3NoticeBrightnessGreaterThanOne",
	kQ3NoticeScaleContainsZeroEntries, "kQ3NoticeScaleContainsZeroEntries",
	kQ3NoticeSystemAlreadyInitialized, "kQ3NoticeSystemAlreadyInitialized",
	kQ3NoticeViewSyncCalledAgain, "kQ3NoticeViewSyncCalledAgain",
	kQ3NoticeFileCancelled, "kQ3NoticeFileCancelled"
};


struct {
	TQ3Warning	theWarning;
	const char*	theString;
} warningTable[] = {
	/* General System */
	kQ3WarningInternalException, "kQ3WarningInternalException",	
	/* Object Warnings */
	kQ3WarningNoObjectSupportForDuplicateMethod, "kQ3WarningNoObjectSupportForDuplicateMethod",
	kQ3WarningNoObjectSupportForDrawMethod, "kQ3WarningNoObjectSupportForDrawMethod",
	kQ3WarningNoObjectSupportForWriteMethod, "kQ3WarningNoObjectSupportForWriteMethod",
	kQ3WarningNoObjectSupportForReadMethod, "kQ3WarningNoObjectSupportForReadMethod",
	kQ3WarningUnknownElementType, "kQ3WarningUnknownElementType",
	kQ3WarningTypeAndMethodAlreadyDefined, "kQ3WarningTypeAndMethodAlreadyDefined",
	kQ3WarningTypeIsOutOfRange, "kQ3WarningTypeIsOutOfRange",
	kQ3WarningTypeHasNotBeenRegistered, "kQ3WarningTypeHasNotBeenRegistered",
	/* Parameter Warnings */
	kQ3WarningVector3DNotUnitLength, "kQ3WarningVector3DNotUnitLength",
	/* IO Warnings */
	kQ3WarningInvalidSubObjectForObject, "kQ3WarningInvalidSubObjectForObject",
	kQ3WarningInvalidHexString, "kQ3WarningInvalidHexString",
	kQ3WarningUnknownObject, "kQ3WarningUnknownObject",
	kQ3WarningInvalidMetafileObject, "kQ3WarningInvalidMetafileObject",
	kQ3WarningUnmatchedBeginGroup, "kQ3WarningUnmatchedBeginGroup",
	kQ3WarningUnmatchedEndGroup, "kQ3WarningUnmatchedEndGroup",
	kQ3WarningInvalidTableOfContents, "kQ3WarningInvalidTableOfContents",
	kQ3WarningUnresolvableReference, "kQ3WarningUnresolvableReference",
	kQ3WarningNoAttachMethod, "kQ3WarningNoAttachMethod",
	kQ3WarningInconsistentData, "kQ3WarningInconsistentData",
	kQ3WarningReadLessThanSize, "kQ3WarningReadLessThanSize",
	kQ3WarningFilePointerResolutionFailed, "kQ3WarningFilePointerResolutionFailed",
	kQ3WarningFilePointerRedefined, "kQ3WarningFilePointerRedefined",
	kQ3WarningStringExceedsMaximumLength, "kQ3WarningStringExceedsMaximumLength",
	/* Memory Warnings */
	kQ3WarningLowMemory, "kQ3WarningLowMemory",
	kQ3WarningPossibleMemoryLeak, "kQ3WarningPossibleMemoryLeak",
	/* View Warnings */
	kQ3WarningViewTraversalInProgress, "kQ3WarningViewTraversalInProgress",
	kQ3WarningNonInvertibleMatrix, "kQ3WarningNonInvertibleMatrix",
	/* Quaternion Warning */
	kQ3WarningQuaternionEntriesAreZero, "kQ3WarningQuaternionEntriesAreZero",
	/* Renderer Warning */
	kQ3WarningFunctionalityNotSupported, "kQ3WarningFunctionalityNotSupported",
	/* DrawContext Warning */
	kQ3WarningInvalidPaneDimensions, "kQ3WarningInvalidPaneDimensions",
	/* Pick Warning */
	kQ3WarningPickParamOutside, "kQ3WarningPickParamOutside",
	/* Scale Warnings */
	kQ3WarningScaleEntriesAllZero, "kQ3WarningScaleEntriesAllZero",
	kQ3WarningScaleContainsNegativeEntries, "kQ3WarningScaleContainsNegativeEntries",
	/* Generic Warnings */
	kQ3WarningParameterOutOfRange, "kQ3WarningParameterOutOfRange",
	/* Extension Warnings */
	kQ3WarningExtensionNotLoading, "kQ3WarningExtensionNotLoading",
	/* Object Warnings */
	kQ3WarningTypeAlreadyRegistered, "kQ3WarningTypeAlreadyRegistered",
	kQ3WarningTypeSameVersionAlreadyRegistered, "kQ3WarningTypeSameVersionAlreadyRegistered",
	kQ3WarningTypeNewerVersionAlreadyRegistered, "kQ3WarningTypeNewerVersionAlreadyRegistered",
	/* Invalid Group Object */
	kQ3WarningInvalidObjectInGroupMetafile, "kQ3WarningInvalidObjectInGroupMetafile"
};


struct {
	TQ3Error	theError;
	const char*	theString;
} errorTable[] = {
	/* System Errors */
	kQ3ErrorNotInitialized, "kQ3ErrorNotInitialized",
	kQ3ErrorAlreadyInitialized, "kQ3ErrorAlreadyInitialized",
	kQ3ErrorUnimplemented, "kQ3ErrorUnimplemented",
	kQ3ErrorRegistrationFailed, "kQ3ErrorRegistrationFailed",
	/* OS Errors */
	kQ3ErrorUnixError, "kQ3ErrorUnixError",
	kQ3ErrorMacintoshError, "kQ3ErrorMacintoshError",
	kQ3ErrorX11Error, "kQ3ErrorX11Error",
	/* Memory Errors */
	kQ3ErrorMemoryLeak, "kQ3ErrorMemoryLeak",
	kQ3ErrorOutOfMemory, "kQ3ErrorOutOfMemory",
	/* Parameter errors */
	kQ3ErrorNULLParameter, "kQ3ErrorNULLParameter",
	kQ3ErrorParameterOutOfRange, "kQ3ErrorParameterOutOfRange",
	kQ3ErrorInvalidParameter, "kQ3ErrorInvalidParameter",			
	kQ3ErrorInvalidData, "kQ3ErrorInvalidData",				
	kQ3ErrorAcceleratorAlreadySet, "kQ3ErrorAcceleratorAlreadySet",		
	kQ3ErrorVector3DNotUnitLength, "kQ3ErrorVector3DNotUnitLength",
	kQ3ErrorVector3DZeroLength, "kQ3ErrorVector3DZeroLength",
	/* Object Errors */
	kQ3ErrorInvalidObject, "kQ3ErrorInvalidObject",
	kQ3ErrorInvalidObjectClass, "kQ3ErrorInvalidObjectClass",
	kQ3ErrorInvalidObjectType, "kQ3ErrorInvalidObjectType",
	kQ3ErrorInvalidObjectName, "kQ3ErrorInvalidObjectName",
	kQ3ErrorObjectClassInUse, "kQ3ErrorObjectClassInUse",			
	kQ3ErrorAccessRestricted, "kQ3ErrorAccessRestricted",
	kQ3ErrorMetaHandlerRequired, "kQ3ErrorMetaHandlerRequired",
	kQ3ErrorNeedRequiredMethods, "kQ3ErrorNeedRequiredMethods",
	kQ3ErrorNoSubClassType, "kQ3ErrorNoSubClassType",
	kQ3ErrorUnknownElementType, "kQ3ErrorUnknownElementType",
	kQ3ErrorNotSupported, "kQ3ErrorNotSupported",
	/* Extension Errors */
	kQ3ErrorNoExtensionsFolder, "kQ3ErrorNoExtensionsFolder",
	kQ3ErrorExtensionError, "kQ3ErrorExtensionError",
	kQ3ErrorPrivateExtensionError, "kQ3ErrorPrivateExtensionError",
	/* Geometry Errors */
	kQ3ErrorDegenerateGeometry, "kQ3ErrorDegenerateGeometry",
	kQ3ErrorGeometryInsufficientNumberOfPoints, "kQ3ErrorGeometryInsufficientNumberOfPoints",
	/* IO Errors */
	kQ3ErrorNoStorageSetForFile, "kQ3ErrorNoStorageSetForFile",
	kQ3ErrorEndOfFile, "kQ3ErrorEndOfFile",
	kQ3ErrorFileCancelled, "kQ3ErrorFileCancelled",
	kQ3ErrorInvalidMetafile, "kQ3ErrorInvalidMetafile",
 	kQ3ErrorInvalidMetafilePrimitive, "kQ3ErrorInvalidMetafilePrimitive",
 	kQ3ErrorInvalidMetafileLabel, "kQ3ErrorInvalidMetafileLabel",
 	kQ3ErrorInvalidMetafileObject, "kQ3ErrorInvalidMetafileObject",
 	kQ3ErrorInvalidMetafileSubObject, "kQ3ErrorInvalidMetafileSubObject",
	kQ3ErrorInvalidSubObjectForObject, "kQ3ErrorInvalidSubObjectForObject",
	kQ3ErrorUnresolvableReference, "kQ3ErrorUnresolvableReference",
	kQ3ErrorUnknownObject, "kQ3ErrorUnknownObject",
	kQ3ErrorStorageInUse, "kQ3ErrorStorageInUse",
	kQ3ErrorStorageAlreadyOpen, "kQ3ErrorStorageAlreadyOpen",
	kQ3ErrorStorageNotOpen, "kQ3ErrorStorageNotOpen",
	kQ3ErrorStorageIsOpen, "kQ3ErrorStorageIsOpen",
	kQ3ErrorFileAlreadyOpen, "kQ3ErrorFileAlreadyOpen",
	kQ3ErrorFileNotOpen, "kQ3ErrorFileNotOpen",
	kQ3ErrorFileIsOpen, "kQ3ErrorFileIsOpen",
	kQ3ErrorBeginWriteAlreadyCalled, "kQ3ErrorBeginWriteAlreadyCalled",
	kQ3ErrorBeginWriteNotCalled, "kQ3ErrorBeginWriteNotCalled",
	kQ3ErrorEndWriteNotCalled, "kQ3ErrorEndWriteNotCalled",
	kQ3ErrorReadStateInactive, "kQ3ErrorReadStateInactive",
	kQ3ErrorStateUnavailable, "kQ3ErrorStateUnavailable",
	kQ3ErrorWriteStateInactive, "kQ3ErrorWriteStateInactive",
	kQ3ErrorSizeNotLongAligned, "kQ3ErrorSizeNotLongAligned",
	kQ3ErrorFileModeRestriction, "kQ3ErrorFileModeRestriction",
	kQ3ErrorInvalidHexString, "kQ3ErrorInvalidHexString",
	kQ3ErrorWroteMoreThanSize, "kQ3ErrorWroteMoreThanSize",
	kQ3ErrorWroteLessThanSize, "kQ3ErrorWroteLessThanSize",
	kQ3ErrorReadLessThanSize, "kQ3ErrorReadLessThanSize",
	kQ3ErrorReadMoreThanSize, "kQ3ErrorReadMoreThanSize",
	kQ3ErrorNoBeginGroup, "kQ3ErrorNoBeginGroup",
	kQ3ErrorSizeMismatch, "kQ3ErrorSizeMismatch",
	kQ3ErrorStringExceedsMaximumLength, "kQ3ErrorStringExceedsMaximumLength",
	kQ3ErrorValueExceedsMaximumSize, "kQ3ErrorValueExceedsMaximumSize",
	kQ3ErrorNonUniqueLabel, "kQ3ErrorNonUniqueLabel",
	kQ3ErrorEndOfContainer, "kQ3ErrorEndOfContainer",
	kQ3ErrorUnmatchedEndGroup, "kQ3ErrorUnmatchedEndGroup",
	kQ3ErrorFileVersionExists, "kQ3ErrorFileVersionExists",
	/* View errors */
	kQ3ErrorViewNotStarted, "kQ3ErrorViewNotStarted",
	kQ3ErrorViewIsStarted, "kQ3ErrorViewIsStarted",
	kQ3ErrorRendererNotSet, "kQ3ErrorRendererNotSet",
	kQ3ErrorRenderingIsActive, "kQ3ErrorRenderingIsActive",
	kQ3ErrorImmediateModeUnderflow, "kQ3ErrorImmediateModeUnderflow",
	kQ3ErrorDisplayNotSet, "kQ3ErrorDisplayNotSet",
	kQ3ErrorCameraNotSet, "kQ3ErrorCameraNotSet",
	kQ3ErrorDrawContextNotSet, "kQ3ErrorDrawContextNotSet",
	kQ3ErrorNonInvertibleMatrix, "kQ3ErrorNonInvertibleMatrix",
	kQ3ErrorRenderingNotStarted, "kQ3ErrorRenderingNotStarted",
	kQ3ErrorPickingNotStarted, "kQ3ErrorPickingNotStarted",
	kQ3ErrorBoundsNotStarted, "kQ3ErrorBoundsNotStarted",
	kQ3ErrorDataNotAvailable, "kQ3ErrorDataNotAvailable",
	kQ3ErrorNothingToPop, "kQ3ErrorNothingToPop",
	/* Renderer Errors */
	kQ3ErrorUnknownStudioType, "kQ3ErrorUnknownStudioType",			
	kQ3ErrorAlreadyRendering, "kQ3ErrorAlreadyRendering",
	kQ3ErrorStartGroupRange, "kQ3ErrorStartGroupRange",
	kQ3ErrorUnsupportedGeometryType, "kQ3ErrorUnsupportedGeometryType",
	kQ3ErrorInvalidGeometryType, "kQ3ErrorInvalidGeometryType",
	kQ3ErrorUnsupportedFunctionality, "kQ3ErrorUnsupportedFunctionality",
	/* Group Errors */
	kQ3ErrorInvalidPositionForGroup, "kQ3ErrorInvalidPositionForGroup",
	kQ3ErrorInvalidObjectForGroup, "kQ3ErrorInvalidObjectForGroup",
	kQ3ErrorInvalidObjectForPosition, "kQ3ErrorInvalidObjectForPosition",
	/* Transform Errors */
	kQ3ErrorScaleOfZero, "kQ3ErrorScaleOfZero",				
	/* String Errors */
	kQ3ErrorBadStringType, "kQ3ErrorBadStringType",				
	/* Attribute Errors */
	kQ3ErrorAttributeNotContained, "kQ3ErrorAttributeNotContained",		
	kQ3ErrorAttributeInvalidType, "kQ3ErrorAttributeInvalidType",		
	/* Camera Errors */
	kQ3ErrorInvalidCameraValues, "kQ3ErrorInvalidCameraValues",		
	/* DrawContext Errors */
	kQ3ErrorBadDrawContextType, "kQ3ErrorBadDrawContextType",
	kQ3ErrorBadDrawContextFlag, "kQ3ErrorBadDrawContextFlag",
	kQ3ErrorBadDrawContext, "kQ3ErrorBadDrawContext",
	kQ3ErrorUnsupportedPixelDepth, "kQ3ErrorUnsupportedPixelDepth",
	/* Controller Errors */
	kQ3ErrorController, "kQ3ErrorController",
	/* Tracker Errors */
	kQ3ErrorTracker, "kQ3ErrorTracker",
	/* Another OS Error */
	kQ3ErrorWin32Error, "kQ3ErrorWin32Error",
	/* Object Errors */
	kQ3ErrorTypeAlreadyExistsAndHasSubclasses, "kQ3ErrorTypeAlreadyExistsAndHasSubclasses",
	kQ3ErrorTypeAlreadyExistsAndOtherClassesDependOnIt, "kQ3ErrorTypeAlreadyExistsAndOtherClassesDependOnIt",
	kQ3ErrorTypeAlreadyExistsAndHasObjectInstances, "kQ3ErrorTypeAlreadyExistsAndHasObjectInstances",
	kQ3ErrorPickingLoopFailed, "kQ3ErrorPickingLoopFailed",
	kQ3ErrorRenderingLoopFailed, "kQ3ErrorRenderingLoopFailed",
	kQ3ErrorWritingLoopFailed, "kQ3ErrorWritingLoopFailed",
	kQ3ErrorBoundingLoopFailed, "kQ3ErrorBoundingLoopFailed"
};
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// CompilerCheck
//
// Test to make sure the compiler options are correct for QuickDraw 3D.
//
//---------------------------------------------------------------
static void CompilerCheck()
{
	// ints must be 4 bytes (this isn't mentioned in QD3D.h but I'm
	// pretty sure QuickDraw 3D requires it).
	COMPILE_CHECK(sizeof(int) == 4);
	
	// enums must always be ints in QD3D and in applications.
	COMPILE_CHECK(sizeof(TQ3Status) == sizeof(int));
	
	// alignment of char and short arrays in structures is not on
	// long boundaries.
	struct SArrayAlignment {
		char  a;
		char  b[4];
		short c[4];
	};
	
	COMPILE_CHECK(offsetof(SArrayAlignment, a) == 0);
	COMPILE_CHECK(offsetof(SArrayAlignment, b) == 1);
	COMPILE_CHECK(offsetof(SArrayAlignment, c) == 6);
	
#if __POWERPC__
	// alignment of longs, floats, and pointers in structures
	// is on long boundaries for PowerPC
	struct SLongAlignment {
		char   pad1;
		long   a;
		char   pad2;
		float  b;
		char   pad3;
		short* c;
	};
	
	COMPILE_CHECK(offsetof(SLongAlignment, pad1) == 0);
	COMPILE_CHECK(offsetof(SLongAlignment, a) == 4);	
	COMPILE_CHECK(offsetof(SLongAlignment, b) == 12);
	COMPILE_CHECK(offsetof(SLongAlignment, c) == 20);
#endif
}


//---------------------------------------------------------------
//
// MyNoticeHandler
//
//---------------------------------------------------------------
#if DEBUG
static void MyNoticeHandler(TQ3Notice sticky, TQ3Notice latest, uint32 refCon)
{
	UNUSED(refCon);
	
	if (latest != kQ3NoticeSystemAlreadyInitialized) {		// 1.5 and 1.5.1 report this error when the viewer is used and QD 3D is already inited
		std::wstring myNoticeStr;
		std::wstring theWholeString;

		int stickyIndex = sticky - kQ3NoticeDataAlreadyEmpty;
		int latestIndex = latest - kQ3NoticeDataAlreadyEmpty;
		
		// if the error is out of range, then just tell us that something
		// "unknown" happened. Otherwise look up the error from the table.
		if (sticky != kQ3NoticeNone && (sticky < kQ3NoticeDataAlreadyEmpty || sticky > kQ3NoticeFileCancelled)) {
			theWholeString  = FromPlatformStr(stringTable[kNoticeString].errorString);
			theWholeString += FromPlatformStr(stringTable[kUnknownErrorString].errorString);

		} else {
			theWholeString = FromPlatformStr(stringTable[kNoticeString].errorString);
			
			theWholeString += FromPlatformStr(stringTable[kFirstErrorString].errorString);
			myNoticeStr = FromPlatformStr(noticeTable[stickyIndex].theString);
			theWholeString += myNoticeStr;
			
			theWholeString += FromPlatformStr(stringTable[kLastErrorString].errorString);
			myNoticeStr = FromPlatformStr(noticeTable[stickyIndex].theString);
			theWholeString += myNoticeStr + L"\n";
				
			// and display the error string 
			TRACEFLOW("ZNotice", theWholeString);		
		}
	}
		
	// clear down the error
	latest = Q3Notice_Get(&sticky);
}
#endif


//---------------------------------------------------------------
//
// MyWarningHandler
//
//---------------------------------------------------------------
#if DEBUG
static void MyWarningHandler(TQ3Warning sticky, TQ3Warning latest, uint32 refCon)
{
	UNUSED(refCon);
	
	std::wstring myWarningStr;
	std::wstring theWholeString;

	int stickyIndex = sticky - kQ3WarningInternalException;
	int latestIndex = latest - kQ3WarningInternalException;
	
	// if the error is out of range, then just tell us that something
	// "unknown" happened. Otherwise look up the error from the table.
	if (sticky != kQ3ErrorNone && (sticky < kQ3WarningInternalException || sticky > kQ3WarningInvalidObjectInGroupMetafile)) {
		theWholeString  = FromPlatformStr(stringTable[kWarningString].errorString);
		theWholeString += FromPlatformStr(stringTable[kUnknownErrorString].errorString);

	} else {
		theWholeString = FromPlatformStr(stringTable[kWarningString].errorString);
		
		theWholeString += FromPlatformStr(stringTable[kFirstErrorString].errorString);
		myWarningStr = FromPlatformStr(warningTable[stickyIndex].theString);
		theWholeString += myWarningStr;
		
		theWholeString += FromPlatformStr(stringTable[kLastErrorString].errorString);
		myWarningStr = FromPlatformStr(warningTable[stickyIndex].theString);
		theWholeString += myWarningStr + L"\n";
			
		// and display the error string 
		TRACEFLOW("ZWarning", theWholeString);		
	}
	
	// clear down the error
	latest = Q3Warning_Get(&sticky);
}
#endif


//---------------------------------------------------------------
//
// MyErrorHandler
//
//---------------------------------------------------------------
#if DEBUG
static void MyErrorHandler(TQ3Error sticky, TQ3Error latest, uint32 refCon)
{
	UNUSED(refCon);
	
	if (latest != kQ3ErrorAlreadyInitialized) {		// 1.5 and 1.5.1 report this error when the viewer is used and QD 3D is already inited
		std::wstring myErrorStr;
		std::wstring theWholeString;

		int stickyIndex = sticky - kQ3ErrorNotInitialized;
		int latestIndex = latest - kQ3ErrorNotInitialized;
		
		// if the error is out of range, then just tell us that something
		// "unknown" happened. Otherwise look up the error from the table.
		if (sticky != kQ3ErrorNone && (sticky < kQ3ErrorNotInitialized || sticky > kQ3ErrorBoundingLoopFailed)) {
			theWholeString  = FromPlatformStr(stringTable[kErrorString].errorString);
			theWholeString += FromPlatformStr(stringTable[kUnknownErrorString].errorString);

		} else {
			theWholeString = FromPlatformStr(stringTable[kErrorString].errorString);
			
			theWholeString += FromPlatformStr(stringTable[kFirstErrorString].errorString);
			myErrorStr = FromPlatformStr(errorTable[stickyIndex].theString);
			theWholeString += myErrorStr;
			
			theWholeString += FromPlatformStr(stringTable[kLastErrorString].errorString);
			myErrorStr = FromPlatformStr(errorTable[stickyIndex].theString);
			theWholeString += myErrorStr;
				
			// and display the error string 
			DEBUGSTR(theWholeString);		
		}
	}
			
	// clear down the error
	latest = Q3Error_Get(&sticky);
}
#endif

#pragma mark -

// ===================================================================================
//	External Functions
// ===================================================================================

//---------------------------------------------------------------
//
// Init3D
//
//---------------------------------------------------------------
bool Init3D()
{	
	TQ3Status status = kQ3Failure;

#if TARGET_OS_MAC		
	if (CheckCFMLink(Q3Initialize)) 
#elif TARGET_OS_WIN32	
	if (true) 							// $$$ need something better here 
#endif  
	{
		static bool firstTime = true;

		status = Q3Initialize();
		if (status == kQ3Success) {
			if (firstTime) {
#if DEBUG
				Q3Error_Register(MyErrorHandler, 0);		
				Q3Warning_Register(MyWarningHandler, 0);		
				Q3Notice_Register(MyNoticeHandler, 0);		
#endif

				firstTime = false;
			}
			
		} 
	}
		
	return status == kQ3Success;
}


//---------------------------------------------------------------
//
// Terminate3D
//
//---------------------------------------------------------------
void Terminate3D()
{
#if TARGET_OS_MAC		
	if (CheckCFMLink(Q3Initialize)) {
		if (Q3IsInitialized()) {
			TQ3Status status = Q3Exit();
			ASSERT(status == kQ3Success);
		}
	}	
	
#elif TARGET_OS_WIN32					// $$$
	TQ3Status status = Q3Exit();
	ASSERT(status == kQ3Success);
#endif	
}


//---------------------------------------------------------------
//
// ThrowIf3DError
//
//---------------------------------------------------------------
void ThrowIf3DError(TQ3Status status)
{
	if (status != kQ3Success) {
		TQ3Error error = Q3Error_Get(nil);
		ASSERT(error != kQ3ErrorNone);
		
#if TARGET_OS_MAC		
		if (error == kQ3ErrorMacintoshError) {
			OSErr status = Q3MacintoshError_Get(nil);
			ASSERT(status != noErr);
			
			ThrowIfOSErr(status);
		
		} else
			ThrowIfOSErr(error);
			
#elif TARGET_OS_WIN32		
		if (error == kQ3ErrorWin32Error) {
			uint32 status = Q3Error_Get(nil);		
			ASSERT(status != noErr);
			
			ThrowIfErr(status);
		
		} else
			ThrowIfErr(error);
#endif	
	}
}


}	// namespace Whisper
