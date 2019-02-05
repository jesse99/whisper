/*
 *  File:       WButtonValueControl.cpp
 *  Summary:   	The value interface for things like radio buttons and checkboxes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WButtonValueControl.cpp,v $
 *		Revision 1.7  2001/04/21 03:30:28  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/02/24 09:37:41  jesjones
 *		Calls IValueChanged.
 *		
 *		Revision 1.5  2001/02/21 08:15:31  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2000/12/14 08:29:53  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/07 09:49:51  jesjones
 *		Put the class declaration in a header.
 *		
 *		Revision 1.2  2000/11/09 12:05:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IValueControl.h>

#include <windowsx.h>

#include <IControl.h>
#include <IValueChanged.h>
#include <XStringUtils.h>

namespace Whisper {


// ===================================================================================
//	class WButtonValueControl
//!		IValueControl implementation used by Windows button controls.
// ===================================================================================
class WButtonValueControl : public IValueControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WButtonValueControl();
	
						WButtonValueControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		GetValue() const;		
	virtual bool 		SetValue(int32 value, bool redraw = kRedraw);

	virtual int32		GetMinValue() const									{return mMinValue;}
	virtual int32		GetMaxValue() const									{return mMaxValue;}

	virtual void		SetMinValue(int32 newMin, bool redraw = kRedraw)	{this->SetMinMaxValue(newMin, this->GetMaxValue(), redraw);}
	virtual void		SetMaxValue(int32 newMax, bool redraw = kRedraw)	{this->SetMinMaxValue(this->GetMinValue(), newMax, redraw);}

	virtual void		SetMinMaxValue(int32 newMin, int32 newMax, bool redraw);
	
//-----------------------------------
//	New API
//
private:
			void 		DoSetValue(int32 value, bool redraw);
			void		DoSetMinMaxValue(int32 newMin, int32 newMax, bool redraw);
	
//-----------------------------------
//	Member Data
//
private:
	int32	mMinValue;
	int32	mMaxValue;
};


DEFINE_INTERFACE_FACTORY(WButtonValueControl)

//---------------------------------------------------------------
//
// WButtonValueControl::~WButtonValueControl
//
//---------------------------------------------------------------
WButtonValueControl::~WButtonValueControl()
{
}


//---------------------------------------------------------------
//
// WButtonValueControl::WButtonValueControl
//
//---------------------------------------------------------------
WButtonValueControl::WButtonValueControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
	
	mMinValue = 0;
	mMaxValue = 0;
}


//---------------------------------------------------------------
//
// WButtonValueControl::GetValue
//
//---------------------------------------------------------------
int32 WButtonValueControl::GetValue() const
{
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	int32 value = Button_GetCheck(controlH);

	return value;
}


//---------------------------------------------------------------
//
// WButtonValueControl::SetValue
//
//---------------------------------------------------------------
bool WButtonValueControl::SetValue(int32 inValue, bool redraw)
{
	bool changed = false;

	IControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	int32 value = MinMax(mMinValue, inValue, mMaxValue);
	
	int32 oldValue = Button_GetCheck(controlH);
	if (value != oldValue) {
		this->DoSetValue(value, redraw);
		
		changed = true;
	}

	POSTCONDITION(this->GetValue() == value);
	
	return changed;
}


//---------------------------------------------------------------
//
// WButtonValueControl::SetMinMaxValue
//
//---------------------------------------------------------------
void WButtonValueControl::SetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{		
	if (newMin != mMinValue || newMax != mMaxValue)
		this->DoSetMinMaxValue(newMin, newMax, redraw);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// WButtonValueControl::DoSetValue
//
//---------------------------------------------------------------
void WButtonValueControl::DoSetValue(int32 value, bool redraw)
{
	PRECONDITION(value >= mMinValue);
	PRECONDITION(value <= mMaxValue);
	COMPILE_CHECK(BST_CHECKED == 1);
	COMPILE_CHECK(BST_INDETERMINATE == 2);
	
	IControlPtr control(this);
	HWND controlH = control->GetOSControl();
		
	if (!redraw)
		VERIFY(::LockWindowUpdate(controlH));

	Button_SetCheck(controlH, value);
		
	IValueChangedPtr notify(this);
	if (notify)
		notify->OnValueChanged(value);

	if (!redraw)
		VERIFY(::LockWindowUpdate(nil));
}


//---------------------------------------------------------------
//
// WButtonValueControl::DoSetMinMaxValue
//
//---------------------------------------------------------------
void WButtonValueControl::DoSetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{
	PRECONDITION(newMin <= newMax);
	
	IControlPtr control(this);
	HWND controlH = control->GetOSControl();
					
	mMinValue = newMin;
	mMaxValue = newMax;
					
	int32 oldValue = Button_GetCheck(controlH);
	int32 value = MinMax(mMinValue, oldValue, mMaxValue);
	if (value != oldValue)
		this->SetValue(value, redraw);
}


}	// namespace Whisper
