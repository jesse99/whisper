/*
 *  File:       MExitActionMixIn.cpp
 *  Summary:   	Mixin for objects that need to do cleanup when the app exists abnormally.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MExitAction.cpp,v $
 *		Revision 1.3  2001/04/21 03:25:48  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 09:13:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
  */

#include <XWhisperHeader.h>
#include <MExitAction.h>

#include <Set>

#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XDebug.h>

namespace Whisper {


//-----------------------------------
//	Types
//
typedef std::set<MExitActionMixIn*> ZActionList;


//-----------------------------------
//	Variables
//
static XAutoPtr<ZActionList>	sActionList;
static XCriticalSection			sSection;


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// GetExitList
//
//---------------------------------------------------------------
static ZActionList& GetExitList()
{
	if (sActionList.Get() == nil) {
		XEnterCriticalSection enter(sSection);

		if (sActionList.Get() == nil) 
			sActionList.Reset(new ZActionList);
	}
	
	return *sActionList;
}

#pragma mark -

// ===================================================================================
//	class MExitActionMixIn
// ===================================================================================

//---------------------------------------------------------------
//
// MExitActionMixIn::~MExitActionMixIn
//
//---------------------------------------------------------------
MExitActionMixIn::~MExitActionMixIn()
{
	XEnterCriticalSection enter(sSection);

	ZActionList::iterator iter = GetExitList().find(this);
	PRECONDITION(iter != GetExitList().end());
	
	GetExitList().erase(iter);
}


//---------------------------------------------------------------
//
// MExitActionMixIn::MExitActionMixIn
//
//---------------------------------------------------------------
MExitActionMixIn::MExitActionMixIn()
{
	PRECONDITION(GetExitList().find(this) == GetExitList().end());
	
	XEnterCriticalSection enter(sSection);

	GetExitList().insert(this);
}


//---------------------------------------------------------------
//
// MExitActionMixIn::DoExitActions						[static]
//
//---------------------------------------------------------------
void MExitActionMixIn::DoExitActions()
{
	XEnterCriticalSection enter(sSection);

	ZActionList::iterator iter = GetExitList().begin();
	while (iter != GetExitList().end()) {
		MExitActionMixIn* action = *iter++;
		
		action->OnAbnormalExit();
	}
}


}	// namespace Whisper
