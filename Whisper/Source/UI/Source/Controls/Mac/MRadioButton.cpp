/*
 *  File:       MRadioButton.cpp
 *  Summary:   	System radio button control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MRadioButton.cpp,v $
 *		Revision 1.7  2001/04/21 03:28:07  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/02/07 02:28:34  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.5  2001/02/04 03:47:43  jesjones
 *		Removed an obsolete include.
 *		
 *		Revision 1.4  2001/02/04 03:35:24  jesjones
 *		Removed the implementation inheritance from the mouse handling classes.
 *		
 *		Revision 1.3  2000/12/31 04:33:17  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.2  2000/12/14 08:28:37  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.1  2000/12/05 09:02:13  jesjones
 *		Checked in	
 */

#include <XWhisperHeader.h>
#include <IRadioButton.h>

#include <IControl.h>
#include <IControlPressedHelper.h>
#include <IHierarchy.h>
#include <IText.h>
#include <IValueChanged.h>
#include <IValueControl.h>
#include <XExceptions.h>

namespace Whisper {

extern bool gExitingNormally;


// ========================================================================================
//	class ZRadioButtonMgr
//		Singleton used to to ensure that only one radio button in a family is set at
//		any one time.
// ========================================================================================
class ZRadioButtonMgr {

public:
								~ZRadioButtonMgr();
	
								ZRadioButtonMgr();
			
	static	ZRadioButtonMgr*	Instance();

			void 				AddButton(IRadioButton* button, const std::wstring& family);
			
			void 				RemoveButton(IRadioButton* button);
						
			void 				UpdateButtons(IRadioButton* onButton);

private:
	struct Button {
		IRadioButton* 	radio;
		std::wstring	family;
		WindowRef		wptr;
		
					Button(IRadioButton* r, const std::wstring& f, WindowRef w) : radio(r), family(f), wptr(w) {}
		
			bool 	operator==(const Button& rhs) const			{return radio == rhs.radio;}
			bool 	operator==(const IRadioButton* rhs) const	{return radio == rhs;}

			bool 	operator!=(const IRadioButton* rhs) const	{return radio != rhs;}
	};
			
private:
	std::list<Button>	mButtons;
};

//---------------------------------------------------------------
//
// ZRadioButtonMgr::~ZRadioButtonMgr
//
//---------------------------------------------------------------
ZRadioButtonMgr::~ZRadioButtonMgr()
{
#if DEBUG
	if (gExitingNormally)
		ASSERT(mButtons.empty());
#endif
}


//---------------------------------------------------------------
//
// ZRadioButtonMgr::ZRadioButtonMgr
//
//---------------------------------------------------------------
ZRadioButtonMgr::ZRadioButtonMgr()
{
}


//---------------------------------------------------------------
//
// ZRadioButtonMgr::Instance							[static]
//
//---------------------------------------------------------------
ZRadioButtonMgr* ZRadioButtonMgr::Instance()
{
	static ZRadioButtonMgr instance;
	
	return &instance;
}


//---------------------------------------------------------------
//
// ZRadioButtonMgr::AddButton
//
//---------------------------------------------------------------
void ZRadioButtonMgr::AddButton(IRadioButton* button, const std::wstring& family)
{
	PRECONDITION(button != nil);
	PRECONDITION(find(mButtons.begin(), mButtons.end(), button) == mButtons.end());
	
	IControlPtr control(button);
	WindowRef wptr = control->GetOSWindow();
	
	mButtons.push_back(Button(button, family, wptr));
}


//---------------------------------------------------------------
//
// ZRadioButtonMgr::RemoveButton
//
//---------------------------------------------------------------
void ZRadioButtonMgr::RemoveButton(IRadioButton* button)
{
	PRECONDITION(button != nil);
	
	std::list<Button>::iterator iter = find(mButtons.begin(), mButtons.end(), button);
	PRECONDITION(iter != mButtons.end());
	
	mButtons.erase(iter);
}


//---------------------------------------------------------------
//
// ZRadioButtonMgr::UpdateButtons
//
//---------------------------------------------------------------
void ZRadioButtonMgr::UpdateButtons(IRadioButton* inButton)
{
	PRECONDITION(inButton != nil);
	PRECONDITION(inButton->IsSelected());		
	PRECONDITION(!inButton->IsIndeterminate());		// should not be called for mixed mode
		
	// Find the button's family (bit inefficient to search twice,
	// but there shouldn't ever be enough active radio buttons to
	// cause problems).
	std::list<Button>::iterator iter = find(mButtons.begin(), mButtons.end(), inButton);
	PRECONDITION(iter != mButtons.end());
	const Button& onButton = *iter;
	
	// Turn off the other radio buttons in the family.
	iter = mButtons.begin();
	while (iter != mButtons.end()) {
		const Button& button = *iter;
		++iter;
		
		if (button != inButton && button.family == onButton.family && button.wptr == onButton.wptr) {
			IValueControlPtr value(button.radio);
			value->SetValue(0);
		}
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XRadioButton
//!		System radio button control.
// ===================================================================================
class XRadioButton : public IRadioButton, public IControlPressedHelper, public IValueChanged {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XRadioButton();
	
						XRadioButton(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		Init(const IHierarchyPtr& parent, const std::wstring& name, 
							 const XRect& frame, const std::wstring& text, 
							 const std::wstring& family, uint32 style = kDefaultRadioButtonStyle,
							 uint32 procID = kDefaultRadioButtonProcID);
	
	virtual bool		IsSelected() const;

	virtual bool		IsIndeterminate() const;

protected:
	virtual void 		OnPressed(const XMouseDownEvent& event);

	virtual void 		OnValueChanged(int32 value);
};

DEFINE_INTERFACE_FACTORY(XRadioButton)

//---------------------------------------------------------------
//
// XRadioButton::~XRadioButton
//
//---------------------------------------------------------------
XRadioButton::~XRadioButton()
{
	ZRadioButtonMgr::Instance()->RemoveButton(this);
}


//---------------------------------------------------------------
//
// XRadioButton::XRadioButton
//
//---------------------------------------------------------------
XRadioButton::XRadioButton(XBoss* boss) 
{	
	IRadioButton::DoSetBoss(boss);
	IControlPressedHelper::DoSetBoss(boss);
	IValueChanged::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XRadioButton::Init 
//
//---------------------------------------------------------------
void XRadioButton::Init(const IHierarchyPtr& parent, const std::wstring& name, 
					   const XRect& frame, const std::wstring& title, 
					   const std::wstring& family, uint32 style, uint32 procID)
{
	UNUSED(style);
	
	// Get the control's bounding box in window coordinates
	IControlPtr super = GetInterface<IControl>(parent);
	XRect bounds = super->LocalToWindow(frame);
	
	// Set the port and hide the pen (we want to wait for an
	// update event before we draw anything)
	GrafPtr port = super->GetOSDrawContext();
	::SetPort(port);
	::HidePen();
	
	// Create the control and show the pen
	Rect temp = bounds.GetOSRect();
	ControlHandle controlH = ::NewControl(super->GetOSWindow(), &temp, "\p", true, 0, 0, 2, (int16) procID, 0);
	
	::ShowPen();
	ThrowIfNil(controlH);
	
	// Let the IControl interface do a bit of initialization
	IRadioButton* thisPtr = this;
	IControlPtr control(thisPtr);
	control->DoPostCreate(name, parent, controlH);
	
	// Set the control's title		
	ITextPtr text(thisPtr);
	text->SetText(title, kDontRedraw);

	// Register ourself with the radio button manager
	ZRadioButtonMgr::Instance()->AddButton(this, family);
}


//---------------------------------------------------------------
//
// XRadioButton::IsSelected
//
//---------------------------------------------------------------
bool XRadioButton::IsSelected() const
{
	const IRadioButton* thisPtr = this;
	IConstValueControlPtr control(thisPtr);
	int32 value = control->GetValue();
	
	return value > 0;
}


//---------------------------------------------------------------
//
// XRadioButton::IsIndeterminate
//
//---------------------------------------------------------------
bool XRadioButton::IsIndeterminate() const
{
	const IRadioButton* thisPtr = this;
	IConstValueControlPtr control(thisPtr);
	int32 value = control->GetValue();
	
	return value == 2;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XRadioButton::OnPressed
//
//---------------------------------------------------------------
void XRadioButton::OnPressed(const XMouseDownEvent& event)
{
	UNUSED(event);
	
	IRadioButton* thisPtr = this;
	IValueControlPtr control(thisPtr);
	int32 value = control->GetValue();

	if (value != 1) 
		control->SetValue(1, kRedraw);
}


//---------------------------------------------------------------
//
// XRadioButton::OnValueChanged
//
//---------------------------------------------------------------
void XRadioButton::OnValueChanged(int32 value)
{	
	if (value == 1)
		ZRadioButtonMgr::Instance()->UpdateButtons(this);		
}


}	// namespace Whisper
