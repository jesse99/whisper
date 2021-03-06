/*
 *  File:		XXMLContentParser.cpp
 *  Summary:	Classes used to parse the non-DTD portions of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLContentParser.cpp,v $
 *		Revision 1.3  2001/04/27 04:38:27  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 13:00:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <11>	 8/11/00	JDJ		XXMLElementDeclaration::GetAttribute returns a pointer instead
 *									of a reference (to allow for better error reporting).
 *		 <10>	 3/26/00	JDJ		ZAttributeValueParser::Validate now validates attributes declared as #IMPLIED.
 *		 <9>	 3/26/00	JDJ		ZContentParser::DoParseCDSection doesn't allow white-space to appear inside
 *									element content if the doc is stand-alone. 
 *		 <8>	 3/26/00	JDJ		Standalone documents are now OK if there are un-referenced external entities. 
 *		 <7>	 3/26/00	JDJ		Added some code to ZElementParser::DoDefaultAttributes for the Standalone 
 *									Document Declaration check.
 *		 <6>	 3/26/00	JDJ		ZAttributeValueParser::Parse normalizes whitespace included via entities.
 *		 <5>	 3/22/00	JDJ		Tweaked the process instruction code to allow targets like "xml-stylesheet".
 *		 <4>	 6/27/99	JDJ		ZElementParser::Parse issues a warning instead of throwing for undeclared
 *									elements (this is per section 3.2 of the standard).
 *		 <3>	 4/19/99	JDJ		ZContentParser::Parse and DoParseCDSection handle #PCDATA and CDATA
 *									sections more efficiently.
 *		 <2>	 3/19/99	JDJ		ZGeneralReferenceParser::Parse no longer throws for the Apple logo character.
 *		 <1>	 2/02/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLContentParser.h>

#include <XAutoPtr.h>
#include <XIntConversions.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XXMLCallback.h>
#include <XXMLDTDParser.h>
#include <XXMLLexer.h>
#include <XXMLParser.h>

namespace Whisper {

#if MAC && CODE_WARRIOR && !DEBUG
	#pragma optimization_level	2		// $$$ With Pro 5.2 I get out of memory errors in release builds if the optimization_level is over 2...
#endif


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
	else if (StackSpace() < 20*1024L)			// Mac memory management sucks and will normally blow the stack long before the above limit is reached...
		ok = false;
#endif

	return ok;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZGeneralReferenceParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZGeneralReferenceParser::ZGeneralReferenceParser (XXMLParser*, XXMLLexer*)
//
// [67] Reference ::= EntityRef | CharRef
//
// [66] CharRef ::= '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
//
// [68] EntityRef ::= '&' Name ';' 					[WFC: Entity Declared] 	ZGeneralReferenceParser::Parse
//													[VC: Entity Declared] 	ZGeneralReferenceParser::Parse
//													[WFC: Parsed Entity] 	ZGeneralReferenceParser::Parse
//													[WFC: No Recursion]		handled within our callers
//
//---------------------------------------------------------------
ZGeneralReferenceParser::ZGeneralReferenceParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);

	mParser = parser;
	mLexer = lexer;
	mCharRef = false;

	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kAmpersandToken);
	mLexer->ReadToken();
	
	token = mLexer->GetToken();
	if (token.num == kPoundToken) {
		mLexer->ReadToken();
		mCharRef = true;
	
		token = mLexer->GetToken();
		mName = mLexer->GetText(token);	
		if (token.num == kAlphaNumToken) {

			// '&#x' [0-9a-fA-F]+
			if (mName[0] == 'x')
				mName = L"$" + mName.substr(1);	
		
			// '&#' [0-9]+
			else
				;	
	
			mLexer->ReadToken();
		
		} else
			XXMLException::Throw(mLexer, "Expected an 'x' or digits following '&#' (not '#1').", mName);
		
	// '&' Name
	} else {
		mName = mLexer->GetText(token);	
		if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(mName[0]) && mName[0] != '_' && mName[0] != ':'))
			XXMLException::Throw(mLexer, "'#1' isn't a valid character entity name (note that names start with a letter, underscore, or colon).", mLexer->GetText(token));
		mLexer->ReadToken();
	}
	
	// ';'
	token = mLexer->GetToken();
	if (token.num != kSemiColonToken)
		XXMLException::Throw(mLexer, "Expected ';' marking the end of a character entity reference (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}


//---------------------------------------------------------------
//
// ZGeneralReferenceParser::IsPredefined
//
//---------------------------------------------------------------
bool ZGeneralReferenceParser::IsPredefined() const
{
	bool is = false;
	
	if (!mCharRef) {
		if (mName == L"amp")					
			is = true;
		else if (mName == L"lt")
			is = true;
		else if (mName == L"gt")
			is = true;
		else if (mName == L"quot")
			is = true;
		else if (mName == L"apos")
			is = true;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// ZGeneralReferenceParser::IsExternal
//
//---------------------------------------------------------------
bool ZGeneralReferenceParser::IsExternal() const
{
	bool is = false;
	
	if (!this->IsCharacter() && !this->IsPredefined()) {
		const XXMLParser::SEntity& entity = mParser->GetGeneralEntity(mName);
		is = !entity.internal;
	}
	
	return is;
}


//---------------------------------------------------------------
//
// ZGeneralReferenceParser::Parse
//
// [2] Char ::= #x9 | #xA | #xD | [#x20-#D7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
//
//---------------------------------------------------------------
std::wstring ZGeneralReferenceParser::Parse()
{
	std::wstring text;
	
	if (mCharRef) {
		uint32 n = StrToUInt32(mName);
		if (!(n == 0x09 || n == 0x0A || n == 0x0D || (n >= 0x20 && n <= 0xD7FF) || (n >= 0xE000 && n <= 0xFFFD) || (n >= 0x10000 && n <= 0x10FFFF)))
			if (n != 0x14)							// $$ non-standard extension to support Mac Menu Manager's colored Apple logo character
				XXMLException::Throw(mLexer, "The char reference doesn't correspond to a valid character.");	// XXMLLexer::IsChar won't always
		
		if (n < 0x10000)
			text = (wchar_t) n;
		
		else {
			n -= 0x10000;
			text += (wchar_t) ((n >> 10) + 0xD800);
			text += (wchar_t) ((n & 0x3FF) + 0xDC00);
		}
					
	} else {
		if (mName == L"amp")					
			text = L"&";
		else if (mName == L"lt")
			text = L"<";
		else if (mName == L"gt")
			text = L">";
		else if (mName == L"quot")
			text = L"\"";
		else if (mName == L"apos")
			text = L"'";
		
		else {
			const XXMLParser::SEntity& entity = mParser->GetGeneralEntity(mName);	// WFC: Entity Declared
			if (!entity.internal)													// WFC: No External Entity References and WFC: Parsed Entity
				XXMLException::Throw(mLexer, "External general references are forbidden here."); 
			
			if (entity.externalSubSet && mParser->IsStandAlone())					// VC: Standalone Document Declaration
				XXMLException::Throw(mLexer, "Entity '#1' was declared in the external subset so the document cannot be standalone.", mName);

			text = entity.text;
		}
	}
	
	return text;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZExtParsedEntParser															
// ===================================================================================

//---------------------------------------------------------------
//
// ZExtParsedEntParser::ZExtParsedEntParser
//
//---------------------------------------------------------------
ZExtParsedEntParser::ZExtParsedEntParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& name, const std::wstring& elementName, XXMLElementDeclaration* parent, uint32 level) : mName(name), mElementName(elementName)
{
	PRECONDITION(parser != nil);	
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	mLevel = level;
	
	mParent = parent;
}


//---------------------------------------------------------------
//
// ZExtParsedEntParser::Parse
//
// [78] extParsedEnt ::= TextDecl? content
//
//---------------------------------------------------------------
void ZExtParsedEntParser::Parse()
{
	// Load the entity and create a lexer object
	const XXMLParser::SEntity& entity = mParser->GetGeneralEntity(mName);
	ASSERT(!entity.internal);				
										
	if (entity.id.dataType.length() > 0)
		XXMLException::Throw(mLexer, "Unparsed entities cannot be included here."); 

	if (entity.externalSubSet && mParser->IsStandAlone())			// VC: Standalone Document Declaration
		XXMLException::Throw(mLexer, "Entity '#1' was declared in the external subset so the document cannot be standalone.", mName);

	XAutoPtr<XResource> data(entity.Load(mLexer->GetURI()));
	if (data.Get() == nil)
		XXMLException::Throw(mLexer, "Couldn't find the external general entity named: '#1'", mName);

	XAutoPtr<XXMLLexer> lexer(mParser->CreateLexer(*data, false));
	lexer->ReadToken();
	
	// Parse the language encoding
	SXMLToken token = lexer->GetToken();
	if (token.num == kXMLTagToken) {
		ZTextDeclarationParser textParser(mParser, lexer.Get());
		std::wstring language = textParser.Parse();
		
		mParser->UpdateLexer(*data, lexer.Get(), language);
	
	} else	
		mParser->UpdateLexer(*data, lexer.Get(), L"@default");
	
	// Parse the declarations
	ZContentParser contParser(mParser, lexer.Get(), mElementName, mParent, mLevel+1);
	contParser.Parse();	
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZAttributeValueParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZAttributeValueParser::~ZAttributeValueParser 
//
//---------------------------------------------------------------
ZAttributeValueParser::~ZAttributeValueParser()
{
	if (mOwnsLexer)
		delete mLexer;

	mLexer = nil;
	mParser = nil;
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::ZAttributeValueParser (XXMLParser*, XXMLLexer)
//
//---------------------------------------------------------------
ZAttributeValueParser::ZAttributeValueParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLevel = 0;
	mLexer = lexer;
	mOwnsLexer = false;
	
	SXMLToken token = mLexer->GetToken();
	if (token.num != kDoubleQuoteToken && token.num != kSingleQuoteToken)
		XXMLException::Throw(mLexer, "Expected a single or double quote to start the attribute value (not '#1').", mLexer->GetText(token));

	mLexer->ExpandInternalParameterReferences(false);
	mLexer->ReadToken();					
	mLexer->ExpandInternalParameterReferences(true);
		
	mQuote = token.num;
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::ZAttributeValueParser (XXMLParser*, wstring, uint32)
//
//---------------------------------------------------------------
ZAttributeValueParser::ZAttributeValueParser(XXMLParser* parser, const std::wstring& text, uint32 level)
{
	PRECONDITION(parser != nil);
		
	mParser = parser;
	mLevel = level;
	mLexer = new XXMLLexer(parser, new XScanner(text), parser->GetLexer()->GetURI(), parser->GetLexer()->GetInternal());
	mOwnsLexer = true;
	
	mLexer->ExpandInternalParameterReferences(false);
	mLexer->ReadToken();
	mLexer->ExpandInternalParameterReferences(true);
	
	mQuote = LONG_MAX;			// we'll consume the entire string
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::Parse
//
// [10] AttValue ::= '"' ([^<&"] | Reference)* '"' |� "'" ([^<&'] | Reference)* "'" 
//
//---------------------------------------------------------------
std::wstring ZAttributeValueParser::Parse()
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
				std::wstring literal = genRefParser.Parse();		

				if (genRefParser.IsCharacter() || genRefParser.IsPredefined()) 
					text += ZAttributeValueParser::ProcessWhiteSpace(genRefParser.Parse());		// included
					
				else {					
					if (!RecursionCheck(mLevel))
						XXMLException::Throw(mLexer, "'#1' has a recursive definition.", genRefParser.GetName());
					
					ZAttributeValueParser attParser(mParser, literal, mLevel + 1);
					text += ZAttributeValueParser::ProcessWhiteSpace(attParser.Parse());			// included as literal
				}
				}
				break;
				
			case kDoubleQuoteToken:
			case kSingleQuoteToken:
				text += ch;
				mLexer->ReadToken();					
				break;
				
			case kLessToken:									// WFC: No < in Attribute Values
				XXMLException::Throw(mLexer, "'<' can't be used within an attribute value.");
				break;
				
			default:
				std::wstring whiteText = ZAttributeValueParser::ProcessWhiteSpace(mLexer->GetText(token));
				text += whiteText;
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
		XXMLException::Throw(mLexer, "The entity reference doesn't match the AttValue production.");			

	return text;
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::ProcessWhiteSpace				[static]
//
//---------------------------------------------------------------
std::wstring ZAttributeValueParser::ProcessWhiteSpace(const std::wstring& input)
{
	std::wstring output;
	output.resize(input.length());
	
	for (uint32 index = 0; index < input.length(); ++index) {
		wchar_t c = input[index];
		
		if (c == 0x20 || c == 0x09 || c == 0x0A || c == 0x0D)	// 0x0D should only appear here as the result of entity expansion
			output[index] = ' ';
		else
			output[index] = input[index];
	}

	return output;
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::Normalize						[static]
//
//---------------------------------------------------------------
void ZAttributeValueParser::Normalize(std::wstring& value)
{
	if (value.length() > 0) {
		std::wstring temp = value;
		
		// strip leading spaces
		uint32 index = 0;
		while (index < temp.length() && temp[index] == ' ')
			++index;
			
		// strip trailing spaces
		uint32 last = temp.length() - 1;
		while (last > index && temp[last] == ' ')
			--last;
			
		// replace runs of spaces with a single space
		value = L"";
		while (index <= last) {
			if (temp[index] != ' ' || (index+1 <= last && temp[index+1] != ' '))
				value += temp[index];
			++index;
		}
	}
}


//---------------------------------------------------------------
//
// ZAttributeValueParser::Validate						[static]
//
//---------------------------------------------------------------
void ZAttributeValueParser::Validate(const XXMLAttributeDeclaration& attribute, const std::wstring& inValue, XXMLParser* parser)
{		
	PRECONDITION(parser != nil);
	
	uint32 index;
	
	std::wstring name, white;
	white += (wchar_t) 0x20;
	white += (wchar_t) 0x09;
	white += (wchar_t) 0x0D;
	white += (wchar_t) 0x0A;
	
	std::wstring value = inValue;
				
	EAttributeType type = attribute.GetType();
	switch (type) {
		case kStringAttribute:
			break;

		case kIDAttribute:	
			if (value.length() == 0)										// VC: ID (ID values must match the Name production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ID attributes must have a name.")));

			if (!XXMLLexer::IsLetter(value[0]) && value[0] != '_' && value[0] != ':')
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ID values must start with a letter, underscore, or colon.")));

			for (index = 1; index < value.length(); ++index) 
				if (!XXMLLexer::IsNameChar(value[index]))
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", value)));
			break;
			
		case kIDRefAttribute:
			if (value.length() == 0)										// VC: IDREF (ID values must match the Name production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"IDREF attributes must have a name.")));

			if (!XXMLLexer::IsLetter(value[0]) && value[0] != '_' && value[0] != ':')
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"IDREF values must start with a letter, underscore, or colon.")));

			for (index = 1; index < value.length(); ++index) 
				if (!XXMLLexer::IsNameChar(value[index]))
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", value)));

			parser->ValidateIDRef(value);									
			break;
			
		case kIDRefsAttribute:
			if (value.length() == 0)										// VC: IDREF (ID values must match the Name production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"IDREFS attributes must have at least one name.")));

			name = Whisper::Parse(value, white);
			while (name.length() > 0) {
				if (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':')
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"IDREFS names must start with a letter, underscore, or colon.")));
	
				for (index = 1; index < name.length(); ++index) 
					if (!XXMLLexer::IsNameChar(name[index]))
						throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", name)));

				parser->ValidateIDRef(name);								
				name = Whisper::Parse(value, white);
			}
			break;
			
		case kEntityAttribute:
			{
			if (value.length() == 0)										// VC: Entity Name (entity values must match the Name production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ENTITY attributes must have a name.")));

			if (!XXMLLexer::IsLetter(value[0]) && value[0] != '_' && value[0] != ':')
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ENTITY values must start with a letter, underscore, or colon.")));

			for (index = 1; index < value.length(); ++index) 
				if (!XXMLLexer::IsNameChar(value[index]))
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", value)));
					
			const XXMLParser::SEntity& entity = parser->GetGeneralEntity(value);
			if (entity.id.dataType.length() == 0)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' doesn't refer to an unparsed entity.", value)));
			}
			break;

		case kEntitiesAttribute:
			if (value.length() == 0)										// VC: Entity Name (entity values must match the Name production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ENTITIES attributes must have at least one name.")));

			name = Whisper::Parse(value, white);
			while (name.length() > 0) {
				if (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':')
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ENTITIES names must start with a letter, underscore, or colon.")));
	
				for (index = 1; index < name.length(); ++index) 
					if (!XXMLLexer::IsNameChar(name[index]))
						throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", name)));

				const XXMLParser::SEntity& entity = parser->GetGeneralEntity(name);
				if (entity.id.dataType.length() == 0)
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' doesn't refer to an unparsed entity.", name)));

				name = Whisper::Parse(value, white);
			}
			break;
			
		case kTokenAttribute:
			if (value.length() == 0)										// VC: Name Token (name token values must match the Nmtoken production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"NMTOKEN attributes must have a name.")));

			for (index = 0; index < value.length(); ++index) 
				if (!XXMLLexer::IsNameChar(value[index]))
					throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", value)));
			break;

		case kTokensAttribute:
			if (value.length() == 0)										// VC: Name Token (name token values must match the Nmtoken production)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"NMTOKENS attributes must have at least one name.")));

			name = Whisper::Parse(value, white);
			while (name.length() > 0) {
				for (index = 0; index < name.length(); ++index) 
					if (!XXMLLexer::IsNameChar(name[index]))
						throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't a valid name.", name)));

				name = Whisper::Parse(value, white);
			}
			break;
			
		case kNotationAttribute:
			if (!attribute.MatchEnum(value))								// VC: Notation Attributes
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' doesn't match the NOTATION names.", value)));
			break;
			
		case kEnumAttribute:
			if (!attribute.MatchEnum(value))								// VC: Enumeration
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' doesn't match any of the enumerated names.", value)));
			break;
				
		default:
			DEBUGSTR("Bogus attribute type in ZAttributeValueParser::Validate");
	}
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZProcessInstructionParser											
// ===================================================================================

//---------------------------------------------------------------
//
// ZProcessInstructionParser::ZProcessInstructionParser
//
//---------------------------------------------------------------
ZProcessInstructionParser::ZProcessInstructionParser(XXMLParser* parser, XXMLLexer* lexer)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
}


//---------------------------------------------------------------
//
// ZProcessInstructionParser::Parse
//
// [16] PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
//
// [17] PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
//
//---------------------------------------------------------------
void ZProcessInstructionParser::Parse()
{
	// '<?'
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kPIStartToken || token.num == kXMLTagToken);	
	std::wstring prefix = token.num == kXMLTagToken ? L"xml" : L"";
	mLexer->ReadToken();

	// PITarget
	token = mLexer->GetToken();
	std::wstring target = prefix + mLexer->GetText(token);		// for names like "xml-stylesheet"
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(target[0]) && target[0] != '_' && target[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid process instruction target.", target);
	if (ConvertToLowerCase(target) == L"xml")					
		XXMLException::Throw(mLexer, "Process instruction target cannot be 'xml'.");
	mLexer->ReadToken();

	// S (Char* - (Char* '?>' Char*))
	std::wstring data;
	
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadFixedToken(2);
		
		bool done = false;
		while (!done) {											// loop over the tokens
			if (mLexer->FindToken(kPIEndToken, token)) {		// if a kPIEndToken is found we're all done
				mLexer->SkipToken(token);
				mLexer->ReadToken();
				done = true;
		
			} else if (mLexer->FindToken(kCharToken, token)) {	// if a kCharToken is found we'll add it to our text
				mLexer->SkipToken(token);
				mLexer->ReadFixedToken(2);
				data += mLexer->GetChar(token);
			
			} else												// otherwise we've found an error
				XXMLException::Throw(mLexer, "Found an invalid character in the process instruction text.");
		}
		
	} else {
	
		// '?>'
		token = mLexer->GetToken();
		if (token.num != kPIEndToken)
			XXMLException::Throw(mLexer, "Expected '?>' marking the end of a process instruction (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	}
	
	mParser->GetCallback()->OnProcessInstruction(target, data);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZContentParser											
// ===================================================================================

//---------------------------------------------------------------
//
// ZContentParser::ZContentParser
//
//---------------------------------------------------------------
ZContentParser::ZContentParser(XXMLParser* parser, XXMLLexer* lexer, const std::wstring& elementName, XXMLElementDeclaration* parent, uint32 level) : mElementName(elementName)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	mLevel = level;
	
	mParent = parent;
}


//---------------------------------------------------------------
//
// ZContentParser::Parse
//
// [43] content ::= (element | CharData | Reference | CDSect | PI | Comment)*
//
// [14] CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
//
//---------------------------------------------------------------
void ZContentParser::Parse()
{
#if MAC
	PRECONDITION(StackSpace() > 10*1024L);		// default stack space on Macs is only 64K which isn't always enough for the parser...
#endif

	bool done = false;
	
	bool inElementContent = mParent != nil && (mParent->GetType() == kEmptyElement || mParent->GetType() == kChildrenElement);

	while (!done) {
		SXMLToken token = mLexer->GetToken();
		switch (token.num) {
			case kLessToken:										
				{
				ZElementParser elemParser(mParser, mLexer, mParent);
				elemParser.Parse();
				}
				break;
			
			case kAmpersandToken:
				{
				ZGeneralReferenceParser genRefParser(mParser, mLexer);

				if (genRefParser.IsCharacter() || genRefParser.IsPredefined()) {
					std::wstring text = genRefParser.Parse();		
					if (mParent != nil)
						if (!mParent->ValidText(text))
							XXMLException::Throw(mLexer, "The ELEMENT declaration doesn't allow character data to appear here.");

					mParser->GetCallback()->OnText(mElementName, text, inElementContent);
				
				} else {
					if (!RecursionCheck(mLevel))
						XXMLException::Throw(mLexer, "'#1' has a recursive definition.", genRefParser.GetName());
						
					if (genRefParser.IsExternal()) {					
						ZExtParsedEntParser extParser(mParser, mLexer, genRefParser.GetName(), mElementName, mParent, mLevel + 1);
						extParser.Parse();
	
					} else {					
						std::wstring literal = genRefParser.Parse();		

						XAutoPtr<XXMLLexer> lexer(new XXMLLexer(mParser, new XScanner(literal), mLexer->GetURI(), mLexer->GetInternal()));
						lexer->ReadToken();
	
						ZContentParser contParser(mParser, lexer.Get(), mElementName, mParent, mLevel + 1);	// well formed internal general entities must match content production
						contParser.Parse();

						if (!lexer->AtEnd())
							XXMLException::Throw(mLexer, "'#1' doesn't match the content production.", genRefParser.GetName());			
					}
				}
				}
				break;
				
			case kCDSectTagToken:
				this->DoParseCDSection();
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
				std::wstring contents = commentParser.Parse();
				mParser->GetCallback()->OnComment(contents);
				}
				break;
				
			case kEndTagToken:			// if called from element
			case kNoToken:				// if called from extParsedEnt
				done = true;
				break;			
				
			case kWhiteSpaceToken:
				{
				std::wstring whiteText = mLexer->GetText(token);
				if (mParent != nil) {
					if (mParent->IsExternal() && mParser->IsStandAlone())		// VC: Standalone Document Declaration
						if (mParent->GetType() == kChildrenElement)
							XXMLException::Throw(mLexer, "Element was declared in the external subset, has element content, and includes white space so the document cannot be standalone.");

					if (!mParent->ValidText(whiteText))
						XXMLException::Throw(mLexer, "The element declaration doesn't allow character data to appear here.");
				}
				
				mParser->GetCallback()->OnText(mElementName, whiteText, inElementContent);
				mLexer->ReadToken();
				}
				break;
								
			default:
				std::wstring miscText;
				
				bool isText = true;
				bool hasWhite = false;
				
				while (isText) {
					switch (token.num) {
						case kLessToken:				// $$ note that these case statements must match those above									
						case kAmpersandToken:			// $$ this is fairly ugly, but this allows us to call OnText much less often
						case kCDSectTagToken:			// $$ which will be much more efficient
						case kPIStartToken:										
						case kCommentStartToken:
						case kEndTagToken:			
						case kNoToken:				
							isText = false;
							break;
											
						case kWhiteSpaceToken:
							hasWhite = true;
							// fall thru
											
						default:
							miscText += mLexer->GetText(token);
							mLexer->ReadToken();
							token = mLexer->GetToken();
					}
				}

				if (hasWhite && mParent != nil)
					if (mParent->IsExternal() && mParser->IsStandAlone())		// VC: Standalone Document Declaration
						if (mParent->GetType() == kChildrenElement)
							XXMLException::Throw(mLexer, "Element was declared in the external subset, has element content, and includes white space so the document cannot be standalone.");

				if (miscText.find(L'<') != std::wstring::npos)
					XXMLException::Throw(mLexer, "'<' cannot be used in character data (use '&lt;' instead).");
				if (miscText.find(L'&') != std::wstring::npos)
					XXMLException::Throw(mLexer, "'&' cannot be used in character data (use '&amp;' instead).");
				if (miscText.find(L"]]>") != std::wstring::npos)
					XXMLException::Throw(mLexer, "']]>' cannot be used in character data (use ']]&gt;' instead).");
					
				if (mParent != nil)
					if (!mParent->ValidText(miscText))
						XXMLException::Throw(mLexer, "The ELEMENT declaration doesn't allow character data to appear here.");

				mParser->GetCallback()->OnText(mElementName, miscText, inElementContent);
		}
	}
}


//---------------------------------------------------------------
//
// ZContentParser::DoParseCDSection
//
// [18] CDSect ::= CDStart CData CDEnd
//
// [19] CDStart ::= '<![CDATA['
//
// [20] CData ::= (Char* - (Char* ']]>' Char*)) 
//
// [21] CDEnd ::= ']]>'
//
//---------------------------------------------------------------
void ZContentParser::DoParseCDSection()
{
	// '<![CDATA[' 
	SXMLToken token = mLexer->GetToken();
	ASSERT(token.num == kCDSectTagToken);
	mLexer->ReadToken();
	
	// (Char* - (Char* ']]>' Char*))  
	std::wstring text;

	token = mLexer->GetToken();
	while (token.num != kDataEndToken && token.num != kNoToken) {
		if (token.num == kWhiteSpaceToken && mParent->IsExternal() && mParser->IsStandAlone())		// VC: Standalone Document Declaration
			if (mParent->GetType() == kChildrenElement)
				XXMLException::Throw(mLexer, "Element was declared in the external subset, has element content, and includes white space so the document cannot be standalone.");

		text += mLexer->GetText(token);
		
		mLexer->ReadToken();
		token = mLexer->GetToken();
	}
	
	if (mParent != nil)
		if (!mParent->ValidText(text))
			XXMLException::Throw(mLexer, "The ELEMENT declaration doesn't allow character data to appear here.");

	mParser->GetCallback()->OnText(mElementName, text, false);

	// ']]>'
	if (token.num != kDataEndToken)
		XXMLException::Throw(mLexer, "Expected ']]>' marking the end of a CDATA section (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class ZElementParser
// ===================================================================================

//---------------------------------------------------------------
//
// ZElementParser::ZElementParser
//
//---------------------------------------------------------------
ZElementParser::ZElementParser(XXMLParser* parser, XXMLLexer* lexer, XXMLElementDeclaration* parent)
{
	PRECONDITION(parser != nil);
	PRECONDITION(lexer != nil);
	
	mParser = parser;
	mLexer = lexer;
	
	mParent = parent;
}


//---------------------------------------------------------------
//
// ZElementParser::Parse
//
// [39] element ::= EmptyElemTag | STag content ETag 		[WFC: Element Type Match] 	see below
//															[VC: Element Valid]			XXMLElementDeclaration::ValidText and ValidChild
//
// [44] EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'	[WFC: Unique Att Spec] 		ZElementParser::DoParseAttribute
//
// [40] STag ::= '<' Name (S Attribute)* S? '>'				[WFC: Unique Att Spec]		ZElementParser::DoParseAttribute
//
// [42] ETag ::= '</' Name S? '>' 
//
//---------------------------------------------------------------
void ZElementParser::Parse()	
{	
#if MAC
	PRECONDITION(StackSpace() > 10*1024L);		// default stack space on Macs is only 64K which isn't always enough for the parser...
#endif

	XXMLCallbackMixin* callback = mParser->GetCallback();	// 2080
	XXMLDTD* dtd = mParser->GetDTD().Get();					// 1120
	
	// '<' Name
	SXMLToken token = mLexer->GetToken();
	if (token.num != kLessToken)	
		XXMLException::Throw(mLexer, "Expected a '<' marking the start of an element tag (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();

	token = mLexer->GetToken();
	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
		XXMLException::Throw(mLexer, "'#1' isn't a valid element name (note that names start with a letter, underscore, or colon).", name);
	mLexer->ReadToken();

	callback->OnBeginElement(name);
	
	XXMLElementDeclaration* declaration = dtd != nil ? dtd->GetElement(name) : nil;
	if (dtd != nil && declaration->GetType() == kUndeclaredElement)							
		mParser->DoWarning(L"Element '" + name + L"' isn't a declared element.");
	
	if (mParent != nil)
		if (!mParent->ValidChild(name))
			if (mParent->GetType() == kEmptyElement)
				XXMLException::Throw(mLexer, "The element named '#1' cannot be used here (the parent element was declared as #EMPTY).", name);
			else
				XXMLException::Throw(mLexer, "The element named '#1' cannot be used here (it doesn't match the ELEMENT declaration of the parent element).", name);
	
	if (declaration != nil && declaration->GetType() != kUndeclaredElement)
		declaration->StartValidating();

	// (S Attribute)* S?
	bool foundWhite = false;
	token = mLexer->GetToken();
	if (token.num == kWhiteSpaceToken) {
		mLexer->ReadToken();
		token = mLexer->GetToken();
		foundWhite = true;
	}
	
	while (token.num != kEmptyTagToken && token.num != kGreaterToken) {
		if (!foundWhite)
			XXMLException::Throw(mLexer, "Attributes must be seperated with whitespace.");

		this->DoParseAttribute(name, declaration);
		
		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();

		} else
			foundWhite = false;
	}
	
	if (declaration != nil && declaration->GetType() != kUndeclaredElement)
		this->DoDefaultAttributes(name, declaration);
	
	// '/>'
	if (token.num == kEmptyTagToken) {
		mLexer->ReadToken();
	
	// '>'  
	} else {
		mLexer->ReadToken();
		
		// content
		ZContentParser contParser(mParser, mLexer, name, declaration);
		contParser.Parse();

		// '</' 
		token = mLexer->GetToken();
		if (token.num != kEndTagToken)	
			XXMLException::Throw(mLexer, "Expected '</' marking the start of an end tag (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	
		// Name 
		token = mLexer->GetToken();
		std::wstring name2 = mLexer->GetText(token);
		if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name2[0]) && name2[0] != '_' && name2[0] != ':'))
				XXMLException::Throw(mLexer, "'#1' isn't a valid element name (note that names start with a letter, underscore, or colon).", name2);
		mLexer->ReadToken();
	
		if (name2 != name)									// WFC: Element Type Match
			XXMLException::Throw(mLexer, "Expected an end tag named '#1' instead of '#2'.", name, name2);

		// S? '>' 
		token = mLexer->GetToken();
		if (token.num == kWhiteSpaceToken) {
			mLexer->ReadToken();
			token = mLexer->GetToken();
		}
	
		if (token.num != kGreaterToken)	
			XXMLException::Throw(mLexer, "Expected '>' marking the end of an end tag (not '#1').", mLexer->GetText(token));
		mLexer->ReadToken();
	}

	if (declaration != nil && declaration->GetType() != kUndeclaredElement && !declaration->StopValidating())
		XXMLException::Throw(mLexer, "The element named '#1' is missing one or more required child elements.", name);

	callback->OnEndElement(name);
}


//---------------------------------------------------------------
//
// ZElementParser::DoDefaultAttributes
//
//---------------------------------------------------------------
void ZElementParser::DoDefaultAttributes(const std::wstring& elementName, XXMLElementDeclaration* declaration)
{
	PRECONDITION(declaration != nil);
	
	XXMLElementDeclaration::iterator iter = declaration->begin();
	while (iter != declaration->end()) {		
		const XXMLAttributeDeclaration& attribute = iter->second;
		
		std::vector<std::wstring>::const_iterator iter2 = std::find(mAttributes.begin(), mAttributes.end(), iter->first);
		bool found = iter2 != mAttributes.end();
		
		EAttributeDefault defaultType = attribute.GetDefaultType();
		EAttributeType attType = attribute.GetType();

		if (defaultType == kRequiredAttribute) {							// VC: Required Attribute
			if (!found)	
				XXMLException::Throw(mLexer, "Attribute '#1' is a required attribute, but was not specified.", iter->first);
			
		} else if (defaultType == kImpliedAttribute) {
			if (!found)	
				mParser->GetCallback()->OnAttribute(elementName, iter->first, L"", true, attType == kIDAttribute);
			
		} else if (defaultType == kFixedAttribute) {			
			if (!found)	{
				std::wstring value = attribute.GetDefaultValue();
				mParser->GetCallback()->OnAttribute(elementName, iter->first, value, false, attType == kIDAttribute);
			} 
						
		} else if (defaultType == kDefaultAttribute) {
			if (!found)	{
				if (attribute.IsExternal() && mParser->IsStandAlone())		// VC: Standalone Document Declaration
					XXMLException::Throw(mLexer, "Attribute '#1' was declared in the external subset and the default value is being used so the document cannot be standalone.", iter->first);

				std::wstring value = attribute.GetDefaultValue();
				mParser->GetCallback()->OnAttribute(elementName, iter->first, value, false, attType == kIDAttribute);
			}
							
		} else
			DEBUGSTR("Bogus attribute default type");
			
		++iter;
	}
}


//---------------------------------------------------------------
//
// ZElementParser::DoParseAttribute
//
// [41] Attribute ::= Name Eq AttValue 			[VC: Attribute Value Type] 				see below
//												[WFC: No External Entity References] 	ZGeneralReferenceParser::Parse
//												[WFC: No < in Attribute Values]			ZAttributeValueParser::Parse
//
//---------------------------------------------------------------
void ZElementParser::DoParseAttribute(const std::wstring& elementName, XXMLElementDeclaration* element)
{
	// Name
	SXMLToken token = mLexer->GetToken();
	std::wstring name = mLexer->GetText(token);
	if (token.num != kAlphaNumToken || (!XXMLLexer::IsLetter(name[0]) && name[0] != '_' && name[0] != ':'))
			XXMLException::Throw(mLexer, "'#1' isn't a valid attribute name (note that names start with a letter, underscore, or colon).", name);
	mLexer->ReadToken();
	
	if (std::find(mAttributes.begin(), mAttributes.end(), name) != mAttributes.end())	// WFC: Unique Att Spec
		XXMLException::Throw(mLexer, "Attribute '#1' has already been defined.", name);

	mAttributes.push_back(name);
					
	// Eq
	token = mLexer->GetToken();
	if (token.num != kEqualToken)
		XXMLException::Throw(mLexer, "Expected '=' following the attribute name (not '#1').", mLexer->GetText(token));
	mLexer->ReadToken();
	
	// AttValue
	ZAttributeValueParser attParser(mParser, mLexer);
	std::wstring value = attParser.Parse();

	if (element != nil && element->GetType() != kUndeclaredElement) {
		const XXMLAttributeDeclaration* attribute = element->GetAttribute(name);	// VC: Attribute Value Type
		if (attribute == nil)
			XXMLException::Throw(mLexer, "Couldn't find the '#1'  attribute!", name);

		EAttributeType attType = attribute->GetType();
		
		if (attType != kStringAttribute) {											// XML 1.0 errata says that attributes must be normalized before the validity constraints are checked		
			if (attribute->IsExternal() && mParser->IsStandAlone()) {
				std::wstring oldValue = value;
				ZAttributeValueParser::Normalize(value);
	
				if (value != oldValue)												// VC: Standalone Document Declaration
					XXMLException::Throw(mLexer, "Attribute '#1' was changed via normalization so the document cannot be standalone.", name);
			
			} else				
				ZAttributeValueParser::Normalize(value);
		}
		
		ZAttributeValueParser::Validate(*attribute, value, mParser);		

		if (attType == kIDAttribute)
			mParser->AddID(value);													// VC: One ID per Element Type

		if (attribute->GetDefaultType() == kFixedAttribute)			
			if (attribute->GetDefaultValue() != value)								// VC: Fixed Attribute Default
				XXMLException::Throw(mLexer, "Attribute '#1'has a fixed value, but was defined with a different value.", name);

		mParser->GetCallback()->OnAttribute(elementName, name, value, false, attType == kIDAttribute);

	} else	
		mParser->GetCallback()->OnAttribute(elementName, name, value, false, false);
}


}	// namespace Whisper
