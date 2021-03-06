/*
 *  File:       XInterface.cpp
 *  Summary:    The object used to create interfaces.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XInterface.cpp,v $
 *		Revision 1.4  2001/04/27 04:33:35  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/03/17 12:25:34  jesjones
 *		Outlined XImplementation ctors. XImplementation::GetName is no longer debug only.
 *		
 *		Revision 1.2  2000/11/09 12:39:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <5>	 8/25/00	JDJ		XImplementation::GetInterface temporarily increments the
 *									boss's ref count. GetInterface also sets the interface's
 *									ref count to one.
 *		 <4>	 8/12/00	JDJ		Added a grouped flag to XInterface.
 *		 <3>	 7/07/00	JDJ		XImplementation will assert if someone tries to query
 *									for an interface as it's being constructed.
 *		 <2>	 5/21/00	JDJ		Made some changes to support interface aggregation.
 *		 <1>	 5/20/00	JDJ		Created (from XBoss.cpp)
 */

#include <XWhisperHeader.h>
#include <XInterface.h>

#include <XBoss.h>
#include <XUnknown.h>

#if GARBAGE_COLLECT_COM
	#include <XGarbageCollector.h>
#endif

namespace Whisper {


// ===================================================================================
//	class XImplementation
// ===================================================================================

//---------------------------------------------------------------
//
// XImplementation::~XImplementation
//
//---------------------------------------------------------------
XImplementation::~XImplementation()						
{
	delete mInterface;
}


//---------------------------------------------------------------
//
// XImplementation::XImplementation ()
//
//---------------------------------------------------------------
XImplementation::XImplementation() : XReferenceCountedMixin(0)	
{
	mFactory = nil;
	mInterface = nil;
	mAggregated = false;
	mConstructing = false;
}


//---------------------------------------------------------------
//
// XImplementation::XImplementation (InterfaceFactory, bool, wstring)
//
//---------------------------------------------------------------
XImplementation::XImplementation(InterfaceFactory f, bool a, const std::wstring& n) : XReferenceCountedMixin(0), mName(n)	
{
	mFactory = f;
	mInterface = nil;
	mAggregated = a;
	mConstructing = false;
}


//---------------------------------------------------------------
//
// XImplementation::Clone
//
//---------------------------------------------------------------
XImplementation* XImplementation::Clone() const
{
	PRECONDITION(!mConstructing);
	
	XImplementation* implementation = new XImplementation(mFactory, mAggregated, mName);
	
	return implementation;
}


//---------------------------------------------------------------
//
// XImplementation::GetInterface
//
//---------------------------------------------------------------
XUnknown* XImplementation::GetInterface(XBoss* boss)
{	
	PRECONDITION(boss != nil);
	PRECONDITION(!mConstructing);				// this will fire if someone queries for an interface as it's being constructed
	PRECONDITION(mFactory != nil);
	
	XBumpRefCount bump(boss);			// need this to handle the case where the first interface does a query in its ctor (without this boss ref count will go to one and then zero which will cause the boss to be deleted)
		
	if (mInterface == nil) {
		mConstructing = true;
		
		SImplementation imp = mFactory(boss);
		mInterface = imp.ptr;
#if !GARBAGE_COLLECT_COM
		ASSERT(mInterface->GetRefCount() == 0);
#else
		XGarbageCollector::Instance()->ConstructedInterface(boss, imp.ptr, imp.bytes);
#endif
		ASSERT(!boss->IsPrototype() || boss->IsSingleton());	// doesn't make sense to query for an interface on a prototype if it's not a singleton

		mConstructing = false;
	}
	
#if !GARBAGE_COLLECT_COM
	mInterface->DoAddReference();		// our caller will remove this reference (we need this so XBumpRefCount doesn't delete the boss when the first interface is created)
#endif

	return mInterface;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XInterface
// ===================================================================================

//---------------------------------------------------------------
//
// XInterface::~XInterface
//
//---------------------------------------------------------------
XInterface::~XInterface()
{
	if (mImplementation != nil)
		mImplementation->RemoveReference();
}


//---------------------------------------------------------------
//
// XInterface::XInterface (wstring, XImplementation*, bool)
//
//---------------------------------------------------------------
XInterface::XInterface(const std::wstring& encodedName, XImplementation* implementation, bool grouped) : mEncodedName(encodedName)
{
	PRECONDITION(implementation != nil);

	mImplementation = implementation;
	mImplementation->AddReference();
	
	mGrouped = grouped;
}


//---------------------------------------------------------------
//
// XInterface::XInterface (XInterface)
//
//---------------------------------------------------------------
XInterface::XInterface(const XInterface& rhs) : mEncodedName(rhs.mEncodedName)	
{
	mImplementation = rhs.mImplementation;
	
	if (mImplementation != nil)
		mImplementation->AddReference();
	
	mGrouped = rhs.mGrouped;
}


//---------------------------------------------------------------
//
// XInterface::operator=
//
//---------------------------------------------------------------
XInterface& XInterface::operator=(const XInterface& rhs)
{
	if (this != &rhs) {
		mEncodedName = rhs.mEncodedName;			// do this first since it's the only thing that may throw

		if (mImplementation != nil)
			mImplementation->RemoveReference();

		mImplementation = rhs.mImplementation;	
		mGrouped = rhs.mGrouped;

		if (mImplementation != nil)
			mImplementation->AddReference();
	}
	
	return *this;
}


//---------------------------------------------------------------
//
// XInterface::DoCompare
//
//---------------------------------------------------------------
int XInterface::DoCompare(const char* rhs) const
{
	PRECONDITION(rhs != nil);
	
	int result = 0;
	
	uint32 index;
	for (index = 0; index < mEncodedName.length() && result == 0; ++index) {
		if (rhs[index] == '\0')
			result = +1;			
		else if (mEncodedName[index] < rhs[index])
			result = -1;
		else if (mEncodedName[index] > rhs[index])
			result = +1;
	}
		
	if (result == 0 && rhs[index] != '\0')
		result = -1;
	
	return result;
}



}	// namespace Whisper

