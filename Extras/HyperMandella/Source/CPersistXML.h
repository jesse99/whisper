/*
 *  File:       CPersistXML.h
 *  Summary:   	Functions used to stream the fractal out and in as XML. 
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	For conditions of distribution and use, see copyright notice in ZTypes.h  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CPersistXML.h,v $
 *		Revision 1.1  2000/12/28 02:50:17  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 08:56:15  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 9/22/00	JDJ		Created
 */

#pragma once

#include <IReadXML.h>
#include <XBoss.h>

using namespace Whisper;


// ===================================================================================
//	Global Functions
// ===================================================================================
XXMLElement* WriteDocInfo(const XBoss* boss, XBoss::const_iterator iter);
XXMLElement* WriteCamera(const XBoss* boss, XBoss::const_iterator iter);
XXMLElement* WriteOrthoCamera(const XBoss* boss, XBoss::const_iterator iter);

void 		ReadDocInfo(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
void 		ReadCamera(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
void 		ReadOrthoCamera(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

