/*
 *  File:		XXMLParser.cpp
 *  Summary:	Classes used to parse XML documents.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLParser.cpp,v $
 *		Revision 1.4  2001/04/27 04:39:34  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/01/21 00:48:19  jesjones
 *		Improved the error message in the event transcoding the XML fails.
 *		
 *		Revision 1.2  2000/11/10 04:47:36  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <6>	 3/22/00	JDJ		Added support for Shift-JIS and EUC-JP language encodings (Mac only).
 *		 <5>	 6/25/99	JDJ		__MSL_FIX_ITERATORS__ is only used in profile and debug builds.
 *		 <4>	 6/21/99	JDJ		Added __MSL_FIX_ITERATORS__ (for Pro 3).
 *		 <3>	 5/07/99	JDJ		UpdateLexer allows ISO names to be used for US-ASCII, utf-8, and utf-16.
 *		 <2>	 3/19/99	JDJ		Added AddID.
 *		 <1>	 2/01/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLParser.h>

#include <XLocker.h>
#include <XMiscUtils.h>
#include <XResource.h>
#include <XStringUtils.h>
#include <XTextTranscoders.h>		
#include <XURIResolver.h>
#include <XXMLCallback.h>
#include <XXMLContentParser.h>
#include <XXMLDTDParser.h>
#include <XXMLLexer.h>

namespace Whisper {


// ===================================================================================
//	class XXMLException
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLException::XXMLException 
//
//---------------------------------------------------------------
XXMLException::XXMLException(const XXMLLexer* lexer, const std::wstring& str) : std::runtime_error(ToUTF8Str(str))
{
	PRECONDITION(lexer != nil);
	
	mPos = lexer->GetScanner()->GetPosition();
	mURI = lexer->GetURI();
}


//---------------------------------------------------------------
//
// XXMLException::Throw (const XXMLLexer*, const char*)	[static]
//
//---------------------------------------------------------------
void XXMLException::Throw(const XXMLLexer* lexer, const char* str)
{
	throw XXMLException(lexer, LoadWhisperString(FromUTF8Str(str)));
}


//---------------------------------------------------------------
//
// XXMLException::Throw (const XXMLLexer*, const char*, wstring) [static]
//
//---------------------------------------------------------------
void XXMLException::Throw(const XXMLLexer* lexer, const char* format, const std::wstring& arg1)
{
	std::wstring str = LoadWhisperString(FromUTF8Str(format), arg1);
	
	throw XXMLException(lexer, str);
}


//---------------------------------------------------------------
//
// XXMLException::Throw (const XXMLLexer*, const char*, wstring, wstring) [static]
//
//---------------------------------------------------------------
void XXMLException::Throw(const XXMLLexer* lexer, const char* format, const std::wstring& arg1, const std::wstring& arg2)
{
	std::wstring str = LoadWhisperString(FromUTF8Str(format), arg1, arg2);
	
	throw XXMLException(lexer, str);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	struct XXMLParser::SEntity														
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLParser::SEntity::Load
//
//---------------------------------------------------------------
XResource* XXMLParser::SEntity::Load(const XURI& base) const
{
	PRECONDITION(!internal);
	
	XURI systemURI(base, id.systemID);
	XResource* data = XURIResolver::Instance()->Resolve(systemURI);
	
	if (data == nil) {
		XURI publicURI(base, id.publicID);
		data = XURIResolver::Instance()->Resolve(publicURI);
	}
	
	return data;
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLParser														
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLParser::~XXMLParser
//
//---------------------------------------------------------------
XXMLParser::~XXMLParser()
{
	delete mLexer;
	
	mData->RemoveReference();
	mData = nil;
	mCallback = nil;
}


//---------------------------------------------------------------
//
// XXMLParser::XXMLParser
//
//---------------------------------------------------------------
XXMLParser::XXMLParser(XResource* data)
{
	PRECONDITION(data != nil);
	
	mData = data;
	mData->AddReference();
	
	mCallback = nil;
	mOptimized = false;
	mStandAlone = false;
			
	mWarningMode = kLogWarnings;

	try {
		mLexer = nil;
		mLexer = this->CreateLexer(*mData, true);
		
	} catch (...) {
		mData->RemoveReference();		// add and remove ref in case it was created with a zero ref count
		throw;
	}
}


//---------------------------------------------------------------
//
// XXMLParser::Parse
//
// [1] document ::= prolog element Misc*
//
//---------------------------------------------------------------
void XXMLParser::Parse(XXMLCallbackMixin& callback)
{
	mLexer->Reset();
	mCallback = &callback;
	
	mDTD.Reset(nil);					// will be created when and if a doctypedecl is parsed
		
	mLexer->ReadToken();
	mCallback->OnBeginDocument();
	
	std::wstring docName;
	{
		{
		ZPrologParser proParser(this, mLexer, mData, mOptimized);
		docName = proParser.Parse();
		}
		
		{
		ZElementParser elemParser(this, mLexer, nil);
		elemParser.Parse();
		}
		
		ZMiscBlockParser miscParser(this, mLexer);
		miscParser.Parse();

		if (mDTD.Get() != nil)
			this->DoValidateRefs();	
	}

	if (!mLexer->AtEnd())
		XXMLException::Throw(mLexer, "Found extraneous characters at the end of the document.");
				
	mCallback->OnEndDocument(docName);
			
	mCallback = nil;
}


//---------------------------------------------------------------
//
// XXMLParser::CreateLexer				
//
// If the first word of an XML document is 0xFEFF then it's
// encoded using big endian UTF-16. If the first word is 0xFFFE
// then it's in little endian UTF-16. Otherwise the document is
// assumed to be in UTF-8 until an EncodingDecl is read saying
// otherwise.
//
//---------------------------------------------------------------
XXMLLexer* XXMLParser::CreateLexer(XResource& data, bool internal)
{	
	std::wstring text;
	
	uint32 bytes = data.GetSize();
	if (bytes > 0) {
	
		// Convert the data into a string
		XLocker lock(data);
		
		wchar_t* header = (wchar_t*) data.GetPtr();			// note that the in-place byte swap may not be persistent (eg if an XResource subclass purges and reloads)
		if (data.GetPtr()[0] == 0xFE && data.GetPtr()[1] == 0xFF) {
#if !BIG_ENDIAN
			for (uint32 index = 0; index < bytes/2; ++index)
				ByteSwap(header[index]);
#endif
			if (!XUTF8Transcoder::IsValidUTF16(header + 1, bytes/2 - 1))
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Found invalid utf-16 characters in #1.", data.GetURI().GetAddress())));

			text = std::wstring(header + 1, bytes/2 - 1);
		
		} else if (data.GetPtr()[0] == 0xFF && data.GetPtr()[1] == 0xFE) {
#if BIG_ENDIAN
			for (uint32 index = 0; index < bytes/2; ++index)
				ByteSwap(header[index]);
#endif
			if (!XUTF8Transcoder::IsValidUTF16(header + 1, bytes/2 - 1))
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Found invalid utf-16 characters in #1.", data.GetURI().GetAddress())));

			text = std::wstring(header + 1, bytes/2 - 1);
		
		} else { 		
			XNaiveTranscoder transcoder;				// we'll convert the text for real when we check for a language encoding (if one isn't found we'll use utf-8)
			text = FromPlatformStr((const char*) data.GetPtr(), bytes, &transcoder);
		}

		text = Replace(text, L"\r\n", L"\n");			// this is tough to handle inside the parser because CRs are supposed to be normalized unless they're the result of entity expansion...
		text = Replace(text, L"\r", L"\n");
	}
	
	return new XXMLLexer(this, new XScanner(text), data.GetURI(), internal);
}


//---------------------------------------------------------------
//
// XXMLParser::UpdateLexer							
//
//---------------------------------------------------------------
void XXMLParser::UpdateLexer(const XResource& data, XXMLLexer* lexer, const std::wstring& inLanguage)
{
	PRECONDITION(lexer != nil);
	
	try {
		std::wstring language = ConvertToLowerCase(inLanguage);
		
		XLocker lock(data);
		const wchar_t* header = reinterpret_cast<const wchar_t*>(data.GetPtr());
		uint32 bytes = data.GetSize();
			
		std::wstring newText;
		XAutoPtr<XTextTranscoder> transcoder;							

		if (language == L"us-ascii" || language == L"iso-ir-6" || language == L"iso646-us") {							
			transcoder.Reset(new XASCIITranscoder); 
		
		} else if (language == L"mac-roman") {			// $$$ there doesn't appear to be an ISO name for this
			transcoder.Reset(new XMacRomanTranscoder); 

		} else if (language == L"win-latin-1" || language == L"iso-8859-1") {
			transcoder.Reset(new XWindowsLatin1Transcoder); 

		} else if (language == L"naive") {				// XXMLStorage can create documents with this encoding
			transcoder.Reset(new XNaiveTranscoder); 

		} else if (language == L"@default") {
			if (*header != 0xFEFF && *header != 0xFFFE)
				transcoder.Reset(new XUTF8Transcoder); 

		} else if (language == L"utf-8" || language == L"iso-10646-utf-2") { 
			transcoder.Reset(new XUTF8Transcoder); 
			if (*header == 0xFEFF || *header == 0xFFFE)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The language encoding is 'utf-8' but the file has a Byte Order Mark.")));

		} else if (language == L"utf-16" || language == L"iso-10646-ucs-2") {
			if (*header != 0xFEFF && *header != 0xFFFE)
				throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"The language encoding is 'utf-16' but the file is missing the Byte Order Mark.")));

	#if MAC
		} else if (language == L"euc-jp" || language == L"iso-646") {
			transcoder.Reset(new XEUCJPTranscoder); 

		} else if (language == L"shift-jis") {
			transcoder.Reset(new XShiftJISTranscoder); 
	#endif

		} else 
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Only 'utf-16/iso-10646-ucs-2', 'utf-8/iso-10646-utf-2', 'US-ASCII/iso646-us', 'Mac-Roman', and 'Win-Latin-1/iso-8859-1' language encodings are currently supported.")));

		if (transcoder.Get() != nil) {
			transcoder->SetToUTF16Flags(kRejectInvalidCharacters);
			newText = FromPlatformStr((const char*) data.GetPtr(), bytes, transcoder.Get());

			newText = Replace(newText, L"\r\n", L"\n");			// this is tough to handle inside the parser because CRs are supposed to be normalized unless they're the result of entity expansion...
			newText = Replace(newText, L"\r", L"\n");
		}
		
		if (newText.length() > 0) 
			lexer->UpdateLanguage(newText);
			
	} catch (const std::exception& e) {
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't convert #1 to Unicode (#2).", data.GetURI().GetFile(), FromUTF8Str(e.what()))));

	} catch (...) {
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't convert #1 to Unicode.", data.GetURI().GetFile())));
	}
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// XXMLParser::GetGeneralEntity
//
//---------------------------------------------------------------
const XXMLParser::SEntity& XXMLParser::GetGeneralEntity(const std::wstring& name) const
{	
	EntityMap::const_iterator iter = mGeneralEntities.find(name);
	if (iter == mGeneralEntities.end())						// VC: Entity Declared
		XXMLException::Throw(mLexer, "The general entity '#1' isn't a declared entity.", name);	
			
	return iter->second;	
}


//---------------------------------------------------------------
//
// XXMLParser::GetParameterEntity
//
//---------------------------------------------------------------
const XXMLParser::SEntity& XXMLParser::GetParameterEntity(const std::wstring& name) const
{
	EntityMap::const_iterator iter = mParameterEntities.find(name);
	if (iter == mParameterEntities.end())
		XXMLException::Throw(mLexer, "The parameter entity reference '#1' isn't a declared entity.", name);	
	
	if (iter->second.externalSubSet && mStandAlone)			// VC: Standalone Document Declaration
		XXMLException::Throw(mLexer, "Entity '#1' was declared in the external subset so the document cannot be standalone.", name);

	return iter->second;
}


//---------------------------------------------------------------
//
// XXMLParser::AddGeneralEntity
//
//---------------------------------------------------------------
void XXMLParser::AddGeneralEntity(const std::wstring& name, const SEntity& entity)
{
	if (!mGeneralEntities.insert(EntityMap::value_type(name, entity)).second)
		this->DoWarning(LoadWhisperString(L"General entity '#1' was declared more than once.", name));
}


//---------------------------------------------------------------
//
// XXMLParser::AddParameterEntity
//
//---------------------------------------------------------------
void XXMLParser::AddParameterEntity(const std::wstring& name, const SEntity& entity)
{	
	if (!mParameterEntities.insert(EntityMap::value_type(name, entity)).second)
		this->DoWarning(LoadWhisperString(L"Parameter entity '#1' was declared more than once.", name));
}


//---------------------------------------------------------------
//
// XXMLParser::AddUnparsedEntity
//
//---------------------------------------------------------------
void XXMLParser::AddUnparsedEntity(const std::wstring& name, const SEntity& entity)
{
	mCallback->OnUnparsedEntity(name, entity.id);

	this->AddGeneralEntity(name, entity);
}


//---------------------------------------------------------------
//
// XXMLParser::AddNotation
//
//---------------------------------------------------------------
void XXMLParser::AddNotation(const std::wstring& name, const SExternalID& id)
{
	(void) mNotations.insert(name);

	mCallback->OnNotation(name, id);
}


//---------------------------------------------------------------
//
// XXMLParser::AddID
//
//---------------------------------------------------------------
void XXMLParser::AddID(const std::wstring& id)			
{
	bool added = mIDs.insert(id).second;
	if (!added)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"ID '#1' has already been used.", id)));
}


//---------------------------------------------------------------
//
// XXMLParser::DoWarning
//
//---------------------------------------------------------------
void XXMLParser::DoWarning(const std::wstring& mesg)
{
	if (mWarningMode == kTreatWarningsAsErrors) 
		throw XXMLException(mLexer, mesg);
	
	else if (mWarningMode == kLogWarnings) 
		mWarnings.push_back(mesg);
	
	else if (mWarningMode != kIgnoreWarnings) 
		DEBUGSTR("mWarning is bogus!");
}


//---------------------------------------------------------------
//
// XXMLParser::DoValidateRefs
//
// Note that we have to wait until the entire document is parsed
// to perform these checks because the reference can occur before
// the item it points to.
//
//---------------------------------------------------------------
void XXMLParser::DoValidateRefs()
{
	// make sure all ID references point to a valid element
	std::vector<std::wstring>::const_iterator iter = mIDRefs.begin();
	while (iter != mIDRefs.end()) {
		const std::wstring& id = *iter;
		++iter;
		
		std::set<std::wstring>::const_iterator i = mIDs.find(id);
		if (i == mIDs.end())
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the element with ID: '#1'", id)));
	}
	
	mIDs.clear();
	mIDRefs.clear();

	// make sure all entity references exist and point to unparsed entities
	iter = mEntityRefs.begin();
	while (iter != mEntityRefs.end()) {
		const std::wstring& name = *iter;
		++iter;
		
		const SEntity& entity = this->GetGeneralEntity(name);	// note that only general entities can be unparsed
		if (entity.internal)
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't an external general entity", name)));

		if (entity.id.dataType.length() == 0)
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"'#1' isn't an unparsed entity", name)));
	}
	mEntityRefs.clear();

	// make sure all notation references exist
	iter = mNotationRefs.begin();
	while (iter != mNotationRefs.end()) {
		const std::wstring& notation = *iter;
		++iter;
		
		std::set<std::wstring>::const_iterator j = mNotations.find(notation);
		if (j == mNotations.end())
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the notation named: '#1'", notation)));
	}
	mNotationRefs.clear();
}


}	// namespace Whisper

