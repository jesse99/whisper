/*
 *  File:       XXMLArchitecture.h
 *  Summary:   	Functions uses to stream the architecture classes into and out of XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLArchitecture.h,v $
 *		Revision 1.4  2001/02/07 04:59:23  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/19 07:59:57  jesjones
 *		Added an IValidateText loader and some range loaders.
 *		
 *		Revision 1.2  2000/11/09 12:59:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <IReadXML.h>
#include <XInterfacePtr.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class IHierarchy;
class XURI;
class XXMLDoc;
class XXMLElement;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


// ===================================================================================
//	Global Functions
// ===================================================================================
UI_EXPORT void 		LoadBinding(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadBrushDrawer(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadDoubleRange(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadGeometry(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadHierarchy(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadInt32Range(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadSetHelp(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadSetDisabledHelp(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadSetCheckedHelp(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadUInt32Range(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadValidateText(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
