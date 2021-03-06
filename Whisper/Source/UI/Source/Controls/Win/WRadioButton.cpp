/*
 *  File:       WRadioButton.cpp
 *  Summary:   	System radio button control.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WRadioButton.cpp,v $
 *		Revision 1.7  2001/04/21 03:33:22  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/03/16 10:28:47  jesjones
 *		Fixed gExitingNormally for Intel DLL targets.
 *		
 *		Revision 1.5  2001/02/21 08:16:51  jesjones
 *		Got the Windows file compiling after the war on implementation inheritance.
 *		
 *		Revision 1.4  2000/12/31 04:44:36  jesjones
 *		Renamed the class.
 *		
 *		Revision 1.3  2000/12/14 08:31:53  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/12/08 08:11:22  jesjones
 *		Made it work.
 *		
 *		Revision 1.1  2000/12/07 09:49:15  jesjones
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
#include <IWindow.h>
#include <WSystemInfo.h>
#include <XExceptions.h>

namespace Whisper {

#if DEBUG
	extern RUNTIME_EXPORT bool gExitingNormally;
#endif


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
		HWND			wptr;
		
					Button(IRadioButton* r, const std::wstring& f, HWND w) : radio(r), family(f), wptr(w) {}
		
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
		PRECONDITION(mButtons.empty());
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
	IWindowPtr window = GetParent<IWindow>(control);
	HWND wptr = window->GetOSWindow();
	
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
	ASSERT(iter != mButtons.end());
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
	virtual void 		OnValueChanged(int32 newValue);
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
	UNUSED(procID);
	
	IControlPtr super = GetInterface<IControl>(parent);

	// Create the control
	HWND controlH = ::CreateWindowEx(0,						// extended style
									 _TEXT("BUTTON"),		// class name
									 _TEXT(""),				// window name
									 style,					// style
									 frame.left,			// location
									 frame.top,
									 frame.GetWidth(),		// size
									 frame.GetHeight(),
									 super->GetOSControl(),	// parent
									 nil,					// menu
									 WSystemInfo::GetAppInstance(),	// app instance
									 nil);					// user data
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
	
	bool selected = value == BST_CHECKED || value == BST_INDETERMINATE;
	
	return selected;
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
	
	bool indeterminate = value == BST_INDETERMINATE;
	
	return indeterminate;
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
