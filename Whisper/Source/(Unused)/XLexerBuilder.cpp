/*
 *  File:		XLexerBuilder.cpp
 *  Summary:	A class that can be used to build a lexer grammar from a resource.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexerBuilder.cpp,v $
 *		Revision 1.3  2000/12/10 04:10:17  jesjones
 *		Replaced int16_cast, uint32_cast, etc with numeric_cast.
 *		
 *		Revision 1.2  2000/11/09 12:41:14  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#include <XWhisperHeader.h>
#include <XLexerBuilder.h>

#include <XFile.h>
#include <XFileSpec.h>
#include <XFolderSpec.h>
#include <XHandle.h>
#include <XHandleStream.h>
#include <XIntConversions.h>
#include <XLexerGrammar.h>
#include <XLocker.h>
#include <XMiscUtils.h>
#include <XStreaming.h>
#include <XStringUtils.h>

#if MAC
	#include <MResUtils.h>
#elif WIN
	#include <XFileStream.h>
	#include <XPointerStream.h>
	#include <WUtils.h>
#endif

namespace Whisper {


//-----------------------------------
//	Types
//
#if MAC
	#pragma options align=mac68k

	struct SEntry {
		int32	code;
		Str255	str;					// regular expressions, empty string, and then simple strings
	};

	struct STable {						// TxGr resource format
		int32	numEntries;
		SEntry	entries[1];
	};

	#pragma options align=reset
#endif


// ===================================================================================
//	Internal Functions
// ===================================================================================

//---------------------------------------------------------------
//
// NextEntry
//
// We have to do this grody pointer arithmetic because ResEdit only 
// saves the characters that are actually used by the string.
//
//---------------------------------------------------------------
#if MAC
static const SEntry* NextEntry(const SEntry* entry)
{
	const uint8* ptr = reinterpret_cast<const uint8*>(entry);
	
	ptr += sizeof(int32) + sizeof(char) + entry->str[0];
	int32 delta = (int32) ptr & 3;				// // ResEdit will add pad bytes to keep token number aligned.
	if (delta > 0)
		ptr += 4 - delta;
	
	return reinterpret_cast<const SEntry*>(ptr);
}
#endif

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XLexerBuilder
// ===================================================================================

//---------------------------------------------------------------
//
// XLexerBuilder::~XLexerBuilder
//
//---------------------------------------------------------------
XLexerBuilder::~XLexerBuilder()
{
}


//---------------------------------------------------------------
//
// XLexerBuilder::XLexerBuilder
//
//---------------------------------------------------------------
XLexerBuilder::XLexerBuilder(ResID id)
{
	mID = id;
}


//---------------------------------------------------------------
//
// XLexerBuilder::CreateGrammar
//
//---------------------------------------------------------------
XLexerGrammar* XLexerBuilder::CreateGrammar()
{
	XLexerGrammar* grammar = nil;
	
#if DEBUG && MAC
	XHandle hand(ReadResource('TxGr', mID));
	
	{
	XLocker lock(hand, kMoveHigh);
		uint32 textCRC = ComputeCRC(hand.GetPtr(), hand.GetSize());

		// If we have a binary resource and the text resource hasn't
		// changed then we'll use the binary resource.
		if (HasResource('BnGr', mID)) {
			XInHandleStream stream(ReadResource('BnGr', mID));
			
			uint32 binaryCRC;
			stream >> binaryCRC;
			
			if (binaryCRC == textCRC) {
				grammar = new XLexerGrammar;
				stream >> *grammar;
			}
		}
	
		// Otherwise we'll use the text resource and save a new
		// binary resource. (The binary resource should be added
		// to a ".rsrc" file so that it can be used next time).
		if (grammar == nil) {
			TRACEFLOW("XWarning", "Building new binary lexer grammar\n");	// remind clients to copy the 'BnGr' resource
			
			const STable* table = reinterpret_cast<const STable*>(hand.GetPtr());
		
			grammar = this->DoCreateFromText(table);

			XOutHandleStream stream(100);
			stream << textCRC;
			stream << *grammar;

			WriteResource('BnGr', mID, stream.GetHandle().GetHandle());
		}
	}

#elif DEBUG && WIN
	const void* text = GetResource(RT_RCDATA, numeric_cast<int16>(mID));

	// $$$ We should be using a custom resource type here. (The Rc.hlp
	// $$$ says that custom resource type names must be unique, but
	// $$$ this appears to be in error).
	uint32 textCRC = ComputeCRC(text, GetResourceSize(RT_RCDATA, numeric_cast<int16>(mID)));

	// If we have a binary resource and the text resource hasn't
	// changed then we'll use the binary resource.
	if (HasResource(RT_RCDATA, numeric_cast<int16>(mID+1))) {
		XInPointerStream stream(GetResource(RT_RCDATA, numeric_cast<int16>(mID+1)), GetResourceSize(RT_RCDATA, numeric_cast<int16>(mID+1)));
		
		uint32 binaryCRC;
		stream >> binaryCRC;
		
		if (binaryCRC == textCRC) {
			grammar = new XLexerGrammar;
			stream >> *grammar;
		}
	}

	// Otherwise we'll use the text resource and save a new
	// binary resource. (The binary resource should be added
	// to a ".rsrc" file so that it can be used next time).
	if (grammar == nil) {
		TRACEFLOW("XWarning", "Building new binary lexer grammar\n");	// remind clients to copy the 'BnGr' resource
			
		grammar = this->DoCreateFromText((const char*) text);

		XFile file(XFileSpec(XFolderSpec::GetAppFolder(), L"Lexer Grammar " + Int32ToStr(mID+1) + L".bin"));
		file.Open('\?\?\?\?', 'BINA', kWritePermission);
		{
		XOutFileStream stream(file, kRaw);
			stream << textCRC;
			stream << *grammar;
		}
		file.Close();
	}

#else	
#if MAC
	XInHandleStream stream(ReadResource('BnGr', mID));	
#elif WIN
	XInPointerStream stream(GetResource(RT_RCDATA, mID+1), GetResourceSize(RT_RCDATA, mID+1));
#endif

	uint32 binaryCRC;						// CRC isn't used in release builds
	stream >> binaryCRC;
	
	grammar = new XLexerGrammar;
	stream >> *grammar;
#endif

	return grammar;
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XLexerBuilder::DoCreateFromText (const STable*)
//
//---------------------------------------------------------------
#if MAC
XLexerGrammar* XLexerBuilder::DoCreateFromText(const STable* table)
{
	XLexerGrammar* grammar = new XLexerGrammar;
	
	bool regExpr = true;
	
	const SEntry* entry = &((*table).entries[0]);
	for (int32 index = 0; index < table->numEntries; index++) {
		std::wstring str = entry->str;
		
		if (str.size() > 0) {
			if (regExpr)
				grammar->AddExpr(str, entry->code);
			else
				grammar->AddStr(str, entry->code);
				
		}  else
			regExpr = false;
			
		entry = NextEntry(entry);
	}
	
	return grammar;
}
#endif


//---------------------------------------------------------------
//
// XLexerBuilder::DoCreateFromText (const char*)
//
//---------------------------------------------------------------
#if WIN
XLexerGrammar* XLexerBuilder::DoCreateFromText(const char* table)
{
	XLexerGrammar* grammar = new XLexerGrammar;
	
	bool regExpr = true;
	
	while (table[0] != '\0') {
		std::wstring str1 = table;
		ASSERT(str1.length() == 1);
		
		table += str1.length() + 1;
		
		if (str1[0] != '\t') {
			std::wstring str2 = table;
			table += str2.length() + 1;

			if (regExpr)
				grammar->AddExpr(str2, str1[0]);
			else
				grammar->AddStr(str2, str1[0]);
				
		}  else
			regExpr = false;
	}
	
	return grammar;
}
#endif


}	// namespace Whisper
