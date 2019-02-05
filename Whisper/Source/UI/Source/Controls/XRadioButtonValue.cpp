/*
 *  File:       XRadioButtonValue.cpp
 *  Summary:   	Attached to the parent of a radio button group to make radios easier to use.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XRadioButtonValue.cpp,v $
 *		Revision 1.4  2001/04/27 04:35:27  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2000/12/19 07:58:15  jesjones
 *		Replaced IInt32Value with a template IValue.
 *		
 *		Revision 1.2  2000/12/14 08:37:00  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/06 07:15:55  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IValue.h>

#include <IHierarchy.h>
#include <IValueControl.h>

namespace Whisper {


// ===================================================================================
//	class XRadioButtonValue
//!		Attached to the parent of a radio button group to make radios easier to use.
// ===================================================================================
class XRadioButtonValue : public IValue<int32> {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XRadioButtonValue();
	
						XRadioButtonValue(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		Get() const;
	
	virtual void 		Set(int32 value, bool redraw = kRedraw);
};

DEFINE_INTERFACE_FACTORY(XRadioButtonValue)

//---------------------------------------------------------------
//
// XRadioButtonValue::~XRadioButtonValue
//
//---------------------------------------------------------------
XRadioButtonValue::~XRadioButtonValue()
{
}


//---------------------------------------------------------------
//
// XRadioButtonValue::XRadioButtonValue
//
//---------------------------------------------------------------
XRadioButtonValue::XRadioButtonValue(XBoss* boss) 
{	
	IValue<int32>::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XRadioButtonValue::Get
//
//---------------------------------------------------------------
int32 XRadioButtonValue::Get() const
{
	int32 value = -1, index = 0;
	
	IConstHierarchyPtr node(this);
	IHierarchy::iterator iter = node->begin();
	while (iter != node->end() && value < 0) {
		IConstValueControlPtr control = *iter;
		++iter;
		
		if (control) {
			if (control->GetValue() == 1)
				value = index;
			else
				++index;
		}
	}
	
	POSTCONDITION(value >= 0);
	
	return value;
}

	
//---------------------------------------------------------------
//
// XRadioButtonValue::Set
//
//---------------------------------------------------------------
void XRadioButtonValue::Set(int32 value, bool redraw)
{
	PRECONDITION(value >= 0);
	
	bool found = false;
	int32 index = 0;
	
	IHierarchyPtr node(this);
	IHierarchy::iterator iter = node->begin();
	while (iter != node->end() && !found) {
		IValueControlPtr control = *iter;
		++iter;
		
		if (control) {
			if (index == value) {
				control->SetValue(1, redraw);
				found = true;

			} else
				++index;
		}
	}
	
	POSTCONDITION(found);
}


}	// namespace Whisper
