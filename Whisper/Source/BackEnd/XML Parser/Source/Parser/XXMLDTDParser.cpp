/*
 *  File:		XXMLDTDParser.cpp
 *  Summary:	Classes used to parse DTD (declaration section) of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDTDParser.cpp,v $
 *		Revision 1.3  2001/04/27 04:39:06  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/10 04:46:27  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <7>	 3/26/00	JDJ		Rewrote ZExternalDeclarationsParser::DoParseIgnoreSectionContents.
 *		 <6>	 3/26/00	JDJ		ZMarkupDeclarationParser::DoParseChildrenElementDeclaration handles
 *									'?>' at the end.
 *		 <5>	 3/26/00	JDJ		Fixed ZCommentParser::Parse so that "-->" is recognized when
 *									there's no white space before the token.
 *		 <4>	 3/26/00	JDJ		ZPublicIDParser::Parse correctly normalizes carriage returns and line feeds.
 *		 <3>	 3/22/00	JDJ		ZMarkupDeclarationParser::DoParseMixedElementDeclaration handles
 *									"(#PCDATA)*".
 *		 <2>	 8/01/99	JDJ		Instead of using "@External Subset" when throwing ZExtPEParser::Parse
 *									uses the actual entity name.
 *		 <1>	 2/02/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLDTDParser.h>

#include <XAutoPtr.h>
#include <XCursor.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XXMLContentParser.h>
#include <XXMLLexer.h>
#include <XXMLParser.h>

namespace Whisper {

 
// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// RecursionCheck
//
//---------------------------------------------------------------
static bool RecursionCheck(uint32 level)
{
	bool ok = true;
	
//	if (level > 0)
//		TRACE("level = %d, stack = %dK\n", level, StackSpace()/1024);

	if (level > 25)
		ok = false;
		
#if MAC
	else if (StackSpace() < 20*1024L)		// Mac memory management sucks and will normally blow the stack long before the above limit is reached...
		ok = false;
#endif

	return ok;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZParameterReferenceParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZParameterReferenceParser::ZParameterReferenceParser (XXMLParser*, XXMLLexer*)
//
// [69] PEReference ::= '%' Name ';' 		[VC: Entity Declared] 	ZParameterReferenceParser::Parse
//											[WFC: No Recursion] 	handled by our callers
//											[WFC: In DTD]			handled by the grammar
//
//---------------------------------------------------------------
ZParameterReferenceParser::ZParameterReferenceParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);

	mParser = parser;
	mLexer = lexer;

	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kPercentToken);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	mName = mLexer->GetText(token);	
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(mName[0]) && mName[0] != '_' && mName[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid parameter entity name (note that names start with a letter, underscore, or colon).", mName);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	if (token.num != kSemiColonToken)
		XXMLException::Throw(mLexer, "Expected ';' marking the end of a parameter entity reference (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZParameterReferenceParser::IsExternal
//
//---------------------------------------------------------------
bool ZParameterReferenceParser::IsExternal() const						
{
	const XXMLParser::SEntity& entity = mParser->GetParameterEntity(mName);

	return !entity.internal;
}


//---------------------------------------------------------------
//
// ZParameterReferenceParser::Parse
//
//---------------------------------------------------------------
std::wstring ZParameterReferenceParser::Parse()
{	
	const XXMLParser::SEntity& entity = mParser->GetParameterEntity(mName);	// VC: Entity Declared
	if (!entity.internal)					
		XXMLException::Throw(mLexer, "External parameter references are forbidden here."); 
			
	return entity.text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZExtPEParser															
// ===================================================================================

//---------------------------------------------------------------
//
// ZExtPEParser::ZExtPEParser (XXMLParser*, XXMLLexer*, uint32)
//
//---------------------------------------------------------------
ZExtPEParser::ZExtPEParser(XXMLParser* parser, XXMLLexer* lexer, uint32 level)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	mLevel = level;

	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kPercentToken);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	mName = mLexer->GetText(token);	
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(mName[0]) && mName[0] != '_' && mName[0] != ':'))
		XXMLException::Throw(mLexer,  "'#1' isn't a valid parameter entity name (note that names start with a letter, underscore, or colon).", mName);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	if (token.num != kSemiColonToken)
		XXMLException::Throw(mLexer, "Expected ';' marking the end of a parameter entity reference (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZExtPEParser::ZExtPEParser (XXMLParser*, XXMLLexer*, wstring)
//
//---------------------------------------------------------------
ZExtPEParser::ZExtPEParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& name) : mName(name)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	mLevel = 0;
}


//---------------------------------------------------------------
//
// ZExtPEParser::Parse
//
// [79] extPE ::= TextDecl? extSubsetDecl
//
//---------------------------------------------------------------
void ZExtPEParser::Parse()
{
	const XXMLParser::SEntity& entity = mParser->GetParameterEntity(mName);
	
	XAutoPtr<XXMLLexer> lexer;

	if (entity.internal) {
		lexer.Reset(new XXMLLexer(mParser, new XScanner(entity.text), XURI(L"entity://" + mName), mLexer->GetInternal()));
		lexer->ReadToken();
			
	} else {		
		if (entity.id.dataType.length() > 0)
			XXMLException::Throw(mLexer, "Unparsed entities cannot be included here."); 
	
		// Load the entity and create a lexer object
		XAutoPtr<XResource> data(entity.Load(mLexer->GetURI()));
		if (data.Get() == nil) {
			if (mName.length() > 0 && mName[0] == '@') {		// check for "@External Subset"
				if (entity.id.systemID.length() > 0)
					XXMLException::Throw(mLexer, "Couldn't find the external parameter entity named: '#1'", entity.id.systemID);
				else if (entity.id.publicID.length() > 0)
					XXMLException::Throw(mLexer, "Couldn't find the external parameter entity named: '#1'", entity.id.publicID);
			}
			
			XXMLException::Throw(mLexer, "Couldn't find the external parameter entity named: '#1'", mName);
		}
		
		lexer.Reset(mParser->CreateLexer(*data, false));
		lexer->ReadToken();
		
		// Parse the language encoding
		SXMLToken token = lexer->GetToken();
		if (token.num == kXMLTagToken) {
			ZTextDeclarationParser textParser(mParser, lexer.Get());
			std::wstring language = textParser.Parse();
			
			mParser->UpdateLexer(*data, lexer.Get(), language);

		} else
			mParser->UpdateLexer(*data, lexer.Get(), L"@default");
	}
		
	// Parse the declarations
	ZExternalDeclarationsParser decParser(mParser, lexer.Get(), mLevel);
	decParser.Parse();	

	if (!lexer->AtEnd())
		if (mName == L"@External Subset")
			XXMLException::Throw(mLexer, "The external subset doesn't match the extSubsetDecl production (found extraneous characters at the end).");			
		else
			XXMLException::Throw(mLexer, "'#1' doesn't match the extSubsetDecl production (found extraneous characters at the end).", mName);			
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZVersionInfoParser															
// ===================================================================================

//---------------------------------------------------------------
//
// ZVersionInfoParser::ZVersionInfoParser
//
//---------------------------------------------------------------
ZVersionInfoParser::ZVersionInfoParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZVersionInfoParser::Parse
//
// [24] VersionInfo ::= S 'version' Eq (' VersionNum ' | " VersionNum ")
//
//---------------------------------------------------------------
std::wstring ZVersionInfoParser::Parse()
{
	// 'version'
	SXMLToken token = mLexer->GetToken();		// our caller will have handled the whitespace
	ASSERT(token.num == kAlphaNumToken && mLexer->GetText(token) == L"version");
	mLexer->ReadToken();

	// Eq
	token = mLexer->GetToken();
	if (token.num != kEqualToken)
		XXMLException::Throw(mLexer, "Expected '=' following the 'version' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	// '"'
	token = mLexer->GetToken();
	if (token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken)
		XXMLException::Throw(mLexer, "The XML version number must be inside single or double quotes.");
	wchar_t quote = mLexer->GetChar(token);
	mLexer->ReadToken();
	
	// VersionNum
	token = mLexer->GetToken();
	std::wstring version = mLexer->GetText(token);
	if (token.num != kAlphaNumToken)
		XXMLException::Throw(mLexer, "Expected a version number (not '#1').", version);
		
	bool valid = true;
	for (uint32 index = 0; index < version.length() && valid; ++index) {
		wchar_t ch = version[index];
		valid = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '.' || ch == ':' || ch == '-';
	}
	
	if (!valid)
		XXMLException::Throw(mLexer, "'#1' isn't a valid version number.", version);
	mLexer->ReadToken();

	// '"'
	token = mLexer->GetToken();
	if ((token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken) || mLexer->GetChar(token) != quote)
		XXMLException::Throw(mLexer, "Expected a '#1' closing the version number (not '#2').", std::wstring(1, quote), mLexer->GetText(token));
	mLexer->ReadToken();
	
	// $$$ The XML spec leaves the structure of the version number completely
	// $$$ undefined. In particular, there's no discussion of major and minor
	// $$$ version numbers so it's difficult to assume that, for example, a
	// $$$ 1.1 document is parsable by a 1.0 engine.
	if (version != L"1.0")				
		XXMLException::Throw(mLexer, "Only XML 1.0 documented are supported.");
		
	return version;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZEncodingDeclarationParser															
// ===================================================================================

//---------------------------------------------------------------
//
// ZEncodingDeclarationParser::ZEncodingDeclarationParser
//
//---------------------------------------------------------------
ZEncodingDeclarationParser::ZEncodingDeclarationParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZEncodingDeclarationParser::Parse
//
// [80] EncodingDecl ::= S 'encoding' Eq ('"' EncName '"' | "'" EncName "'" ) 
//
// [81] EncName ::= [A-Za-z] ([A-Za-z0-9._] | '-')*
//
//---------------------------------------------------------------
std::wstring ZEncodingDeclarationParser::Parse()
{
	// 'encoding'
	SXMLToken token = mLexer->GetToken();			
	ASSERT(token.num == kAlphaNumToken && mLexer->GetText(token) == L"encoding");	// whitespace is checked by our caller
	mLexer->ReadToken();

	// Eq
	token = mLexer->GetToken();
	if (token.num != kEqualToken)
		XXMLException::Throw(mLexer, "Expected '=' following the 'encoding' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	// '"'
	token = mLexer->GetToken();
	if (token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken)
		XXMLException::Throw(mLexer, "The XML language encoding must be inside single or double quotes.");
	wchar_t quote = mLexer->GetChar(token);
	mLexer->ReadToken();
	
	// EncName
	token = mLexer->GetToken();
	std::wstring language = mLexer->GetText(token);

	if (token.num != kAlphaNumToken)
		XXMLException::Throw(mLexer, "Expected a language encoding (not '#1').", language);
		
	wchar_t ch = language[0];
	bool valid = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	for (uint32 index = 1; index < language.length() && valid; ++index) {
		ch = language[index];
		valid = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '.' || ch == '_' || ch == '-';
	}
	
	if (!valid)
		XXMLException::Throw(mLexer, "'#1' isn't a valid language encoding.", language);
	language = ConvertToLowerCase(language);
	mLexer->ReadToken();

	// '"'
	token = mLexer->GetToken();
	if ((token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken) || mLexer->GetChar(token) != quote)
		XXMLException::Throw(mLexer, "Expected a '#1' closing the language encoding (not '#2').", std::wstring(1, quote), mLexer->GetText(token));
	mLexer->ReadToken();
	
	return language;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZTextDeclarationParser															
// ===================================================================================

//---------------------------------------------------------------
//
// ZTextDeclarationParser::ZTextDeclarationParser
//
//---------------------------------------------------------------
ZTextDeclarationParser::ZTextDeclarationParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZTextDeclarationParser::Parse
//
// [77] TextDecl ::= '<?xml' VersionInfo? EncodingDecl S? '?>'
//
//---------------------------------------------------------------
std::wstring ZTextDeclarationParser::Parse()
{
	// '<?xml'
	SXMLToken token = mLexer->GetToken();
	ASSERT(mLexer->GetToken().num == kXMLTagToken);
	mLexer->ReadToken();
	
	// VersionInfo?
	SXMLToken whiteToken = mLexer->GetToken();
	if (whiteToken.num == kWhiteSpaceToken)
		mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kAlphaNumToken && mLexer->GetText(token) == L"version") {
		if (whiteToken.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the '<?xml' keyword.");

		ZVersionInfoParser versionParser(mParser, mLexer);
		versionParser.Parse();

		whiteToken = mLexer->GetToken();
		if (whiteToken.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace before the 'encoding' keyword).");
		mLexer->ReadToken();
	}

	// EncodingDecl
	token = mLexer->GetToken();
	if (token.num != kAlphaNumToken || mLexer->GetText(token) != L"encoding") 
		XXMLException::Throw(mLexer, "Expected the 'encoding' keyword (not '#1').", mLexer->GetText(token));
	
	ZEncodingDeclarationParser encodingParser(mParser, mLexer);
	std::wstring language = encodingParser.Parse();

	// S? '?>'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	}

	if (token.num != kPIEndToken)
		XXMLException::Throw(mLexer, "Expected '?>' marking the end of the XML version tag (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	return language;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZEntityValueParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZEntityValueParser::~ZEntityValueParser
//
//---------------------------------------------------------------
ZEntityValueParser::~ZEntityValueParser()
{
	if (mOwnsLexer)
		delete mLexer;

	mLexer = nil;
	mParser = nil;
}


//---------------------------------------------------------------
//
// ZEntityValueParser::ZEntityValueParser (XXMLParser*, wstring, XURI, uint32)
//
//---------------------------------------------------------------
ZEntityValueParser::ZEntityValueParser(XXMLParser* parser, const std::wstring& text, const XURI& uri, uint32 level)
{
	PRECONDITION(parser != nil);	
	
	mParser = parser;
	mLevel = level;
	mLexer = new XXMLLexer(parser, new XScanner(text), uri, parser->GetLexer()->GetInternal());
	mOwnsLexer = true;

	mLexer->ExpandInternalParameterReferences(false);
	mLexer->ReadToken();					
	mLexer->ExpandInternalParameterReferences(true);
	
	mQuote = LONG_MAX;			// we'll consume the entire string
}


//---------------------------------------------------------------
//
// ZEntityValueParser::ZEntityValueParser (XXMLParser*, XXMLLexer*)
//
//---------------------------------------------------------------
ZEntityValueParser::ZEntityValueParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLevel = 0;
	mLexer = lexer;
	mOwnsLexer = false;

	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kDoubleQuoteToken || token.num == kSingleQuoteToken);

	mLexer->ExpandInternalParameterReferences(false);
	mLexer->ReadToken();					
	mLexer->ExpandInternalParameterReferences(true);
		
	mQuote = token.num;
}


//---------------------------------------------------------------
//
// ZEntityValueParser::Parse
//
// [9] EntityValue ::= '"' ([^%&"] | PEReference | Reference)* '"' |� "'" ([^%&'] | PEReference | Reference)* "'"
//
//---------------------------------------------------------------
std::wstring ZEntityValueParser::Parse()
{
	std::wstring text;
	
	mLexer->ExpandInternalParameterReferences(false);

	SXMLToken token = mLexer->GetToken();
	while (token.num != mQuote && token.num != kNoToken) {
		wchar_t ch = mLexer->GetChar(token);

		switch (token.num) {
			case kAmpersandToken:
				{
				ZGeneralReferenceParser genRefParser(mParser, mLexer);
				if (genRefParser.IsCharacter())
					text += genRefParser.Parse();					// included 
				else
					text += L"&" + genRefParser.GetName() + L";";	// bypassed			
				}
				break;
				
			case kPercentToken:
				{
				std::wstring literal;
			
				if (mLexer->GetInternal())
					XXMLException::Throw(mLexer, "Parameter entities only be referenced inside markup within the external DTD.");
				
				XURI uri;
				ZParameterReferenceParser paramRefParser(mParser, mLexer);
				if (paramRefParser.IsExternal()) {
					const XXMLParser::SEntity& entity = mParser->GetParameterEntity(paramRefParser.GetName());
					ASSERT(entity.id.dataType.length() == 0);		// parameter entities cannot be unparsed
			
					XAutoPtr<XResource> data(entity.Load(mLexer->GetURI()));
					if (data.Get() == nil)
						XXMLException::Throw(mLexer, "Couldn't find the external parameter entity named: '#1'", paramRefParser.GetName());
				
					XAutoPtr<XXMLLexer> temp(mParser->CreateLexer(*data, false));		// $$ yuck
					literal = temp->GetScanner()->GetText();		// included as literal
					uri = data->GetURI();
									
				} else {
					literal = paramRefParser.Parse();				// included as literal
					uri = XURI(L"entity://" + paramRefParser.GetName());
				}
				
				if (!RecursionCheck(mLevel))
					XXMLException::Throw(mLexer, "'#1' has a recursive definition.", paramRefParser.GetName());
				
				int32 count1 = std::count(literal.begin(), literal.end(), '<');	// VC: Proper PE Nesting
				int32 count2 = std::count(literal.begin(), literal.end(), '>');	// $$ may not be sufficient...
				if (count1 != count2)
					XXMLException::Throw(mLexer, "'#1' has mismatched '<' and '>'.", paramRefParser.GetName());	
			
				count1 = std::count(literal.begin(), literal.end(), '(');
				count2 = std::count(literal.begin(), literal.end(), ')');
				if (count1 != count2)
					XXMLException::Throw(mLexer, "'#1' has mismatched '(' and ')'.", paramRefParser.GetName());	

				ZEntityValueParser entParser(mParser, literal, uri, mLevel + 1);
				text += entParser.Parse();
				}
				break;
				
			case kDoubleQuoteToken:
			case kSingleQuoteToken:
				text += ch;
				mLexer->ReadToken();					
				break;
				
			default:
#if 0
				std::wstring miscText = mLexer->GetText(token);
				for (uint32 index = 0; index < miscText.length(); ++index) {
					wchar_t& c = miscText[index];
					
					if (c == 0x20 || c == 0x09 || c == 0x0A)	
						c = ' ';
					else if (!XXMLLexer::IsChar(c))
						XXMLException::Throw(mLexer, "The entity reference doesn't match the EntityValue production.");
				}

				text += miscText;
#else
				mLexer->AppendText(text, token);
#endif
				mLexer->ReadToken();					
		}

		token = mLexer->GetToken();
	}

	if (token.num == mQuote)
		mLexer->ReadToken();					
	else if (mQuote != LONG_MAX)
		XXMLException::Throw(mLexer, "Couldn't find the closing quote.");
	
	mLexer->ExpandInternalParameterReferences(true);

	if (mOwnsLexer && !mLexer->AtEnd())
		XXMLException::Throw(mLexer, "Found extraneous characters at the end of the entity.");			

	return text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZCommentParser											
// ===================================================================================

//---------------------------------------------------------------
//
// ZCommentParser::ZCommentParser
//
//---------------------------------------------------------------
ZCommentParser::ZCommentParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZCommentParser::Parse
//
// [15] Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
//
//---------------------------------------------------------------
std::wstring ZCommentParser::Parse()
{
	std::wstring contents;
	
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kCommentStartToken);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	while (token.num != kCommentEndToken && token.num != kNoToken) {
		std::wstring text = mLexer->GetText(token);

		uint32 len = text.length();
		if (len >= 2 && text[len-2] == '-' && text[len-1] == '-') {
			if (len >= 3 && text[len-3] == '-') 
				XXMLException::Throw(mLexer, "Comments can't end with a '-'.");

			contents += text.substr(0, len-2);
			
			mLexer->ReadToken();
			token = mLexer->GetToken();
			if (token.num == kGreaterToken)
				token.num = kCommentEndToken;
			else
				XXMLException::Throw(mLexer, "'--' cannot be used within a comment.");
		
		} else if (text.find(L"--") != std::wstring::npos) {
			XXMLException::Throw(mLexer, "'--' cannot be used within a comment.");
		
		} else {
			contents += text;
		
			mLexer->ReadToken();
			token = mLexer->GetToken();
		}
	}
	
	if (token.num != kCommentEndToken)
		XXMLException::Throw(mLexer, "Expected '-->' marking the end of a comment (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	return contents;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZPublicIDParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZPublicIDParser::ZPublicIDParser 
//
//---------------------------------------------------------------
ZPublicIDParser::ZPublicIDParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZPublicIDParser::Parse
//
// [83] PublicID ::= 'PUBLIC' S PubidLiteral 
//
// [12] PubidLiteral ::= '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
//
//---------------------------------------------------------------
SExternalID ZPublicIDParser::Parse()
{
	SExternalID id;
	
	// 'PUBLIC' S  
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kAlphaNumToken && mLexer->GetText(token) == L"PUBLIC");
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the 'PUBLIC' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();	
	
	// '"' 
	SXMLToken quote = mLexer->GetToken();
	if (quote.num != kDoubleQuoteToken && quote.num != kSingleQuoteToken)
		XXMLException::Throw(mLexer, "Public literals must be in single or double quotes.");
	mLexer->ReadToken();					

	// PubidChar*
	token = mLexer->GetToken();
	while (token.num != quote.num && token.num != kNoToken) {
		std::wstring text = mLexer->GetText(token);
		
		for (uint32 index = 0; index < text.length(); ++index) {
			wchar_t ch = text[index];
			
			if (!this->IsPublicIDChar(ch)) 
				XXMLException::Throw(mLexer, "'#1' isn't a valid public id character.", std::wstring(1, ch));
		}
				
		id.publicID += text;

		mLexer->ReadToken();					
		token = mLexer->GetToken();
	}

	// '"'
	if (token.num != quote.num)
		XXMLException::Throw(mLexer, "Couldn't find the closing public id quote.");
	mLexer->ReadToken();					

	id.publicID = ZAttributeValueParser::ProcessWhiteSpace(id.publicID);
	ZAttributeValueParser::Normalize(id.publicID);						// note that system id's aren't normalized...

	return id;
}


//---------------------------------------------------------------
//
// IsPublicIDChar
//
// [13] PubidChar ::= #x20 | #xD | #xA |�[a-zA-Z0-9] |�[-'()+,./:=?;!*#@$_%] 
//
//---------------------------------------------------------------
bool ZPublicIDParser::IsPublicIDChar(wchar_t ch) const
{
	bool is = ch == 0x20 || ch == 0xD || ch == 0xA || 
	
			  (ch >= 'a' && ch <= 'z') ||
			  (ch >= 'A' && ch <= 'Z') ||
			  (ch >= '0' && ch <= '9') ||

			  ch == '-' || ch == '\'' || ch == '(' || ch == ')' || ch == '+' ||
			  ch == ',' || ch == '.' || ch == '/' || ch == ':' || ch == '=' ||
			  ch == '?' || ch == ';' || ch == '!' || ch == '*' || ch == '#' || 
			  ch == '@' || ch == '$' || ch == '_' || ch == '%';								
	
	return is;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZExternalIDParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZExternalIDParser::ZExternalIDParser 
//
//---------------------------------------------------------------
ZExternalIDParser::ZExternalIDParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZExternalIDParser::Parse
//
// [75] ExternalID ::= 'SYSTEM' S SystemLiteral | 'PUBLIC' S PubidLiteral S SystemLiteral 
//
//---------------------------------------------------------------
SExternalID ZExternalIDParser::Parse()
{
	SExternalID id;
	
	// 'SYSTEM' S
	SXMLToken token = mLexer->GetToken();
	std::wstring text = mLexer->GetText(token);
	
	if (token.num == kAlphaNumToken && text == L"SYSTEM") {
		mLexer->ReadToken();
	
		token = mLexer->GetToken();
		if (token.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the 'SYSTEM' keyword (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();	

	// 'PUBLIC' S PubidLiteral S  
	} else if (token.num == kAlphaNumToken && text == L"PUBLIC") {
		ZPublicIDParser pubParser(mParser, mLexer);
		id = pubParser.Parse();
	
		token = mLexer->GetToken();
		if (token.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the public literal (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();	
	
	} else
		XXMLException::Throw(mLexer, "Expected 'SYSTEM' or 'PUBLIC' in the entity declaration (not '#1').", text);
	
	// SystemLiteral
	id.systemID = this->ParseSystemLiteral();

	return id;
}


//---------------------------------------------------------------
//
// ZExternalIDParser::ParseSystemLiteral
//
// [11] SystemLiteral ::= ('"' [^"]* '"') |�("'" [^']* "'") 
//
//---------------------------------------------------------------
std::wstring ZExternalIDParser::ParseSystemLiteral()
{
	std::wstring text;
	
	// '"' [^"]* '"' 
	SXMLToken quote = mLexer->GetToken();
	if (quote.num != kDoubleQuoteToken && quote.num != kSingleQuoteToken)
		XXMLException::Throw(mLexer, "System literals must be in single or double quotes.");
	mLexer->ReadToken();					

	SXMLToken token = mLexer->GetToken();
	while (token.num != quote.num && token.num != kNoToken) {		
		mLexer->AppendText(text, token);

		mLexer->ReadToken();					
		token = mLexer->GetToken();
	}

	if (token.num != quote.num)
		XXMLException::Throw(mLexer, "Couldn't find the closing system id quote.");
	mLexer->ReadToken();					

	return text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZMarkupDeclarationParser											
// ===================================================================================

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::ZMarkupDeclarationParser
//
//---------------------------------------------------------------
ZMarkupDeclarationParser::ZMarkupDeclarationParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::Parse
//
// [29] markupdecl ::= elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment 	[VC: Proper Declaration/PE Nesting] XXMLLexer::DoExpandPE
//																								[WFC: PEs in Internal Subset]		handled using ExpandInternalParameterReferences
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::Parse()
{
#if MAC
	PRECONDITION(StackSpace() > 10*1024L);		// default stack space on Macs is only 64K which isn't always enough for the parser...
#endif

	XCursorUtils::StillBusy();

	SXMLToken token = mLexer->GetToken();
	switch (token.num) {
		case kElementTagToken:
			this->DoParseElementDeclaration();
			break;
		
		case kAttListTagToken:
			this->DoParseAttributeListDeclaration();
			break;
		
		case kEntityTagToken:
			this->DoParseEntityDeclaration();
			break;
		
		case kNotationTagToken:
			this->DoParseNotationDeclaration();
			break;
		
		case kPIStartToken:
			{
			ZProcessInstructionParser piParser(mParser, mLexer);
			piParser.Parse();
			}
			break;
		
		case kCommentStartToken:
			{
			ZCommentParser commentParser(mParser, mLexer);
			(void) commentParser.Parse();
			}
			break;
			
		default:
			XXMLException::Throw(mLexer, "Expected '<!ELEMENT', '<!ATTLIST', '<!ENTITY', '<!NOTATION', '<?', or '<!--' tags (not '#1').", mLexer->GetText(token));
	}
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseElementDeclaration
//
// [45] elementdecl ::= '<!ELEMENT' S Name S contentspec S? '>' [VC: Unique Element Type Declaration] 	see below
//
// [46] contentspec ::= 'EMPTY' | 'ANY' | Mixed | children 		
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseElementDeclaration()
{
	mLexer->ExpandInternalParameterReferences();

	// '<!ELEMENT' S
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kElementTagToken);	
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<!ELEMENT' tag (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	// Name S
	token = mLexer->GetToken();
	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid element name (note that names must start with a letter, underscore, or colon).", name);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the element name (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// 'EMPTY' | 'ANY' | Mixed | children 
	token = mLexer->GetToken();

	XAutoPtr<XXMLElementDeclaration> element;
	if (token.num == kAlphaNumToken) {
		std::wstring text = mLexer->GetText(token);
		
		if (text == L"EMPTY") {
			element.Reset(new XXMLElementDeclaration(mParser->GetDTD().Get(), kEmptyElement));
			element->SetExternal(!mLexer->GetInternal());
			mLexer->ReadToken();
			
		} else if (text == L"ANY") {
			element.Reset(new XXMLElementDeclaration(mParser->GetDTD().Get(), kAnyElement));
			element->SetExternal(!mLexer->GetInternal());
			mLexer->ReadToken();

		} else
			XXMLException::Throw(mLexer, "Expected 'EMPTY', 'ANY', or '(' after the element name (not '#1').", text);
			
	} else if (token.num == kLeftParensToken) {
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		}
		
		if (token.num == kPCDataToken)
			element.Reset(this->DoParseMixedElementDeclaration());
		else
			element.Reset(this->DoParseChildrenElementDeclaration());

	} else
		XXMLException::Throw(mLexer, "Expected 'EMPTY', 'ANY', or '(' after the element name (not '#1').", mLexer->GetText(token));

	bool added = mParser->GetDTD()->AddElement(name, element.Release());
	if (!added)													// VC: Unique Element Type Declaration
		XXMLException::Throw(mLexer, "Element '#1' has already been declared.", name);

	// S? '>'
	token = mLexer->GetToken();
	while (token.num == kWhiteSpaceToken) {			// need to use a loop because DoParseChildrenElementDeclaration uses ReadFixedToken
		mLexer->ReadToken();
		token = mLexer->GetToken();
	}
	
	mLexer->ExpandInternalParameterReferences(false);

	if (token.num != kGreaterToken)
		XXMLException::Throw(mLexer, "Expected '>' marking the end of an element declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseMixedElementDeclaration
//
// [51] Mixed ::= '(' S? '#PCDATA' (S? '|' S? Name)* S? ')*' | '(' S? '#PCDATA' S? ')' 	[VC: Proper Group/PE Nesting] 	XXMLLexer::DoExpandPE
//																						[VC: No Duplicate Types] 		see below
//
//---------------------------------------------------------------
XXMLElementDeclaration* ZMarkupDeclarationParser::DoParseMixedElementDeclaration()
{
	XAutoPtr<XXMLElementDeclaration> element(new XXMLElementDeclaration(mParser->GetDTD().Get(), kMixedElement));
	element->SetExternal(!mLexer->GetInternal());
	
	// '#PCDATA'
	SXMLToken token = mLexer->GetToken();			// our caller will have parsed the '('
	ASSERT(token.num == kPCDataToken);	
	mLexer->ReadToken();

	// S?
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 
	
	// (S? '|' S? Name)* S? ')*'
	if (token.num == kBarToken) {
		while (token.num == kBarToken) {
			mLexer->ReadToken();
			
			token = mLexer->GetToken();
			if (token.num == kWhiteSpaceToken) {
				mLexer->ReadToken();
				token = mLexer->GetToken();
			} 
	
			std::wstring name = mLexer->GetText(token);
			if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
				XXMLException::Throw(mLexer, "'#1' isn't a valid element name (note that names start with a letter, underscore, or colon).", name);
			element->AddMixedElement(name);			// VC: No Duplicate Types
			mLexer->ReadToken();

			token = mLexer->GetToken();
			if (token.num == kWhiteSpaceToken) {
				mLexer->ReadToken();
				token = mLexer->GetToken();
			} 
		}
	
		if (token.num != kRightParensToken)
			XXMLException::Throw(mLexer, "Expected ')*' marking the end of a mixed element declaration (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	
		token = mLexer->GetToken();
		if (token.num != kStarToken)
			XXMLException::Throw(mLexer, "Expected ')*' marking the end of a mixed element declaration (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();

	// ')' '*'?
	} else {
		if (token.num != kRightParensToken)
			XXMLException::Throw(mLexer, "#PCDATA must be followed with either '|' or ')' (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	
		token = mLexer->GetToken();
		if (token.num == kStarToken)
			mLexer->ReadToken();
	}
	
	return element.Release();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseChildrenElementDeclaration
//
// [47] children ::= (choice | seq) ('?' | '*' | '+')? 
//
// [49] choice ::= '(' S? cp ( S? '|' S? cp )* S? ')' 		[vc: Proper Group/PE Nesting] 	XXMLLexer::DoExpandPE
//
// [50] seq ::= '(' S? cp ( S? ',' S? cp )* S? ')' 			[vc: Proper Group/PE Nesting] 	XXMLLexer::DoExpandPE
//
//---------------------------------------------------------------
XXMLElementDeclaration* ZMarkupDeclarationParser::DoParseChildrenElementDeclaration()
{
	XAutoPtr<XXMLElementDeclaration> element(new XXMLElementDeclaration(mParser->GetDTD().Get(), kChildrenElement));
	element->SetExternal(!mLexer->GetInternal());
	
	// '(' S?
	element->AppendChildSymbol('(');			// our caller will have parsed the first '('
	
	SXMLToken token = mLexer->GetToken();			
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();			
	}
	
	// cp S?
	this->DoParseCP(element.Get());
	token = mLexer->GetToken();			
	
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();			
	}
	
	// ('|' S? cp S?)* | (',' S? cp S?)*
	if (token.num == kBarToken || token.num == kCommaToken) {
		SXMLToken sep = token;
		while (token.num == sep.num) {
			if (sep.num == kBarToken)
				element->AppendChildSymbol(L'|');
	
			mLexer->ReadToken();
			token = mLexer->GetToken();			
	
			if (token.num == kWhiteSpaceToken) {
				mLexer->ReadToken();
				token = mLexer->GetToken();			
			}
			
			this->DoParseCP(element.Get());
			token = mLexer->GetToken();			
	
			if (token.num == kWhiteSpaceToken) {
				mLexer->ReadToken();
				token = mLexer->GetToken();			
			}
		}
	}
	
	// S? ')'
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();			
	}
	
	if (token.num != kRightParensToken) 
		XXMLException::Throw(mLexer, "Expected a ')' closing the children element declaration (not '#1').", mLexer->GetText(token));
	element->AppendChildSymbol(L')');
	mLexer->ReadFixedToken(1);				// don't want to get '?>' here
	token = mLexer->GetToken();			
	
	// ('?' | '*' | '+')? 
	if (token.num == kQuestionToken || token.num == kStarToken || token.num == kPlusToken) {
		element->AppendChildSymbol(mLexer->GetChar(token));
		mLexer->ReadToken();
	}
	
	if (token.num == kCharToken) {			// ReadFixedToken(1) returns kCharToken instead of kWhiteSpaceToken...
		wchar_t ch = mLexer->GetChar(token);
		if (ch == kSpaceChar || ch == kTabChar || ch == kReturnChar || ch == kLineFeedChar)
			mLexer->ReadToken();
	}
			
	return element.Release();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseCP
//
// [48] cp ::= (Name | choice | seq) ('?' | '*' | '+')? 
//
// [49] choice ::= '(' S? cp ( S? '|' S? cp )* S? ')' 		[vc: Proper Group/PE Nesting] 	XXMLLexer::DoExpandPE
//
// [50] seq ::= '(' S? cp ( S? ',' S? cp )* S? ')' 			[vc: Proper Group/PE Nesting] 	XXMLLexer::DoExpandPE
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseCP(XXMLElementDeclaration* element)
{
	PRECONDITION(element != nil);
		
	// '(' S?
	SXMLToken token = mLexer->GetToken();			
	if (token.num == kLeftParensToken) {
		element->AppendChildSymbol('(');			
		
		mLexer->ReadToken();
		token = mLexer->GetToken();			
		
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();			
		}
		
		// cp S?
		this->DoParseCP(element);
		token = mLexer->GetToken();			
		
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();			
		}
		
		// ('|' S? cp S?)* | (',' S? cp S?)*
		if (token.num == kBarToken || token.num == kCommaToken) {
			SXMLToken sep = token;
			while (token.num == sep.num) {
				if (sep.num == kBarToken)
					element->AppendChildSymbol(L'|');
				
				mLexer->ReadToken();
				token = mLexer->GetToken();			
		
				if (token.num == kWhiteSpaceToken) {
					mLexer->ReadToken();
					token = mLexer->GetToken();			
				}
				
				this->DoParseCP(element);
				token = mLexer->GetToken();			
		
				if (token.num == kWhiteSpaceToken) {
					mLexer->ReadToken();
					token = mLexer->GetToken();			
				}
			}
		}
		
		// S? ')'
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();			
		}
	
		if (token.num != kRightParensToken) 
			XXMLException::Throw(mLexer, "Expected a ')' closing the children element declaration (not '#1').", mLexer->GetText(token));
		element->AppendChildSymbol(L')');
		mLexer->ReadToken();
		token = mLexer->GetToken();			
	
	// Name
	} else {
		std::wstring name = mLexer->GetText(token);
		
		if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
			XXMLException::Throw(mLexer, "Expected a child element name or a '(' (not '#1').", name);
		element->AppendChildElement(name);
		
		mLexer->ReadToken();
		token = mLexer->GetToken();			
	}
	
	// ('?' | '*' | '+')? 
	if (token.num == kQuestionToken || token.num == kStarToken || token.num == kPlusToken) {
		element->AppendChildSymbol(mLexer->GetChar(token));
		mLexer->ReadToken();
	}
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseAttributeListDeclaration
//
// [52] AttlistDecl ::= '<!ATTLIST' S Name AttDef* S? '>' 
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseAttributeListDeclaration()
{
	mLexer->ExpandInternalParameterReferences();

	// '<!ATTLIST' S
	SXMLToken token = mLexer->GetToken();			
	ASSERT(token.num == kAttListTagToken);	
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<!ATTLIST' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// Name
	token = mLexer->GetToken();
	std::wstring elementName = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(elementName[0]) && elementName[0] != '_' && elementName[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid element name (note that names start with a letter, underscore, or colon).", elementName);
	mLexer->ReadToken();

	XXMLElementDeclaration* element = mParser->GetDTD()->GetElement(elementName);
	
	// AttDef* S? 
	while (true) {
		token = mLexer->GetToken();

		if (token.num == kWhiteSpaceToken)
			mLexer->ReadToken();			
		else if (token.num == kGreaterToken)
			break;			
		else
			this->DoParseAttributeDeclaration(element);
	}
	
	mLexer->ExpandInternalParameterReferences(false);

	// '>' 
	token = mLexer->GetToken();
	if (token.num != kGreaterToken)
		XXMLException::Throw(mLexer, "Expected '>' marking the end of the declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseAttributeDeclaration
//
// [53] AttDef ::= S Name S AttType S DefaultDecl 
//
// [54] AttType ::= StringType | TokenizedType | EnumeratedType  
//
// [60] DefaultDecl ::= '#REQUIRED' |�'#IMPLIED' | (('#FIXED' S)? AttValue) [VC: Required Attribute] 		ZElementParser::DoValidateAttributes
//																			[VC: Attribute Default Legal] 	ZAttributeValueParser::Validate
//																			[WFC: No < in Attribute Values] ZAttributeValueParser::Parse
//																			[VC: Fixed Attribute Default]	ZElementParser::DoParseAttribute
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseAttributeDeclaration(XXMLElementDeclaration* element)
{
	PRECONDITION(element != nil);
	
	// Name S
	SXMLToken token = mLexer->GetToken();				// our caller will have parsed the S
	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid attribute name (note that names start with a letter, underscore, or colon).", name);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after attribute name (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// StringType | TokenizedType | EnumeratedType 
	XXMLAttributeDeclaration attribute;
	attribute.SetExternal(!mLexer->GetInternal());

	token = mLexer->GetToken();
	if (token.num == kAlphaNumToken) {
		std::wstring text = mLexer->GetText(token);

		if (text == L"CDATA") {
			attribute.SetType(kStringAttribute);
			mLexer->ReadToken();

		} else if (text == L"ID") {
			attribute.SetType(kIDAttribute);
			mLexer->ReadToken();

			if (element->HasIDAttribute())
				XXMLException::Throw(mLexer, "The element already has an ID attribute.");

		} else if (text == L"IDREF") {
			attribute.SetType(kIDRefAttribute);
			mLexer->ReadToken();

		} else if (text == L"IDREFS") {
			attribute.SetType(kIDRefsAttribute);
			mLexer->ReadToken();

		} else if (text == L"ENTITY") {
			attribute.SetType(kEntityAttribute);
			mLexer->ReadToken();

		} else if (text == L"ENTITIES") {
			attribute.SetType(kEntitiesAttribute);
			mLexer->ReadToken();

		} else if (text == L"NMTOKEN") {
			attribute.SetType(kTokenAttribute);
			mLexer->ReadToken();

		} else if (text == L"NMTOKENS") {
			attribute.SetType(kTokensAttribute);
			mLexer->ReadToken();
		
		} else if (text == L"NOTATION") {
			XXMLElementDeclaration::iterator iter = element->begin();	// VC: One Notation per Element Type: No element type may have more than one NOTATION attribute specified (from the XML 1.0 errata)
			while (iter != element->end()) {
				const XXMLAttributeDeclaration& a = iter->second;
				++iter;
				
				if (a.GetType() == kNotationAttribute)
					XXMLException::Throw(mLexer, "The element already has a NOTATION attribute defined.");
			}
			
			this->DoParseNotationAttributeDeclaration(attribute);

		} else
			XXMLException::Throw(mLexer, "Expected 'CDATA', 'ID', 'IDREF', 'IDREFS', 'ENTITY', 'ENTITIES', 'NMTOKEN', 'NMTOKENS', 'NOTATION', or an enumerated list (not '#1').", text);
	
	} else if (token.num == kLeftParensToken)
		this->DoParseEnumerationAttributeDeclaration(attribute);
	
	else
		XXMLException::Throw(mLexer, "Expected 'CDATA', 'ID', 'IDREF', 'IDREFS', 'ENTITY', 'ENTITIES', 'NMTOKEN', 'NMTOKENS', 'NOTATION', or an enumerated list (not '#1').", mLexer->GetText(token));
	
	// S  
	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the attribute type (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// '#REQUIRED'
	token = mLexer->GetToken();
	if (token.num == kRequiredToken) {
		attribute.SetDefaultType(kRequiredAttribute);
		mLexer->ReadToken();
	
	// '#IMPLIED'
	} else if (token.num == kImpliedToken) {
		attribute.SetDefaultType(kImpliedAttribute);
		mLexer->ReadToken();
	
	// ('#FIXED' S)? AttValue
	} else {
		if (token.num == kFixedToken) {
			attribute.SetDefaultType(kFixedAttribute);
			mLexer->ReadToken();

			token = mLexer->GetToken();
			if (token.num != kWhiteSpaceToken)
				XXMLException::Throw(mLexer, "Expected whitespace after the '#FIXED' keyword (not '#1').", mLexer->GetText(token));
			mLexer->ReadToken();

		} else
			attribute.SetDefaultType(kDefaultAttribute);

		token = mLexer->GetToken();
		if (token.num == kDoubleQuoteToken || token.num == kSingleQuoteToken) {
			ZAttributeValueParser valueParser(mParser, mLexer);			
			std::wstring value = valueParser.Parse();
				
			if (attribute.GetType() != kStringAttribute) 		// XML 1.0 errata says that attributes must be normalized before the validty constraints are checked	
				ZAttributeValueParser::Normalize(value);

			attribute.SetDefaultValue(value);
	
		} else
			XXMLException::Throw(mLexer, "Expected '#REQUIRED', '#IMPLIED', '#FIXED', or a default attribute value in quotes (not '#1').", mLexer->GetText(token));
	}
	
	this->DoValidateAttributeDeclaration(element, attribute);

	if (!element->AddAttribute(name, attribute))
		mParser->DoWarning(LoadWhisperString(L"Attribute '#1' has already been declared.", name));
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseNotationAttributeDeclaration
//
// [58] NotationType ::= 'NOTATION' S '(' S? Name (S? '|' Name)* S? ')' [VC: Notation Attributes] 	ZAttributeValueParser::Validate
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseNotationAttributeDeclaration(XXMLAttributeDeclaration& attribute)
{
	attribute.SetType(kNotationAttribute);

	// 'NOTATION' S
	SXMLToken token = mLexer->GetToken();			
	ASSERT(token.num == kAlphaNumToken && mLexer->GetText(token) == L"NOTATION");	
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the 'NOTATION' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// '(' S? Name 
	token = mLexer->GetToken();
	if (token.num != kLeftParensToken)
		XXMLException::Throw(mLexer, "Expected '(' after the 'NOTATION' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
		XXMLException::Throw(mLexer, "#1 isn't a valid notation name (note that names start with a letter, underscore, or colon).", name);
	attribute.AddEnum(name);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	// (S? '|' Name)*
	while (token.num == kBarToken) {
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 
		
		name = mLexer->GetText(token);
		if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
			XXMLException::Throw(mLexer, "'#1' isn't a valid notation name (note that names start with a letter, underscore, or colon).", name);
		attribute.AddEnum(name);
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 
	}
	
	// S? ')'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	if (token.num != kRightParensToken)
		XXMLException::Throw(mLexer, "Expected ')' marking the end of the 'NOTATION' declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseEnumerationAttributeDeclaration
//
// [59] Enumeration ::= '(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')' 		[VC: Enumeration]  	ZAttributeValueParser::Validate
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseEnumerationAttributeDeclaration(XXMLAttributeDeclaration& attribute)
{
	attribute.SetType(kEnumAttribute);

	// '(' S? Nmtoken 
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kLeftParensToken);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken)
		XXMLException::Throw(mLexer, "'#1' isn't a valid enum name.", name);
	attribute.AddEnum(name);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	// (S? '|' S? Nmtoken)*
	while (token.num == kBarToken) {
		mLexer->ReadToken();
		
		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 

		name = mLexer->GetText(token);
		if (token.num != kAlphaNumToken)
			XXMLException::Throw(mLexer, "'#1' isn't a valid enum name.", name);
		attribute.AddEnum(name);
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 
	}
	
	// S? ')'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	if (token.num != kRightParensToken)
		XXMLException::Throw(mLexer, "Expected ')' marking the end of an enumeration declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoValidateAttributeDeclaration
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoValidateAttributeDeclaration(XXMLElementDeclaration* element, const XXMLAttributeDeclaration& attribute)
{
	PRECONDITION(element != nil);
	
	EAttributeDefault dflt = attribute.GetDefaultType();
	std::wstring value = attribute.GetDefaultValue();
	
	if (value.length() > 0)
		ZAttributeValueParser::Validate(attribute, value, mParser);
				
	std::wstring name, white;
	white += (wchar_t) 0x20;
	white += (wchar_t) 0x09;
	white += (wchar_t) 0x0D;
	white += (wchar_t) 0x0A;
	
	EAttributeType type = attribute.GetType();
	switch (type) {															// note that the reference attributes are passed to the parser so that it can check that the references are valid after the whole DTD is parsed
		case kStringAttribute:			
		case kTokenAttribute:			
		case kTokensAttribute:
		case kEnumAttribute:
		case kIDRefAttribute:
		case kIDRefsAttribute:
			break;
			
		case kIDAttribute:				
			if (dflt != kImpliedAttribute && dflt != kRequiredAttribute)	// VC: ID Attribute Default
				XXMLException::Throw(mLexer, "ID attributes must be declared as '#REQUIRED' or '#IMPLIED'.");
			break;
			
		case kEntityAttribute:
			if (value.length() > 0)
				mParser->ValidateEntityRef(value);								
			break;

		case kEntitiesAttribute:
			if (value.length() > 0) {
				name = Whisper::Parse(value, white);
				while (name.length() > 0) {
					mParser->ValidateEntityRef(name);							
					name = Whisper::Parse(value, white);
				}
			}
			break;
						
		case kNotationAttribute:
			{
			XXMLAttributeDeclaration::iterator iter = attribute.begin();
			while (iter != attribute.end()) {
				mParser->ValidateNotationRef(*iter);
				++iter;
			}
			}
			break;
			
		default:
			DEBUGSTR("Bogus attribute type in XXMLParser::DoValidateAttributeDeclaration");
	}
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseEntityDeclaration
//
// [70] EntityDecl ::= GEDecl | PEDecl
//
// [71] GEDecl ::= '<!ENTITY' S Name S EntityDef S? '>' 
//
// [72] PEDecl ::= '<!ENTITY' S '%' S Name S PEDef S? '>' 
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseEntityDeclaration()
{
	mLexer->ExpandInternalParameterReferences();

	// '<!ENTITY' S
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kEntityTagToken);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<!ENTITY' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// Name S EntityDef
	std::wstring name;
	token = mLexer->GetToken();
	if (token.num == kAlphaNumToken) {
		name = mLexer->GetText(token);
		if (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':')
			XXMLException::Throw(mLexer, "'#1' isn't a valid name (note that names start with a letter, underscore, or colon).", name);
		mLexer->ReadToken();
	
		token = mLexer->GetToken();
		if (token.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the general entity name (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();

		this->DoParseGeneralEntityDeclaration(name);

	// '%' S Name S PEDef
	} else if (token.num == kPercentToken) {
		mLexer->ReadToken();
	
		token = mLexer->GetToken();
		if (token.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the '%' symbol (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();

		token = mLexer->GetToken();
		name = mLexer->GetText(token);
		if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
			XXMLException::Throw(mLexer, "'#1' isn't a valid paramter entity name (note that names start with a letter, underscore, or colon).", name);
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace after the parameter entity name (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();

		this->DoParseParamEntityDeclaration(name);

	} else
		XXMLException::Throw(mLexer, "Expected a general entity name or '%' (not '#1').", mLexer->GetText(token));

	// S? '>'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	mLexer->ExpandInternalParameterReferences(false);

	if (token.num != kGreaterToken)
		XXMLException::Throw(mLexer, "Expected '>' marking the end of the ENTITY declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseGeneralEntityDeclaration
//
// [73] EntityDef ::= EntityValue | (ExternalID NDataDecl?)
//
// [76] NDataDecl ::= S 'NDATA' S Name 				[VC: Notation Declared]		see below
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseGeneralEntityDeclaration(const std::wstring& name)
{
	SXMLToken token = mLexer->GetToken();
	std::wstring text = mLexer->GetText(token);

	// ExternalID
	if (token.num == kAlphaNumToken) {
		if (text == L"SYSTEM" || text == L"PUBLIC") {	
			ZExternalIDParser idParser(mParser, mLexer);
			SExternalID id = idParser.Parse();
						
			// S?
			SXMLToken white = mLexer->GetToken();
			if (white.num == kWhiteSpaceToken)
				mLexer->ReadToken();

			// ('NDATA' S Name)?
			token = mLexer->GetToken();
			if (token.num == kAlphaNumToken && mLexer->GetText(token) == L"NDATA") {
				if (white.num != kWhiteSpaceToken)
					XXMLException::Throw(mLexer, "Expected whitespace before the 'NDATA' keyword.");

				mLexer->ReadToken();

				token = mLexer->GetToken();
				if (token.num != kWhiteSpaceToken)
					XXMLException::Throw(mLexer, "Expected whitespace after the 'NDATA' keyword (not '#1').", mLexer->GetText(token));
				mLexer->ReadToken();
				
				token = mLexer->GetToken();
				id.dataType = mLexer->GetText(token);
				if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(id.dataType[0]) && id.dataType[0] != '_' && id.dataType[0] != ':'))
					XXMLException::Throw(mLexer, "'#1' isn't a valid name (note that names start with a letter, underscore, or colon).", id.dataType);
				mLexer->ReadToken();
			}
			
			mParser->AddUnparsedEntity(name, XXMLParser::SEntity(id, mLexer->GetInternal()));

			if (id.dataType.length() > 0)
				mParser->ValidateNotationRef(id.dataType);		// VC: Notation Declared

		} else
			XXMLException::Throw(mLexer, "Expected a quote, 'SYSTEM', or 'PUBLIC' keyword in the entity declaration (not '#1').", text);
	
	// EntityValue
	} else if (token.num == kDoubleQuoteToken || token.num == kSingleQuoteToken) {
		ZEntityValueParser entParser(mParser, mLexer);
		std::wstring value = entParser.Parse();
		
		mParser->AddGeneralEntity(name, XXMLParser::SEntity(value, mLexer->GetInternal()));

	} else
		XXMLException::Throw(mLexer, "Expected a quote, 'SYSTEM', or 'PUBLIC' keyword in the entity declaration (not '#1').", text);
}


//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseParamEntityDeclaration
//
// [74] PEDef ::= EntityValue | ExternalID 
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseParamEntityDeclaration(const std::wstring& name)
{
	SXMLToken token = mLexer->GetToken();
	std::wstring text = mLexer->GetText(token);

	if (token.num == kAlphaNumToken) {
		if (text == L"SYSTEM" || text == L"PUBLIC") {
			ZExternalIDParser idParser(mParser, mLexer);
			SExternalID id = idParser.Parse();
			
			mParser->AddParameterEntity(name, XXMLParser::SEntity(id, mLexer->GetInternal()));
		
		} else
			XXMLException::Throw(mLexer, "Expected a quote, 'SYSTEM', or 'PUBLIC' keyword in the entity declaration (not '#1').", text);
	
	} else if (token.num == kDoubleQuoteToken || token.num == kSingleQuoteToken) {
		ZEntityValueParser entParser(mParser, mLexer);
		std::wstring value = entParser.Parse();
		
		mParser->AddParameterEntity(name, XXMLParser::SEntity(value, mLexer->GetInternal()));
	
	} else
		XXMLException::Throw(mLexer, "Expected a quote, 'SYSTEM', or 'PUBLIC' keyword in the entity declaration (not '#1').", text);
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// ZMarkupDeclarationParser::DoParseNotationDeclaration
//
// [82] NotationDecl ::= '<!NOTATION' S Name S (ExternalID | PublicID) S? '>'
//
//---------------------------------------------------------------
void ZMarkupDeclarationParser::DoParseNotationDeclaration()
{
	mLexer->ExpandInternalParameterReferences();

	// '<!NOTATION' S
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kNotationTagToken);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<!NOTATION' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// Name S
	token = mLexer->GetToken();
	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid notation name (note that names start with a letter, underscore, or colon).", name);
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the notation name (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// ExternalID | PublicID
	token = mLexer->GetToken();
	if (token.num != kAlphaNumToken) 
		XXMLException::Throw(mLexer, "Expected a 'SYSTEM', or 'PUBLIC' after the notation name (not '#1').", mLexer->GetText(token));
	
	SExternalID id;
	std::wstring text = mLexer->GetText(token);
	if (text == L"PUBLIC") {
		ZPublicIDParser pubParser(mParser, mLexer);
		id = pubParser.Parse();
			
		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 

		if (token.num != kGreaterToken) {
			ZExternalIDParser sysParser(mParser, mLexer);
			id.systemID = sysParser.ParseSystemLiteral();
		}
	
	} else if (text == L"SYSTEM") {
		ZExternalIDParser sysParser2(mParser, mLexer);
		id = sysParser2.Parse();
			
	} else
		XXMLException::Throw(mLexer, "Expected a 'SYSTEM', or 'PUBLIC' after the notation name (not '#1').", text);
		
	mParser->AddNotation(name, id);

	// S? '>'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 

	mLexer->ExpandInternalParameterReferences(false);

	if (token.num != kGreaterToken)
		XXMLException::Throw(mLexer, "Expected '>' marking the end of a notation declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZExternalDeclarationsParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZExternalDeclarationsParser::ZExternalDeclarationsParser
//
//---------------------------------------------------------------
ZExternalDeclarationsParser::ZExternalDeclarationsParser(XXMLParser* parser, XXMLLexer* lexer, uint32 level)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	mLevel = level;
}


//---------------------------------------------------------------
//
// ZExternalDeclarationsParser::Parse
//
// [31] extSubsetDecl ::= ( markupdecl | conditionalSect | PEReference | S )*
//
//---------------------------------------------------------------
void ZExternalDeclarationsParser::Parse()
{
	bool found = true;
	while (found) {
		SXMLToken token = mLexer->GetToken();
		switch (token.num) {
			case kElementTagToken:
			case kAttListTagToken:
			case kEntityTagToken:
			case kNotationTagToken:
			case kPIStartToken:
			case kCommentStartToken:
				{
				ZMarkupDeclarationParser markupParser(mParser, mLexer);
				markupParser.Parse();
				}
				break;
				
			case kDataStartToken:
				this->DoParseConditionalSection();	
				break;		
			
			case kPercentToken:
				{
				ZExtPEParser peParser(mParser, mLexer, mLevel + 1);
				if (!RecursionCheck(mLevel))
					XXMLException::Throw(mLexer, "'#1' has a recursive definition.", peParser.GetName());
				
				peParser.Parse();
				}
				break;		
				
			case kWhiteSpaceToken:
				mLexer->ReadToken();
				break;		
			
			default:
				found = false;
		}
	}
}


//---------------------------------------------------------------
//
// ZExternalDeclarationsParser::DoParseConditionalSection
//
// [61] conditionalSect ::= includeSect | ignoreSect 
//
// [62] includeSect ::= '<![' S? 'INCLUDE' S? '[' extSubsetDecl ']]>' 
//
// [63] ignoreSect ::= '<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'
//
//---------------------------------------------------------------
void ZExternalDeclarationsParser::DoParseConditionalSection()
{
	mLexer->ExpandInternalParameterReferences();

	// '<![' S?
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kDataStartToken);	
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	} 
	
	// 'INCLUDE' S? '[' extSubsetDecl
	std::wstring text = mLexer->GetText(token);
	if (token.num == kAlphaNumToken && text == L"INCLUDE") {
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 

		if (token.num != kLeftBracketToken)
			XXMLException::Throw(mLexer, "Expected '[' after the 'INCLUDE' keyword (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	
		ZExternalDeclarationsParser extDecParser(mParser, mLexer);
		extDecParser.Parse();

	// 'IGNORE' S? '[' ignoreSectContents* 
	} else if (token.num == kAlphaNumToken && text == L"IGNORE") {
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		} 

		if (token.num != kLeftBracketToken)
			XXMLException::Throw(mLexer, "Expected '[' after the 'IGNORE' keyword (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	
		this->DoParseIgnoreSectionContents();
	
	} else
		XXMLException::Throw(mLexer, "Expected the 'INCLUDE' or 'IGNORE' keywords (not '#1').", text);

	mLexer->ExpandInternalParameterReferences(false);
	
	// ']]>' 
	token = mLexer->GetToken();
	if (token.num != kDataEndToken)
		XXMLException::Throw(mLexer, "Expected ']]>' marking the end of a conditional section (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZExternalDeclarationsParser::DoParseIgnoreSectionContents
//
// [64] ignoreSectContents ::= Ignore ('<![' ignoreSectContents ']]>' Ignore)*
//
// [65] Ignore ::= Char* - (Char* ('<![' | ']]>') Char*) 
//
//---------------------------------------------------------------
void ZExternalDeclarationsParser::DoParseIgnoreSectionContents()
{		
	while (true) {							
		SXMLToken token = mLexer->GetToken();
		if (token.num == kDataStartToken) {
			mLexer->ReadToken();
			this->DoParseIgnoreSectionContents();

			token = mLexer->GetToken();
			if (token.num != kDataEndToken)
				XXMLException::Throw(mLexer, "Expected ']]>' marking the end of an ignored subsection (not '#1').", mLexer->GetText(token));
			mLexer->ReadToken();
		
		} else if (token.num == kDataEndToken) {
			break;
		
		} else if (token.num == kNoToken) {
			XXMLException::Throw(mLexer, "Expected ']]>' marking the end of an ignored subsection (not '#1').", mLexer->GetText(token));
		
		} else
			mLexer->ReadToken();
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZPrologParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZPrologParser::ZPrologParser
//
//---------------------------------------------------------------
ZPrologParser::ZPrologParser(XXMLParser* parser, XXMLLexer* lexer, XResource* data, bool optimized)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	PRECONDITION(data != nil);
	
	mParser = parser;
	mLexer = lexer;
	mData = data;
	mOptimized = optimized;
	
	mHasExternalSubSet = false;
}


//---------------------------------------------------------------
//
// ZPrologParser::Parse
//
// [22] prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?
//
//---------------------------------------------------------------
std::wstring ZPrologParser::Parse()	
{
	std::wstring docName;

	// XMLDecl?
	SXMLToken token = mLexer->GetToken();
	if (mLexer->GetToken().num == kXMLTagToken) {
		std::wstring language = this->DoParseLanguage();
		if (language.length() > 0)
			mParser->UpdateLexer(*mData, mLexer, language);
		else
			mParser->UpdateLexer(*mData, mLexer, L"@default");

	} else
		mParser->UpdateLexer(*mData, mLexer, L"@default");
	
	// Misc*
	ZMiscBlockParser m1Parser(mParser, mLexer);
	m1Parser.Parse();
	
	// (doctypedecl Misc*)?
	token = mLexer->GetToken();
	if (token.num == kDocTypeTagToken) {
		mParser->CreateDTD();
		
		docName = this->DoParseDocType();

		ZMiscBlockParser m2Parser(mParser, mLexer);
		m2Parser.Parse();
		
		if (mParser->IsStandAlone() && mOptimized)
			mParser->GetDTD().Reset(nil);
		
		else if (mHasExternalSubSet) {					// external subset is considered to have occured after the internal subset
			ZExtPEParser extParser(mParser, mLexer, L"@External Subset");
			extParser.Parse();
		}		
	}
		
	return docName;		
}


//---------------------------------------------------------------
//
// ZPrologParser::DoParseLanguage
//
// [23] XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
//
//---------------------------------------------------------------
std::wstring ZPrologParser::DoParseLanguage()
{
	std::wstring language;
	
	// '<?xml'
	SXMLToken token = mLexer->GetToken();
	ASSERT(mLexer->GetToken().num == kXMLTagToken);
	mLexer->ReadToken();
	
	// VersionInfo
	SXMLToken whiteToken = mLexer->GetToken();
	if (whiteToken.num == kWhiteSpaceToken)
		mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kAlphaNumToken || mLexer->GetText(token) != L"version") 
		XXMLException::Throw(mLexer, "Expected the 'version' keyword (not '#1').", mLexer->GetText(token));
	
	if (whiteToken.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<?xml' keyword.");

	ZVersionInfoParser versionParser(mParser, mLexer);
	versionParser.Parse();

	// EncodingDecl?
	whiteToken = mLexer->GetToken();
	if (whiteToken.num == kWhiteSpaceToken)
		mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num == kAlphaNumToken && mLexer->GetText(token) == L"encoding") {	
		if (whiteToken.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace before the 'encoding' keyword.");

		ZEncodingDeclarationParser encodingParser(mParser, mLexer);
		language = encodingParser.Parse();

		whiteToken = mLexer->GetToken();
		if (whiteToken.num == kWhiteSpaceToken)
			mLexer->ReadToken();
	}
	
	// SDDecl?
	token = mLexer->GetToken();
	if (token.num == kAlphaNumToken && mLexer->GetText(token) == L"standalone") {
		if (whiteToken.num != kWhiteSpaceToken)
			XXMLException::Throw(mLexer, "Expected whitespace before the 'standalone' keyword.");
		
		bool standAlone = this->DoParseStandAlone();
		mParser->SetStandAlone(standAlone);
	}

	// S? '?>'
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	}

	if (token.num != kPIEndToken)
		XXMLException::Throw(mLexer, "Expected '?>' marking the end of the XML version tag (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	return language;
}


//---------------------------------------------------------------
//
// ZPrologParser::DoParseStandAlone
//
// [32] SDDecl ::= S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"')) 	[VC: Standalone Document Declaration]	ZElementParser::DoValidateAttributes, XXMLParser::GetGeneralEntity, XXMLParser::GetParameterEntity, ZContentParser::Parse, and ZElementParser::DoParseAttribute
//
//---------------------------------------------------------------
bool ZPrologParser::DoParseStandAlone()
{
	// 'standalone'
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kAlphaNumToken && mLexer->GetText(token) == L"standalone");		// whitespace is checked by our caller
	mLexer->ReadToken();

	// Eq
	token = mLexer->GetToken();
	if (token.num != kEqualToken)
		XXMLException::Throw(mLexer, "Expected '=' following the 'standalone' keyword (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	// '"'
	token = mLexer->GetToken();
	if (token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken)
		XXMLException::Throw(mLexer, "The standalone declaration must be inside single or double quotes.");
	wchar_t quote = mLexer->GetChar(token);
	mLexer->ReadToken();
	
	// 'yes' | 'no'
	token = mLexer->GetToken();
	std::wstring text = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (text != L"yes" && text != L"no"))
		XXMLException::Throw(mLexer, "The standalone declaration must be 'yes' or 'no' (not '#1').", text);
	bool standAlone = text == L"yes";
	mLexer->ReadToken();

	// '"'
	token = mLexer->GetToken();
	if ((token.num != kSingleQuoteToken && token.num != kDoubleQuoteToken) || mLexer->GetChar(token) != quote)
		XXMLException::Throw(mLexer, "Expected a '#1' closing the standalone declaration (not '#2').", std::wstring(1, quote), mLexer->GetText(token));
	mLexer->ReadToken();
	
	return standAlone;
}


//---------------------------------------------------------------
//
// ZPrologParser::DoParseDocType
//
// [28] doctypedecl ::= '<!DOCTYPE' S Name (S ExternalID)? S? ('[' (markupdecl | PEReference | S)* ']' S?)? '>' [VC: Root Element Type]		XBuildXMLDocument::OnEndDocument (strange place...)
//
//---------------------------------------------------------------
std::wstring ZPrologParser::DoParseDocType()
{	
	// '<!DOCTYPE' S
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kDocTypeTagToken);	
	mLexer->ReadToken();

	token = mLexer->GetToken();
	if (token.num != kWhiteSpaceToken)
		XXMLException::Throw(mLexer, "Expected whitespace after the '<!DOCTYPE' tag (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	// Name
	token = mLexer->GetToken();
	std::wstring docName = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(docName[0]) && docName[0] != '_' && docName[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid DOCTYPE name (note that names must start with a letter, underscore, or colon).", docName);
	mLexer->ReadToken();

	// (S ExternalID)?
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();

		token = mLexer->GetToken();
		std::wstring text = mLexer->GetText(token);
		if (token.num == kAlphaNumToken && (text == L"SYSTEM" || text == L"PUBLIC")) {
			ZExternalIDParser extParser(mParser, mLexer);
			SExternalID id = extParser.Parse();
			
			mParser->AddParameterEntity(L"@External Subset", XXMLParser::SEntity(id, true));	// start with an illegal character so we don't conflict with a user name
			mHasExternalSubSet = true;
		}
	}

	// S?
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
	}

	// ('[' (markupdecl | PEReference | S)* ']' S?)?
	if (token.num == kLeftBracketToken) {
		mLexer->ReadToken();
		
		token = mLexer->GetToken();
		while (token.num != kRightBracketToken) {
			switch (token.num) {
				case kElementTagToken:
				case kAttListTagToken:
				case kEntityTagToken:
				case kNotationTagToken:
				case kPIStartToken:
				case kCommentStartToken:
					{
					ZMarkupDeclarationParser markupParser(mParser, mLexer);
					markupParser.Parse();
					token = mLexer->GetToken();
					}
					break;
					
				case kPercentToken:
					{
					ZExtPEParser peParser(mParser, mLexer, 0);
					peParser.Parse();								
					token = mLexer->GetToken();
					}
					break;		
					
				case kWhiteSpaceToken:
					mLexer->ReadToken();
					token = mLexer->GetToken();
					break;		
				
				case kRightBracketToken:
					break;		
				
				default:
					XXMLException::Throw(mLexer, "Expected a markup declaration, a parameter entity reference, whitespace, or a right bracket (not '#1').", mLexer->GetText(token));
			}
		}
		mLexer->ReadToken();

		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) 
			mLexer->ReadToken();
	}

	// '>'
	token = mLexer->GetToken();
	if (token.num != kGreaterToken)
		XXMLException::Throw(mLexer, "Expected '>' ending the DOCTYPE declaration (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	return docName;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZMiscBlockParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZMiscBlockParser::ZMiscBlockParser
//
//---------------------------------------------------------------
ZMiscBlockParser::ZMiscBlockParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZMiscBlockParser::Parse
//
// MiscBlock ::= Misc*
//
// [27] Misc ::= Comment | PI | S
//
//---------------------------------------------------------------
void ZMiscBlockParser::Parse()	
{
	bool done = false;

	while (!done) {
		SXMLToken token = mLexer->GetToken();
		switch (token.num) {
			case kCommentStartToken:
				{
				ZCommentParser commentParser(mParser, mLexer);
				(void) commentParser.Parse();
				}
				break;
				
			case kPIStartToken:
			case kXMLTagToken:
				{
				ZProcessInstructionParser piParser(mParser, mLexer);
				piParser.Parse();
				}
				break;
			
			case kWhiteSpaceToken:
				mLexer->ReadToken();
				break;		
				
			default:
				done = true;
		}
	}
}


}	// namespace Whisper
