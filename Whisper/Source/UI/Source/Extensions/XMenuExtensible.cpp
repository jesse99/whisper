/*
 *  File:       XMenuExtensible.cpp
 *  Summary:   	An interface used to manage IMenuExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XMenuExtensible.cpp,v $
 *		Revision 1.7  2001/04/27 04:34:25  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.6  2001/02/07 04:58:33  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.5  2001/01/30 07:37:35  jesjones
 *		Re-added my changes that were lost when I reverted the delete.
 */

#include <XWhisperHeader.h>
#include <IMenuExtensible.h>

#include <IMenuExtension.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XMenuExtensible
//!		An interface used to manage IMenuExtension objects.
// ===================================================================================
class XMenuExtensible : public IMenuExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XMenuExtensible();
	
						XMenuExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);
	virtual IMenuExtensionPtr RemoveExtension(const std::wstring& name);
	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;
	virtual IMenuExtensionPtr GetExtension(const std::wstring& name) const;

	virtual bool		HandleCommand(const XMenuCommand& command, const XEvent& event);
	virtual bool		HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event);
};

DEFINE_INTERFACE_FACTORY(XMenuExtensible)

//---------------------------------------------------------------
//
// XMenuExtensible::~XMenuExtensible
//
//---------------------------------------------------------------
XMenuExtensible::~XMenuExtensible()
{
}


//---------------------------------------------------------------
//
// XMenuExtensible::XMenuExtensible
//
//---------------------------------------------------------------
XMenuExtensible::XMenuExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XMenuExtensible::AddExtension
//
//---------------------------------------------------------------
void XMenuExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	
	const char* encodedName = typeid(IMenuExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromAsciiStr(encodedName), new XImplementation(factory, true, implementationName));
	
	IMenuExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// XMenuExtensible::RemoveExtension
//
//---------------------------------------------------------------
IMenuExtensionPtr XMenuExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	
	IMenuExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMenuExtensionPtr candidate(boss, iter);
		
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
// XMenuExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void XMenuExtensible::RemoveExtensions()
{		
	XBoss* boss = this->GetBoss();
	
	int32 count = boss->end() - boss->begin();
	for (int32 index = count - 1; index >= 0; --index) {	// simpler and more efficient to go backwards
		XBoss::iterator iter = boss->begin() + index;

		XUnknown* ptr = nil;
		{
		IMenuExtensionPtr extension(boss, iter);
			ptr = extension.Get();
		}
		
		if (ptr != nil)		// we want to remove all interfaces that are aggregated into a menu extension
			boss->RemoveInterface(iter);
	}
}


//---------------------------------------------------------------
//
// XMenuExtensible::HasExtension
//
//---------------------------------------------------------------
bool XMenuExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstMenuExtensionPtr extension(boss, iter);
		if (extension && extension->GetName() == name)
			has = true;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// XMenuExtensible::GetExtension
//
//---------------------------------------------------------------
IMenuExtensionPtr XMenuExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IMenuExtensionPtr extension;
	
	XBoss* boss = const_cast<XMenuExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMenuExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// XMenuExtensible::HandleCommand
//
//---------------------------------------------------------------
bool XMenuExtensible::HandleCommand(const XMenuCommand& command, const XEvent& event)
{
	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMenuExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleCommand(command, event);
		
		++iter;
	}
				
	return handled;
}


//---------------------------------------------------------------
//
// XMenuExtensible::HandleEnableCommand
//
//---------------------------------------------------------------
bool XMenuExtensible::HandleEnableCommand(const XMenuCommand& command, SCommandStatus& status, const XEvent& event)
{
	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMenuExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleEnableCommand(command, status, event);
		
		++iter;
	}
				
	return handled;
}


}	// namespace Whisper

