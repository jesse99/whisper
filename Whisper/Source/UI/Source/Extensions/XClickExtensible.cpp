/*
 *  File:       XClickExtensible.cpp
 *  Summary:   	An interface used to manage IClickExtension objects.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1998-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XClickExtensible.cpp,v $
 *		Revision 1.6  2001/04/27 04:30:56  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.5  2001/04/17 01:43:20  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.4  2001/02/07 04:57:47  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.3  2000/12/14 08:33:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:27:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IClickExtensible.h>

#include <IClickExtension.h>
#include <XInterfaceMacros.h>
#include <XMouseEvents.h>
#include <XObjectModel.h>
#include <XTranscode.h>

namespace Whisper {


// ===================================================================================
//	class XClickExtensible
//!		An interface used to manage IClickExtension objects.
// ===================================================================================
class XClickExtensible : public IClickExtensible {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XClickExtensible();
	
						XClickExtensible(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		AddExtension(const std::wstring& name, const std::wstring& implementationName);

	virtual IClickExtensionPtr RemoveExtension(const std::wstring& name);

	virtual void		RemoveExtensions();

	virtual bool		HasExtension(const std::wstring& name) const;

	virtual IClickExtensionPtr GetExtension(const std::wstring& name) const;

	virtual void		HandleClick(const XMouseDownEvent& event);
};

DEFINE_INTERFACE_FACTORY(XClickExtensible)

//---------------------------------------------------------------
//
// XClickExtensible::~XClickExtensible
//
//---------------------------------------------------------------
XClickExtensible::~XClickExtensible()
{
}


//---------------------------------------------------------------
//
// XClickExtensible::XClickExtensible
//
//---------------------------------------------------------------
XClickExtensible::XClickExtensible(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XClickExtensible::AddExtension
//
//---------------------------------------------------------------
void XClickExtensible::AddExtension(const std::wstring& name, const std::wstring& implementationName)
{
	PRECONDITION(!this->HasExtension(name));
	PRECONDITION(name.length() > 0);
	CHECK_INVARIANT;
	
	const char* encodedName = typeid(IClickExtension).name();
	InterfaceFactory factory = XObjectModel::Instance()->GetInterfaceFactory(implementationName);
	
	XBoss* boss = this->GetBoss();
	XBoss::iterator iter = boss->AddInterface(FromAsciiStr(encodedName), new XImplementation(factory, true, implementationName));

	IClickExtensionPtr extension(boss, iter);
	extension->SetName(name);
	
	POSTCONDITION(this->HasExtension(name));
}


//---------------------------------------------------------------
//
// XClickExtensible::RemoveExtension
//
//---------------------------------------------------------------
IClickExtensionPtr XClickExtensible::RemoveExtension(const std::wstring& name)
{
	PRECONDITION(this->HasExtension(name));
	CHECK_INVARIANT;
	
	IClickExtensionPtr extension;
	
	XBoss* boss = this->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IClickExtensionPtr candidate(boss, iter);
		
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
// XClickExtensible::RemoveExtensions
//
//---------------------------------------------------------------
void XClickExtensible::RemoveExtensions()
{
	CHECK_INVARIANT;
		
	XBoss* boss = this->GetBoss();
	
	int32 count = boss->end() - boss->begin();
	for (int32 index = count - 1; index >= 0; --index) {	// simpler and more efficient to go backwards
		XBoss::iterator iter = boss->begin() + index;

		XUnknown* ptr = nil;
		{
		IClickExtensionPtr extension(boss, iter);
			ptr = extension.Get();
		}
		
		if (ptr != nil)		// we want to remove all interfaces that are aggregated into a menu extension
			boss->RemoveInterface(iter);
	}
}


//---------------------------------------------------------------
//
// XClickExtensible::HasExtension
//
//---------------------------------------------------------------
bool XClickExtensible::HasExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	bool has = false;
	
	const XBoss* boss = this->GetBoss();
	
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end() && !has) {
		IConstClickExtensionPtr extension(boss, iter);
		if (extension && extension->GetName() == name)
			has = true;

		++iter;
	}
	
	return has;
}


//---------------------------------------------------------------
//
// XClickExtensible::GetExtension
//
//---------------------------------------------------------------
IClickExtensionPtr XClickExtensible::GetExtension(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	IClickExtensionPtr extension;
	
	XBoss* boss = const_cast<XClickExtensible*>(this)->GetBoss();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end() && !extension) {
		IClickExtensionPtr candidate(boss, iter);
		if (candidate && candidate->GetName() == name)
			extension = candidate;

		++iter;
	}
	
	return extension;
}


//---------------------------------------------------------------
//
// XClickExtensible::HandleClick
//
//---------------------------------------------------------------
void XClickExtensible::HandleClick(const XMouseDownEvent& event)
{
	CHECK_INVARIANT;

	XBoss* boss = this->GetBoss();
	uint32 count = event.GetClickCount();
	
	XBoss::iterator iter = boss->begin();
	while (iter != boss->end()) {
		IClickExtensionPtr extension(boss, iter);
		if (extension) {
			uint32 n = extension->GetClickCount();
			if (n == 0 || n == count)
				extension->HandleClick(event);
		}
		
		++iter;
	}
}


}	// namespace Whisper

