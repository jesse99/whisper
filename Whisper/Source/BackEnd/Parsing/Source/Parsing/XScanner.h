/*
 *  File:		XScanner.h
 *  Summary:	A class that returns selected characters from a string.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1998-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XScanner.h,v $
 *		Revision 1.4  2000/12/11 01:31:10  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.3  2000/11/21 09:25:52  jesjones
 *		Added <vector> include.
 *		
 *		Revision 1.2  2000/11/09 12:50:25  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <3>	 3/14/00	JDJ		Made the whitespace more Unicode savvy.
 *		 <2>	 2/01/99	JDJ		Added Reset.
 *		 <1>	12/17/98	JDJ		Created (from Raven)
 */

#pragma once

#include <vector>

#include <XExceptions.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XScannerPos
//!		Class holding line and character index for an XScanner.
// ===================================================================================
class PARSE_EXPORT XScannerPos {

	friend class XScanner;

public:
						XScannerPos()								{}
				
			uint32 		GetIndex() const							{return mIndex;}
			uint32 		GetLine() const								{return mLine;}

			bool 		operator==(const XScannerPos& rhs) const	{return mIndex == rhs.mIndex && mLine == rhs.mLine;}
			bool 		operator!=(const XScannerPos& rhs) const	{return !this->operator==(rhs);}

private:
						XScannerPos(uint32 index, uint32 line)		{mIndex = index; mLine = line;}

private:
	uint32	mIndex;
	uint32	mLine;	
};


// ===================================================================================
//	class XParserException
//!		XScanner does not throw, but clients can throw one of these to include the line number the error ocurred on.
// ===================================================================================
class PARSE_EXPORT XParserException : public std::runtime_error {

	typedef std::runtime_error Inherited;

public:
						XParserException(const XScanner* scanner, const std::wstring& str);

			uint32 		GetLine() const							{return mPos.GetLine() + 1;}
						/**< The line the parser choked on. Note that this is 1 based (since
						this is what users expect). */

			uint32 		GetIndex() const						{return mPos.GetIndex();}
						/**< Returns the offset from the start of the text. */

private:
	XScannerPos mPos;
};


// ===================================================================================
//	class XScanner
//!		A class that returns selected characters from a string.
// 		Note that scanners do not throw exceptions.
// ===================================================================================
class PARSE_EXPORT XScanner {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XScanner();

						XScanner();

						XScanner(const std::wstring& text);
						
private:
						XScanner(const XScanner& rhs);
						
			XScanner& 	operator=(const XScanner& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Inquiry
	//@{
			bool 		AtEnd() const									{return mIndex >= mText.length();}

			wchar_t 	Peek(int32 offset = 0) const;
						/**< Peeking past the end returns '\0'. */

			uint32 		GetLine() const									{return mLine;}
			
			std::wstring& GetText()										{return mText;}
			const std::wstring& GetText() const							{return mText;}
	//@}
						
	//! @name Advancing
	//@{
			void 		Advance(int32 delta = 1);

	virtual	void 		SkipWhiteSpace();
						/**< Note that none of the other functions skip white space. */
	//@}

	//! @name Saving Positions
	//@{
			XScannerPos GetPosition() const								{return XScannerPos(mIndex, mLine);}
						/**< This can be used to save a position within the scanner.
						(Although this doesn't allow subclasses to add extra
						information it is very fast which is important in a
						scanner). */
			
	virtual void 		SetPosition(const XScannerPos& pos);
	//@}

	//! @name Misc
	//@{
	virtual	void 		Reset()											{mIndex = mLine = 0;}
						/**< Moves the scanner back to the start of the text. */
			
	virtual void 		SetText(const std::wstring& text);
	
			void 		SetWhiteSpace(wchar_t ch, bool isWhite = true);

			void 		ClearWhiteSpace();
						/**< Zaps all the whitespace characters. */
	//@}
					
//-----------------------------------
//	Internal API
//
protected:
			bool 		DoIsWhiteSpace(wchar_t ch) const;
			
private:
	virtual void 		OnIsWhiteSpace(int32)		{}
	virtual void 		OnSkipWhiteSpace(int32)		{}
			void 		DoBuildWhiteSpaceTable()	{}
						// Obsolete methods $$$ remove in Whisper 2.1 $$$

//-----------------------------------
//	Member Data
//
protected:
	std::wstring			mText;
	uint32					mIndex;
	uint32					mLine;

private:
	std::vector<wchar_t>	mWhiteSpace;	//!< defaults to HORIZONTAL TABULATION, LINE FEED, CARRIAGE RETURN, SPACE, SIX-PER-EM SPACE, ZERO WIDTH SPACE, and IDEOGRAPHIC SPACE
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
