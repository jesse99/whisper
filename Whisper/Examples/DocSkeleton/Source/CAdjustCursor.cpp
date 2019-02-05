/*
 *  File:       CAdjustCursor.cpp
 *  Summary:   	IAdjustCursorHelper implementation that sets the cursor to a cross-hair.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CAdjustCursor.cpp,v $
 *		Revision 1.1  2001/02/04 03:41:05  jesjones
 *		Checked in
 *		
 */

#include "AppHeader.h"
#include <IAdjustCursorHelper.h>

#include <XConstants.h>
#include <XCursor.h>

using namespace Whisper;


// ===================================================================================
//	class CAdjustCursor
//!		IAdjustCursorHelper implementation that sets the cursor to a cross-hair.
// ===================================================================================
class CAdjustCursor : public IAdjustCursorHelper {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~CAdjustCursor();
	
						CAdjustCursor(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
protected:
	virtual bool 		OnAdjustCursor(const XMouseMoveEvent& event);
};

DEFINE_INTERFACE_FACTORY(CAdjustCursor)

//---------------------------------------------------------------
//
// CAdjustCursor::~CAdjustCursor
//
//---------------------------------------------------------------
CAdjustCursor::~CAdjustCursor()
{
}


//---------------------------------------------------------------
//
// CAdjustCursor::CAdjustCursor
//
//---------------------------------------------------------------
CAdjustCursor::CAdjustCursor(XBoss* boss)
{
	IAdjustCursorHelper::DoSetBoss(boss);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CAdjustCursor::OnAdjustCursor
//
//---------------------------------------------------------------
bool CAdjustCursor::OnAdjustCursor(const XMouseMoveEvent& event)
{
	UNUSED(event);
	
	XCursorUtils::SetCursor(kCrossHairCursor);
	
	return kHandled;
}





