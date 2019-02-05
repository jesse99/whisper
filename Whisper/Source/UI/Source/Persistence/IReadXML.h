/*
 *  File:       IReadXML.h
 *  Summary:   	Interface used to stream bosses in from XML.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IReadXML.h,v $
 *		Revision 1.4  2001/03/05 05:38:15  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/14 08:24:35  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 09:06:17  jesjones
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
class IHierarchy;
class XURI;
class XXMLDoc;
class XXMLElement;
typedef XInterfacePtr<IHierarchy> IHierarchyPtr;


//-----------------------------------
//	Types and Constants
//
enum ELoadPass {kLoadingBoss, kLoadedBoss};


// ===================================================================================
//	class IReadXML
//!		Interface used to stream bosses in from XML.
// ===================================================================================
class IReadXML : public XUnknown {

//-----------------------------------
//	Types
//
public:
	typedef void (*ReadProc)(XBoss* boss, XBoss::iterator iter, const XXMLElement& element, const IHierarchyPtr& parent, ELoadPass pass);

//-----------------------------------
//	API
//
public:
	virtual XUnknownPtr	CreateBoss(const XXMLElement& element, const IHierarchyPtr& parent) = 0;	
	virtual XUnknownPtr	CreateBoss(const XXMLDoc& doc, const std::wstring& id, const IHierarchyPtr& parent) = 0;	
	virtual XUnknownPtr	CreateBoss(const XURI& uri, const std::wstring& id, const IHierarchyPtr& parent) = 0;
						/**< id is an XML ID and can point to either a Boss element 
						or to a LocaleTable. If id points to a LocaleTable a new 
						id is fetched from the table based on the current locale 
						and platform. */
							
	virtual void 		ReadBoss(XUnknownPtr& unknown, const XXMLElement& element) = 0;
						/**< Iterates through the immediate child elements of element and
						searches for a loader using the child element name. If a loader
						is found it's called to load that interface. If not a warning
						is traced out (this makes it possible for old app's to read 
						new documents). */

	virtual void 		AddReader(const std::wstring& interfaceName, ReadProc proc) = 0;
						/**< Used to attach loader functions that know how to stream in a
						particular interface. */
};

typedef XInterfacePtr<IReadXML> IReadXMLPtr;
typedef XInterfacePtr<const IReadXML> IConstReadXMLPtr;


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
