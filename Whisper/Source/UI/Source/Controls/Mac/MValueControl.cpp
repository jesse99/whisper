/*
 *  File:       MValueControl.cpp
 *  Summary:   	The interface used for controls that have values.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MValueControl.cpp,v $
 *		Revision 1.7  2001/04/21 03:29:21  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/02/07 02:28:40  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2000/12/10 03:59:25  jesjones
 *		Removed two ASSERTs.
 *		
 *		Revision 1.4  2000/12/07 09:46:22  jesjones
 *		Tweaked the ASSERTs.
 *		
 *		Revision 1.3  2000/12/05 09:11:26  jesjones
 *		Moved the declaration into a header.
 *		
 *		Revision 1.2  2000/11/09 09:21:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IValueControl.h>

#include <IControl.h>
#include <IHierarchy.h>
#include <IRootControl.h>
#include <IValueChanged.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class MValueControl
//!		IValueControl implementation used by most of the Mac value controls.
// ===================================================================================
class MValueControl : public IValueControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~MValueControl();
	
						MValueControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		GetValue() const;		
	virtual bool 		SetValue(int32 value, bool redraw = kRedraw);

	virtual int32		GetMinValue() const;
	virtual int32		GetMaxValue() const;

	virtual void		SetMinValue(int32 newMin, bool redraw = kRedraw)	{this->SetMinMaxValue(newMin, this->GetMaxValue(), redraw);}
	virtual void		SetMaxValue(int32 newMax, bool redraw = kRedraw)	{this->SetMinMaxValue(this->GetMinValue(), newMax, redraw);}

	virtual void		SetMinMaxValue(int32 newMin, int32 newMax, bool redraw);
	
//-----------------------------------
//	Internal API
//
private:
	virtual void 		DoSetValue(int32 value, bool redraw);
	virtual void		DoSetMinMaxValue(int32 newMin, int32 newMax, bool redraw);
};

DEFINE_INTERFACE_FACTORY(MValueControl)

//---------------------------------------------------------------
//
// MValueControl::~MValueControl
//
//---------------------------------------------------------------
MValueControl::~MValueControl()
{
}


//---------------------------------------------------------------
//
// MValueControl::MValueControl
//
//---------------------------------------------------------------
MValueControl::MValueControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// MValueControl::GetValue
//
//---------------------------------------------------------------
int32 MValueControl::GetValue() const
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	int32 value = ::GetControl32BitValue(controlH);

	return value;
}


//---------------------------------------------------------------
//
// MValueControl::SetValue
//
//---------------------------------------------------------------
bool MValueControl::SetValue(int32 inValue, bool redraw)
{
	bool changed = false;
	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	int32 minValue = ::GetControl32BitMinimum(controlH);
	int32 maxValue = ::GetControl32BitMaximum(controlH);
	int32 oldValue = ::GetControl32BitValue(controlH);
	
	int32 value = MinMax(minValue, inValue, maxValue);
	
	if (value != oldValue) {
		this->DoSetValue(value, redraw);
			
		changed = true;
	}

	ASSERT(this->GetValue() == value);
	
	return changed;
}


//---------------------------------------------------------------
//
// MValueControl::GetMinValue
//
//---------------------------------------------------------------
int32 MValueControl::GetMinValue() const				
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	int32 minValue = ::GetControl32BitMinimum(controlH);
	
	return minValue;
}


//---------------------------------------------------------------
//
// MValueControl::GetMaxValue
//
//---------------------------------------------------------------
int32 MValueControl::GetMaxValue() const				
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	int32 maxValue = ::GetControl32BitMaximum(controlH);
	
	return maxValue;
}


//---------------------------------------------------------------
//
// MValueControl::SetMinMaxValue
//
//---------------------------------------------------------------
void MValueControl::SetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	int32 minValue = ::GetControl32BitMinimum(controlH);
	int32 maxValue = ::GetControl32BitMaximum(controlH);
	
	if (newMin != minValue || newMax != maxValue)
		this->DoSetMinMaxValue(newMin, newMax, redraw);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// MValueControl::DoSetValue
//
//---------------------------------------------------------------
void MValueControl::DoSetValue(int32 value, bool redraw)
{
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	IControlPtr root = GetParent<IRootControl>(this);
	XControlContext context(root);
	context.MakeCurrent();

	if (!redraw)
		::HidePen();

	::SetControl32BitValue(controlH, value);

	if (!redraw)
		::ShowPen();
		
	IValueChangedPtr changed(this);
	if (changed)
		changed->OnValueChanged(value);
}

	
//---------------------------------------------------------------
//
// MValueControl::DoSetMinMaxValue
//
//---------------------------------------------------------------
void MValueControl::DoSetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{
	PRECONDITION(newMin <= newMax);
	
	IControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();

	int32 oldValue = ::GetControl32BitValue(controlH);
	
	::SetPort(control->GetOSDrawContext());
	::HidePen();
		
	::SetControl32BitMinimum(controlH, newMin);	
	::SetControl32BitMaximum(controlH, newMax);
			
	::ShowPen();
	
	int32 value = ::GetControl32BitValue(controlH);
	if (value != oldValue && redraw)
		control->Invalidate();
		
	POSTCONDITION(value >= newMin);
	POSTCONDITION(value <= newMax);
}


}	// namespace Whisper
