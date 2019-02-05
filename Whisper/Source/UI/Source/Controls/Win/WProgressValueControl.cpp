/*
 *  File:       WProgressValueControl.cpp
 *  Summary:   	A value control that groks progress bars.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WProgressValueControl.cpp,v $
 *		Revision 1.6  2001/04/21 03:33:18  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:42:31  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2000/12/14 08:31:36  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/12/10 04:07:42  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:11:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IValueControl.h>

#include <commctrl.h>

#include <IControl.h>
#include <WSystemInfo.h>
#include <XStringUtils.h>

namespace Whisper {	


#define Progress_SetRange(hwnd, low, high) \
   (LRESULT) SendMessage(hwnd, PBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG(low, high))

#define Progress_GetRange(hwnd, whichLimit, range) \
   (LRESULT) SendMessage(hwnd, PBM_GETRANGE, (WPARAM) whichLimit, (LPARAM) range)

#define Progress_SetStep(hwnd, delta) \
   (int) SendMessage(hwnd, PBM_SETSTEP, (WPARAM) delta, 0)

#define Progress_StepIt(hwnd) \
   (int) SendMessage(hwnd, PBM_STEPIT, 0, 0)

#define Progress_SetPos(hwnd, pos) \
   (void) SendMessage(hwnd, PBM_SETPOS, (WPARAM) pos, 0)

#define Progress_DeltaPos(hwnd, delta) \
   (int16) SendMessage(hwnd, PBM_DELTAPOS, (WPARAM) delta, 0)


//-----------------------------------
//	Constants
//
const int32 kShortRange = 30000;		// the stock progress bar only handles unsigned 16-bit values so we'll map values outside the legal range to [0, 30000]


// ===================================================================================
//	class WProgressValueControl
//!		A value control that groks progress bars.
// ===================================================================================
class WProgressValueControl : public IValueControl {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~WProgressValueControl();
	
						WProgressValueControl(XBoss* boss);
						
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

DEFINE_INTERFACE_FACTORY(WProgressValueControl)

//---------------------------------------------------------------
//
// WProgressValueControl::~WProgressValueControl
//
//---------------------------------------------------------------
WProgressValueControl::~WProgressValueControl()
{
}


//---------------------------------------------------------------
//
// WProgressValueControl::WProgressValueControl
//
//---------------------------------------------------------------
WProgressValueControl::WProgressValueControl(XBoss* boss) 
{	
	this->DoSetBoss(boss);
	
	mMinValue = 0;				// note that the OSControl hasn't been created yet
	mMaxValue = 0;
}


//---------------------------------------------------------------
//
// WProgressValueControl::GetValue
//
//---------------------------------------------------------------
int32 WProgressValueControl::GetValue() const
{
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	int16 shortValue = Progress_DeltaPos(controlH, 0);
	int32 value = this->DoMapShortToLong(shortValue);

	return value;
}


//---------------------------------------------------------------
//
// WProgressValueControl::SetValue
//
//---------------------------------------------------------------
bool WProgressValueControl::SetValue(int32 inValue, bool redraw)
{
	PRECONDITION(inValue >= mMinValue);
	PRECONDITION(inValue <= mMaxValue);
	CHECK_INVARIANT;
	
	bool changed = false;
	
	int32 value = MinMax(mMinValue, inValue, mMaxValue);
	
	if (value != this->GetValue()) {
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();
	
		if (!redraw)
			VERIFY(::LockWindowUpdate(controlH));
	
		Progress_SetPos(controlH, this->DoMapLongToShort(value));
		changed = true;
		
		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
	}

	POSTCONDITION(Abs(this->GetValue() - value) <= (1.0/kShortRange)*(mMaxValue - mMinValue));
	
	return changed;
}


//---------------------------------------------------------------
//
// WProgressValueControl::SetMinMaxValue
//
//---------------------------------------------------------------
void WProgressValueControl::SetMinMaxValue(int32 newMin, int32 newMax, bool redraw)
{	
	PRECONDITION(newMin <= newMax);

	if (newMin != mMinValue || newMax != mMaxValue) {		
		IControlPtr control(this);
		HWND controlH = control->GetOSControl();
			
		if (!redraw)
			VERIFY(::LockWindowUpdate(controlH));
	
		mMinValue = newMin;
		mMaxValue = newMax;
				
		int16 min = this->DoMapLongToShort(newMin);
		int32 max = this->DoMapLongToShort(newMax);
		Progress_SetRange(controlH, min, max);

		if (!redraw)
			VERIFY(::LockWindowUpdate(nil));
	}
}


//---------------------------------------------------------------
//
// WProgressValueControl::Invariant
//
//---------------------------------------------------------------
void WProgressValueControl::Invariant() const
{				
	IConstControlPtr control(this);
	HWND controlH = control->GetOSControl();
	
	double maxError = (1.0/kShortRange)*(mMaxValue - mMinValue);

	int16 shortValue = Progress_DeltaPos(controlH, 0);
	ASSERT(shortValue >= 0);
	ASSERT(shortValue <= kShortRange);

	int32 longValue = this->GetValue();		
	ASSERT(Abs(this->DoMapLongToShort(longValue) - shortValue) <= maxError);

	int32 value = this->GetValue();
	ASSERT(value >= mMinValue);
	ASSERT(value <= mMaxValue);
	
	if (WSystemInfo::IsWin98() || WSystemInfo::IsWin2000()) {	
		PBRANGE range;
		Progress_GetRange(controlH, true, &range);
		
		ASSERT(Abs(this->DoMapLongToShort(mMinValue) - range.iLow) <= maxError);
		ASSERT(Abs(this->DoMapLongToShort(mMaxValue) - range.iHigh) <= maxError);
	
		ASSERT(shortValue >= range.iLow);
		ASSERT(shortValue <= range.iHigh);
	}
}


//---------------------------------------------------------------
//
// WProgressValueControl::DoMapLongToShort
//
// Maps values in [mMinValue, mMaxValue] to [0, kShortRange].
//
//---------------------------------------------------------------
int16 WProgressValueControl::DoMapLongToShort(int32 longValue) const
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
// WProgressValueControl::DoMapShortToLong
//
// Maps values in [0, kShortRange] to [mMinValue, mMaxValue].
//
//---------------------------------------------------------------
int32 WProgressValueControl::DoMapShortToLong(int16 shortValue) const
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
