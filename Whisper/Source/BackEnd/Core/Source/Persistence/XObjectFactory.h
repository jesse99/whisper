/*
 *  File:		XObjectFactory.h
 *  Summary:	A singleton that knows how to create objects by name.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XObjectFactory.h,v $
 *		Revision 1.4  2001/03/05 05:42:42  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:54:08  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:45:09  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <map>

#include <XAutoPtr.h>
#include <XConstants.h>
#include <XCriticalSection.h>
#include <XExceptions.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XCreateByNameMixin;


// ===================================================================================
//	class XObjectFactoryException
//!		Thrown by XObjectFactory::GetCreator when it can't find a  creator proc for className.
// ===================================================================================
class CORE_EXPORT XObjectFactoryException : public std::runtime_error { 

public: 
						XObjectFactoryException(const std::wstring& className);
						
			std::wstring GetClassName() const					{return mClassName;}
			
protected:
	std::wstring		mClassName;
};


// ===================================================================================
//	class XObjectFactory
//!		A singleton that knows how to create objects by name.
// ===================================================================================
class CORE_EXPORT XObjectFactory {

//-----------------------------------
//	Types
//
public:
	typedef XCreateByNameMixin* (*CreatorProcPtr)(XCreateByNameMixin* parent);

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XObjectFactory();
	
						XObjectFactory();
						
	static XObjectFactory* Instance();

private:
						XObjectFactory(const XObjectFactory& rhs);
						
			XObjectFactory& operator=(const XObjectFactory& rhs);

//-----------------------------------
//	API
//
public:
	// ----- Registration -----
			void 		RegisterClass(const std::wstring& className, CreatorProcPtr createProc);
						/**< The classy way to do this is with static template objects
						defined in the cpp that the persistent object is defined in.
						Unfortunately this isn't a very portable technique... */
			
			void 		UnregisterClass(const std::wstring& className);

	// ----- Creation Functions -----
			bool 		HasCreator(const std::wstring& className) const;

			CreatorProcPtr GetCreator(const std::wstring& className) const;

	// ----- Create by Name -----
			XCreateByNameMixin* CreateObject(const std::wstring& className, XCreateByNameMixin* parent);

//-----------------------------------
//	Internal Types
//
protected:
	typedef std::map<std::wstring, CreatorProcPtr> CreatorTable;

//-----------------------------------
//	Member Data
//
protected:
	CreatorTable					mTable;
	mutable XCriticalSection		mMutex;

private:
	static XAutoPtr<XObjectFactory>	msInstance;			
	static XCriticalSection			msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

