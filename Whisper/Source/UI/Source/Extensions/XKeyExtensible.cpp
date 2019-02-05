/*
 *  File:       XKeyExtensible.cpp
 *  Summary:   	An interface used to manage IKeyExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XKeyExtensible.cpp,v $
 *		Revision 1.6  2001/04/27 04:33:48  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/04/17 01:44:07  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/07 04:58:29  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:36:17  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:40:52  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IKeyExtensible.h>

#include <IKeyExtension.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XKeyExtensible
//!		An interface used to manage IKeyExtension objects.
// ===================================================================================
class XKeyExtensible : public IKeyExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XKeyExtensible();
	
						XKeyExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);

	virtual IKeyExtensionPtr RemoveExtension(const std::wstring& name);

	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;

	virtual IKeyExtensionPtr GetExtension(const std::wstring& name) const;

	virtual bool		HandleKeyDown(XKeyDownEvent& event);
	
	virtual bool		HandleChar(XCharEvent& event);
	
	virtual bool		HandleKeyUp(XKeyUpEvent& event);
};

DEFINE_INTERFACE_FACTORY(XKeyExtensible)

//---------------------------------------------------------------
//
// XKeyExtensible::~XKeyExtensible
//
//---------------------------------------------------------------
XKeyExtensible::~XKeyExtensible()
{
}


//---------------------------------------------------------------
//
// XKeyExtensible::XKeyExtensible
//
//---------------------------------------------------------------
XKeyExtensible::XKeyExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XKeyExtensible::AddExtension
//
//---------------------------------------------------------------
void XKeyExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	CHECK_INVARIANT;
	
	const char* encodedName = typeid(IKeyExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromAsciiStr(encodedName), new XImplementation(factory, true, implementationName));
	
	IKeyExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// XKeyExtensible::RemoveExtension
//
//---------------------------------------------------------------
IKeyExtensionPtr XKeyExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	CHECK_INVARIANT;
	
	IKeyExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IKeyExtensionPtr candidate(boss, iter);
		
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
// XKeyExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void XKeyExtensible::RemoveExtensions()
{
	CHECK_INVARIANT;
		
	XBoss* boss = this->GetBoss();
	
	int32 count = boss->end() - boss->begin();
	for (int32 index = count - 1; index >= 0; --index) {	// simpler and more efficient to go backwards
		XBoss::iterator iter = boss->begin() + index;

		XUnknown* ptr = nil;
		{
		IKeyExtensionPtr extension(boss, iter);
			ptr = extension.Get();
		}
		
		if (ptr != nil)		// we want to remove all interfaces that are aggregated into a menu extension
			boss->RemoveInterface(iter);
	}
}


//---------------------------------------------------------------
//
// XKeyExtensible::HasExtension
//
//---------------------------------------------------------------
bool XKeyExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstKeyExtensionPtr extension(boss, iter);
		if (extension && extension->GetName() == name)
			has = true;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// XKeyExtensible::GetExtension
//
//---------------------------------------------------------------
IKeyExtensionPtr XKeyExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IKeyExtensionPtr extension;
	
	XBoss* boss = const_cast<XKeyExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IKeyExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// XKeyExtensible::HandleKeyDown
//
//---------------------------------------------------------------
bool XKeyExtensible::HandleKeyDown(XKeyDownEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IKeyExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleKeyDown(event);
		
		++iter;
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XKeyExtensible::HandleChar
//
//---------------------------------------------------------------
bool XKeyExtensible::HandleChar(XCharEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IKeyExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleChar(event);
		
		++iter;
	}
	
	return handled;
}


//---------------------------------------------------------------
//
// XKeyExtensible::HandleKeyUp
//
//---------------------------------------------------------------
bool XKeyExtensible::HandleKeyUp(XKeyUpEvent& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IKeyExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleKeyUp(event);
		
		++iter;
	}
	
	return handled;
}


}	// namespace Whisper

