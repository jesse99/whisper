/*
 *  File:       XMouseExtensible.cpp
 *  Summary:   	An interface used to manage IMenuExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMouseExtensible.cpp,v $
 *		Revision 1.6  2001/04/27 04:34:42  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/04/17 01:44:30  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/07 04:58:45  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:36:37  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:43:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMouseExtensible.h>

#include <IMouseExtension.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XMouseExtensible
//!		An interface used to manage IMenuExtension objects.
// ===================================================================================
class XMouseExtensible : public IMouseExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMouseExtensible();
	
						XMouseExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);

	virtual IMouseExtensionPtr RemoveExtension(const std::wstring& name);

	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;

	virtual IMouseExtensionPtr GetExtension(const std::wstring& name) const;

	virtual bool		HandleMouseDown(XMouseDownEvent& event);
	
	virtual bool		HandleMouseMove(XMouseMoveEvent& event);

	virtual bool		HandleMouseUp(XMouseUpEvent& event);
};

DEFINE_INTERFACE_FACTORY(XMouseExtensible)

//---------------------------------------------------------------
//
// XMouseExtensible::~XMouseExtensible
//
//---------------------------------------------------------------
XMouseExtensible::~XMouseExtensible()
{
}


//---------------------------------------------------------------
//
// XMouseExtensible::XMouseExtensible
//
//---------------------------------------------------------------
XMouseExtensible::XMouseExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XMouseExtensible::AddExtension
//
//---------------------------------------------------------------
void XMouseExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	CHECK_INVARIANT;
	
	const char* encodedName = typeid(IMouseExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromAsciiStr(encodedName), new XImplementation(factory, true, implementationName));
	
	IMouseExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// XMouseExtensible::RemoveExtension
//
//---------------------------------------------------------------
IMouseExtensionPtr XMouseExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	CHECK_INVARIANT;
	
	IMouseExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMouseExtensionPtr candidate(boss, iter);
		
		if (candidate && candidate->GetName() == name)
			extension = candidate;
		else
			++iter;
	}
	
	PRECONDITION(iter != boss->end());
	boss->RemoveInterface(iter);
		
	POSTCONDITION(!this->HasExtension(name));
	
	return extension;
}


//---------------------------------------------------------------
//
// XMouseExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void XMouseExtensible::RemoveExtensions()
{
	CHECK_INVARIANT;
		
	XBoss* boss = this->GetBoss();
	
	int32 count = boss->end() - boss->begin();
	for (int32 index = count - 1; index >= 0; --index) {	// simpler and more efficient to go backwards
		XBoss::iterator iter = boss->begin() + index;

		XUnknown* ptr = nil;
		{
		IMouseExtensionPtr extension(boss, iter);
			ptr = extension.Get();
		}
		
		if (ptr != nil)		// we want to remove all interfaces that are aggregated into a menu extension
			boss->RemoveInterface(iter);
	}
}


//---------------------------------------------------------------
//
// XMouseExtensible::HasExtension
//
//---------------------------------------------------------------
bool XMouseExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstMouseExtensionPtr extension(boss, iter);
		if (extension && extension->GetName() == name)
			has = true;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// XMouseExtensible::GetExtension
//
//---------------------------------------------------------------
IMouseExtensionPtr XMouseExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IMouseExtensionPtr extension;
	
	XBoss* boss = const_cast<XMouseExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMouseExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// XMouseExtensible::HandleMouseDown
//
//---------------------------------------------------------------
bool XMouseExtensible::HandleMouseDown(XMouseDownEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMouseExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleMouseDown(event);
		
		++iter;
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XMouseExtensible::HandleMouseMove
//
//---------------------------------------------------------------
bool XMouseExtensible::HandleMouseMove(XMouseMoveEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMouseExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleMouseMove(event);
		
		++iter;
	}
				
	return handled;
}


//---------------------------------------------------------------
//
// XMouseExtensible::HandleMouseUp
//
//---------------------------------------------------------------
bool XMouseExtensible::HandleMouseUp(XMouseUpEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMouseExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleMouseUp(event);
		
		++iter;
	}
				
	return handled;
}


}	// namespace Whisper

