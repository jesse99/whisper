/*
 *  File:		XXMLLexer.cpp
 *  Summary:	A class used to extract tokens from XML documents
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLLexer.cpp,v $
 *		Revision 1.5  2001/04/27 04:39:22  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/03/06 07:33:47  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2000/12/10 04:12:36  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/10 04:47:11  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <XXMLLexer.h>

#include <XAutoPtr.h>
#include <XNumbers.h>
#include <XResource.h>
#include <XScanner.h>
#include <XStringUtils.h>
#include <XXMLParser.h> 

namespace Whisper {


// ===================================================================================
//	struct SCharRange
// ===================================================================================
struct SCharRange {
  wchar_t		start;
  wchar_t 	end;
};


// ===================================================================================
//	Character Tables
// ===================================================================================
const SCharRange kLetterChars[] = {		// these are from the expat source 
  { '_' },								
  { ':' },
  /* BaseChar */
  { 0x0041, 0x005a },
  { 0x0061, 0x007a },
  { 0x00c0, 0x00d6 },
  { 0x00d8, 0x00f6 },
  { 0x00f8, 0x00ff },
  { 0x0100, 0x0131 },
  { 0x0134, 0x013e },
  { 0x0141, 0x0148 },
  { 0x014a, 0x017e },
  { 0x0180, 0x01c3 },
  { 0x01cd, 0x01f0 },
  { 0x01f4, 0x01f5 },
  { 0x01fa, 0x0217 },
  { 0x0250, 0x02a8 },
  { 0x02bb, 0x02c1 },
  { 0x0386 },
  { 0x0388, 0x038a },
  { 0x038c },
  { 0x038e, 0x03a1 },
  { 0x03a3, 0x03ce },
  { 0x03d0, 0x03d6 },
  { 0x03da },
  { 0x03dc },
  { 0x03de },
  { 0x03e0 },
  { 0x03e2, 0x03f3 },
  { 0x0401, 0x040c },
  { 0x040e, 0x044f },
  { 0x0451, 0x045c },
  { 0x045e, 0x0481 },
  { 0x0490, 0x04c4 },
  { 0x04c7, 0x04c8 },
  { 0x04cb, 0x04cc },
  { 0x04d0, 0x04eb },
  { 0x04ee, 0x04f5 },
  { 0x04f8, 0x04f9 },
  { 0x0531, 0x0556 },
  { 0x0559 },
  { 0x0561, 0x0586 },
  { 0x05d0, 0x05ea },
  { 0x05f0, 0x05f2 },
  { 0x0621, 0x063a },
  { 0x0641, 0x064a },
  { 0x0671, 0x06b7 },
  { 0x06ba, 0x06be },
  { 0x06c0, 0x06ce },
  { 0x06d0, 0x06d3 },
  { 0x06d5 },
  { 0x06e5, 0x06e6 },
  { 0x0905, 0x0939 },
  { 0x093d },
  { 0x0958, 0x0961 },
  { 0x0985, 0x098c },
  { 0x098f, 0x0990 },
  { 0x0993, 0x09a8 },
  { 0x09aa, 0x09b0 },
  { 0x09b2 },
  { 0x09b6, 0x09b9 },
  { 0x09dc, 0x09dd },
  { 0x09df, 0x09e1 },
  { 0x09f0, 0x09f1 },
  { 0x0a05, 0x0a0a },
  { 0x0a0f, 0x0a10 },
  { 0x0a13, 0x0a28 },
  { 0x0a2a, 0x0a30 },
  { 0x0a32, 0x0a33 },
  { 0x0a35, 0x0a36 },
  { 0x0a38, 0x0a39 },
  { 0x0a59, 0x0a5c },
  { 0x0a5e },
  { 0x0a72, 0x0a74 },
  { 0x0a85, 0x0a8b },
  { 0x0a8d },
  { 0x0a8f, 0x0a91 },
  { 0x0a93, 0x0aa8 },
  { 0x0aaa, 0x0ab0 },
  { 0x0ab2, 0x0ab3 },
  { 0x0ab5, 0x0ab9 },
  { 0x0abd },
  { 0x0ae0 },
  { 0x0b05, 0x0b0c },
  { 0x0b0f, 0x0b10 },
  { 0x0b13, 0x0b28 },
  { 0x0b2a, 0x0b30 },
  { 0x0b32, 0x0b33 },
  { 0x0b36, 0x0b39 },
  { 0x0b3d },
  { 0x0b5c, 0x0b5d },
  { 0x0b5f, 0x0b61 },
  { 0x0b85, 0x0b8a },
  { 0x0b8e, 0x0b90 },
  { 0x0b92, 0x0b95 },
  { 0x0b99, 0x0b9a },
  { 0x0b9c },
  { 0x0b9e, 0x0b9f },
  { 0x0ba3, 0x0ba4 },
  { 0x0ba8, 0x0baa },
  { 0x0bae, 0x0bb5 },
  { 0x0bb7, 0x0bb9 },
  { 0x0c05, 0x0c0c },
  { 0x0c0e, 0x0c10 },
  { 0x0c12, 0x0c28 },
  { 0x0c2a, 0x0c33 },
  { 0x0c35, 0x0c39 },
  { 0x0c60, 0x0c61 },
  { 0x0c85, 0x0c8c },
  { 0x0c8e, 0x0c90 },
  { 0x0c92, 0x0ca8 },
  { 0x0caa, 0x0cb3 },
  { 0x0cb5, 0x0cb9 },
  { 0x0cde },
  { 0x0ce0, 0x0ce1 },
  { 0x0d05, 0x0d0c },
  { 0x0d0e, 0x0d10 },
  { 0x0d12, 0x0d28 },
  { 0x0d2a, 0x0d39 },
  { 0x0d60, 0x0d61 },
  { 0x0e01, 0x0e2e },
  { 0x0e30 },
  { 0x0e32, 0x0e33 },
  { 0x0e40, 0x0e45 },
  { 0x0e81, 0x0e82 },
  { 0x0e84 },
  { 0x0e87, 0x0e88 },
  { 0x0e8a },
  { 0x0e8d },
  { 0x0e94, 0x0e97 },
  { 0x0e99, 0x0e9f },
  { 0x0ea1, 0x0ea3 },
  { 0x0ea5 },
  { 0x0ea7 },
  { 0x0eaa, 0x0eab },
  { 0x0ead, 0x0eae },
  { 0x0eb0 },
  { 0x0eb2, 0x0eb3 },
  { 0x0ebd },
  { 0x0ec0, 0x0ec4 },
  { 0x0f40, 0x0f47 },
  { 0x0f49, 0x0f69 },
  { 0x10a0, 0x10c5 },
  { 0x10d0, 0x10f6 },
  { 0x1100 },
  { 0x1102, 0x1103 },
  { 0x1105, 0x1107 },
  { 0x1109 },
  { 0x110b, 0x110c },
  { 0x110e, 0x1112 },
  { 0x113c },
  { 0x113e },
  { 0x1140 },
  { 0x114c },
  { 0x114e },
  { 0x1150 },
  { 0x1154, 0x1155 },
  { 0x1159 },
  { 0x115f, 0x1161 },
  { 0x1163 },
  { 0x1165 },
  { 0x1167 },
  { 0x1169 },
  { 0x116d, 0x116e },
  { 0x1172, 0x1173 },
  { 0x1175 },
  { 0x119e },
  { 0x11a8 },
  { 0x11ab },
  { 0x11ae, 0x11af },
  { 0x11b7, 0x11b8 },
  { 0x11ba },
  { 0x11bc, 0x11c2 },
  { 0x11eb },
  { 0x11f0 },
  { 0x11f9 },
  { 0x1e00, 0x1e9b },
  { 0x1ea0, 0x1ef9 },
  { 0x1f00, 0x1f15 },
  { 0x1f18, 0x1f1d },
  { 0x1f20, 0x1f45 },
  { 0x1f48, 0x1f4d },
  { 0x1f50, 0x1f57 },
  { 0x1f59 },
  { 0x1f5b },
  { 0x1f5d },
  { 0x1f5f, 0x1f7d },
  { 0x1f80, 0x1fb4 },
  { 0x1fb6, 0x1fbc },
  { 0x1fbe },
  { 0x1fc2, 0x1fc4 },
  { 0x1fc6, 0x1fcc },
  { 0x1fd0, 0x1fd3 },
  { 0x1fd6, 0x1fdb },
  { 0x1fe0, 0x1fec },
  { 0x1ff2, 0x1ff4 },
  { 0x1ff6, 0x1ffc },
  { 0x2126 },
  { 0x212a, 0x212b },
  { 0x212e },
  { 0x2180, 0x2182 },
  { 0x3041, 0x3094 },
  { 0x30a1, 0x30fa },
  { 0x3105, 0x312c },
  { 0xac00, 0xd7a3 },
  /* Ideographic */
  { 0x4e00, 0x9fa5 },
  { 0x3007 },
  { 0x3021, 0x3029 },
};

const SCharRange kNameChars[] = {
  { '.' },
  { '-' },
  /* CombiningChar */
  { 0x0300, 0x0345 },
  { 0x0360, 0x0361 },
  { 0x0483, 0x0486 },
  { 0x0591, 0x05a1 },
  { 0x05a3, 0x05b9 },
  { 0x05bb, 0x05bd },
  { 0x05bf },
  { 0x05c1, 0x05c2 },
  { 0x05c4 },
  { 0x064b, 0x0652 },
  { 0x0670 },
  { 0x06d6, 0x06dc },
  { 0x06dd, 0x06df },
  { 0x06e0, 0x06e4 },
  { 0x06e7, 0x06e8 },
  { 0x06ea, 0x06ed },
  { 0x0901, 0x0903 },
  { 0x093c },
  { 0x093e, 0x094c },
  { 0x094d },
  { 0x0951, 0x0954 },
  { 0x0962, 0x0963 },
  { 0x0981, 0x0983 },
  { 0x09bc },
  { 0x09be },
  { 0x09bf },
  { 0x09c0, 0x09c4 },
  { 0x09c7, 0x09c8 },
  { 0x09cb, 0x09cd },
  { 0x09d7 },
  { 0x09e2, 0x09e3 },
  { 0x0a02 },
  { 0x0a3c },
  { 0x0a3e },
  { 0x0a3f },
  { 0x0a40, 0x0a42 },
  { 0x0a47, 0x0a48 },
  { 0x0a4b, 0x0a4d },
  { 0x0a70, 0x0a71 },
  { 0x0a81, 0x0a83 },
  { 0x0abc },
  { 0x0abe, 0x0ac5 },
  { 0x0ac7, 0x0ac9 },
  { 0x0acb, 0x0acd },
  { 0x0b01, 0x0b03 },
  { 0x0b3c },
  { 0x0b3e, 0x0b43 },
  { 0x0b47, 0x0b48 },
  { 0x0b4b, 0x0b4d },
  { 0x0b56, 0x0b57 },
  { 0x0b82, 0x0b83 },
  { 0x0bbe, 0x0bc2 },
  { 0x0bc6, 0x0bc8 },
  { 0x0bca, 0x0bcd },
  { 0x0bd7 },
  { 0x0c01, 0x0c03 },
  { 0x0c3e, 0x0c44 },
  { 0x0c46, 0x0c48 },
  { 0x0c4a, 0x0c4d },
  { 0x0c55, 0x0c56 },
  { 0x0c82, 0x0c83 },
  { 0x0cbe, 0x0cc4 },
  { 0x0cc6, 0x0cc8 },
  { 0x0cca, 0x0ccd },
  { 0x0cd5, 0x0cd6 },
  { 0x0d02, 0x0d03 },
  { 0x0d3e, 0x0d43 },
  { 0x0d46, 0x0d48 },
  { 0x0d4a, 0x0d4d },
  { 0x0d57 },
  { 0x0e31 },
  { 0x0e34, 0x0e3a },
  { 0x0e47, 0x0e4e },
  { 0x0eb1 },
  { 0x0eb4, 0x0eb9 },
  { 0x0ebb, 0x0ebc },
  { 0x0ec8, 0x0ecd },
  { 0x0f18, 0x0f19 },
  { 0x0f35 },
  { 0x0f37 },
  { 0x0f39 },
  { 0x0f3e },
  { 0x0f3f },
  { 0x0f71, 0x0f84 },
  { 0x0f86, 0x0f8b },
  { 0x0f90, 0x0f95 },
  { 0x0f97 },
  { 0x0f99, 0x0fad },
  { 0x0fb1, 0x0fb7 },
  { 0x0fb9 },
  { 0x20d0, 0x20dc },
  { 0x20e1 },
  { 0x302a, 0x302f },
  { 0x3099 },
  { 0x309a },
  /* Digit */
  { 0x0030, 0x0039 },
  { 0x0660, 0x0669 },
  { 0x06f0, 0x06f9 },
  { 0x0966, 0x096f },
  { 0x09e6, 0x09ef },
  { 0x0a66, 0x0a6f },
  { 0x0ae6, 0x0aef },
  { 0x0b66, 0x0b6f },
  { 0x0be7, 0x0bef },
  { 0x0c66, 0x0c6f },
  { 0x0ce6, 0x0cef },
  { 0x0d66, 0x0d6f },
  { 0x0e50, 0x0e59 },
  { 0x0ed0, 0x0ed9 },
  { 0x0f20, 0x0f29 },
  /* Extender */
  { 0xb7 },
  { 0x02d0 },
  { 0x02d1 },
  { 0x0387 },
  { 0x0640 },
  { 0x0e46 },
  { 0x0ec6 },
  { 0x3005 },
  { 0x3031, 0x3035 },
  { 0x309d, 0x309e },
  { 0x30fc, 0x30fe },
};


// ===================================================================================
//	class XXMLLexer
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLLexer::~XXMLLexer
//
//---------------------------------------------------------------
XXMLLexer::~XXMLLexer()
{
	delete mScanner;

	mParser = nil;
}


//---------------------------------------------------------------
//
// XXMLLexer::XXMLLexer
//
//---------------------------------------------------------------
XXMLLexer::XXMLLexer(XXMLParser* parser, XScanner* takeScanner, const XURI& uri, bool internal) : mURI(uri)
{
	PRECONDITION(parser != nil);
	PRECONDITION(takeScanner != nil);
	
	mParser = parser;
	mScanner = takeScanner;
	mInternal = internal;
	
	mExpandPEs = 0;
	mCurrentEnd = ULONG_MAX;		

	mScanner->ClearWhiteSpace();	
	mScanner->SetWhiteSpace(kTabChar);	
	mScanner->SetWhiteSpace(kLineFeedChar);	
	mScanner->SetWhiteSpace(kReturnChar);	
	mScanner->SetWhiteSpace(kSpaceChar);	
}


//---------------------------------------------------------------
//
// XXMLLexer::Reset
//
//---------------------------------------------------------------
void XXMLLexer::Reset()
{
	mTokens.clear();
	
	mScanner->Reset();
}


//---------------------------------------------------------------
//
// XXMLLexer::UpdateLanguage
//
//---------------------------------------------------------------
void XXMLLexer::UpdateLanguage(const std::wstring& newText)
{
	XScannerPos pos = mScanner->GetPosition();
	
	mScanner->SetText(newText);
	mScanner->SetPosition(pos);
}


//---------------------------------------------------------------
//
// XXMLLexer::ExpandInternalParameterReferences
//
//---------------------------------------------------------------
void XXMLLexer::ExpandInternalParameterReferences(bool expand)
{
	if (expand)
		mExpandPEs++;
	else
		mExpandPEs--;
}


//---------------------------------------------------------------
//
// XXMLLexer::AtEnd
//
//---------------------------------------------------------------
bool XXMLLexer::AtEnd() const
{
	bool atEnd = false;
	
	if (mScanner->AtEnd())
		if (mTokens.empty())
			atEnd = true;
	
	return atEnd;
}


//---------------------------------------------------------------
//
// XXMLLexer::ReadToken
//
//---------------------------------------------------------------
void XXMLLexer::ReadToken()
{
	mTokens.clear();
	
	this->DoReadTokens(ULONG_MAX);
	
	if (!mScanner->AtEnd())
		if (mTokens.empty())
			mScanner->Advance();
		else
			mScanner->Advance(numeric_cast<int32>(mTokens.back().length));

	if (mExpandPEs > 0 && !mTokens.empty()) {
		XScannerPos oldPos = mTokens.back().pos;	

		TokenNum token = mTokens.back().num;
		if (token == kPercentToken) {
			if (this->DoExpandPE()) {
				mScanner->SetPosition(oldPos);
				this->ReadToken();
			}
				
		} else if (token == kWhiteSpaceToken) {			// when expanding a PE whitespace is added to the replacement text
			if (mScanner->Peek() == '%') {				// this can cause problems in the parsers because we can wind up with two whitespace tokens in a row
				if (mScanner->Peek(1) > kSpaceChar) {	// so we use this ugly code to merge the tokens...					
					this->ReadToken();
					
					mScanner->SetPosition(oldPos);
					if (this->GetToken().num != kPercentToken)
						this->ReadToken();
				}
			}
		}
	}
}


//---------------------------------------------------------------
//
// XXMLLexer::ReadFixedToken
//
//---------------------------------------------------------------
void XXMLLexer::ReadFixedToken(uint32 maxChars)
{
	mTokens.clear();
	
	this->DoReadTokens(maxChars);
	ASSERT(mTokens.empty() || mTokens.back().length <= maxChars);
		
	if (!mScanner->AtEnd())
		if (mTokens.empty())
			mScanner->Advance();
		else
			mScanner->Advance(numeric_cast<int32>(mTokens.back().length));

	if (mExpandPEs > 0) {
		XScannerPos oldPos = mTokens.back().pos;	

		if (this->GetToken().num == kPercentToken) {
			if (this->DoExpandPE()) {
				mScanner->SetPosition(oldPos);
				this->ReadFixedToken(maxChars);
			}
			
		} else if (this->GetToken().num == kWhiteSpaceToken) {	// when expanding a PE whitespace is added to the replacement text
			if (mScanner->Peek() == '%') {						// this can cause problems in the parsers because we can wind up with two whitespace tokens in a row
				if (mScanner->Peek(1) > kSpaceChar) {			// so we use this ugly code to merge the tokens...					
					this->ReadToken();
					
					mScanner->SetPosition(oldPos);
					if (this->GetToken().num != kPercentToken)
						this->ReadToken();
				}
			}
		}
	}
}


//---------------------------------------------------------------
//
// XXMLLexer::GetText
//
// CW 5 won't inline this...
//
//---------------------------------------------------------------
std::wstring XXMLLexer::GetText(const SXMLToken& token) const					
{
	return mScanner->GetText().substr(token.pos.GetIndex(), token.length);
}


//---------------------------------------------------------------
//
// XXMLLexer::SkipToken
//
//---------------------------------------------------------------
void XXMLLexer::SkipToken(const SXMLToken& token)
{
	int32 delta = numeric_cast<int32>(mTokens.back().length - token.length);

	if (delta > 0)
		mScanner->Advance(-delta);
}


//---------------------------------------------------------------
//
// XXMLLexer::GetToken
//
//---------------------------------------------------------------
SXMLToken XXMLLexer::GetToken(uint32 index) const
{	
	SXMLToken token;
	
	if (!mTokens.empty())
		token = mTokens[mTokens.size() - index - 1];
	
	else {
		token.num    = kNoToken;
		token.pos    = mScanner->GetPosition();
		token.length = ULONG_MAX;
	}
	
	return token;
}


//---------------------------------------------------------------
//
// XXMLLexer::FindToken
//
//---------------------------------------------------------------
bool XXMLLexer::FindToken(TokenNum num, SXMLToken& token) const
{
	bool found = false;
	
	for (uint32 index = 0; index < mTokens.size() && !found; ++index) {
		const SXMLToken& candidate = mTokens[index];
		
		if (num == candidate.num) {
			token = candidate;
			found = true;
		}
	}
	
	return found;
}


//---------------------------------------------------------------
//
// XXMLLexer::IsLetter									[static]
//
// [85] Letter ::= BaseChar | Ideographic
//
// There are lot's of ways to handle this. I chose to use one of
// the simpler methods: a 65536 element lookup table. The only
// problem with this is the size of the table and the accompanying
// concern about thrashing the cache. The table size is of minor
// concern on OS's with decent VM (on the Mac the MSL in Pro4
// specializes vector<bool> so that the bits are packed into a
// byte so the table sizes are only 8K each which isn't too bad).
// Cache thrashing is independant of the table size: what matters
// is the amount of the table that actually gets used. For documents
// written in most of the European languages the number of elements
// used will be much less than 100 which won't affect the cache
// much even if vector<bool> isn't specialized. However documents
// written in some of the Asian languages could wind up using a
// *lot* of characters which could cause some cache thrashing.
//
//---------------------------------------------------------------
bool XXMLLexer::IsLetter(wchar_t ch)
{
	static std::vector<bool> letters;
	
	if (letters.size() == 0) {
		letters.resize(65536, false);
				
		for (uint32 i = 0; i < sizeof(kLetterChars)/sizeof(SCharRange); ++i) {
			letters[kLetterChars[i].start] = true;
			
			if (kLetterChars[i].end != 0) 
				for (uint32 n = kLetterChars[i].start + 1UL; n <= kLetterChars[i].end; ++n) 
					letters[n] = true;
		}
	}
			
	bool is = letters[ch];
	
	return is;
}


//---------------------------------------------------------------
//
// XXMLLexer::IsNameChar								[static]
//
// [4] NameChar ::= Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
//
//---------------------------------------------------------------
bool XXMLLexer::IsNameChar(wchar_t ch)
{
	static std::vector<bool> names;
	
	if (names.size() == 0) {
		names.resize(65536, false);
				
		for (uint32 i = 0; i < sizeof(kNameChars)/sizeof(SCharRange); ++i) {
			names[kNameChars[i].start] = true;
			
			if (kNameChars[i].end != 0) 
				for (uint32 n = kNameChars[i].start + 1UL; n <= kNameChars[i].end; ++n) 
					names[n] = true;
		}
	}
			
	bool is = names[ch] || IsLetter(ch);
	
	return is;
}


//---------------------------------------------------------------
//
// XXMLLexer::IsChar									[static]
//
// [2] Char ::= #x9 | #xA | #xD | [#x20-#D7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
//
//---------------------------------------------------------------
bool XXMLLexer::IsChar(wchar_t ch)
{			
	bool is = ch == 0x9 || ch == 0xA || ch == 0xD || 
			  (ch >=    0x20 && ch <=   0xD7FF) ||
			  (ch >=  0xE000 && ch <=   0xFFFD);
			  
	if (!is && ch >= 0xD800 && ch <= 0x0DFFF) {
		uint32 a = 0xD800;
		uint32 b = (0x00FFFF >> 10) + 0xD800;
		uint32 n = (ch >> 10) + 0xD800UL;
		is = n >= a && n <= b;
		
		if (!is) {
			a = 0xDC00;
			b = (0x00FFFF & 0x3FF) + 0xDC00;
			n = (ch & 0x3FF) + 0xDC00UL;
			is = n >= a && n <= b;
		}
	}
	
	return is;
}

#if __MWERKS__
#pragma mark �
#endif

//---------------------------------------------------------------
//
// XXMLLexer::DoExpandPE
//
// Parameter references are hard to handle gracefully because they
// can appear *anywhere* in the DTD. It's possible to deal with
// them within the parser code, but this really clutters the code.
// To avoid this we handle them within the lexer (which creates
// some ugly coupling with XXMLReader).
//
//---------------------------------------------------------------
bool XXMLLexer::DoExpandPE()
{
	PRECONDITION(mExpandPEs > 0);
	
	std::vector<SXMLToken> oldTokens = mTokens;
	XScannerPos oldPos = mScanner->GetPosition();

	// '%'
	ASSERT(mTokens.back().num == kPercentToken);
	uint32 index = mTokens.back().pos.GetIndex();	
	this->ReadToken();
	
	// Name
	bool expanded = false;
	SXMLToken token = this->GetToken();
	std::wstring name = this->GetText(token);	
	if (token.num == kAlphaNumToken && (XXMLLexer::IsLetter(name[0]) || name[0] == '_' || name[0] == ':')) {
		this->ReadToken();
		
		// ';'
		token = this->GetToken();
		if (token.num == kSemiColonToken) {
			if (mInternal)
				XXMLException::Throw(this, "Parameter entities can only be referenced inside markup within the external DTD.");
		
			// Find the replacement text
			const XXMLParser::SEntity& entity = mParser->GetParameterEntity(name);
			ASSERT(entity.id.dataType.length() == 0);			// parameter entities cannot be unparsed
			
			std::wstring replacement;
			if (entity.internal) {
				replacement = entity.text;
				
			} else {
				XAutoPtr<XResource> data(entity.Load(mURI));
				if (data.Get() == nil)
					XXMLException::Throw(this, "Couldn't find the external parameter entity named: '#1'",  name);
			
				XAutoPtr<XXMLLexer> temp(mParser->CreateLexer(*data, false));		// $$ yuck
				replacement = temp->GetScanner()->GetText();
			}
			
			// Check for proper nesting
			int32 count1 = std::count(replacement.begin(), replacement.end(), '<');	// VC: Proper PE Nesting
			int32 count2 = std::count(replacement.begin(), replacement.end(), '>');	// $$ may not be sufficient...
			if (count1 != count2)
				XXMLException::Throw(this, "'#1' has mismatched '<' and '>'.", name);	

			count1 = std::count(replacement.begin(), replacement.end(), '(');
			count2 = std::count(replacement.begin(), replacement.end(), ')');
			if (count1 != count2)
				XXMLException::Throw(this, "'#1' has mismatched '(' and ')'.", name);	

			// Replace the reference with the replacement text
			if (mCurrentEntity.length() == 0 || index >= mCurrentEnd) {
				mCurrentEntity = name;
				mCurrentEnd = index + replacement.length() + 3;
				
			} else if (mCurrentEntity == name)								// WFC: No Recursion
				XXMLException::Throw(this, "'#1' has a recursive definition.", name);	
			
			else if (mCurrentEnd < ULONG_MAX)
				mCurrentEnd += replacement.length() + 2;

			mScanner->GetText().replace(index, name.length() + 2, L" " + replacement + L" ");	// included as PE (this shouldn't be too inefficient since it can only happen inside the external DTD which will almost never be huge)

			expanded = true;
		}
	}
		
	if (!expanded) {
		mTokens = oldTokens;
		mScanner->SetPosition(oldPos);
	}
	
	return expanded;
}


//---------------------------------------------------------------
//
// XXMLLexer::DoReadTokens
//
// $$ This is the primary bottle neck for the XML parser. When
// $$ parsing the content of a ten thousand line XML file 30% of 
// $$ the time was spent inside this function (the next largest
// $$ bottle neck was XMemoryHeap::Deallocate at 7.5% which could
// $$ probably be improved with a better allocator (I was using
// $$ XSimpleAllocator which starts to perform poorly as the
// $$ operator new heap fragments)).
//
//---------------------------------------------------------------
void XXMLLexer::DoReadTokens(uint32 maxChars)
{
	PRECONDITION(maxChars > 0);
	
	XScannerPos pos = mScanner->GetPosition();
	
	wchar_t ch;
	int32 index;
	uint32 count;
	
	wchar_t ch0 = mScanner->Peek(0);
	wchar_t ch1 = maxChars >= 2 ? mScanner->Peek(1) : L'\0';
	wchar_t ch2 = maxChars >= 3 ? mScanner->Peek(2) : L'\0';

	std::wstring text(1, ch0);

	switch (ch0) {
		case '<':
		case '#':
			{
				wchar_t ch3 = maxChars >= 4 ? mScanner->Peek(3) : L'\0';
				wchar_t ch4 = maxChars >= 5 ? mScanner->Peek(4) : L'\0';
				wchar_t ch5 = maxChars >= 6 ? mScanner->Peek(5) : L'\0';
				wchar_t ch6 = maxChars >= 7 ? mScanner->Peek(6) : L'\0';
				wchar_t ch7 = maxChars >= 8 ? mScanner->Peek(7) : L'\0';
				wchar_t ch8 = maxChars >= 9 ? mScanner->Peek(8) : L'\0';
				wchar_t ch9 = maxChars >= 10 ? mScanner->Peek(9) : L'\0';
	
				if (ch0 == '<') {
					mTokens.push_back(SXMLToken(kCharToken, pos, 1));
					mTokens.push_back(SXMLToken(kLessToken, pos, 1));
					
					if (maxChars > 1) {
						if (ch1 == '/') {
							mTokens.push_back(SXMLToken(kEndTagToken, pos, 2));
							
						} else if (ch1 == '?') {
							mTokens.push_back(SXMLToken(kPIStartToken, pos, 2));
							if (ch2 == 'x' && ch3 == 'm' && ch4 == 'l')
								mTokens.push_back(SXMLToken(kXMLTagToken, pos, 5));
						
						} else if (ch1 == '!') {
							if (ch2 == '[') {
								mTokens.push_back(SXMLToken(kDataStartToken, pos, 3));
								if (ch3 == 'C' && ch4 == 'D' && ch5 == 'A' && ch6 == 'T' && ch7 == 'A' && ch8 == '[')
									mTokens.push_back(SXMLToken(kCDSectTagToken, pos, 9));			
							
							} else if (ch2 == '-' && ch3 == '-')
								mTokens.push_back(SXMLToken(kCommentStartToken, pos, 4));
			
							else if (ch2 == 'A' && ch3 == 'T' && ch4 == 'T' && ch5 == 'L' && ch6 == 'I' && ch7 == 'S' && ch8 == 'T')
								mTokens.push_back(SXMLToken(kAttListTagToken, pos, 9));
								
							else if (ch2 == 'D' && ch3 == 'O' && ch4 == 'C' && ch5 == 'T' && ch6 == 'Y' && ch7 == 'P' && ch8 == 'E')
								mTokens.push_back(SXMLToken(kDocTypeTagToken, pos, 9));
								
							else if (ch2 == 'E') {
								if (ch3 == 'N' && ch4 == 'T' && ch5 == 'I' && ch6 == 'T' && ch7 == 'Y')
									mTokens.push_back(SXMLToken(kEntityTagToken, pos, 8));
								else if (ch3 == 'L' && ch4 == 'E' && ch5 == 'M' && ch6 == 'E' && ch7 == 'N' && ch8 == 'T')
									mTokens.push_back(SXMLToken(kElementTagToken, pos, 9));
			
							} else if (ch2 == 'N' && ch3 == 'O' && ch4 == 'T' && ch5 == 'A' && ch6 == 'T' && ch7 == 'I' && ch8 == 'O' && ch9 == 'N')
								mTokens.push_back(SXMLToken(kNotationTagToken, pos, 10));
						}
					}
				
				} else {
					mTokens.push_back(SXMLToken(kCharToken, pos, 1));
					mTokens.push_back(SXMLToken(kPoundToken, pos, 1));
					
					if (maxChars > 1) {
						if (ch1 == 'F' && ch2 == 'I' && ch3 == 'X' && ch4 == 'E' && ch5 == 'D')
							mTokens.push_back(SXMLToken(kFixedToken, pos, 6));
			
						else if (ch1 == 'I' && ch2 == 'M' && ch3 == 'P' && ch4 == 'L' && ch5 == 'I' && ch6 == 'E' && ch7 == 'D')
							mTokens.push_back(SXMLToken(kImpliedToken, pos, 8));
			
						else if (ch1 == 'P' && ch2 == 'C' && ch3 == 'D' && ch4 == 'A' && ch5 == 'T' && ch6 == 'A')
							mTokens.push_back(SXMLToken(kPCDataToken, pos, 7));
			
						else if (ch1 == 'R' && ch2 == 'E' && ch3 == 'Q' && ch4 == 'U' && ch5 == 'I' && ch6 == 'R' && ch7 == 'E' && ch8 == 'D')
							mTokens.push_back(SXMLToken(kRequiredToken, pos, 9));
					}
				}
			}
			break;

		case '>':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kGreaterToken, pos, 1));
			break;
		
		case '/':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			if (ch1 == '>')
				mTokens.push_back(SXMLToken(kEmptyTagToken, pos, 2));
			break;
		
		case '?':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kQuestionToken, pos, 1));
			if (ch1 == '>')
				mTokens.push_back(SXMLToken(kPIEndToken, pos, 2));
			break;
		
		case '%':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kPercentToken, pos, 1));
			break;
		
		case '&':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kAmpersandToken, pos, 1));
			break;
		
		case '-':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			if (ch1 == '-' && ch2 == '>')
				mTokens.push_back(SXMLToken(kCommentEndToken, pos, 3));
			break;
		
		case '+':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kPlusToken, pos, 1));
			break;
		
		case '*':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kStarToken, pos, 1));
			break;
		
		case '|':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kBarToken, pos, 1));
			break;
		
		case '\'':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kSingleQuoteToken, pos, 1));
			break;
		
		case '"':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kDoubleQuoteToken, pos, 1));
			break;
		
		case ',':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kCommaToken, pos, 1));
			break;
		
		case ';':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kSemiColonToken, pos, 1));
			break;
		
		case ')':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kRightParensToken, pos, 1));
			break;
		
		case '(':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kLeftParensToken, pos, 1));
			break;
		
		case ']':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kRightBracketToken, pos, 1));
			if (ch1 == ']' && ch2 == '>')
				mTokens.push_back(SXMLToken(kDataEndToken, pos, 3));
			break;
		
		case '[':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			mTokens.push_back(SXMLToken(kLeftBracketToken, pos, 1));
			break;
		
		case '=':
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			
			if (maxChars > 1) {
				count = 1;
				
				index = 1;
				ch = mScanner->Peek(index);
				while (ch == kSpaceChar || ch == kTabChar || ch == kReturnChar || ch == kLineFeedChar) {
					++count;
					ch = mScanner->Peek(++index);
				}
				
				mTokens.push_back(SXMLToken(kEqualToken, pos, count));
			}
			break;

		case kSpaceChar:
		case kTabChar:
		case kReturnChar:
		case kLineFeedChar:
			mTokens.push_back(SXMLToken(kCharToken, pos, 1));
			
			if (maxChars > 1) {
				count = 1;
				
				index = 1;
				ch = mScanner->Peek(index);
				while (ch == kSpaceChar || ch == kTabChar || ch == kReturnChar || ch == kLineFeedChar) {
					++count;
					ch = mScanner->Peek(++index);
				}
				
				mTokens.push_back(SXMLToken(kWhiteSpaceToken, pos, count));
				
				if (ch == '=') {
					++count;
					ch = mScanner->Peek(++index);
					while (ch == kSpaceChar || ch == kTabChar || ch == kReturnChar || ch == kLineFeedChar) {
						++count;
						ch = mScanner->Peek(++index);
					}
	
					mTokens.push_back(SXMLToken(kEqualToken, pos, count));
				}
			}
			break;

		default:
			if (IsChar(ch0))
				mTokens.push_back(SXMLToken(kCharToken, pos, 1));
	}		

	// kAlphaNumToken
	if (maxChars == ULONG_MAX) {		// this is a bit weird, but we don't really want to match a fixed length name if the following characters are also valid name characters...
		index = 0;
		count = 0;
	
		ch = ch0;
		while (IsNameChar(ch)) {
			++count;
			ch = mScanner->Peek(++index);
		}
		
		if (count > 0)
			if (mTokens.empty() || count >= mTokens.back().length)
				mTokens.push_back(SXMLToken(kAlphaNumToken, pos, count));
	}
}


}	// namespace Whisper

