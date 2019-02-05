/*
 *  File:       XMemento.cpp
 *  Summary:   	Interface used to save and restore an object's state.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMemento.cpp,v $
 *		Revision 1.2  2000/12/24 03:17:08  jesjones
 *		Added CanMerge to SMemento.
 *		
 *		Revision 1.1  2000/12/22 09:31:36  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IMemento.h>

namespace Whisper {


// ===================================================================================
//	struct SMemento
// ===================================================================================

//---------------------------------------------------------------
//
// SMemento::~SMemento
//
//---------------------------------------------------------------
SMemento::~SMemento()
{
}


//---------------------------------------------------------------
//
// SMemento::CanMerge
//
//---------------------------------------------------------------
bool SMemento::CanMerge(const SMemento* newMemento, const XConstUnknownPtr& object) const
{
	UNUSED(newMemento);
	UNUSED(object);
	
	return false;
}


}	// namespace Whisper
