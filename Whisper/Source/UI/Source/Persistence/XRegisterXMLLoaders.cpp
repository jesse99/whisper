/*
 *  File:       XRegisterXMLLoaders.cpp
 *  Summary:   	Registers XML load functions for the Whisper classes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRegisterXMLLoaders.cpp,v $
 *		Revision 1.9  2001/04/01 04:59:06  jesjones
 *		Added listbox control.
 *		
 *		Revision 1.8  2001/03/23 10:59:45  jesjones
 *		Added ISeperator support.
 *		
 *		Revision 1.7  2001/03/20 12:02:20  jesjones
 *		Added bevel button.
 *		
 *		Revision 1.6  2000/12/19 07:58:48  jesjones
 *		Added ITextBox and some range interfaces.
 *		
 *		Revision 1.5  2000/12/17 03:51:13  jesjones
 *		Added a color swatch control.
 *		
 *		Revision 1.4  2000/12/06 07:20:50  jesjones
 *		Registered more stuff.
 *		
 *		Revision 1.3  2000/12/05 09:19:39  jesjones
 *		Added ICheckBox and IDialogBox loaders.
 *		
 *		Revision 1.2  2000/11/09 12:49:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XRegisterXMLLoaders.h>

#include <IReadXML.h>
#include <XXMLArchitecture.h>
#include <XXMLControls.h>
#include <XXMLWindows.h>

namespace Whisper {


// ===================================================================================
//	Global Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RegisterWhisperLoaders	
//
//---------------------------------------------------------------
void RegisterWhisperLoaders()
{
	IReadXMLPtr persist(L"XML Persist");
	
	persist->AddReader(L"IBevelButton", LoadBevelButton);
	persist->AddReader(L"IBinding", LoadBinding);
	persist->AddReader(L"IBrushDrawer", LoadBrushDrawer);
	
	persist->AddReader(L"ICheckBox", LoadCheckBox);
	persist->AddReader(L"IColorSwatch", LoadColorSwatch);
	persist->AddReader(L"IControl", LoadControl);
	 
	persist->AddReader(L"IDefaultPositionWindow", LoadDefaultPositionWindow);
	persist->AddReader(L"IDialogBox", LoadDialogBox);
	persist->AddReader(L"IDoubleRange", LoadDoubleRange);

	persist->AddReader(L"IGeometry", LoadGeometry);
	persist->AddReader(L"IGroupBox", LoadGroupBox);
	persist->AddReader(L"IGroupControl", LoadGroupControl);
	
	persist->AddReader(L"IHierarchy", LoadHierarchy);
	
	persist->AddReader(L"IInt32Range", LoadInt32Range);

	persist->AddReader(L"IListBox", LoadListBox);

	persist->AddReader(L"IProgressBar", LoadProgressBar);
	persist->AddReader(L"IPushButton", LoadPushButton);
	
	persist->AddReader(L"IRadioButton", LoadRadioButton);
	persist->AddReader(L"IRootControl", LoadRootControl);
	
	persist->AddReader(L"IScrollableControl", LoadScrollableControl);
	persist->AddReader(L"IScrollBar", LoadScrollBar);
	persist->AddReader(L"IScrollerControl", LoadScrollerControl);
	persist->AddReader(L"ISeperator", LoadSeperator);
	persist->AddReader(L"ISetHelp", LoadSetHelp);
	persist->AddReader(L"ISetDisabledHelp", LoadSetDisabledHelp);
	persist->AddReader(L"ISetCheckedHelp", LoadSetCheckedHelp);
	persist->AddReader(L"IStaticText", LoadStaticText);

	persist->AddReader(L"ITextBox", LoadTextBox);
	
	persist->AddReader(L"IUInt32Range", LoadUInt32Range);
	persist->AddReader(L"IUserControl", LoadUserControl);
	
	persist->AddReader(L"IValidateText", LoadValidateText);
	persist->AddReader(L"IValueControl", LoadValueControl);
	
	persist->AddReader(L"IWindow", LoadWindow);
}


}	// namespace Whisper

