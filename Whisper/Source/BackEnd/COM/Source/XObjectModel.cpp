/*
 *  File:       XObjectModel.h
 *  Summary:    Singleton that manages bosses and interfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XObjectModel.cpp,v $
 *		Revision 1.10  2001/04/27 04:34:58  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.9  2001/03/13 11:09:40  jesjones
 *		Fixed some minor problems with DLL Intel targets.
 *		
 *		Revision 1.8  2001/03/06 10:07:37  jesjones
 *		Added a method to XObjectModel to write the bosses and interfaces to a log file.
 *		
 *		Revision 1.7  2001/03/06 07:33:07  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.6  2001/01/05 06:24:57  jesjones
 *		Added an ASSERT.
 *		
 *		Revision 1.5  2000/11/10 06:52:51  jesjones
 *		Removed the log keyword from the initial checkin comment.
 *		
 *		Revision 1.4  2000/11/10 06:50:55  jesjones
 *		Removed some bogus comment lines.
 *		
 *		Revision 1.3  2000/11/10 06:49:52  jesjones
 *		No longer asserts that Terminate was called if the app was shut down abnormally.
 *		
 *		Revision 1.2  2000/11/09 12:45:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	10/08/00	JDJ		Added an ASSERT to check that Shutdown was called.
 *		 <8>	10/06/00	JDJ		Shutdown releases the bosses (instead of the dtor).
 *		 <7>	 8/11/00	JDJ		GetInterfaceFactory no longer asserts if the factory cannot
 *									be found.
 *		 <6>	 8/05/00	JDJ		Boss leaks log includes the number of leaks.
 *		 <5>	 7/07/00	JDJ		Moved the boss parsing code into XBoss. Added support for
 *									add-in bosses.
 *		 <4>	 5/27/00	JDJ		Added support for target specific interfaces.
 *		 <3>	 5/21/00	JDJ		Changed DoAddInterfaces to support interface aggregation.
 *		 <2>	 5/10/00	JDJ		Added GetInterfaceFactory.
 *		 <1>	11/27/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XObjectModel.h>

#include <XAddPluginResources.h>
#include <XBoss.h>
#include <XLoadXML.h>
#include <XLogFile.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XUnknown.h>
#include <XURIResolver.h>
#include <XXMLCallbacks.h>
#include <XXMLDoc.h>
#include <XXMLItems.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {

#if DEBUG
	extern RUNTIME_EXPORT bool gExitingNormally;
#endif


// ===================================================================================
//	class XObjectModel
// ===================================================================================

XAutoPtr<XObjectModel>	XObjectModel::msInstance;		
XCriticalSection 		XObjectModel::msCreateMutex;

//---------------------------------------------------------------
//
// XObjectModel::~XObjectModel
//
// It'd be nice if we collected garbage here, but because the
// garbage collector is also a singleton there are order of
// initialization problems. So, client code will have to make
// a final call to the collector just before the app shuts down.
//
//---------------------------------------------------------------
XObjectModel::~XObjectModel()
{
	XBoss* boss = nil;
	
	PRECONDITION(mTerminated || !gExitingNormally);			// need to call Shutdown

#if !GARBAGE_COLLECT_COM
#if DEBUG
	if (!mBossList.empty() && gExitingNormally) {
		XLogFile log("Boss Leaks.txt");
		
		log.Print("There were %d boss leaks:\n\n", mBossList.size());

		BossList::iterator iter = mBossList.begin();
		while (iter != mBossList.end()) {
			boss = *iter;
			++iter;
			
			// Print the boss name,
			std::wstring name = boss->GetName();
			log.Print("'%ls'\n", name.c_str());
			
			// and the leaky interfaces.
			uint32 count = 0;
			XBoss::iterator iter2 = boss->begin();
			while (iter2 != boss->end()) {
				const XInterface& value = *iter2;
				
				const XImplementation* implementation = iter2->GetImplementation();
				const XUnknown* theInterface = implementation->FindInterface();
				
				// $$$ this isn't quite right for aggregated interfaces: 
				// $$$ FindInterface always returns a pointer to the first
				// $$$ XUnknown* in the object
				uint32 leakCount = theInterface != nil ? theInterface->GetRefCount() : 0UL;
				if (leakCount > 0) {
					if (leakCount == 1) 
						log.Print("\t'%ls' has 1 outstanding reference\n", value.GetEncodedName().c_str());
					else
						log.Print("\t'%ls' has %d outstanding references\n", value.GetEncodedName().c_str(), leakCount);
					++count;
				}

				++iter2;
			}
			
			if (count == 0)
				log.Print("\tOne of the aggregated interfaces has an outstanding reference\n");
				
			log.Print("\n");
		}
		log.Flush();
		
		char buffer[1024];
		std::sprintf(buffer, "There were %d boss leaks: see 'Boss Leaks.txt' for details.", mBossList.size());
		DEBUGSTR(buffer);			// can't use iostreams at static dtor time...
	}
#endif
#endif
}


//---------------------------------------------------------------
//
// XObjectModel::XObjectModel
//
//---------------------------------------------------------------
XObjectModel::XObjectModel()
{
#if DEBUG
	mTerminated = false;
#endif
}


//---------------------------------------------------------------
//
// XObjectModel::Instance								[static]
//
//---------------------------------------------------------------
XObjectModel* XObjectModel::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XObjectModel);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XObjectModel::Shutdown
//
//---------------------------------------------------------------
void XObjectModel::Shutdown()
{
	PRECONDITION(!mTerminated);
	
	XBoss* boss = nil;

#if GARBAGE_COLLECT_COM
	XGarbageCollector::Instance()->Collect(true);

	BossTable::iterator iter = mBossTable.begin();
	while (iter != mBossTable.end()) {
		boss = iter->second;
		++iter;
		
		delete boss;
	}
	
	mBossTable.clear();
#else

#if DEBUG
	BossTable::iterator iter = mBossTable.begin();
	while (iter != mBossTable.end()) {
		boss = iter->second;
		++iter;
		
		if (boss->IsSingleton() && boss->GetRefCount() > 1)	// singleton bosses aren't placed into mBossList
			mBossList.insert(boss);
		
		boss->RemoveReference();
	}

	mTerminated = true;
#endif
#endif
}


//---------------------------------------------------------------
//
// XObjectModel::CreateBoss
//
//---------------------------------------------------------------
XBoss* XObjectModel::CreateBoss(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	XBoss* boss = nil;
	
	{
	XEnterCriticalSection enter(mMutex);
		BossTable::const_iterator iter = mBossTable.find(name);
		if (iter != mBossTable.end())
			boss = iter->second;
		else
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the '#1' boss.", name)));
			
		ASSERT(!boss->IsAbstract());
			
		if (!boss->IsSingleton()) {
			boss = boss->Clone();
#if DEBUG && !GARBAGE_COLLECT_COM
			mBossList.insert(boss);
			ASSERT(boss->GetRefCount() == 0);		// don't have any constructed interfaces yet
#endif	
		}
	}
		
	return boss;
}

		
//---------------------------------------------------------------
//
// XObjectModel::GetBoss
//
//---------------------------------------------------------------
const XBoss* XObjectModel::GetBoss(const std::wstring& name) const
{
	PRECONDITION(name.length() > 0);
	
	XBoss* boss = nil;
	
	{
	XEnterCriticalSection enter(mMutex);
		BossTable::const_iterator iter = mBossTable.find(name);
		if (iter != mBossTable.end())
			boss = iter->second;
	}
		
	return boss;
}

		
//---------------------------------------------------------------
//
// XObjectModel::LoadBosses (XPlugin*, XURI)
//
//---------------------------------------------------------------
void XObjectModel::LoadBosses(XPlugin* plugin, const XURI& uri)
{
	XXMLDoc doc;
	
	{
	XAddPluginResources add(plugin);
		Whisper::Load(doc, uri);
	}
			
	this->LoadBosses(plugin, doc);
}


//---------------------------------------------------------------
//
// XObjectModel::LoadBosses (XPlugin*, XXMLDoc)
//
//---------------------------------------------------------------
void XObjectModel::LoadBosses(XPlugin* plugin, const XXMLDoc& doc)
{
	const XXMLElement* root = doc.GetRootElement();
	PRECONDITION(root->GetName() == L"BossList");
	
	{
	XEnterCriticalSection enter(mMutex);
		this->DoAddBosses(plugin, root);
	}
}


//---------------------------------------------------------------
//
// XObjectModel::UnloadBosses
//
//---------------------------------------------------------------
void XObjectModel::UnloadBosses(XPlugin* plugin)
{
	XEnterCriticalSection enter(mMutex);

	BossTable::iterator iter = mBossTable.begin();
	while (iter != mBossTable.end()) {
		BossTable::iterator temp = iter;
		XBoss* boss = iter->second;
		++iter;
		
		if (boss->GetPlugin() == plugin) {
			mBossTable.erase(temp);			
			
#if !GARBAGE_COLLECT_COM
			ASSERT(boss->GetRefCount() == 1);
			boss->RemoveReference();
#endif
		}
	}
}

			
//---------------------------------------------------------------
//
// XObjectModel::AddInterfaceFactory
//
//---------------------------------------------------------------
void XObjectModel::AddInterfaceFactory(const std::wstring& implementation, InterfaceFactory factory)
{
	PRECONDITION(implementation.length() > 0);
	PRECONDITION(factory != nil);

	{
	XEnterCriticalSection enter(mMutex);
		PRECONDITION(mFactoryTable.find(implementation) == mFactoryTable.end());
	
		mFactoryTable.insert(FactoryTable::value_type(implementation, factory));
	}
}


//---------------------------------------------------------------
//
// XObjectModel::RemoveInterfaceFactory
//
//---------------------------------------------------------------
void XObjectModel::RemoveInterfaceFactory(const std::wstring& implementation)
{
	PRECONDITION(implementation.length() > 0);

	{
	XEnterCriticalSection enter(mMutex);
		FactoryTable::iterator iter = mFactoryTable.find(implementation);
		PRECONDITION(iter != mFactoryTable.end());
	
		mFactoryTable.erase(iter);
	}
}


//---------------------------------------------------------------
//
// XObjectModel::GetInterfaceFactory
//
//---------------------------------------------------------------
InterfaceFactory XObjectModel::GetInterfaceFactory(const std::wstring& implementation) const
{
	InterfaceFactory factory = nil;
	
	FactoryTable::const_iterator iter = mFactoryTable.find(implementation);
	if (iter != mFactoryTable.end())
		factory = iter->second;
		
	return factory;
}


//---------------------------------------------------------------
//
// XObjectModel::AddInterfaceName
//
//---------------------------------------------------------------
void XObjectModel::AddInterfaceName(const std::wstring& encodedName, const std::wstring& interfaceName)
{
	PRECONDITION(encodedName.length() > 0);
	PRECONDITION(interfaceName.length() > 0);

	{
	XEnterCriticalSection enter(mMutex);
		InterfaceTable::iterator iter = mInterfaceTable.find(interfaceName);
		if (iter == mInterfaceTable.end())
			mInterfaceTable.insert(InterfaceTable::value_type(interfaceName, encodedName));
		else
			ASSERT(iter->second == encodedName);			// interface names are registered inside implementation files so we may be called more than once
	}
}


//---------------------------------------------------------------
//
// XObjectModel::GetEncodedName
//
//---------------------------------------------------------------
std::wstring XObjectModel::GetEncodedName(const std::wstring& interfaceName) const
{
	PRECONDITION(interfaceName.length() > 0);
	
	std::wstring encodedName;

	{
	XEnterCriticalSection enter(mMutex);
		InterfaceTable::const_iterator iter = mInterfaceTable.find(interfaceName);
		if (iter != mInterfaceTable.end())
			encodedName = iter->second;
		else
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the interface name for '#1' (register it with REGISTER_INTERFACE_NAME).", interfaceName)));
	}
	
	return encodedName;
}


//---------------------------------------------------------------
//
// XObjectModel::WriteBosses
//
//---------------------------------------------------------------
#if DEBUG
void XObjectModel::WriteBosses(const char* fileName)
{
	XLogFile log(fileName);
	
	for (BossTable::const_iterator iter = mBossTable.begin(); iter != mBossTable.end(); ++iter) {
		const XBoss* boss = iter->second;

		if (!boss->IsAbstract())
			this->DoWriteBoss(log, boss);
	}
}
#endif

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XObjectModel::DoWriteBoss
//
//---------------------------------------------------------------
#if DEBUG
void XObjectModel::DoWriteBoss(XLogFile& log, const XBoss* boss)
{
	log.Print(boss->GetName().c_str());
	log.Print(" Boss");
	if (boss->IsSingleton())
		log.Print(" (singleton)");	
	log.Print(":\n");
	
	InterfaceTable interfaces;
	for (XBoss::const_iterator iter = boss->begin(); iter != boss->end(); ++iter) {
		const XInterface& interface = *iter;
		const XImplementation* implementation = interface.GetImplementation();
		
		std::wstring name = interface.GetEncodedName();
		uint32 index = name.rfind(L"::");
		name = index != std::wstring::npos ? name.substr(index+2) : name;	// strip off namespaces
		
		InterfaceTable::iterator iter2 = interfaces.find(implementation->GetName());
		if (iter2 == interfaces.end())
			interfaces[implementation->GetName()] = name;	// mapping from implementation name to list of interface names
		else
			iter2->second += L", " + name;
	}
	
	this->DoWriteInterfaces(log, interfaces);
}
#endif


//---------------------------------------------------------------
//
// XObjectModel::DoWriteInterfaces
//
// Prints interfaces on the left and implementation name on the right.
//
//---------------------------------------------------------------
#if DEBUG
void XObjectModel::DoWriteInterfaces(XLogFile& log, const InterfaceTable& interfaces)
{
	std::wstring word;
	
	uint32 maxLen = 20;						// get the longest interface name so we can align the implementation names
	for (InterfaceTable::const_iterator iter = interfaces.begin(); iter != interfaces.end(); ++iter) {
		std::wstring temp = iter->second;
		word = Parse(temp , L", ");
		if (word.length()+1 > maxLen)
			maxLen = word.length()+1;
	}	
	
	for (InterfaceTable::const_iterator iter = interfaces.begin(); iter != interfaces.end(); ++iter) {
		bool wroteImplementation = false;
		std::wstring name = iter->second;

		while (!name.empty()) {
			word = Parse(name , L", ");
			
			log.Print("\t");
			log.Print(word.c_str());
			
			if (!wroteImplementation) {
				log.Print(std::wstring(maxLen - word.length(), ' ').c_str());
				log.Print(iter->first.c_str());
				wroteImplementation = true;
			}
			
			log.Print("\n");
		}
	}	
	
	log.Print("\n");
}
#endif


//---------------------------------------------------------------
//
// XObjectModel::DoDeletingBoss
//
//---------------------------------------------------------------
#if DEBUG && !GARBAGE_COLLECT_COM
void XObjectModel::DoDeletingBoss(XBoss* boss)
{
	PRECONDITION(boss != nil);

	{
	XEnterCriticalSection enter(mMutex);
		BossList::iterator iter = mBossList.find(boss);
		PRECONDITION(iter != mBossList.end());
	
		mBossList.erase(iter);
	}
}
#endif


//---------------------------------------------------------------
//
// XObjectModel::DoAddBosses
//
//---------------------------------------------------------------
void XObjectModel::DoAddBosses(XPlugin* plugin, const XXMLElement* root)
{
	PRECONDITION(root != nil);
	
	XXMLElement::const_item_iterator iter = root->item_begin();
	while (iter != root->item_end()) {
		const XXMLItem* item = *iter;
		++iter;
		
		const XXMLElement* elem = dynamic_cast<const XXMLElement*>(item);
		ASSERT(elem != nil);
		ASSERT(elem->GetName() == L"Boss" || elem->GetName() == L"AbstractBoss" || elem->GetName() == L"AddInBoss");
				
		XBoss* boss = new XBoss(plugin, elem);			
#if !GARBAGE_COLLECT_COM
		ASSERT(boss->GetRefCount() == 0);
		boss->AddReference();
#endif
		
		const std::wstring& name = boss->GetName();
	
		if (elem->GetName() == L"AddInBoss") {
			BossTable::const_iterator iter = mBossTable.find(name);
			ASSERT(iter != mBossTable.end());
			
			this->DoAddIn(iter->second, boss);

#if !GARBAGE_COLLECT_COM
			boss->RemoveReference();
#endif

		} else {
#if DEBUG
#if !GARBAGE_COLLECT_COM
			mBossList.insert(boss);
#endif

			BossTable::const_iterator iter = mBossTable.find(name);
			ASSERT(iter == mBossTable.end());
#endif	

			mBossTable.insert(BossTable::value_type(name, boss));
		}
	}
}


//---------------------------------------------------------------
//
// XObjectModel::DoAddIn
//
//---------------------------------------------------------------
void XObjectModel::DoAddIn(XBoss* boss, const XBoss* addIn)
{
	PRECONDITION(boss != nil);
	PRECONDITION(addIn != nil);
	
	XBoss::const_iterator iter = addIn->begin();
	while (iter != addIn->end()) {
		const XInterface& interface = *iter;
		++iter;
		
		boss->ReplaceInterface(interface);
	}	
}


}	// namespace Whisper

