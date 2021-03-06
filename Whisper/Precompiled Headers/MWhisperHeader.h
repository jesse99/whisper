/*
 *  File:       MWhisperHeader.h
 *  Summary:   	OS specific stuff for inclusion into a precompiled header (typically
 *				XWhisperHeader.h).
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MWhisperHeader.h,v $
 *		Revision 1.3  2001/04/13 07:53:57  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:21:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 6/27/99	JDJ		Removed PLStringFuncs.h
 *		 <2>	 4/12/99	JDJ		Moved MSIPL_DEBUG_MODE definition from MWhisperHeader
 *									to XWhisperHeader.
 *		 <1>	10/31/97	JDJ		Created.
 */

#pragma check_header_flags on


// ===================================================================================
//	MSL Defines
// ===================================================================================
#include <ansi_parms.h>

#ifndef __dest_os
#define __dest_os			__mac_os
#endif

#ifndef __MSL_LONGLONG_SUPPORT__
#define __MSL_LONGLONG_SUPPORT__		
#endif

#ifndef MSIPL_DEF_EXPLICIT
#define MSIPL_DEF_EXPLICIT					// prevent explicit from being defined away (unused in Pro 5)
#endif

#if __MWERKS__ >= 0x2000
#define MSIPL_TEMPL_NEWSPEC	1				// enable null_template (unused in Pro 5)
#endif

#if MULTI_FRAGMENT_APP
#include <size_t.h>							// despite ending with a .h this does not have a using declaration
typedef std::size_t size_t;					// $$$ need this to get global_destructor_chain.c (in Runtime.mcp)
#endif


// ===================================================================================
//	Universal Headers
// ===================================================================================
#define TARGET_API_MAC_CARBON			1

#define USE_OLD_INPUT_SPROCKET_LABELS	0
#define USE_OLD_ISPNEED_STRUCT			0

#if PRECOMPILE_SYSTEM_HEADERS
//	#include <ADSP.h>
//	#include <AEObjects.h>
//	#include <AEPackObject.h>
//	#include <AERegistry.h>
//	#include <AEUserTermTypes.h>
//	#include <AIFF.h>
	#include <Aliases.h>
	#include <AppleEvents.h>
//	#include <AppleGuide.h>
//	#include <AppleScript.h>
//	#include <AppleTalk.h>
//	#include <ASDebugging.h>
//	#include <ASRegistry.h>
	#include <Balloons.h>
//	#include <CMApplication.h>
//	#include <CMComponent.h>
//	#include <CodeFragments.h>
	#include <ColorPicker.h>
//	#include <CommResources.h>
//	#include <Components.h>
	#include <ConditionalMacros.h>
//	#include <Connections.h>
//	#include <ConnectionTools.h>
	#include <Controls.h>
//	#include <ControlStrip.h>
//	#include <CRMSerialDevices.h>
//	#include <CTBUtilities.h>
//	#include <CursorCtl.h>
//	#include <CursorDevices.h>
//	#include <DatabaseAccess.h>
//	#include <DeskBus.h>
	#include <Devices.h>
	#include <Dialogs.h>
//	#include <Dictionary.h>
//	#include <DisAsmLookup.h>
//	#include <Disassembler.h>
	#include <DiskInit.h>
//	#include <Disks.h>
	#include <Displays.h>
	#include <Drag.h>
//	#include <Editions.h>
//	#include <ENET.h>
//	#include <EPPC.h>
//	#include <ErrMgr.h>
	#include <Errors.h>
	#include <Events.h>
//	#include <fenv.h>
	#include <Files.h>
//	#include <FileTransfers.h>
//	#include <FileTransferTools.h>
//	#include <FileTypesAndCreators.h>
	#include <Finder.h>
//	#include <FixMath.h>
	#include <Folders.h>
	#include <Fonts.h>
//	#include <fp.h>
//	#include <FragLoad.h>
//	#include <FSM.h>
	#include <Gestalt.h>
//	#include <HyperXCmd.h>
//	#include <Icons.h>
//	#include <ImageCodec.h>
//	#include <ImageCompression.h>
//	#include <IntlResources.h>
//	#include <Language.h>
//	#include <Lists.h>
	#include <LowMem.h>
//	#include <MachineExceptions.h>
//	#include <MacTCP.h>
//	#include <MediaHandlers.h>
	#include <Memory.h>
	#include <Menus.h>
//	#include <MIDI.h>
	#include <MixedMode.h>
//	#include <Movies.h>
//	#include <MoviesFormat.h>
//	#include <Notification.h>
//	#include <OSA.h>
//	#include <OSAComp.h>
//	#include <OSAGeneric.h>
	#include <OSUtils.h>
	#include <Packages.h>
	#include <Palettes.h>
//	#include <PictUtil.h>
//	#include <PictUtils.h>
//	#include <Power.h>
//	#include <PPCToolbox.h>
	#include <Printing.h>
	#include <Processes.h>
	#include <QDOffscreen.h>
	#include <Quickdraw.h>
	#include <QuickdrawText.h>
//	#include <QuickTimeComponents.h>
	#include <Resources.h>
//	#include <Retrace.h>
//	#include <ROMDefs.h>
	#include <Scrap.h>
//	#include <Script.h>
//	#include <SCSI.h>
	#include <SegLoad.h>
//	#include <Serial.h>
//	#include <ShutDown.h>
//	#include <Slots.h>
//	#include <Sound.h>
//	#include <SoundComponents.h>
//	#include <SoundInput.h>
//	#include <Speech.h>
	#include <StandardFile.h>
//	#include <Start.h>
	#include <Strings.h>
//	#include <Terminals.h>
//	#include <TerminalTools.h>
	#include <TextEdit.h>
//	#include <TextServices.h>
	#include <TextUtils.h>
//	#include <Threads.h>
//	#include <Timer.h>
	#include <ToolUtils.h>
//	#include <Translation.h>
//	#include <TranslationExtensions.h>
	#include <Traps.h>
//	#include <TSMTE.h>
	#include <Types.h>
//	#include <Unmangler.h>
//	#include <Video.h>
	#include <Windows.h>
//	#include <WorldScript.h>
#endif // PRECOMPILE_SYSTEM_HEADERS


