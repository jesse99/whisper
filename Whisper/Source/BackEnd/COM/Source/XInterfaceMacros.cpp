/*
 *  File:       XInterfaceMacros.h
 *  Summary:    Macros used to register implementation classes with the object model.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XInterfaceMacros.cpp,v $
 *		Revision 1.2  2000/11/09 12:39:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	11/27/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XInterfaceMacros.h>

#include <XObjectModel.h>

namespace Whisper {


// ===================================================================================
//	class ZRegisterInterface
// ===================================================================================

//---------------------------------------------------------------
//
// ZRegisterInterface::~ZRegisterInterface
//
//---------------------------------------------------------------
ZRegisterInterface::~ZRegisterInterface()
{
//	XObjectModel::Instance()->RemoveInterfaceFactory(mImplementation);
}


//---------------------------------------------------------------
//
// ZRegisterInterface::ZRegisterInterface
//
//---------------------------------------------------------------
ZRegisterInterface::ZRegisterInterface(const std::wstring& implementation, InterfaceFactory factory) : mImplementation(implementation)
{
	XObjectModel::Instance()->AddInterfaceFactory(mImplementation, factory);
}


}	// namespace Whisper

