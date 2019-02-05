/*
 *  File:       XDialogBox.cpp
 *  Summary:   	Saves and restores window positions using a pref.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDialogBox.cpp,v $
 *		Revision 1.4  2001/02/07 02:24:41  jesjones
 *		War on implementation inheritance.
 *		
 *		Revision 1.3  2001/01/30 07:09:34  jesjones
 *		Key events are now handled via callbacks.
 *		
 *		Revision 1.2  2001/01/21 00:44:43  jesjones
 *		No longer disables menu commands (commands on the app have to to this themselves).
 *		
 *		Revision 1.1  2001/01/01 00:29:08  jesjones
 *		Moved from the Windows to the Dialog folder.
 *		
 *		Revision 1.4  2000/12/27 03:59:16  jesjones
 *		XWindowKeyHandler takes care of tabbing.
 *		
 *		Revision 1.3  2000/12/27 01:05:47  jesjones
 *		Added support for tabbing via an ITabber interface.
 *		
 *		Revision 1.2  2000/12/19 07:56:22  jesjones
 *		PoseDialog now takes care of menu enabling. Fixed a silly bug in DoValidateNode.
 *		
 *		Revision 1.1  2000/12/05 09:02:45  jesjones
 *		Checked in
 */

#include <XWhisperHeader.h>
#include <IDialogBox.h>

#include <IActivated.h>
#include <IControl.h>
#include <IHierarchy.h>
#include <IKeyHandler.h>
#include <IOpened.h>
#include <IPushButton.h>
#include <IRootControl.h>
#include <IValidate.h>
#include <IWindow.h>
#include <XKeyEvents.h>

namespace Whisper {


// ===================================================================================
//	class XDialogBox
//!		Interface used with modal dialogs.
// ===================================================================================
class XDialogBox : public IDialogBox, public IActivated, public IOpened {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDialogBox();
	
						XDialogBox(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		ValidateData() const;
						
	virtual IControlPtr GetDefaultButton() const					{return mDefaultButton;}
	virtual IControlPtr GetCancelButton() const						{return mCancelButton;}

	virtual void 		SetDefaultButton(const std::wstring& name);
	virtual void 		SetCancelButton(const std::wstring& name);
						
protected:
	virtual void 		Invariant() const;

	virtual void 		OnOpened();
	virtual void 		OnClosing();

	virtual void 		OnActivated();
	virtual void 		OnDeactivated();
				
//-----------------------------------
//	Internal API
//
private:
			bool 		DoValidateNode(const IConstHierarchyPtr& parent) const;
			
			void 		DoReturnKey();
			void 		DoEscapeKey();

//-----------------------------------
//	Member Data
//
private:
	IControlPtr		mDefaultButton;
	IControlPtr		mCancelButton;

	std::wstring	mDefaultButtonName;
	std::wstring	mCancelButtonName;
};

DEFINE_INTERFACE_FACTORY(XDialogBox)

//---------------------------------------------------------------
//
// XDialogBox::~XDialogBox
//
//---------------------------------------------------------------
XDialogBox::~XDialogBox()
{
}


//---------------------------------------------------------------
//
// XDialogBox::XDialogBox
//
//---------------------------------------------------------------
XDialogBox::XDialogBox(XBoss* boss)
{	
	IDialogBox::DoSetBoss(boss);
	IActivated::DoSetBoss(boss);
	IOpened::DoSetBoss(boss);

	mDefaultButtonName = L"ok";
	mCancelButtonName  = L"cancel";	
}


//---------------------------------------------------------------
//
// XDialogBox::SetDefaultButton
//
//---------------------------------------------------------------
void XDialogBox::SetDefaultButton(const std::wstring& name)
{
	if (name != mDefaultButtonName || !mDefaultButton) {
		mDefaultButtonName = name;
	
		IDialogBox* thisPtr = this;
		IWindowPtr window(thisPtr);
		IRootControlPtr root = window->GetRootControl();
		if (root) {											// may be called before the dialog is populated
			mDefaultButton = root->FindChild(mDefaultButtonName);
			
			IPushButtonPtr defaultBtn = mDefaultButton;
			if (defaultBtn)
				defaultBtn->MakeDefaultButton();
		}
	}
}


//---------------------------------------------------------------
//
// XDialogBox::SetCancelButton
//
//---------------------------------------------------------------
void XDialogBox::SetCancelButton(const std::wstring& name)
{
	if (name != mCancelButtonName || !mCancelButton) {
		mCancelButtonName = name;
	
		IDialogBox* thisPtr = this;
		IWindowPtr window(thisPtr);
		IRootControlPtr root = window->GetRootControl();
		if (root) {											// may be called before the dialog is populated
			mCancelButton = root->FindChild(mCancelButtonName);
			
			IPushButtonPtr cancelBtn = mCancelButton;
			if (cancelBtn)
				cancelBtn->MakeCancelButton();
		}
	}
}

						
//---------------------------------------------------------------
//
// XDialogBox::ValidateData
//
//---------------------------------------------------------------
bool XDialogBox::ValidateData() const
{	
	// Give each control a chance to object,
	const IDialogBox* thisPtr = this;
	IConstHierarchyPtr parent(thisPtr);
	bool valid = this->DoValidateNode(parent);
			
	// and then the dialog a chance to object.
	if (valid) {
		IConstValidatePtr validator(thisPtr);
		if (validator)
			valid = validator->Validate();
	}
		
	return valid;
}

#if __MWERKS__
#pragma mark ~
#endif
				
//---------------------------------------------------------------
//
// XDialogBox::Invariant
//
//---------------------------------------------------------------
void XDialogBox::Invariant() const
{
#if DEBUG
	IDialogBox::Invariant();
	
	if (mDefaultButtonName.length() == 0 || mDefaultButtonName == L"????")
		ASSERT(!mDefaultButton);
	else
		ASSERT(mDefaultButton && mDefaultButton->GetName() == mDefaultButtonName);

	if (mCancelButtonName.length() == 0 || mCancelButtonName == L"????")
		ASSERT(!mCancelButton);
	else
		ASSERT(mCancelButton && mCancelButton->GetName() == mCancelButtonName);
		
	IPushButtonPtr defaultBtn = mDefaultButton;
	if (defaultBtn) {
		ASSERT(defaultBtn->IsDefaultButton());
		ASSERT(!defaultBtn->IsCancelButton());
	}
	
	IPushButtonPtr cancelBtn = mCancelButton;
	if (cancelBtn) {
		ASSERT(!cancelBtn->IsDefaultButton());		
		ASSERT(cancelBtn->IsCancelButton());
		
		ASSERT(defaultBtn != cancelBtn);		// but both may be nil...
	}
#endif
}


//---------------------------------------------------------------
//
// XDialogBox::OnOpened
//
//---------------------------------------------------------------
void XDialogBox::OnOpened()
{
	this->SetDefaultButton(mDefaultButtonName);
	this->SetCancelButton(mCancelButtonName);
}


//---------------------------------------------------------------
//
// XDialogBox::OnClosing
//
//---------------------------------------------------------------
void XDialogBox::OnClosing()
{
	mDefaultButton = IControlPtr();
	mCancelButton = IControlPtr();
}


//---------------------------------------------------------------
//
// XDialogBox::OnActivated
//
//---------------------------------------------------------------
void XDialogBox::OnActivated()
{
	IKeyHandlerPtr handler(L"Application");
	IDialogBox* thisPtr = this;
		
	XCallback0<void> handle(this, &XDialogBox::DoReturnKey);
	handler->RegisterHandler(kReturnKey, handle, thisPtr);
	
#if MAC
	handle = XCallback0<void>(this, &XDialogBox::DoReturnKey);
	handler->RegisterHandler(kEnterKey, handle, thisPtr);
#endif
	
	handle = XCallback0<void>(this, &XDialogBox::DoEscapeKey);
	handler->RegisterHandler(kEscapeKey, handle, thisPtr);
}


//---------------------------------------------------------------
//
// XDialogBox::OnDeactivated
//
//---------------------------------------------------------------
void XDialogBox::OnDeactivated()
{
	IKeyHandlerPtr handler(L"Application");
		
	XCallback0<void> handle(this, &XDialogBox::DoReturnKey);
	handler->UnRegisterHandler(handle);
	
#if MAC
	handle = XCallback0<void>(this, &XDialogBox::DoReturnKey);
	handler->UnRegisterHandler(handle);
#endif
	
	handle = XCallback0<void>(this, &XDialogBox::DoEscapeKey);
	handler->UnRegisterHandler(handle);
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XDialogBox::DoReturnKey
//
//---------------------------------------------------------------
void XDialogBox::DoReturnKey()
{
	IPushButtonPtr button(mDefaultButton);
	if (button)
		button->SimulateClick();
}


//---------------------------------------------------------------
//
// XDialogBox::DoEscapeKey
//
//---------------------------------------------------------------
void XDialogBox::DoEscapeKey()
{
	IPushButtonPtr button(mCancelButton);
	if (button)
		button->SimulateClick();
}


//---------------------------------------------------------------
//
// XDialogBox::DoValidateNode
//
//---------------------------------------------------------------
bool XDialogBox::DoValidateNode(const IConstHierarchyPtr& parent) const
{
	bool valid = true;

	IHierarchy::iterator iter = parent->begin();
	while (iter != parent->end() && valid) {
		IConstHierarchyPtr child = *iter;
		++iter;
		
		IConstValidatePtr validator = child;
		if (validator)
			valid = validator->Validate();
			
		if (valid)
			valid = this->DoValidateNode(child);
	}
			
	return valid;
}


}	// namespace Whisper
