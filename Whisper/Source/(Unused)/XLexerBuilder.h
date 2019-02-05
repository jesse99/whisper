/*
 *  File:		XLexerBuilder.h
 *  Summary:	A class that can be used to build a lexer grammar from a resource.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1998 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XLexerBuilder.h,v $
 *		Revision 1.2  2000/11/09 12:41:18  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <XTypes.h>

#if MAC
	#include <Resources.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XLexerGrammar;
struct STable;


// ===================================================================================
//	class XLexerBuilder
// ===================================================================================
class PARSE_EXPORT XLexerBuilder {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XLexerBuilder();

						XLexerBuilder(ResID id);
						// This works by creating a lexer grammar from a text resource
						// and saving the result into a binary resource. On the next
						// run the binary resource is used if the text resource's CRC
						// remains unchanged. On the Mac the text resource is a 'TxGr'
						// resource (see WhisperDebug.r for an example) and the binary
						// resource is a 'BnGr' resource. Windows doesn't handle custom
						// resources very well so the text resource is an RCDATA resource
						// and the binary resource is also an RCDATA resource with the
						// resource id incremented by one. Because Windows doesn't support
						// writable resources the binary resource is written out to a file
						// (eg if the id passed into the XLexerBuilder ctor is 128 the file
						// name will be "Lexer Grammar 129.bin").
												
private:
						XLexerBuilder(const XLexerBuilder& rhs);
						
			XLexerBuilder& operator=(const XLexerBuilder& rhs);

//-----------------------------------
//	API
//
public:
			XLexerGrammar* CreateGrammar();
				
//-----------------------------------
//	Internal API
//
protected:			
#if MAC
			XLexerGrammar* DoCreateFromText(const STable* table);
#elif WIN
			XLexerGrammar* DoCreateFromText(const char* table);
#endif
				
//-----------------------------------
//	Member Data
//
protected:
	ResID 	mID;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

