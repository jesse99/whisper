/*
 *  File:       M16BitValueControl.cpp
 *  Summary:   	A value control that maps 32-bit values to 16-bit values (most of the
 *				Appearance Manager controls are 32-bit savvy, but progress bars aren't).
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: M16BitValueControl.cpp,v $
 *		Revision 1.6  2001/04/21 02:25:31  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:40:35  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2000/12/14 08:26:32  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:02:27  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 09:08:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IValueControl.h>

#include <IControl.h>
#include <XStringUtils.h>

namespace Whisper {


//-----------------------------------
//	Constants
//
const int32 kShortRange = 30000;


// ===================================================================================
//	class M16BitValueControl
//!		A value control that maps 32-bit values to 16-bit values (most of the Appearance Manager controls are 32-bit savvy, but progress bars aren't).
// ===================================================================================
class M16BitValueControl : public IValueControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~M16BitValueControl();
	
						M16BitValueControl(XBoss* boss);
						
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
	
			int16 		DoMapLongToShort(int32 longValue) const;

			int32 		DoMapShortToLong(int16 shortValue) const;

//-----------------------------------
//	Member Data
//
private:
	int32	mMinValue;
	int32	mMaxValue;
};

DEFINE_INTERFACE_FACTORY(M16BitValueControl)

//---------------------------------------------------------------
//
// M16BitValueControl::~M16BitValueControl
//
//---------------------------------------------------------------
M16BitValueControl::~M16BitValueControl()
{
}


//---------------------------------------------------------------
//
// M16BitValueControl::M16BitValueControl
//
//---------------------------------------------------------------
M16BitValueControl::M16BitValueControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
	
	mMinValue = 0;				// note that the OSControl hasn't been created yet
	mMaxValue = 0;
}


//---------------------------------------------------------------
//
// M16BitValueControl::GetValue
//
//---------------------------------------------------------------
int32 M16BitValueControl::GetValue() const
{
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	int16 shortValue = ::GetControlValue(controlH);
	int32 value = this->DoMapShortToLong(shortValue);

	return value;
}


//---------------------------------------------------------------
//
// M16BitValueControl::SetValue
//
//---------------------------------------------------------------
bool M16BitValueControl::SetValue(int32 inValue, bool redraw)
{
	PRECONDITION(inValue >= mMinValue);
	PRECONDITION(inValue <= mMaxValue);
	CHECK_INVARIANT;
	
	bool changed = false;
	
	int32 value = MinMax(mMinValue, inValue, mMaxValue);
	
	if (value != this->GetValue()) {
		IControlPtr control(this);
		ControlHandle controlH = control->GetOSControl();
		GrafPtr port = control->GetOSDrawContext();
	
		::SetPort(port);
		if (!redraw)				// too much flicker if we rely on update events
			::HidePen();
		::SetControlValue(controlH, this->DoMapLongToShort(value));
		if (!redraw)
			::ShowPen();
			
		changed = true;
	}

	POSTCONDITION(Abs(this->GetValue() - value) <= (1.0/kShortRange)*(mMaxValue - mMinValue));
	
	return changed;
}


//---------------------------------------------------------------
//
// M16BitValueControl::SetMinMaxValue
//
//---------------------------------------------------------------
void M16BitValueControl::SetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{	
	PRECONDITION(newMin <= newMax);
	CHECK_INVARIANT;

	if (newMin != mMinValue || newMax != mMaxValue) {
		int32 oldValue = this->GetValue();
		
		IControlPtr control(this);
		ControlHandle controlH = control->GetOSControl();
		::SetPort(control->GetOSDrawContext());
		::HidePen();
			
		if (newMin != mMinValue) {
			mMinValue = newMin;
			::SetControlMinimum(controlH, this->DoMapLongToShort(newMin));
		}
		
		if (newMax != mMaxValue) {
			mMaxValue = newMax;
			::SetControlMaximum(controlH, this->DoMapLongToShort(newMax));
		}
				
		::ShowPen();
		
		int32 value = this->DoMapShortToLong(::GetControlValue(controlH));
		if (value != oldValue && redraw)
			control->Invalidate();
	}

	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// M16BitValueControl::Invariant
//
//---------------------------------------------------------------
void M16BitValueControl::Invariant() const
{				
	IConstControlPtr control(this);
	ControlHandle controlH = control->GetOSControl();
	
	double maxError = (1.0/kShortRange)*(mMaxValue - mMinValue);
	
	ASSERT(Abs(this->DoMapLongToShort(mMinValue) - GetControlMinimum(controlH)) <= maxError);
	ASSERT(Abs(this->DoMapLongToShort(mMaxValue) - GetControlMaximum(controlH)) <= maxError);

	int16 shortValue = ::GetControlValue(controlH);
	ASSERT(shortValue >= 0);
	ASSERT(shortValue <= kShortRange);
	ASSERT(shortValue >= GetControlMinimum(controlH));
	ASSERT(shortValue <= GetControlMaximum(controlH));
	
	int32 longValue = this->GetValue();		
	ASSERT(Abs(this->DoMapLongToShort(longValue) - shortValue) <= maxError);

	int32 value = this->GetValue();
	ASSERT(value >= mMinValue);
	ASSERT(value <= mMaxValue);
}


//---------------------------------------------------------------
//
// M16BitValueControl::DoMapLongToShort
//
// Maps values in [mMinValue, mMaxValue] to [0, kShortRange].
//
//---------------------------------------------------------------
int16 M16BitValueControl::DoMapLongToShort(int32 longValue) const
{
	PRECONDITION(longValue >= mMinValue);
	PRECONDITION(longValue <= mMaxValue);
	PRECONDITION(mMinValue <= mMaxValue);
		
	int32 longRange  = mMaxValue - mMinValue;	
	int32 shortRange = kShortRange;
	
	double scaleBy = (double) longRange/shortRange;
	
	double value = (longValue - mMinValue)/scaleBy + 0.5;
	value = MinMax(0.0, value, (double) kShortRange);	// allow for rounding errors
	
	int16 shortValue = numeric_cast<int16>(value);
	POSTCONDITION(shortValue >= 0);
		
	return shortValue;
}


//---------------------------------------------------------------
//
// M16BitValueControl::DoMapShortToLong
//
// Maps values in [0, kShortRange] to [mMinValue, mMaxValue].
//
//---------------------------------------------------------------
int32 M16BitValueControl::DoMapShortToLong(int16 shortValue) const
{
	PRECONDITION(mMinValue <= mMaxValue);
	PRECONDITION(shortValue >= 0);
	PRECONDITION(shortValue <= kShortRange);

	int32 longRange  = mMaxValue - mMinValue;	
	int32 shortRange = kShortRange;
	
	double scaleBy = (double) longRange/shortRange;
	
	double value = shortValue*scaleBy + mMinValue + 0.5;
	value = MinMax((double) mMinValue, value, (double) mMaxValue);	// allow for rounding errors
	
	int32 longValue = numeric_cast<int32>(value);
		
	POSTCONDITION(longValue >= mMinValue);
	POSTCONDITION(longValue <= mMaxValue);
	
	return longValue;
}


}	// namespace Whisper
