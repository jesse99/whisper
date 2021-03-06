/*
 *  File:		XXMLDoc.h
 *  Summary:	Encapsulates the elements of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDoc.h,v $
 *		Revision 1.3  2001/04/17 01:45:32  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.2  2000/11/09 13:00:58  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <17>	 3/26/00	JDJ		Moved the notation and unparsed entity stuff from XXMLDTD to XXMLDoc.
 *		 <16>	 3/02/00	JDJ		Got rid of XXMLDoc::Save and Load. 
 *		 <15>	12/21/99	JDJ		XXMLDoc::Save's compress argument is a bool instead of a uint32.
 *		 <14>	10/17/99	JDJ		XXMLDoc::Save returns true if the XML was re-parsed.
 *		 <13>	10/03/99	JDJ		Added XXMLElement::GetNumAttributes and GetNumItems.
 *		 <12>	 9/27/99	JDJ		Added XXMLElement RemoveAttribute, RemoveItem, and DeleteItem.
 *		 <11>	 7/14/99	JDJ		XXMLElement::GetName returns a const reference.
 *		 <10>	 7/14/99	JDJ		Added support for persisting via binary streams.
 *		 <9>	 6/30/99	JDJ		Added XXMLElement::FindAttribute and FindElement.
 *		 <8>	 6/22/99	JDJ		XXMLAttribute::GetName and GetValue return references.
 *		 <7>	 5/04/99	JDJ		Added XXMLItem visitor support.
 *		 <6>	 5/04/99	JDJ		Renamed XXMLDocument XXMLDoc.
 *		 <5>	 5/01/99	JDJ		XXMLAttribute::GetInt32Value, GetUInt32Value, GetFloatValue
 *									and GetDoubleValue have optional min and max arguments.
 *		 <4>	 3/23/99	JDJ		XXMLCharData::GetText returns a const XString& instead of an XString.
 *		 <3>	 3/20/99	JDJ		Added XXMLAttribute ctor overloads for the most
 *									common primitive types.
 *		 <2>	 3/19/99	JDJ		Added XXMLDoc::FindElement and AddElement.
 *									Added XXMLElement::attr_front, attr_back, item_front, and item_back.
 *		 <1>	 1/17/99	JDJ		Created
 */

#pragma once

#include <list>
#include <map>

#include <XBinaryPersistent.h>
#include <XInvariant.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLElement;
class XXMLProcessInstruction;


// ===================================================================================
//	struct SExternalID
// ===================================================================================
struct CORE_EXPORT SExternalID {
	std::wstring	publicID;
	std::wstring	systemID;
	std::wstring	dataType;		// if the length is greater than zero we have a general unparsed entity (ie it's treated as binary data)
};
	

// ===================================================================================
//	class XXMLDoc
// ===================================================================================
class CORE_EXPORT XXMLDoc : public XBinaryPersistentMixin {

//-----------------------------------
//	Types
//
public:
	typedef std::list<XXMLProcessInstruction*> ProcessInstructions;	
	typedef std::map<std::wstring, SExternalID>     NotationMap;
	
protected:
	typedef std::map<std::wstring, XXMLElement*> IDMap;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLDoc();

						XXMLDoc();
						
			void 		Reset();

private:
						XXMLDoc(const XXMLDoc& rhs);
						
			XXMLDoc& operator=(const XXMLDoc& rhs);
						
//-----------------------------------
//	New API
//
public:								
	// ----- Root Element -----
			XXMLElement* 	   GetRootElement()								{return mRootElement;}
			const XXMLElement* GetRootElement() const						{return mRootElement;}
			
			void 		SetRootElement(XXMLElement* takeElement);
			
	// ----- IDs -----
			XXMLElement* 	   FindElement(const std::wstring& id);
			const XXMLElement* FindElement(const std::wstring& id) const;
						// Returns the element with the specified id. Throws if an 
						// appropiate element cannot be found.
			
			void 		AddElement(XXMLElement* element, const std::wstring& id);

	// ----- Unparsed Entities -----
			bool 		AddUnparsedEntity(const std::wstring& name, const SExternalID& id);
						// Returns true if the entity was added.
						
			SExternalID FindUnparsedEntity(const std::wstring& name);
						// Throws if an entity with the specified name cannot be found.
	
	// ----- Notations -----
			bool 		AddNotation(const std::wstring& name, const SExternalID& id);
						// Returns true if the notation was added.
						
			SExternalID FindNotation(const std::wstring& name);
						// Throws if a notation with the specified name cannot be found.
	
			const NotationMap& GetNotations() const			{return mNotations;}

	// ----- Process Instructions -----
			const ProcessInstructions& GetPreProcessInstructions() const	{return mPreProcessInstructions;}
			const ProcessInstructions& GetPostProcessInstructions() const	{return mPostProcessInstructions;}
			
			void 		AddPreProcessInstruction(XXMLProcessInstruction* takeInstruction);
			void 		AddPostProcessInstruction(XXMLProcessInstruction* takeInstruction);
						// Pre process instructions are those PIs that appear before the
						// root element. Post PI appear after the root element.
						
//-----------------------------------
//	Inherited API
//
protected:								
			void 		Invariant() const;

	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	XXMLElement*		mRootElement;
	IDMap				mElementIDs;
	
	NotationMap			mUnparsedEntities;
	NotationMap			mNotations;
	
	ProcessInstructions	mPreProcessInstructions;
	ProcessInstructions	mPostProcessInstructions;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

