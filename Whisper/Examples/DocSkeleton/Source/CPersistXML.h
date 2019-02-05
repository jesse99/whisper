/*
 *  File:       CPersistXML.h
 *  Summary:   	Functions used to stream the document out and in as XML. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPersistXML.h,v $
 *		Revision 1.1  2000/12/31 04:23:44  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <IReadXML.h>
#include <XBoss.h>

using namespace Whisper;


// ===================================================================================
//	Global Functions
// ===================================================================================
XXMLElement* WriteDocInfo(const XBoss* boss, XBoss::const_iterator iter);

void 		ReadDocInfo(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
