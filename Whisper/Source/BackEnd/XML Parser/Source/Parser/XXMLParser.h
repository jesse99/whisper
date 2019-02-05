/*
 *  File:		XXMLParser.h
 *  Summary:	Classes used to parse XML documents.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLParser.h,v $
 *		Revision 1.5  2001/10/03 02:14:38  jesjones
 *		Updated a comment.
 *		
 *		Revision 1.4  2000/12/15 09:07:49  jesjones
 *		Yet more doxyfying.
 *		
 *		Revision 1.3  2000/12/11 01:31:53  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/10 04:47:46  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <2>	 3/19/99	JDJ		Added AddID.
 *		 <1>	 2/01/99	JDJ		Created
 */

#pragma once

#include <XScanner.h>
#include <XURI.h>
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
class XXMLCallbackMixin;
class XXMLLexer;


//-----------------------------------
//	Types
//
typedef XCountedPtr<XXMLDTD> XDTDPointer;


// ===================================================================================
//	class XXMLException
//!		Thrown by XXMLParser.
// ===================================================================================
class XML_EXPORT XXMLException : public std::runtime_error {

	typedef std::runtime_error Inherited;

public:
						XXMLException(const XXMLLexer* lexer, const std::wstring& str);

			uint32 		GetLine() const							{return mPos.GetLine() + 1;}
						/**< The line the parser choked on. Note that this is 1 based (since
						this is what users expect). */

			uint32 		GetIndex() const						{return mPos.GetIndex();}
						/**< Returns the offset from the start of the text. */
						
			XURI 		GetURI() const							{return mURI;}

	static 	void 		Throw(const XXMLLexer* lexer, const char* str);
	static 	void 		Throw(const XXMLLexer* lexer, const char* format, const std::wstring& arg1);
	static 	void 		Throw(const XXMLLexer* lexer, const char* format, const std::wstring& arg1, const std::wstring& arg2);
						/**< The parser uses a lot less stack space if throws are bottlenecked
						through these functions. */

private:
	XScannerPos mPos;
	XURI		mURI;
};


// ===================================================================================
//	class XXMLParser
//!		The class used to parse XML files.		
/*!		XXMLParser implements a fully validating XML parser and correctly handles all 
 *		of James Clark's xmltest files (these are included in the XML folder, you can 
 *		use the XXMLParser unit test to process these files). There are a *lot* of test 
 *		files including some rather tricky ones (James Clark was the XML Working Group 
 *		technical lead) so I'm pretty confident that the parser meets the spec.
 *
 * 		There are two basic approaches to designing an XML processor: the processor can 
 *		build an in-memory representation of the document and hand it off to the 
 *		application or the processor can use a callback when it encounters things like 
 *		elements and attributes. The second approach is generally considered better 
 *		because it's more flexible and can be more efficient (eg if only part of the 
 *		document has to be parsed).
 *
 *		XXMLParser uses the second approach and routes the callbacks through an 
 *		XXMLCallbackMixin object. However you can also build an in-memory representation 
 *		(an XXMLDoc object) using a subclass of XXMLCallbackMixin named XXMLDocumentCallback. 
 *		If you do this you'll wind up with code that looks something like: \code
 *			XXMLDoc doc;
 *			{
 *			XResource data(fileSpec);
 *			XXMLParser parser(&data);
 *			XXMLDocumentCallback builder(doc);
 *				parser.Parse(builder);
 *			} \endcode */										
// ===================================================================================
class XML_EXPORT XXMLParser {

//-----------------------------------
//	Types
//
public:
	enum EWarning {kTreatWarningsAsErrors, 		//!< warnings will cause an exception to be thrown
				   kLogWarnings, 				//!< warnings will be logged (GetWarning can be used to retrieve the warning messages)
				   kIgnoreWarnings				//!< warnings will be completely ignored
				   };

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XXMLParser();
						/**< Decrements the data's ref count. */
						
						XXMLParser(XResource* data);
						/**< Bumps data's ref count (note that this should contain the raw
						XML data since it cannot be converted to an wstring until the
						language encoding info is read). */
						
private:
						XXMLParser(const XXMLParser& rhs);
						
			XXMLParser& operator=(const XXMLParser& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Parsing
	//@{
			void 		Parse(XXMLCallbackMixin& callback);
						/**< Note that you can call this multiple times (eg with different 
						callbacks). Each time it's called the document is reparsed. */

#if 0
			void 		ParseButSkipDTD(XXMLCallbackMixin& callback);	
						/**< Like Parse except that none of the DTD declarations are parsed
						and the external subset is skipped over. This will usually be 
						slightly faster than Parse and if the app never calls Parse the
						linker will dead strip the DTD parser which will provide a 
						substantial space savings. However there are some disadvantages:
						1) Missing attributes are no longer given default values.
						2) The only entities you can use are amp, lt, gt, apos, and quot.
						3) Parse will normalize all attributes but CDATA by discarding
						   leading and trailing spaces, and by replacing sequences of
						   spaces with a single space. None of this will be done when
						   ParseButSkipDTD is used.
						4) It's no longer possible for the parser to determine which
						   elements can contain character data so any whitespace between
						   elements is returned as XXMLCharData.
						5) Applications may need to perform additional validation. */
#endif

			uint32 		GetNumWarnings() const								{return mWarnings.size();}
			const std::wstring& GetWarning(uint32 index)							{ASSERT(index < mWarnings.size()); return mWarnings[index];}
	//@}

	//! @name DTD
	//@{
			XDTDPointer& GetDTD()											{return mDTD;}
	//@}
			
	//! @name Options
	//@{
			void 		OptimizeStandAloneDocuments(bool optimize = true)	{mOptimized = optimize;}
						/**< Stand alone documents can be correctly parsed without
						parsing the external DTD. If you call this function
						the external DTD will not be parsed which will cause
						the document to parse faster (and fail to check most
						of the validity constraints). */
			
			void 		SetWarningMode(EWarning mode)						{mWarningMode = mode;}
						/**< Defaults to kLogWarnings (see above). */
	//@}
		
//-----------------------------------
//	Internal Types
//
public:
	struct SEntity {
		bool 			internal;
		bool			externalSubSet;
		std::wstring	text;
		SExternalID		id;
		
						SEntity() 											: internal(true), externalSubSet(false) 					{}
						SEntity(const std::wstring& t, bool internalSubset)		: internal(true), externalSubSet(!internalSubset), text(t) 	{}
						SEntity(const SExternalID& i, bool internalSubset)	: internal(false), externalSubSet(!internalSubset), id(i) 		{}
			
			XResource* 	Load(const XURI& base) const;
	};
	
	typedef std::map<std::wstring, SEntity>	EntityMap;

//-----------------------------------
//	Internal API
//
public:
			const SEntity& GetGeneralEntity(const std::wstring& name) const;
			const SEntity& GetParameterEntity(const std::wstring& name) const;
			
			void 		AddGeneralEntity(const std::wstring& name, const SEntity& entity);
			void 		AddParameterEntity(const std::wstring& name, const SEntity& entity);
			void 		AddUnparsedEntity(const std::wstring& name, const SEntity& entity);

			void 		AddNotation(const std::wstring& name, const SExternalID& id);

			void 		DoWarning(const std::wstring& mesg);

			XXMLLexer* 	GetLexer()									{return mLexer;}
			XXMLCallbackMixin* GetCallback()						{return mCallback;}
			
			void 		CreateDTD()									{ASSERT(mDTD.Get() == nil); mDTD.Reset(new XXMLDTD);}

			XXMLLexer* 	CreateLexer(XResource& data, bool internal);
			void 		UpdateLexer(const XResource& data, XXMLLexer* lexer, const std::wstring& language);
			
			void 		AddID(const std::wstring& id);
			void 		ValidateIDRef(const std::wstring& id)			{mIDRefs.push_back(id);}
			void 		ValidateEntityRef(const std::wstring& name)		{mEntityRefs.push_back(name);}
			void 		ValidateNotationRef(const std::wstring& name)	{mNotationRefs.push_back(name);}
			
			bool 		IsStandAlone() const						{return mStandAlone;}
			void 		SetStandAlone(bool standAlone)				{mStandAlone = standAlone;}

protected:
			void 		DoValidateRefs();

//-----------------------------------
//	Member Data
//
protected:
	XResource*					mData;
	XXMLLexer*					mLexer;
	XXMLCallbackMixin*			mCallback;
	
	XDTDPointer					mDTD;
	bool						mOptimized;
	bool						mStandAlone;

	EntityMap					mParameterEntities;		// for use in the DTD
	EntityMap					mGeneralEntities;		// for use in the document content
	
	std::set<std::wstring>		mIDs;
	std::set<std::wstring>		mNotations;
	std::vector<std::wstring>	mIDRefs;
	std::vector<std::wstring>	mEntityRefs;
	std::vector<std::wstring>	mNotationRefs;			

	EWarning					mWarningMode;
	std::vector<std::wstring>	mWarnings;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

