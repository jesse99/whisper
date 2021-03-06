/*
 *  File:       WMsgExtensible.cpp
 *  Summary:   	An interface used to manage IMsgExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WMsgExtensible.cpp,v $
 *		Revision 1.3  2001/04/21 03:32:55  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.2  2000/11/09 12:10:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IMsgExtensible.h>

#include <IMsgExtension.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class WMsgExtensible
// ===================================================================================
class WMsgExtensible : public IMsgExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~WMsgExtensible();
	
						WMsgExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);

	virtual IMsgExtensionPtr RemoveExtension(const std::wstring& name);

	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;

	virtual IMsgExtensionPtr GetExtension(const std::wstring& name) const;

	virtual bool		HandleMessage(MSG& message);
};

DEFINE_INTERFACE_FACTORY(WMsgExtensible)

//---------------------------------------------------------------
//
// WMsgExtensible::~WMsgExtensible
//
//---------------------------------------------------------------
WMsgExtensible::~WMsgExtensible()
{
}


//---------------------------------------------------------------
//
// WMsgExtensible::WMsgExtensible
//
//---------------------------------------------------------------
WMsgExtensible::WMsgExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// WMsgExtensible::AddExtension
//
//---------------------------------------------------------------
void WMsgExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	CHECK_INVARIANT;
	
	const char* encodedName = typeid(IMsgExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromPlatformStr(encodedName), new XImplementation(factory, true, implementationName));
	
	IMsgExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// WMsgExtensible::RemoveExtension
//
//---------------------------------------------------------------
IMsgExtensionPtr WMsgExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	CHECK_INVARIANT;
	
	IMsgExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMsgExtensionPtr candidate(boss, iter);
		
		if (candidate && candidate->GetName() == name)
			extension = candidate;
		else
			++iter;
	}
	
	ASSERT(iter != boss->end());
	boss->RemoveInterface(iter);
		
	POSTCONDITION(!this->HasExtension(name));
	
	return extension;
}


//---------------------------------------------------------------
//
// WMsgExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void WMsgExtensible::RemoveExtensions()
{
	CHECK_INVARIANT;
		
	XBoss* boss = this->GetBoss();
	
	XBoss::reverse_iterator iter = boss->rbegin();		// need to use reverse iterators to ensure our iterator isn't invalidated by the erase
	while (iter != boss->rend()) {
		XBoss::iterator temp = iter.base();
		++iter;

		IMsgExtensionPtr extension(boss, temp);
		if (extension)
			boss->RemoveInterface(temp);
	}
}


//---------------------------------------------------------------
//
// WMsgExtensible::HasExtension
//
//---------------------------------------------------------------
bool WMsgExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstMsgExtensionPtr extension(boss, iter);
		has = extension;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// WMsgExtensible::GetExtension
//
//---------------------------------------------------------------
IMsgExtensionPtr WMsgExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IMsgExtensionPtr extension;
	
	XBoss* boss = const_cast<WMsgExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IMsgExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// WMsgExtensible::HandleMessage
//
//---------------------------------------------------------------
bool WMsgExtensible::HandleMessage(MSG& message)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IMsgExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleMessage(message);
		
		++iter;
	}
	
	return handled;
}


}	// namespace Whisper

