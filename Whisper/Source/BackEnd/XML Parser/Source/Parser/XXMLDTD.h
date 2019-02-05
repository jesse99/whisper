/*
 *  File:		XXMLDTD.h
 *  Summary:	A class used to encapsulate an XML DTD (this is the section that describes
 *				what an element can contain, attribute types, and attribute defaults).
 *  Written by:	Jesse Jones
 *
 *	Classes:	XXMLAttributeDeclaration - type, default value, and enumerated values
 *				XXMLElementDeclaration	 - type, attributes, and valid child info
 *				XXMLDTD					 - contains a list of elements
 *
 *  Copyright © 1999-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDTD.h,v $
 *		Revision 1.4  2001/03/05 05:44:43  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/11 01:31:41  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 13:01:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 8/11/00	JDJ		XXMLElementDeclaration::GetAttribute returns a pointer instead
 *									of a reference (to allow for better error reporting).
 *		 <3>	 3/26/00	JDJ		Moved the notation and unparsed entity stuff from XXMLDTD to XXMLDoc.
 *		 <2>	 3/19/99	JDJ		Removed AddID and FindElement.
 *		 <1>	 1/30/99	JDJ		Created
 */

#pragma once

#include <map>
#include <vector>

#include <XCountedPtr.h>
#include <XDFA.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLDTD;


//-----------------------------------
//	Types
//
enum EAttributeType {kStringAttribute, 			//!< attribute value is a string
					 kIDAttribute, 				//!< attribute value is a unique name
					 kIDRefAttribute, 			//!< attribute value is an ID name
					 kIDRefsAttribute, 			//!< attribute value is a list of ID names
					 kEntityAttribute, 			//!< attribute value is an unparsed entity name
					 kEntitiesAttribute, 		//!< attribute value is a list of unparsed entity names
					 kTokenAttribute, 			//!< attribute value is a name
					 kTokensAttribute, 			//!< attribute value is a list of names
					 kNotationAttribute, 		//!< attribute value is the name of a notation that is used to interpret the element content
					 kEnumAttribute				//!< attribute value is member of an enumerated list
					 };

enum EAttributeDefault {kRequiredAttribute, 	//!< attribute must be specified
			   			kImpliedAttribute, 		//!< attribute has no default and need not be specified
			   			kFixedAttribute,		//!< attribute has a default value and cannot be changed
			  	 		kDefaultAttribute		//!< attribute has a default value and need not be specified
			  	 		};
			   
enum EElementType {kEmptyElement,				//!< element has no text and no child elements
				   kAnyElement,					//!< element can include text and any declared child elements
				   kMixedElement,				//!< element can have text and specified child elements in any order
				   kChildrenElement,			//!< element can have child elements, but no text
				   kUndeclaredElement			//!< element has been referenced, but not declared (yet)
				   };
	

// ===================================================================================
//	class XXMLAttributeDeclaration
//!		DTD type, default value, or enumerated values declaration.
// ===================================================================================
class XML_EXPORT XXMLAttributeDeclaration {

//-----------------------------------
//	Internal Types
//
public:
	typedef std::vector<std::wstring> 	EnumList;	// Enums normally don't have too many entries so a vector should be more time and space efficient than a set
	typedef EnumList::const_iterator	iterator;	
		
//-----------------------------------
//	Initialization/Destruction
//
public:
						~XXMLAttributeDeclaration();		

						XXMLAttributeDeclaration();

						XXMLAttributeDeclaration(EAttributeType type, EAttributeDefault deflt, const std::wstring& value);

//-----------------------------------
//	API
//
public:
	//! @name Type and Value
	//@{
			EAttributeType GetType() const							{return mType;}
			void 		SetType(EAttributeType type)				{mType = type;}
			
			EAttributeDefault GetDefaultType() const				{return mDefault;}
			void 		SetDefaultType(EAttributeDefault deflt)		{mDefault = deflt;}
			
			const std::wstring& GetDefaultValue() const				{return mValue;}
			void 		SetDefaultValue(const std::wstring& value)	{mValue = value;}
	//@}

	//! @name Enums
	//@{
			bool 		MatchEnum(const std::wstring& name) const;
						/**< Returns true if the name matches one of the enumerated values. */
						
			void 		AddEnum(const std::wstring& name);

			iterator 	begin() const								{return mEnums.begin();}
			iterator 	end() const									{return mEnums.end();}
	//@}

	//! @name In External Section
	//@{
			bool 		IsExternal() const							{return mExternal;}
			void 		SetExternal(bool external = true)			{mExternal = external;}
	//@}

//-----------------------------------
//	Member Data
//
protected:
	EAttributeType		mType;
	EnumList			mEnums;				// set of legal values (if mType == kEnumAttribute)
	EAttributeDefault	mDefault;				
	std::wstring		mValue;				// valid if mDefault == kFixedAttribute or kDefaultAttribute
	bool				mExternal;
};


// ===================================================================================
//	class XXMLElementDeclaration
//!		DTD type, attributes, and valid child info declaration.
// ===================================================================================
class XML_EXPORT XXMLElementDeclaration {

//-----------------------------------
//	Types
//
public:
	typedef std::map<std::wstring, XXMLAttributeDeclaration>	AttributeMap;
	typedef AttributeMap::const_iterator 				iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XXMLElementDeclaration();		

						XXMLElementDeclaration(XXMLDTD* dtd, EElementType type);

private:
						XXMLElementDeclaration(const XXMLElementDeclaration& rhs);

			XXMLElementDeclaration& operator=(const XXMLElementDeclaration& rhs);
						
//-----------------------------------
//	API
//
public:
	//! @name Type
	//@{
			EElementType GetType() const						{return mType;}
	//@}

	//! @name Attributes
	//@{
			const XXMLAttributeDeclaration* GetAttribute(const std::wstring& name) const;
			
			bool 		AddAttribute(const std::wstring& name, const XXMLAttributeDeclaration& attribute);
						/**< Returns true if the attribute was added. */
			
			iterator 	begin() const						{return mAttributes.begin();}
			iterator 	end() const							{return mAttributes.end();}
	//@}

	//! @name Child Elements
	//@{
			void 		AddMixedElement(const std::wstring& name);
			
			void 		AppendChildElement(const std::wstring& name);
			void 		AppendChildSymbol(wchar_t symbol);
						/**< Symbol can be '?', '*', '+', '(', or ')'. So, for 
						"(chapter, appendix, glossary)+" you would make the
						following calls:
								AppendChildSymbol('(');
								AppendChildElement("chapter");
								AppendChildElement("appendix");
								AppendChildElement("glossary");
								AppendChildSymbol(')');
								AppendChildSymbol('+'); */
	//@}
						
	//! @name Validation
	//@{
			void 		StartValidating();
						/**< Call this to begin checking that the element's children are valid */
						
			bool 		ValidText(const std::wstring& text);			
			bool 		ValidChild(const std::wstring& name);
						/**< These are called in sequence for each distinct section of the
						element's content. They will return false if the text or child
						element cannot appear at the (implicit) location. */
						
			bool 		StopValidating() const;
						/**< This called after the element's content has been parsed to
						check to see if all the required content is present (eg in
						case mType == kChildrenElement). */
	//@}
						
	//! @name In External Section
	//@{
			bool 		IsExternal() const							{return mExternal;}
			void 		SetExternal(bool external = true)			{mExternal = external;}
	//@}

//-----------------------------------
//	Internal API
//
public:
			void 		DeclareElement(XXMLElementDeclaration* element);
			
			bool 		HasIDAttribute() const						{return mHasIDAttribute;}
			
protected:
			wchar_t 	DoGetSymbol() const;

//-----------------------------------
//	Member Data
//
protected:
	XXMLDTD*			mDTD;
	bool				mExternal;
	bool 				mHasIDAttribute;
	
	EElementType		mType;
	AttributeMap		mAttributes;
	XCountedPtr<XDFA>	mDFA;				// used if mType == kMixedElement or kChildrenElement
	wchar_t				mSymbol;			// used when transitioning between mDFA states
	
	std::wstring		mExpression;		// used to build mDFA
};


// ===================================================================================
//	class XXMLDTD
//!		Contains a list of element declarations.
//		This is provided primarily for XML editors: most apps won't have to deal with
//		this.
// ===================================================================================
class XML_EXPORT XXMLDTD {

//-----------------------------------
//	Types
//
public:
	typedef XCountedPtr<XXMLElementDeclaration>	ElementPtr;

	typedef std::map<std::wstring, ElementPtr>		ElementMap;		
	typedef ElementMap::const_iterator			iterator;

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XXMLDTD();		

						XXMLDTD();

			XXMLDTD* 	Clone() const;
	
private:
						XXMLDTD(const XXMLDTD& rhs);
						
			XXMLDTD& 	operator=(const XXMLDTD& rhs);
						
//-----------------------------------
//	API
//
public:
	// ----- Elements -----
			XXMLElementDeclaration* GetElement(const std::wstring& name);
			const XXMLElementDeclaration* GetElement(const std::wstring& name) const;
			
			bool 		AddElement(const std::wstring& name, XXMLElementDeclaration* takeElement);
						/**< Returns true if the element was added. */
			
			iterator 	begin() const						{return mElements.begin();}
			iterator 	end() const							{return mElements.end();}
			
//-----------------------------------
//	Internal Functions
//
public:
			wchar_t 	DoGetSymbol();

//-----------------------------------
//	Member Data
//
protected:
	ElementMap	mElements;
	wchar_t		mNextSymbol;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

