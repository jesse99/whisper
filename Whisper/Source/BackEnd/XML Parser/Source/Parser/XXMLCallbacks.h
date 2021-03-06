/*
 *  File:		XXMLCallbacks.h
 *  Summary:	Classes used when parsing XML content (ie the stuff outside the DTD). Note that
 *				for simple XML documents it's often easier and more efficient to use a custom
 *				XXMLCallback.
 *  Written by:	Jesse Jones
 *
 *	Classes:	XXMLNullCallback	 - Throws away all the document content.
 *				XXMLDocumentCallback - Builds an in-memory representation of an XML file's content.
 *				XXMLElementCallback  - Like XXMLDocumentCallback except that the generated tree is
 *									   for individual elements instead of the entire document.
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLCallbacks.h,v $
 *		Revision 1.4  2001/03/05 05:44:38  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:31:35  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 13:00:17  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <vector>

#include <XXMLCallback.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLDoc;
class XXMLElement;


// ===================================================================================
//	class XXMLNullCallback
//!		Throws away all the document content.
//		This is not a generally useful class, but it is useful for profiling XML 
//		parsing and, possibly, for reading just the DTD.
// ===================================================================================
class XML_EXPORT XXMLNullCallback : public XXMLCallbackMixin {

	typedef XXMLCallbackMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLNullCallback();
						
						XXMLNullCallback();

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnBeginElement(const std::wstring& name);
	virtual void 		OnEndElement(const std::wstring& name);

	virtual void 		OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID);
	virtual void 		OnText(const std::wstring& element, const std::wstring& text, bool inElementContent);
};


// ===================================================================================
//	class XXMLDocumentCallback
//!		Builds an in-memory representation of an XML file's content.
// ===================================================================================
class XML_EXPORT XXMLDocumentCallback : public XXMLCallbackMixin {

	typedef XXMLCallbackMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLDocumentCallback();
						
						XXMLDocumentCallback(XXMLDoc& doc);
						/**< Populates the document with the XML content. */

//-----------------------------------
//	New API
//
public:
			void 		StripInsignifcantWhiteSpace(bool strip = true)	{mStripWhiteSpace = strip;}
						/**< The XML standard requires parsers to return all whitespace
						occuring outside markup to the application. However this
						whitespace is generally only significant for those elements
						that can contain character data. By default this class will
						only pass through whitespace if the parent element supports
						character data. */
						
			void 		StripComments(bool strip = true)				{mStripComments = strip;}
						/**< By default comments are not added to the XXMLDoc. */
									
//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnBeginDocument();
	virtual void 		OnEndDocument(const std::wstring& docName);

	virtual void 		OnBeginElement(const std::wstring& name);
	virtual void 		OnEndElement(const std::wstring& name);

	virtual void 		OnText(const std::wstring& element, const std::wstring& text, bool inElementContent);
	virtual void 		OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID);
	virtual void 		OnProcessInstruction(const std::wstring& target, const std::wstring& data);
	virtual void 		OnComment(const std::wstring& contents);
			
	virtual void 		OnUnparsedEntity(const std::wstring& name, const SExternalID& id);
	virtual void 		OnNotation(const std::wstring& name, const SExternalID& id);

//-----------------------------------
//	Inherited API
//
protected:
	XXMLDoc&					mDoc;
	bool						mFoundRoot;
	bool						mStripWhiteSpace;
	bool						mStripComments;
	
	std::vector<XXMLElement*>	mElements;
};


// ===================================================================================
//	class XXMLElementCallback
//!		Like XXMLDocumentCallback except that the generated tree is for individual elements instead of the entire document.
// ===================================================================================
class XML_EXPORT XXMLElementCallback : public XXMLCallbackMixin {

	typedef XXMLCallbackMixin Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLElementCallback();
						
						XXMLElementCallback(const std::wstring& rootElement);

//-----------------------------------
//	New API
//
public:
			void 		StripInsignifcantWhiteSpace(bool strip)			{mStripWhiteSpace = strip;}
						/**< The XML standard requires parsers to return all whitespace
						occuring outside markup to the application. However this
						whitespace is generally only significant for those elements
						that can contain character data. By default this class will
						only pass through whitespace if the parent element supports
						character data. */
			
protected:
	virtual void 		OnElement(XXMLElement* element) = 0;
						/**< Called for each (immediate) child element of the root element.
						Note that the element pointer will be deleted after this function
						returns. */

//-----------------------------------
//	Inherited API
//
protected:
	virtual void 		OnBeginElement(const std::wstring& name);
	virtual void 		OnEndElement(const std::wstring& name);

	virtual void 		OnAttribute(const std::wstring& element, const std::wstring& name, const std::wstring& value, bool implied, bool isID);
	virtual void 		OnText(const std::wstring& element, const std::wstring& text, bool inElementContent);
			
//-----------------------------------
//	Member Data
//
protected:
	std::wstring				mRootName;
	bool						mStripWhiteSpace;
	
	std::vector<XXMLElement*>	mElements;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

