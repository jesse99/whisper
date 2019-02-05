/*
 *  File:       XXMLControls.h
 *  Summary:   	Functions uses to stream the control classes into and out of XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLControls.h,v $
 *		Revision 1.9  2001/04/01 04:59:21  jesjones
 *		Added listbox control.
 *		
 *		Revision 1.8  2001/03/23 11:00:00  jesjones
 *		Added ISeperator support.
 *		
 *		Revision 1.7  2001/03/20 12:02:35  jesjones
 *		Added bevel button.
 *		
 *		Revision 1.6  2001/02/07 04:59:28  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/19 08:01:00  jesjones
 *		Added ITextBox.
 *		
 *		Revision 1.4  2000/12/17 03:51:29  jesjones
 *		Added a color swatch control.
 *		
 *		Revision 1.3  2000/12/05 09:20:57  jesjones
 *		Added support for checkboxes and radio buttons.
 *		
 *		Revision 1.2  2000/11/09 13:00:49  jesjones
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
UI_EXPORT void 		LoadControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadBevelButton(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadCheckBox(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadColorSwatch(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadGroupBox(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadGroupControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadListBox(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadProgressBar(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadPushButton(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadRadioButton(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadRootControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadScrollableControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadScrollBar(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadScrollerControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadSeperator(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);
UI_EXPORT void 		LoadStaticText(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadTextBox(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadUserControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

UI_EXPORT void 		LoadValueControl(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
