/*
 *  File:       WhisperTypes.r
 *  Summary:   	Whisper resource types
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: WhisperTypes.r,v $
 *		Revision 1.2  2000/11/09 12:08:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log$ keyword.
 *		
 *		 <3>	 6/08/99	JDJ		Added STRX.
 *		 <2>	12/17/98	JDJ		Added TxGr.
 *		 <1>	 7/27/96	JDJ		Created (from Whisper)
 */

#include <Types.r>


// Note that resource ids below 256 are reserved for Whisper.

// ===================================================================================
//	TxGr
//		Text resource used to build a TLexerGrammer. The first set of token numbers
//		and strings are regular expressions. The second set are treated as simple
//		strings. The second set starts after the first empty string.
// ===================================================================================
type 'TxGr' {		
	longint = $$CountOf(Messages);
	array Messages {
		longint;			// token number
		pstring;			// string 
		align long;			// pad byte if string has an odd length (keeps token number aligned)
	};
};


// ===================================================================================
//	STRX
//		A list of integer/string pairs. This is used, for example, to map OSErr's to
//		strings.
// ===================================================================================
type 'STRX' {		
	longint = $$CountOf(Messages);
	array Messages {
		longint;			// code
		pstring;			// string 
		align long;			// pad byte if string has an odd length (keeps code int aligned)
	};
};


