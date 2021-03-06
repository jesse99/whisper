/*
 *  File:		XXMLCallback.h
 *  Summary:	Abstract base class used when parsing XML content (ie the stuff outside the DTD).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLCallback.h,v $
 *		Revision 1.5  2001/03/17 07:44:06  jesjones
 *		Made the assignment operator and the copy ctor private.
 *		
 *		Revision 1.4  2001/03/05 05:44:33  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:31:29  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 13:00:01  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XTypes.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
struct SExternalID;


// ===================================================================================
//	class XXMLCallbackMixin
//!		Abstract base class used with SAX-style XML parsing.
// ===================================================================================
class XML_EXPORT XXMLCallbackMixin {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLCallbackMixin()												{}
							
						XXMLCallbackMixin()													{}
						
private:
						XXMLCallbackMixin(const XXMLCallbackMixin& rhs);
			XXMLCallbackMixin& operator=(const XXMLCallbackMixin& rhs);

//-----------------------------------
//	API
//
public:
	virtual void 		OnBeginDocument() 													{}
	virtual void 		OnEndDocument(const std::wstring& docName) 								{UNUSED(docName);}
						/**< Doc name will be empty if the '<!DOCTYPE' tag is missing. */

	virtual void 		OnBeginElement(const std::wstring& name) = 0;
	virtual void 		OnEndElement(const std::wstring& name) = 0;

	virtual void 		OnText(const std::wstring& element, const std::wstring& text, bool inElementContent) = 0;
						/**< The XML standard requires parsers to return all whitespace
						occuring outside markup to the application. However this
						whitespace is generally only significant for those elements
						that can contain character data: for elements that are empty
						or can contain only child elements inElementContent will be
						true and text can usually be ignored. */

	virtual void 		OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID) = 0;
	
	virtual void 		OnUnparsedEntity(const std::wstring& name, const SExternalID& id)		{UNUSED(name); UNUSED(id);}

	virtual void 		OnNotation(const std::wstring& name, const SExternalID& id)				{UNUSED(name); UNUSED(id);}

	virtual void 		OnProcessInstruction(const std::wstring& target, const std::wstring& data)	{UNUSED(target); UNUSED(data);}
	
	virtual void 		OnComment(const std::wstring& contents)									{UNUSED(contents);}
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

