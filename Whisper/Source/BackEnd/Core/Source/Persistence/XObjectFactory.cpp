/*
 *  File:		XObjectFactory.cpp
 *  Summary:	A singleton that knows how to create objects by name.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XObjectFactory.cpp,v $
 *		Revision 1.3  2001/04/27 04:34:53  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 12:45:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XObjectFactory.h>

#include <map>

#include <XCreateByName.h>

namespace Whisper {


// ===================================================================================
//	class XObjectFactoryException
// ===================================================================================

//---------------------------------------------------------------
//
// XObjectFactoryException::XObjectFactoryException
//
//---------------------------------------------------------------
XObjectFactoryException::XObjectFactoryException(const std::wstring& className) : runtime_error(ToUTF8Str((L"Internal Error: XObjectFactory::GetCreator couldn't find a creator proc for " + className + L"."))) 
{
	mClassName = className;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XObjectFactory
// ===================================================================================

XAutoPtr<XObjectFactory>	XObjectFactory::msInstance;		
XCriticalSection 			XObjectFactory::msCreateMutex;

//---------------------------------------------------------------
//
// XObjectFactory::~XObjectFactory
//
//---------------------------------------------------------------
XObjectFactory::~XObjectFactory()
{
}


//---------------------------------------------------------------
//
// XObjectFactory::XObjectFactory
//
//---------------------------------------------------------------
XObjectFactory::XObjectFactory()
{
}


//---------------------------------------------------------------
//
// XObjectFactory::Instance								[static]
//
//---------------------------------------------------------------
XObjectFactory* XObjectFactory::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil) 
			msInstance.Reset(new XObjectFactory);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XObjectFactory::RegisterClass
//
//---------------------------------------------------------------
void XObjectFactory::RegisterClass(const std::wstring& name, CreatorProcPtr createProc)
{
	PRECONDITION(createProc != nil);
	
	XEnterCriticalSection enter(mMutex);

	std::pair<CreatorTable::iterator, bool> result = mTable.insert(CreatorTable::value_type(name, createProc));
	if (!result.second)
		ASSERT(result.first->second == createProc);		// if name was in map make sure createProc's match
}


//---------------------------------------------------------------
//
// XObjectFactory::UnregisterClass
//
//---------------------------------------------------------------
void XObjectFactory::UnregisterClass(const std::wstring& name)
{
	XEnterCriticalSection enter(mMutex);

	CreatorTable::iterator iter = mTable.find(name);
	if (iter != mTable.end())
		mTable.erase(iter);
}


//---------------------------------------------------------------
//
// XObjectFactory::HasCreator
//
//---------------------------------------------------------------
bool XObjectFactory::HasCreator(const std::wstring& className) const
{
	XEnterCriticalSection enter(mMutex);

	bool has = mTable.find(className) != mTable.end();
	
	return has;
}


//---------------------------------------------------------------
//
// XObjectFactory::GetCreator
//
//---------------------------------------------------------------
XObjectFactory::CreatorProcPtr XObjectFactory::GetCreator(const std::wstring& className) const
{
	CreatorProcPtr creator = nil;

	XEnterCriticalSection enter(mMutex);

	CreatorTable::const_iterator iter = mTable.find(className);
	if (iter != mTable.end())
		creator = iter->second;
	else
		throw XObjectFactoryException(className);
	
	return creator;
}


//---------------------------------------------------------------
//
// XObjectFactory::CreateObject
//
//---------------------------------------------------------------
XCreateByNameMixin* XObjectFactory::CreateObject(const std::wstring& className, XCreateByNameMixin* parent)
{
	XCreateByNameMixin* object;
	
	CreatorProcPtr creator = this->GetCreator(className);

	object = (*creator)(parent);
	
	return object;
}


}	// namespace Whisper
