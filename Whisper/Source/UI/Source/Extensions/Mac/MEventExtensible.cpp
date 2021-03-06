/*
 *  File:       MEventExtensible.cpp
 *  Summary:   	An interface used to manage IEventExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MEventExtensible.cpp,v $
 *		Revision 1.6  2001/04/21 03:25:44  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/17 01:40:54  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/07 04:56:09  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:27:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:13:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IEventExtensible.h>

#include <IEventExtension.h>
#include <XInterfaceMacros.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class MEventExtensible
//!		An interface used to manage IEventExtension objects.
// ===================================================================================
class MEventExtensible : public IEventExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~MEventExtensible();
	
						MEventExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);

	virtual IEventExtensionPtr RemoveExtension(const std::wstring& name);

	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;

	virtual IEventExtensionPtr GetExtension(const std::wstring& name) const;

	virtual bool		HandleEvent(EventRecord& event);
};

DEFINE_INTERFACE_FACTORY(MEventExtensible)

//---------------------------------------------------------------
//
// MEventExtensible::~MEventExtensible
//
//---------------------------------------------------------------
MEventExtensible::~MEventExtensible()
{
}


//---------------------------------------------------------------
//
// MEventExtensible::MEventExtensible
//
//---------------------------------------------------------------
MEventExtensible::MEventExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// MEventExtensible::AddExtension
//
//---------------------------------------------------------------
void MEventExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	CHECK_INVARIANT;
	
	const char* encodedName = typeid(IEventExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromAsciiStr(encodedName), new XImplementation(factory, true, implementationName));
	
	IEventExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// MEventExtensible::RemoveExtension
//
//---------------------------------------------------------------
IEventExtensionPtr MEventExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	CHECK_INVARIANT;
	
	IEventExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IEventExtensionPtr candidate(boss, iter);
		
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
// MEventExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void MEventExtensible::RemoveExtensions()
{
	CHECK_INVARIANT;
		
	XBoss* boss = this->GetBoss();
	
	int32 count = boss->end() - boss->begin();
	for (int32 index = count - 1; index >= 0; --index) {	// simpler and more efficient to go backwards
		XBoss::iterator iter = boss->begin() + index;
		
		XUnknown* ptr = nil;
		{
		IEventExtensionPtr extension(boss, iter);
			ptr = extension.Get();
		}
		
		if (ptr != nil)		// we want to remove all interfaces that are aggregated into a menu extension
			boss->RemoveInterface(iter);
	}
}


//---------------------------------------------------------------
//
// MEventExtensible::HasExtension
//
//---------------------------------------------------------------
bool MEventExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstEventExtensionPtr extension(boss, iter);
		if (extension && extension->GetName() == name)
			has = true;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// MEventExtensible::GetExtension
//
//---------------------------------------------------------------
IEventExtensionPtr MEventExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IEventExtensionPtr extension;
	
	XBoss* boss = const_cast<MEventExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IEventExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// MEventExtensible::HandleEvent
//
//---------------------------------------------------------------
bool MEventExtensible::HandleEvent(EventRecord& event)
{
	CHECK_INVARIANT;

	bool handled = false;
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !handled) {
		IEventExtensionPtr extension(boss, iter);
		if (extension)
			handled = extension->HandleEvent(event);
		
		++iter;
	}
				
	return handled;
}


}	// namespace Whisper

