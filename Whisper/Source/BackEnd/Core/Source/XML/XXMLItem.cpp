/*
 *  File:		XXMLItem.cpp
 *  Summary:	Abstract base class for things that can be nested within an XML element.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLItem.cpp,v $
 *		Revision 1.3  2001/04/17 01:45:35  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/10 04:46:49  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLItem.h>

#include <XStreaming.h>
#include <XXMLDocVisitor.h>

namespace Whisper {


// ===================================================================================
//	class XXMLItem
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLItem::~XXMLItem
//
//---------------------------------------------------------------
XXMLItem::~XXMLItem()
{
}


//---------------------------------------------------------------
//
// XXMLItem::XXMLItem
//
//---------------------------------------------------------------
XXMLItem::XXMLItem(EType type) 
{	
	mType = type;
}


//---------------------------------------------------------------
//
// XXMLItem::OnStreamOut
//
//---------------------------------------------------------------
void XXMLItem::OnStreamOut(XOutStream& stream) const
{
	stream << (int32) mType;
}


//---------------------------------------------------------------
//
// XXMLItem::OnStreamIn
//
//---------------------------------------------------------------
void XXMLItem::OnStreamIn(XInStream& stream)
{
	int32 type;
	stream >> type;
	
	mType = (EType) type;
}


}	// namespace Whisper

