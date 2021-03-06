/*
 *  File:       XImageImporter.cpp
 *  Summary:	Abstract base class that can be used to initialize XBaseImage's.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XImageImporter.cpp,v $
 *		Revision 1.4  2001/04/27 04:33:23  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/04/17 01:43:53  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 12:38:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	  7/15/99	JDJ		Added Register, Unregister, and Create methods.
 *		 <2>	  6/19/99	JDJ		Added comments to ctor and dtor.
 *		 <1>	 11/27/97	JDJ		Created (from Raven's TImageInitter)
 */

#include <XWhisperHeader.h>
#include <XImageImporter.h>

#include <XAutoPtr.h>
#include <XColorTable.h>
#include <XDebug.h>
#include <XExceptions.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XURI.h>
#include <XURIResolver.h>

namespace Whisper {


XImageImporter::SEntry::SEntry(const SEntry& rhs) : extension(rhs.extension), creator(rhs.creator) {}

// ===================================================================================
//	class XImageImporter
// ===================================================================================

XImageImporter::ExtTable	XImageImporter::msExtensionTable;
XCriticalSection 			XImageImporter::msExtensionMutex;

//---------------------------------------------------------------
//
// XImageImporter::~XImageImporter
//
//---------------------------------------------------------------
XImageImporter::~XImageImporter()
{
	CALL_INVARIANT;				// may fire if a subclass ctor throws

	delete mColors;
}


//---------------------------------------------------------------
//
// XImageImporter::XImageImporter
//
//---------------------------------------------------------------
XImageImporter::XImageImporter()
{	
	mWidth  = -1;					// set by subclasses
	mHeight = -1;
	mDepth  = -1;
	mColors = nil;
	
//	CALL_INVARIANT;				// wait for subclass ctors to execute
}


//---------------------------------------------------------------
//
// XImageImporter::Register								[static]
//
//---------------------------------------------------------------
void XImageImporter::Register(const std::wstring& extension, CreatorProcPtr createProc)
{
	PRECONDITION(createProc != nil);
	
	SEntry entry(ConvertToLowerCase(extension), createProc);
	XEnterCriticalSection enter(msExtensionMutex);
	
	ExtTable::iterator iter = std::find(msExtensionTable.begin(), msExtensionTable.end(), entry);
	if (iter == msExtensionTable.end())
		msExtensionTable.push_back(entry);
	else
		ASSERT(entry.creator == iter->creator);
}


//---------------------------------------------------------------
//
// XImageImporter::Unregister								[static]
//
//---------------------------------------------------------------
void XImageImporter::Unregister(const std::wstring& extension)
{
	SEntry entry(ConvertToLowerCase(extension), nil);
	XEnterCriticalSection enter(msExtensionMutex);
	
	ExtTable::iterator iter = std::find(msExtensionTable.begin(), msExtensionTable.end(), entry);
	if (iter != msExtensionTable.end())
		msExtensionTable.erase(iter);
}


//---------------------------------------------------------------
//
// XImageImporter::Create									[static]
//
//---------------------------------------------------------------
XImageImporter* XImageImporter::Create(const XURI& uri)
{	
	std::wstring extension;
	std::wstring file = uri.GetFile();
	uint32 index = file.rfind('.');
	if (index != std::wstring::npos)
		extension = ConvertToLowerCase(file.substr(index+1));

	CreatorProcPtr creator = nil;
	{
	SEntry entry(extension, nil);
	XEnterCriticalSection enter(msExtensionMutex);	
		ExtTable::iterator iter = std::find(msExtensionTable.begin(), msExtensionTable.end(), entry);
		if (iter != msExtensionTable.end())
			creator = iter->creator;
	}

	XImageImporter* image = nil;
	if (creator != nil) {
		XAutoPtr<XResource> resource(XURIResolver::Instance()->Resolve(uri));
		if (resource.Get() != nil)
			image = (*creator)(resource.Release());
		else
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the '#1' resource.", uri.GetFile())));
	
	} else
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find a create proc for the '#1' image extension.", extension)));
	
	return image;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XImageImporter::Invariant
//
//---------------------------------------------------------------
void XImageImporter::Invariant() const
{	
	int32 refCount = this->GetRefCount();
	ASSERT(refCount > 0);
	
	ASSERT(mWidth > 0);
	ASSERT(mHeight > 0);
	ASSERT(mDepth == 1 || mDepth == 2 || mDepth == 4 || mDepth == 8 || mDepth == 16 || mDepth == 24 || mDepth == 32);
	ASSERT(mColors != nil || mDepth > 8);
}


}	// namespace Whisper

