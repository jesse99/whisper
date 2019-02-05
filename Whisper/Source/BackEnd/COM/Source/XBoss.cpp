/*
 *  File:       XBoss.cpp
 *  Summary:    A class that contains a list of interface objects (clients shouldn't
 *				normally have to deal with these).
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XBoss.cpp,v $
 *		Revision 1.7  2001/04/21 03:39:42  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.6  2001/04/17 01:43:08  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.5  2001/03/06 07:32:03  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.4  2000/12/10 04:09:19  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.3  2000/11/10 06:48:23  jesjones
 *		Includes the encoded interface name when asserting that an interface is abstract.
 *		
 *		Revision 1.2  2000/11/09 12:26:40  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <11>	 9/23/00	JDJ		Rewrote DoAddParent so that XImplementation objects aren't
 *									copied more than once.
 *		 <10>	 9/23/00	JDJ		Replaced the Group element with plus signs before interface
 *									names.
 *		 <9>	 8/12/00	JDJ		DoAddParent appends interfaces that were declared inside
 *									a Group element.
 *		 <8>	 8/11/00	JDJ		DoIsValidTarget correctly handles carbon and classic_mac
 *									targets.
 *		 <8>	 8/11/00	JDJ		Tweaked the code to allow for missing implementation
 *									factories in abstract bosses.
 *		 <7>	 8/04/00	JDJ		Copy ctor sets mUnsorted to true.
 *		 <6>	 7/07/00	JDJ		Added support for abstract bosses, groups, and
 *									parent bosses.
 *		 <5>	 5/20/00	JDJ		Moved XInterface into its own file.
 *		 <4>	 5/12/00	JDJ		Added mDeleting checks.
 *		 <3>	 4/23/00	JDJ		Tweaked AddInterface and GetInterface to support
 *									multiple instances of an interface within a boss.
 *		 <2>	 4/23/00	JDJ		Added XBoss::RemoveInterface.
 *		 <1>	11/27/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XBoss.h>

#include <XInvariant.h>
#include <XNumbers.h>
#include <XObjectModel.h>
#include <XUnknown.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	struct SParentImplementations
// ===================================================================================
struct SInterfaces {
	const XImplementation*		oldImp;
	XImplementation*			implementation;
	std::vector<std::wstring>	interfaces;
	bool						grouped;
	
	typedef std::vector<std::wstring>::const_iterator iterator;

	bool 		operator==(const XImplementation* rhs) const	{return oldImp == rhs;}
				
	iterator 	begin() const		{return interfaces.begin();}
	iterator 	end() const			{return interfaces.end();}
};

struct SParentImplementations {	
	std::vector<SInterfaces> implementations;
	
	typedef std::vector<SInterfaces>::const_iterator iterator;
	
				SParentImplementations(const XBoss* boss);
				
	iterator 	begin() const		{return implementations.begin();}
	iterator 	end() const			{return implementations.end();}
};

//---------------------------------------------------------------
//
// SParentImplementations::SParentImplementations
//
// Creates a copy of the interfaces in the specified boss. This
// is a bit tricky because interfaces can share implementations
// and we don't want to clone an implementation more than once.
//
//---------------------------------------------------------------
SParentImplementations::SParentImplementations(const XBoss* boss)
{
	XBoss::const_iterator iter = boss->begin();
	while (iter != boss->end()) {
		const XImplementation* implementation = iter->GetImplementation();
		std::vector<SInterfaces>::iterator iter2 = std::find(implementations.begin(), implementations.end(), implementation);
		
		if (iter2 == implementations.end()) {
			SInterfaces imp;
			imp.oldImp = implementation;
			imp.implementation = implementation->Clone();
			imp.interfaces.push_back(iter->GetEncodedName());
			imp.grouped = iter->IsGrouped();
		
			implementations.push_back(imp);	

		} else
			iter2->interfaces.push_back(iter->GetEncodedName());
		
		++iter;
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XBoss
// ===================================================================================

//---------------------------------------------------------------
//
// XBoss::~XBoss
//
//---------------------------------------------------------------
XBoss::~XBoss()		
{
	CALL_INVARIANT;
	
#if !GARBAGE_COLLECT_COM
	PRECONDITION(mDeleting); 
	PRECONDITION(this->GetRefCount() == 0);
#endif
}


//---------------------------------------------------------------
//
// XBoss::XBoss (XPlugin*, const XXMLElement*)
//
//---------------------------------------------------------------
XBoss::XBoss(XPlugin* plugin, const XXMLElement* elem) 
#if !GARBAGE_COLLECT_COM
	: XReferenceCountedMixin(0)
#endif
{
	PRECONDITION(elem != nil);
	PRECONDITION(elem->GetName() == L"AbstractBoss" || elem->GetName() == L"AddInBoss" || elem->GetName() == L"Boss");
		
	mPlugin = plugin;
	mUnsorted = false;
	mDeleting = false;
	mAbstract = elem->GetName() == L"AbstractBoss";
	mPrototype = true;
	
	mCachedName = nil;
	mCachedInterface = nil;

	const XXMLAttribute& attr = elem->FindAttribute(L"name");
	mName = attr.GetValue();
	ASSERT(mName.length() > 0);
	
	const XXMLAttribute& singleton = elem->FindAttribute(L"singleton");
	mSingleton = singleton.Get<bool>();

	this->DoAddInterfaces(elem);
	
#if DEBUG
	if (!mAbstract) {						// abstract bosses can have abstract interfaces and missing implementation factories
		Interfaces::const_iterator iter = mInterfaces.begin();
		while (iter != mInterfaces.end()) {
			const XImplementation* implementation = iter->GetImplementation();
			++iter;

			const std::wstring& name = implementation->GetName();
			if (name == L"none")
				DEBUGSTR("Boss '", mName, "' has an abstract interface (", iter->GetEncodedName(), L")");
			else if (implementation->IsAbstract())
				DEBUGSTR("Couldn't find a factory for '", name, "' (in boss '", mName, "')");
		}
	}
#endif

//	TRACE("Boss: '", mName, "'\n");
//	Interfaces::const_iterator iter = mInterfaces.begin();
//	while (iter != mInterfaces.end()) {
//		TRACE("'", iter->GetEncodedName(), "' -> '", iter->GetImplementation()->GetName(), "'\t", (int32) iter->GetImplementation(), "\n");
//		++iter;
//	}	
//	TRACE("\n");
	
	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XBoss::XBoss (XBoss)
//
// This is called by the object model when it needs to create a
// new boss from the prototype objects it keeps. The only odd bit
// here is that we can't simply copy mInterfaces. If we did we'd
// wind up sharing the prototype's XImplementation objects which
// would cause all sorts of problems.						
//
//---------------------------------------------------------------
XBoss::XBoss(const XBoss& rhs) 
#if !GARBAGE_COLLECT_COM
	: XReferenceCountedMixin(0), mName(rhs.mName)
#else
	: mName(rhs.mName)
#endif
{
	PRECONDITION(!rhs.mSingleton);
	PRECONDITION(!rhs.mDeleting); 
		
	mPlugin = rhs.mPlugin;
	mUnsorted = true;
	mDeleting = rhs.mDeleting;
	mSingleton = rhs.mSingleton;
	mAbstract = rhs.mAbstract;
	mPrototype = false;
	
	mCachedName = nil;
	mCachedInterface = nil;
	
	// For every interface,
	std::list<XInterface> interfaces(rhs.begin(), rhs.end());
	while (!interfaces.empty()) {
	
		// clone the implementation,
		const XInterface& interface = interfaces.front();
		const XImplementation* oldImp = interface.GetImplementation();
		XImplementation* newImp = oldImp->Clone();

		// and for the interface and the remaining interfaces,
		std::list<XInterface>::iterator iter = interfaces.begin();
		while (iter != interfaces.end()) {
			std::list<XInterface>::iterator temp = iter;
			++iter;
			
			// if the interface is reusing the implementation then,
			const XInterface& interface2 = *temp;
			if (interface2.GetImplementation() == oldImp) {
			
				// create a new interface with the cloned implementation.
				const std::wstring& name = interface2.GetEncodedName();
				bool grouped = interface2.IsGrouped();
				
				mInterfaces.push_back(XInterface(name, newImp, grouped));
				
				interfaces.erase(temp);
			}
		}
	}
	
	POSTCONDITION(mInterfaces.size() == rhs.mInterfaces.size());

	CALL_INVARIANT;
}


//---------------------------------------------------------------
//
// XBoss::Clone
//
//---------------------------------------------------------------
XBoss* XBoss::Clone() const
{
	XEnterCriticalSection enter(mMutex);
		
	XBoss* boss = new XBoss(*this);
	
	return boss;
}


//---------------------------------------------------------------
//
// XBoss::GetImplementation (const char*)
//
//---------------------------------------------------------------
XImplementation* XBoss::GetImplementation(const char* encodedName)
{
	PRECONDITION(encodedName != nil);
	PRECONDITION(*encodedName != '\0');
	CHECK_INVARIANT;
	
	XImplementation* implementation = nil;
	
	if (!mDeleting) {
		XEnterCriticalSection enter(mMutex);
		
		if (mUnsorted) {
			std::stable_sort(mInterfaces.begin(), mInterfaces.end());	// stable_sort so debug extension order is preserved
			mUnsorted = false;
		}
		
		if (encodedName == mCachedName) {
			implementation = mCachedInterface;
		
		} else {
			Interfaces::iterator iter = std::lower_bound(mInterfaces.begin(), mInterfaces.end(), encodedName);
			if (iter != mInterfaces.end() && *iter == encodedName) {
				implementation = iter->GetImplementation();
				
				mCachedName = encodedName;
				mCachedInterface = implementation;

#if DEBUG
				// We allow multiple instances of an interface in a 
				// boss, but you can only access them via iteration.
				++iter;
				ASSERT(iter == mInterfaces.end() || *iter != encodedName);
#endif
			}
		}
	}
		
	return implementation;
}

			
//---------------------------------------------------------------
//
// XBoss::GetImplementation ()
//
//---------------------------------------------------------------
XImplementation* XBoss::GetImplementation()
{
	PRECONDITION(!mDeleting);
	CHECK_INVARIANT;
	
	XImplementation* implementation = nil;
	
	{
	XEnterCriticalSection enter(mMutex);
		if (mUnsorted) {
			std::stable_sort(mInterfaces.begin(), mInterfaces.end());
			mUnsorted = false;
		}
	
		Interfaces::iterator iter = mInterfaces.begin();
		if (iter != mInterfaces.end())
			implementation = iter->GetImplementation();
	}
	
	POSTCONDITION(true);
	
	return implementation;
}

			
//---------------------------------------------------------------
//
// XBoss::AddInterface
//
//---------------------------------------------------------------
XBoss::iterator XBoss::AddInterface(const std::wstring& encodedName, XImplementation* implementation)
{
	PRECONDITION(encodedName.length() > 0);
	PRECONDITION(implementation != nil);
	PRECONDITION(!mDeleting); 
	CHECK_INVARIANT;
	
	iterator iter;
	
	{
	XEnterCriticalSection enter(mMutex);
		XInterface theInterface(encodedName, implementation, true);
		
		mInterfaces.push_back(theInterface);
		mUnsorted = true;
		
		iter = mInterfaces.end() - 1;
	}
	
	POSTCONDITION(true);
	
	return iter;
}


//---------------------------------------------------------------
//
// XBoss::ReplaceInterface
//
//---------------------------------------------------------------
void XBoss::ReplaceInterface(const XInterface& interface)
{
	PRECONDITION(!mDeleting);
	CHECK_INVARIANT;
		
	{
	XEnterCriticalSection enter(mMutex);		
		iterator iter = std::find(mInterfaces.begin(), mInterfaces.end(), interface);
		
		if (iter != mInterfaces.end())
			*iter = interface;			
		else 
			mInterfaces.push_back(interface);

		mUnsorted = true;
	}
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XBoss::RemoveInterface
//
//---------------------------------------------------------------
void XBoss::RemoveInterface(iterator iter)
{
	PRECONDITION(!mDeleting);
	CHECK_INVARIANT;
	
	{
	XEnterCriticalSection enter(mMutex);
		PRECONDITION(iter != mInterfaces.end());		// do this here so we're inside the mutex

		const XImplementation* implementation = iter->GetImplementation();
		const XUnknown* theInterface = implementation->FindInterface();
#if !GARBAGE_COLLECT_COM
		PRECONDITION(theInterface == nil || theInterface->GetRefCount() == 0);
#endif

		if (implementation == mCachedInterface) {
			mCachedName = nil;
			mCachedInterface = nil;
		}
			
		mInterfaces.erase(iter);
	}
	
	POSTCONDITION(true);
}


//---------------------------------------------------------------
//
// XBoss::begin
//
//---------------------------------------------------------------
XBoss::iterator XBoss::begin()									
{
	PRECONDITION(!mDeleting); 

	{
	XEnterCriticalSection enter(mMutex);
		if (mUnsorted) {
			std::stable_sort(mInterfaces.begin(), mInterfaces.end());
			mUnsorted = false;
		}
	}
	
	return mInterfaces.begin();
}


//---------------------------------------------------------------
//
// XBoss::end
//
//---------------------------------------------------------------
XBoss::iterator XBoss::end()							
{
	PRECONDITION(!mDeleting); 

	{
	XEnterCriticalSection enter(mMutex);
		if (mUnsorted) {
			std::stable_sort(mInterfaces.begin(), mInterfaces.end());
			mUnsorted = false;
		}
	}
	
	return mInterfaces.end();
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XBoss::Invariant
//
//---------------------------------------------------------------
void XBoss::Invariant() const
{	
	ASSERT(mName.length() > 0);
	
	if (mCachedName != nil)
		ASSERT(mCachedInterface != nil);
	
#if DEBUG && 0 && !GARBAGE_COLLECT_COM
	if (gIntenseDebugging) {
		uint32 count = 0;
		
		XEnterCriticalSection enter(mMutex);
		
		bool hasAggregate = false;
		
		Interfaces::const_iterator iter = mInterfaces.begin();
		while (iter != mInterfaces.end()) {
			const XImplementation* implementation = iter->GetImplementation();
			const XUnknown* theInterface = implementation->FindInterface();
			++iter;
			
			if (implementation->IsAggregated())
				hasAggregate = true;
			
			if (theInterface != nil)
				count += numeric_cast<uint32>(theInterface->GetRefCount());
		}
		
		// $$$ We can't check aggregated interfaces because FindInterface
		// $$$ only returns the XUnknown* pointer for the first interface.
		if (!hasAggregate) {
			if (mSingleton)
				ASSERT(count+1 == this->GetRefCount());		// singletons reuse the boss prototype in XObjectModel
			else
				ASSERT(count == this->GetRefCount());		// our ref count should be the sum of our interface ref counts
		}
	}
#endif
}


//---------------------------------------------------------------
//
// XBoss::OnLastReference
//
//---------------------------------------------------------------
#if !GARBAGE_COLLECT_COM
void XBoss::OnLastReference()
{
	PRECONDITION(!mDeleting); 

	mDeleting = true;
	
#if DEBUG
	XObjectModel::Instance()->DoDeletingBoss(this);
#endif

	delete this;
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XBoss::DoAddInterfaces
//
//---------------------------------------------------------------
void XBoss::DoAddInterfaces(const XXMLElement* parent)
{
	PRECONDITION(parent != nil);

//	TRACE("\n\nAdding interfaces to '", mName, "'\n");

	XXMLElement::const_item_iterator iter2 = parent->item_begin();
	while (iter2 != parent->item_end()) {
		const XXMLItem* item = *iter2;
		++iter2;
		
		const XXMLElement* elem = dynamic_cast<const XXMLElement*>(item);
		ASSERT(elem != nil);
		
		if (elem->GetName() == L"Interface")
			this->DoAddInterface(elem);
		
		else if (elem->GetName() == L"Inherit")
			this->DoAddParent(elem);

		else
			DEBUGSTR("Bogus element in XBoss::DoAddInterfaces");	// should have been caught by the parser
	}
}


//---------------------------------------------------------------
//
// XBoss::DoAddParent
//
// Generally all interfaces in the parent replace existing interfaces.
// However interfaces that start with a plus sign are appended onto 
// the interface list.
//
//---------------------------------------------------------------
void XBoss::DoAddParent(const XXMLElement* parent)
{
	PRECONDITION(parent != nil);
	PRECONDITION(parent->GetName() == L"Inherit");

	if (this->DoIsValidTarget(parent)) {		
		const XXMLAttribute& name = parent->FindAttribute(L"name");
		const XBoss* boss = XObjectModel::Instance()->GetBoss(name.GetValue());
		ASSERT(boss != nil);
		
		SParentImplementations implementations(boss);
		SParentImplementations::iterator iter = implementations.begin();
		while (iter != implementations.end()) {
			const SInterfaces& interfaces = *iter;
			++iter;
			
			SInterfaces::iterator iter2 = interfaces.begin();
			while (iter2 != interfaces.end()) {
				if (interfaces.grouped)
					this->AddInterface(*iter2, interfaces.implementation);
				else
					this->ReplaceInterface(XInterface(*iter2, interfaces.implementation, interfaces.grouped));
//				TRACE("   added '", boss->GetName(), "' interfaces\n");			

				++iter2;
			}
		}
	}
}


//---------------------------------------------------------------
//
// XBoss::DoAddInterface
//
// Add or replace an interface.
//
//---------------------------------------------------------------
void XBoss::DoAddInterface(const XXMLElement* elem)
{
	PRECONDITION(elem != nil);
	PRECONDITION(elem->GetName() == L"Interface");
		
	const XXMLAttribute& name = elem->FindAttribute(L"name");
	const XXMLAttribute& impl = elem->FindAttribute(L"impl");
	
	if (this->DoIsValidTarget(elem)) {		
		const std::wstring& impName = impl.GetValue();
		InterfaceFactory factory = nil;
		if (impName != L"none")
			factory = XObjectModel::Instance()->GetInterfaceFactory(impName);
		else
			ASSERT(mAbstract);
	
		const std::wstring& interfaces = name.GetValue();
		
		uint32 len = interfaces.find(L',');
		bool aggregate = len != std::wstring::npos;
		XImplementation* implementation = new XImplementation(factory, aggregate, impName);	// starts off with a ref count of zero so we don't have to call RemoveReference
		
		uint32 index = 0;
		while (index < interfaces.length()) {
			std::wstring intName = interfaces.substr(index, len);

			bool append = intName[0] == '+';
			if (append)
				intName = intName.substr(1);
		
			std::wstring encodedName = XObjectModel::Instance()->GetEncodedName(intName);
			if (append)
				this->AddInterface(encodedName, implementation);
			else
				this->ReplaceInterface(XInterface(encodedName, implementation, false));
				
//			TRACE("   added ", intName, "\n");
			
			index += len;
			
			if (index < std::wstring::npos) {
				ASSERT(interfaces[index] == L',');
				++index;
				
				while (index < interfaces.length() && interfaces[index] == L' ')
					++index;
			}

			if (index < std::wstring::npos)
				len = interfaces.find(L',', index) - index;
		}
	}
}


//---------------------------------------------------------------
//
// XBoss::DoIsValidTarget
//
//---------------------------------------------------------------
bool XBoss::DoIsValidTarget(const XXMLElement* elem)
{
	PRECONDITION(elem != nil);
	
	bool valid = false;
		
	const XXMLAttribute& platform = elem->FindAttribute(L"platform");
	const XXMLAttribute& target   = elem->FindAttribute(L"target");
	
	if (platform.GetValue() == L"all" ||
#if MAC
		platform.GetValue() == L"mac"
	#if TARGET_CARBON
			|| platform.GetValue() == L"carbon"
	#else
			|| platform.GetValue() == L"classic_mac"
	#endif
#elif WIN
		platform.GetValue() == L"win"
#endif
	) 
		if (target.GetValue() == L"all"
#if DEBUG
			|| target.GetValue() == L"debug"
#endif
#if RELEASE
			|| target.GetValue() == L"release"
#endif
#if __profile__
			|| target.GetValue() == L"profile"
#endif
		) 
			valid = true;
	
	return valid;
}


}	// namespace Whisper

