/*
 *  File:       XInterfaceMacros.h
 *  Summary:    Macros used to register implementation classes with the object model.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfaceMacros.h,v $
 *		Revision 1.6  2001/10/03 02:10:37  jesjones
 *		Added "Whisper::" to some of the macros.
 *		
 *		Revision 1.5  2000/12/31 04:48:39  jesjones
 *		Includes XTranscode.h
 *		
 *		Revision 1.4  2000/11/21 09:24:24  jesjones
 *		Added a <string> include.
 *		
 *		Revision 1.3  2000/11/09 12:39:45  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <typeinfo>
#include <string>

#include <XTranscode.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XBoss;
class XUnknown;


//-----------------------------------
//	Types
//
struct SImplementation {
	XUnknown*	ptr;
	uint32		bytes;
};

typedef SImplementation (*InterfaceFactory)(XBoss* boss);


// ===================================================================================
//	class ZRegisterInterface
// ===================================================================================
class COM_EXPORT ZRegisterInterface {

public:
						~ZRegisterInterface();

						ZRegisterInterface(const std::wstring& implementation, InterfaceFactory factory);
						
private:
	std::wstring	mImplementation;
};

}	// namespace Whisper


// ===================================================================================
//	Macros
//		The natural way to register interfaces is via a static object in the 
//		implementation file. Unfortunately the linker may dead strip the class since
// 		it's typically not referenced by any other code and our static object may not
//		be constructed until a function in the cpp executes. To get around these
//		problems we use the following macros:
//
// 		DEFINE_INTERFACE_FACTORY is used inside the implementation file and defines a 
//		global function that will create the implementation class. 
//
//		REGISTER_INTERFACE_FACTORY is used within a function in the plugin's startup 
//		code and registers the factory function with the object model.
//
//		REGISTER_INTERFACE_NAME can be used within either the app or plugin startup 
//		code and is used by the object model to map from interface types to interface 
//		names (this means that clients don't need to specify interface names when 
//		constructing XInterfacePtr's). 
// ===================================================================================
#define DEFINE_INTERFACE_FACTORY(ImpClass)										\
	Whisper::SImplementation Create##ImpClass(Whisper::XBoss* boss);			\
	Whisper::SImplementation Create##ImpClass(Whisper::XBoss* boss) 			\
	{																			\
		Whisper::SImplementation imp;											\
		imp.ptr   = reinterpret_cast<Whisper::XUnknown*>(new ImpClass(boss));	\
		imp.bytes = sizeof(ImpClass);											\
		return imp;																\
	}

#define REGISTER_INTERFACE_FACTORY(ImpClass, ImpName)							\
	Whisper::SImplementation Create##ImpClass(Whisper::XBoss* boss);			\
	static Whisper::ZRegisterInterface register##ImpClass(ImpName, Create##ImpClass)
	
#define REGISTER_INTERFACE_NAME(IntClass, IntName)								\
	Whisper::XObjectModel::Instance()->AddInterfaceName(Whisper::FromAsciiStr(typeid(IntClass).name()), IntName)	
	

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif
