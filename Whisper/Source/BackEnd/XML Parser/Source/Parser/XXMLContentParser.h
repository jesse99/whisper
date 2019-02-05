/*
 *  File:		XXMLContentParser.h
 *  Summary:	Classes used to parse the non-DTD portions of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLContentParser.h,v $
 *		Revision 1.2  2000/11/09 13:00:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/02/99	JDJ		Created
 */

#pragma once

#include <XLexerGrammar.h>
#include <XXMLDTD.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLLexer;
class XXMLParser;


// ===================================================================================
//	class ZGeneralReferenceParser											Reference
// ===================================================================================
class ZGeneralReferenceParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZGeneralReferenceParser(XXMLParser* parser, XXMLLexer* lexer);

//-----------------------------------
//	API
//
public:
			bool 		IsCharacter() const						{return mCharRef;}
			bool 		IsPredefined() const;
			bool 		IsExternal() const;
			
			const std::wstring& GetName() const						{ASSERT(!mCharRef); return mName;}
			
			std::wstring Parse();

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*		mParser;	
	XXMLLexer*		mLexer;

	bool			mCharRef;
	std::wstring	mName;
};


// ===================================================================================
//	class ZExtParsedEntParser											extParsedEnt
// ===================================================================================
class ZExtParsedEntParser {

//-----------------------------------
//	Initialization/Destruction
//
public:												
						ZExtParsedEntParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& name, const std::wstring& elementName, XXMLElementDeclaration* parent, uint32 level);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*				mParser;	
	XXMLLexer*				mLexer;
	uint32					mLevel;
	
	std::wstring			mName;
	std::wstring			mElementName;
	XXMLElementDeclaration*	mParent;
};


// ===================================================================================
//	class ZAttributeValueParser												AttValue
// ===================================================================================
class ZAttributeValueParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						~ZAttributeValueParser();

						ZAttributeValueParser(XXMLParser* parser, XXMLLexer* lexer);
						
protected:
						ZAttributeValueParser(XXMLParser* parser, const std::wstring& text, uint32 level);

private:
						ZAttributeValueParser(const ZAttributeValueParser& rhs);

			ZAttributeValueParser& operator=(const ZAttributeValueParser& rhs);

//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
	static	std::wstring ProcessWhiteSpace(const std::wstring& input);

	static	void 		Normalize(std::wstring& value);

	static	void 		Validate(const XXMLAttributeDeclaration& attribute, const std::wstring& value, XXMLParser* parser);
	
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;
	uint32		mLevel;				// lame recursion check
	XXMLLexer*	mLexer;

	TokenNum	mQuote;
	bool		mOwnsLexer;
};


// ===================================================================================
//	class ZProcessInstructionParser													PI
// ===================================================================================
class ZProcessInstructionParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZProcessInstructionParser(XXMLParser* parser, XXMLLexer* lexer);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZContentParser													content
// ===================================================================================
class ZContentParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZContentParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& elementName, XXMLElementDeclaration* parent, uint32 level = 0);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
protected:
			void 		DoParseCDSection();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*				mParser;	
	XXMLLexer*				mLexer;
	uint32					mLevel;
	
	std::wstring			mElementName;
	XXMLElementDeclaration*	mParent;
};


// ===================================================================================
//	class ZElementParser													element
// ===================================================================================
class ZElementParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZElementParser(XXMLParser* parser, XXMLLexer* lexer, XXMLElementDeclaration* parent);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
protected:
			void 		DoDefaultAttributes(const std::wstring& elementName, XXMLElementDeclaration* declaration);

			void 		DoParseAttribute(const std::wstring& elementName, XXMLElementDeclaration* declaration);
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*					mParser;	
	XXMLLexer*					mLexer;
	
	std::vector<std::wstring>	mAttributes;	
	XXMLElementDeclaration*		mParent;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

