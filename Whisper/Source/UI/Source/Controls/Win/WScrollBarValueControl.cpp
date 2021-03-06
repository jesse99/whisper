/*
 *  File:       WScrollBarValueControl.cpp
 *  Summary:   	A value control that groks scroll bars.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WScrollBarValueControl.cpp,v $
 *		Revision 1.6  2001/04/21 03:33:40  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:42:35  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/24 09:39:17  jesjones
 *		Calls IValueChanged.
 *		
 *		Revision 1.3  2000/12/14 08:32:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:11:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IValueControl.h>

#include <windowsx.h>

#include <IControl.h>
#include <IValueChanged.h>
#include <WSystemInfo.h>
#include <XStringUtils.h>

namespace Whisper {	


// ===================================================================================
//	class WScrollBarValueControl
//!		A value control that groks scroll bars.
// ===================================================================================
class WScrollBarValueControl : public IValueControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WScrollBarValueControl();
	
						WScrollBarValueControl(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		GetValue() const;		
	virtual bool 		SetValue(int32 value, bool redraw = kRedraw);

	virtual int32		GetMinValue() const									{return mMinValue;}
	virtual int32		GetMaxValue() const									{return mMaxValue;}

	virtual void		SetMinValue(int32 newMin, bool redraw = kRedraw)	{this->SetMinMaxValue(newMin, mMaxValue, redraw);}
	virtual void		SetMaxValue(int32 newMax, bool redraw = kRedraw)	{this->SetMinMaxValue(mMinValue, newMax, redraw);}

	virtual void		SetMinMaxValue(int32 newMin, int32 newMax, bool redraw);

//-----------------------------------
//	Internal API
//
private:
	virtual void 		Invariant() const;
	
//-----------------------------------
//	Member Data
//
private:
	int32	mMinValue;
	int32	mMaxValue;
};

DEFINE_INTERFACE_FACTORY(WScrollBarValueControl)

//---------------------------------------------------------------
//
// WScrollBarValueControl::~WScrollBarValueControl
//
//---------------------------------------------------------------
WScrollBarValueControl::~WScrollBarValueControl()
{
}


//---------------------------------------------------------------
//
// WScrollBarValueControl::WScrollBarValueControl
//
//---------------------------------------------------------------
WScrollBarValueControl::WScrollBarValueControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
	
	mMinValue = 0;				// note that the OSControl hasn't been created yet
	mMaxValue = 0;
}


//---------------------------------------------------------------
//
// WScrollBarValueControl::GetValue
//
//---------------------------------------------------------------
int32 WScrollBarValueControl::GetValue() const
{
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	int32 value = ScrollBar_GetPos(controlH);

	return value;
}


//---------------------------------------------------------------
//
// WScrollBarValueControl::SetValue
//
//---------------------------------------------------------------
bool WScrollBarValueControl::SetValue(int32 inValue, bool redraw)
{
	CHECK_INVARIANT;
	
	bool changed = false;
	
	int32 value = MinMax(mMinValue, inValue, mMaxValue);
	
	if (value != this->GetValue()) {
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();
	
		SCROLLINFO info;
		info.cbSize = sizeof(info);
		info.fMask  = SIF_POS;
	    info.nPos   = value;
	
		(void) ::SetScrollInfo(controlH, SB_CTL, &info, redraw);
		
		IValueChangedPtr notify(this);
		if (notify)
			notify->OnValueChanged(value);
			
		changed = true;
	}
	
	POSTCONDITION(true);
//	POSTCONDITION(this->GetValue() == value);	// $$ disabled because max value doesn't account for thumb size (we could maybe keep our member in sync, but the OS wants the too large value)
	
	return changed;
}


//---------------------------------------------------------------
//
// WScrollBarValueControl::SetMinMaxValue
//
//---------------------------------------------------------------
void WScrollBarValueControl::SetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{	
	PRECONDITION(newMin <= newMax);
	CHECK_INVARIANT;

	if (newMin != mMinValue || newMax != mMaxValue) {		
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();
			
		SCROLLINFO info;
		info.cbSize = sizeof(info);
		info.fMask  = SIF_RANGE;
	    info.nMin   = newMin;
	    info.nMax   = newMax;
	
		(void) ::SetScrollInfo(controlH, SB_CTL, &info, redraw);
	
		mMinValue = newMin;
		mMaxValue = newMax;
	}
}


//---------------------------------------------------------------
//
// WScrollBarValueControl::Invariant
//
//---------------------------------------------------------------
void WScrollBarValueControl::Invariant() const
{				
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask  = SIF_RANGE;

	VERIFY(::GetScrollInfo(controlH, SB_CTL, &info));

	POSTCONDITION(mMinValue == info.nMin);
	POSTCONDITION(mMaxValue == info.nMax);
}


}	// namespace Whisper
