/*
 *  File:       MExitAction.h
 *  Summary:   	Mixin for objects that need to do cleanup when the app exists abnormally.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MExitAction.h,v $
 *		Revision 1.2  2000/11/09 09:13:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	11/21/97	JDJ		Created (from Raven)
 */

#pragma once

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class MExitActionMixIn
// ===================================================================================
class EXPORT MExitActionMixIn {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual		   		~MExitActionMixIn();
						
						MExitActionMixIn();

//-----------------------------------
//	API
//
public:
	static	void 		DoExitActions();
						// Called by FragTerm when app exits abnormally (eg via ExitToShell,  
						// exit, abort, etc).
			
protected:
	virtual void 		OnAbnormalExit() = 0;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

