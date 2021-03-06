/*
 *  File:       MScrollBarMouseHandler.cpp
 *  Summary:   	Mouse handler for scrollbars.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MScrollBarMouseHandler.cpp,v $
 *		Revision 1.3  2001/02/04 03:35:28  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.2  2000/11/09 09:19:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <ITrackControlHelper.h>

#include <IScrollBar.h>

namespace Whisper {


// ===================================================================================
//	class MScrollBarMouseHandler
//!		Mouse handler for scrollbars.
// ===================================================================================
class MScrollBarMouseHandler : public ITrackControlHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MScrollBarMouseHandler();
	
						MScrollBarMouseHandler(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual int32		OnTrackAction(int16 part);
};

DEFINE_INTERFACE_FACTORY(MScrollBarMouseHandler)

//---------------------------------------------------------------
//
// MScrollBarMouseHandler::~MScrollBarMouseHandler
//
//---------------------------------------------------------------
MScrollBarMouseHandler::~MScrollBarMouseHandler()
{
}


//---------------------------------------------------------------
//
// MScrollBarMouseHandler::MScrollBarMouseHandler
//
//---------------------------------------------------------------
MScrollBarMouseHandler::MScrollBarMouseHandler(XBoss* boss)
{
	ITrackControlHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// MScrollBarMouseHandler::OnTrackAction
//
//---------------------------------------------------------------
int32 MScrollBarMouseHandler::OnTrackAction(int16 part)
{
	int32 delta = 0;
	
	IScrollBarPtr scrollbar(this);
	
	switch (part) {
		case kControlPageUpPart:
			delta = -scrollbar->GetPageDelta();
			break;

		case kControlPageDownPart:
			delta = scrollbar->GetPageDelta();
			break;
			
		case kControlUpButtonPart:
			delta = -scrollbar->GetArrowDelta();
			break;

		case kControlDownButtonPart:
			delta = scrollbar->GetArrowDelta();
			break;
	}

	return delta;
}


}	// namespace Whisper
