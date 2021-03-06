/*
 *  File:		XXMLDTDParser.h
 *  Summary:	Classes used to parse DTD (declaration section) of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDTDParser.h,v $
 *		Revision 1.2  2000/11/10 04:46:41  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	 2/02/99	JDJ		Created
 */

#pragma once

#include <XLexerGrammar.h>
#include <XXMLDoc.h>
#include <XXMLDTD.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XResource;
class XURI;
class XXMLLexer;
class XXMLParser;


// ===================================================================================
//	class ZParameterReferenceParser										PEReference
// ===================================================================================
class ZParameterReferenceParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZParameterReferenceParser(XXMLParser* parser, XXMLLexer* lexer);

//-----------------------------------
//	API
//
public:
			bool 		IsExternal() const;
			
			const std::wstring& GetName() const							{return mName;}
			
			std::wstring Parse();

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*		mParser;	
	XXMLLexer*		mLexer;

	std::wstring	mName;
};


// ===================================================================================
//	class ZExtPEParser															extPE
// ===================================================================================
class ZExtPEParser {

//-----------------------------------
//	Initialization/Destruction
//
public:												
						ZExtPEParser(XXMLParser* parser, XXMLLexer* lexer, uint32 level);

						ZExtPEParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& name);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
			const std::wstring&	GetName() const						{return mName;}

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*		mParser;	
	XXMLLexer*		mLexer;
	uint32			mLevel;
	
	std::wstring	mName;
};


// ===================================================================================
//	class ZVersionInfoParser											VersionInfo
// ===================================================================================
class ZVersionInfoParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZVersionInfoParser(XXMLParser* parser, XXMLLexer* lexer);
						
//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZEncodingDeclarationParser									EncodingDecl
// ===================================================================================
class ZEncodingDeclarationParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZEncodingDeclarationParser(XXMLParser* parser, XXMLLexer* lexer);
						
//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZTextDeclarationParser											TextDecl
// ===================================================================================
class ZTextDeclarationParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZTextDeclarationParser(XXMLParser* parser, XXMLLexer* lexer);
						
//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZEntityValueParser											EntityValue
// ===================================================================================
class ZEntityValueParser {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~ZEntityValueParser();
						
						ZEntityValueParser(XXMLParser* parser, XXMLLexer* lexer);
						
protected:
						ZEntityValueParser(XXMLParser* parser, const std::wstring& text, const XURI& uri, uint32 level);

private:
						ZEntityValueParser(const ZEntityValueParser& rhs);

			ZEntityValueParser& operator=(const ZEntityValueParser& rhs);

//-----------------------------------
//	API
//
public:
			std::wstring Parse();

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;
	uint32		mLevel;				// lame recursion check
	TokenNum	mQuote;
	
	XXMLLexer*	mLexer;
	bool		mOwnsLexer;
};


// ===================================================================================
//	class ZCommentParser													Comment
// ===================================================================================
class ZCommentParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZCommentParser(XXMLParser* parser, XXMLLexer* lexer);

//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZPublicIDParser													PublicID
// ===================================================================================
class ZPublicIDParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZPublicIDParser(XXMLParser* parser, XXMLLexer* lexer);
						
//-----------------------------------
//	API
//
public:
			SExternalID Parse();
			
protected:
			bool 		IsPublicIDChar(wchar_t ch) const;

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZExternalIDParser													ExternalID
// ===================================================================================
class ZExternalIDParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZExternalIDParser(XXMLParser* parser, XXMLLexer* lexer);
						
//-----------------------------------
//	API
//
public:
			SExternalID Parse();

			std::wstring ParseSystemLiteral();

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZMarkupDeclarationParser											markupdecl
// ===================================================================================
class ZMarkupDeclarationParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZMarkupDeclarationParser(XXMLParser* parser, XXMLLexer* lexer);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
protected:
			void 		DoParseElementDeclaration();
			XXMLElementDeclaration* DoParseMixedElementDeclaration();
			XXMLElementDeclaration* DoParseChildrenElementDeclaration();
			void 		DoParseCP(XXMLElementDeclaration* element);
			
			void 		DoParseAttributeListDeclaration();
			void 		DoParseAttributeDeclaration(XXMLElementDeclaration* element);
			void 		DoParseNotationAttributeDeclaration(XXMLAttributeDeclaration& attribute);
			void 		DoParseEnumerationAttributeDeclaration(XXMLAttributeDeclaration& attribute);
			void 		DoValidateAttributeDeclaration(XXMLElementDeclaration* element, const XXMLAttributeDeclaration& attribute);
			
			void 		DoParseEntityDeclaration();
			void 		DoParseGeneralEntityDeclaration(const std::wstring& name);
			void 		DoParseParamEntityDeclaration(const std::wstring& name);
			
			void 		DoParseNotationDeclaration();
						
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
};


// ===================================================================================
//	class ZExternalDeclarationsParser									extSubsetDecl
// ===================================================================================
class ZExternalDeclarationsParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZExternalDeclarationsParser(XXMLParser* parser, XXMLLexer* lexer, uint32 level = 0);

//-----------------------------------
//	API
//
public:
			void 		Parse();
			
protected:
			void 		DoParseConditionalSection();
			
			void 		DoParseIgnoreSectionContents();

//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
	uint32		mLevel;				// lame recursion check
};


// ===================================================================================
//	class ZPrologParser															prolog
// ===================================================================================
class ZPrologParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZPrologParser(XXMLParser* parser, XXMLLexer* lexer, XResource* data, bool optimized);

//-----------------------------------
//	API
//
public:
			std::wstring Parse();
			
protected:
			std::wstring DoParseLanguage();
			
			bool 		DoParseStandAlone();
			
			std::wstring DoParseDocType();
			
//-----------------------------------
//	Member Data
//
protected:
	XXMLParser*	mParser;	
	XXMLLexer*	mLexer;
	
	bool		mOptimized;
	
	bool		mHasExternalSubSet;
	XResource*	mData;
};


// ===================================================================================
//	class ZMiscBlockParser														Misc*
// ===================================================================================
class ZMiscBlockParser {

//-----------------------------------
//	Initialization/Destruction
//
public:						
						ZMiscBlockParser(XXMLParser* parser, XXMLLexer* lexer);

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


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

