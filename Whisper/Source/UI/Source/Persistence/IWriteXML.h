/*
 *  File:       IWriteXML.h
 *  Summary:   	Interface used to convert interfaces and bosses into XML.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IWriteXML.h,v $
 *		Revision 1.4  2001/03/05 05:38:58  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:26:20  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:08:05  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XInterfacePtr.h>
#include <XUnknown.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLElement;


// ===================================================================================
//	class IWriteXML
//!		Interface used to convert interfaces and bosses into XML.
// ===================================================================================
class IWriteXML : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef XXMLElement* (*WriteProc)(const XBoss* boss, XBoss::const_iterator iter);

//-----------------------------------
//	API
//
public:
	virtual XXMLElement* CreateElement(const XBoss* boss, const std::wstring& name = L"Boss") = 0;	
						/**< Creates a Boss element, iterates over each element in the
						boss, and uses the method below to append child elements. */

	virtual XXMLElement* CreateElement(const XBoss* boss, XBoss::const_iterator iter) = 0;	
						/**< Gets the interface name and looks for an associated writer
						function. If a writer is found it's invoked to create the
						XML element. Otherwise nil is returned. */

	virtual void 		AddWriter(const std::wstring& interfaceName, WriteProc proc) = 0;
						/**< Used to attach writer functions that know how to convert
						a particular interface into an XML element. */
};

typedef XInterfacePtr<IWriteXML> IWriteXMLPtr;
typedef XInterfacePtr<const IWriteXML> IConstWriteXMLPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
