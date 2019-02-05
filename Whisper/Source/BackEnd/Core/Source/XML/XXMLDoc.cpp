/*
 *  File:		XXMLDoc.cpp
 *  Summary:	Encapsulates the elements of an XML document.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDoc.cpp,v $
 *		Revision 1.5  2001/04/27 04:38:46  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.4  2001/04/17 01:45:28  jesjones
 *		Got rid of XInvariantMixin.
 *		
 *		Revision 1.3  2001/03/06 07:33:33  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.2  2000/11/09 13:00:53  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <13>	 3/26/00	JDJ		Moved the notation and unparsed entity stuff from XXMLDTD to XXMLDoc.
 *		 <12>	 3/02/00	JDJ		Got rid of XXMLDoc::Save and Load. 
 *		 <11>	12/21/99	JDJ		XXMLDoc::Save's compress argument is a bool instead of a uint32.
 *		 <10>	10/17/99	JDJ		XXMLDoc::Save returns true if the XML was re-parsed.
 *		 <9>	 7/14/99	JDJ		Added support for persisting via binary streams.
 *		 <8>	 6/30/99	JDJ		Added XXMLElement::FindAttribute and FindElement.
 *		 <7>	 5/04/99	JDJ		Adjusted XXMLAttribute decPlaces.
 *		 <6>	 5/04/99	JDJ		Added XXMLItem visitor support.
 *		 <5>	 5/04/99	JDJ		Renamed XXMLDocument XXMLDoc.
 *		 <4>	 5/01/99	JDJ		XXMLAttribute::GetInt32Value, GetUInt32Value, GetFloatValue
 *									and GetDoubleValue have optional min and max arguments.
 *		 <3>	 3/20/99	JDJ		Added XXMLAttribute ctor overloads for the most
 *									common primitive types.
 *		 <2>	 3/19/99	JDJ		Added XXMLDoc::FindElement and AddElement.
 *		 <1>	 1/18/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLDoc.h>

#include <XObjectFactory.h>
#include <XStreaming.h>
#include <XStringUtils.h>
#include <XXMLItems.h>

namespace Whisper {


// ===================================================================================
//	class XXMLDoc
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLDoc::~XXMLDoc
//
//---------------------------------------------------------------
XXMLDoc::~XXMLDoc()
{	
	this->Reset();
	mRootElement = nil;
}


//---------------------------------------------------------------
//
// XXMLDoc::XXMLDoc
//
//---------------------------------------------------------------
XXMLDoc::XXMLDoc()
{
	mRootElement = nil;

	XObjectFactory::Instance()->RegisterClass(L"Whisper::XXMLCharData", XXMLCharData::Create);
	XObjectFactory::Instance()->RegisterClass(L"Whisper::XXMLComment", XXMLComment::Create);
	XObjectFactory::Instance()->RegisterClass(L"Whisper::XXMLProcessInstruction", XXMLProcessInstruction::Create);
	XObjectFactory::Instance()->RegisterClass(L"Whisper::XXMLElement", XXMLElement::Create);
}


//---------------------------------------------------------------
//
// XXMLDoc::Reset
//
//---------------------------------------------------------------
void XXMLDoc::Reset()
{
	ProcessInstructions::iterator iter = mPreProcessInstructions.begin();
	while (iter != mPreProcessInstructions.end()) {
		XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		delete instruction;
	}

	iter = mPostProcessInstructions.begin();
	while (iter != mPostProcessInstructions.end()) {
		XXMLProcessInstruction* instruction = *iter;
		++iter;
		
		delete instruction;
	}
	
	mPreProcessInstructions.clear();
	mPostProcessInstructions.clear();
	
	delete mRootElement;
	mRootElement = nil;
	
	mElementIDs.clear();
}


//---------------------------------------------------------------
//
// XXMLDoc::SetRootElement
//
//---------------------------------------------------------------
void XXMLDoc::SetRootElement(XXMLElement* takeElement)
{
	delete mRootElement;
	mRootElement = takeElement;
}


//---------------------------------------------------------------
//
// XXMLDoc::FindElement 
//
//---------------------------------------------------------------
XXMLElement* XXMLDoc::FindElement(const std::wstring& id)
{
	IDMap::iterator iter = mElementIDs.find(id);
	if (iter == mElementIDs.end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find an element with an id of '#1'.", id)));
		
	XXMLElement* element = iter->second;
		
	return element;
}


//---------------------------------------------------------------
//
// XXMLDoc::FindElement 
//
//---------------------------------------------------------------
const XXMLElement* XXMLDoc::FindElement(const std::wstring& id) const
{
	IDMap::const_iterator iter = mElementIDs.find(id);
	if (iter == mElementIDs.end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find an element with an id of '#1'.", id)));
		
	const XXMLElement* element = iter->second;
		
	return element;
}


//---------------------------------------------------------------
//
// XXMLDoc::AddElement
//
//---------------------------------------------------------------
void XXMLDoc::AddElement(XXMLElement* element, const std::wstring& id)
{
	PRECONDITION(element != nil);
	
	bool added = mElementIDs.insert(IDMap::value_type(id, element)).second;
	POSTCONDITION(added);				// will only fire if id has already been used which the parser should have caught
}


//---------------------------------------------------------------
//
// XXMLDoc::AddUnparsedEntity
//
//---------------------------------------------------------------
bool XXMLDoc::AddUnparsedEntity(const std::wstring& name, const SExternalID& id)
{
	PRECONDITION(name.length() > 0);
	
	bool added = mUnparsedEntities.insert(NotationMap::value_type(name, id)).second;
	
	return added;
}

			
//---------------------------------------------------------------
//
// XXMLDoc::FindUnparsedEntity
//
//---------------------------------------------------------------
SExternalID XXMLDoc::FindUnparsedEntity(const std::wstring& name)
{
	PRECONDITION(name.length() > 0);

	NotationMap::const_iterator iter = mUnparsedEntities.find(name);
	if (iter == mUnparsedEntities.end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the unparsed entity named: '#1'", name)));
		
	return iter->second;
}

	
//---------------------------------------------------------------
//
// XXMLDoc::AddNotation
//
//---------------------------------------------------------------
bool XXMLDoc::AddNotation(const std::wstring& name, const SExternalID& id)
{
	PRECONDITION(name.length() > 0);
	
	bool added = mNotations.insert(NotationMap::value_type(name, id)).second;
	
	return added;
}

			
//---------------------------------------------------------------
//
// XXMLDoc::FindNotation
//
//---------------------------------------------------------------
SExternalID XXMLDoc::FindNotation(const std::wstring& name)
{
	PRECONDITION(name.length() > 0);

	NotationMap::const_iterator iter = mNotations.find(name);
	if (iter == mNotations.end())
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Couldn't find the notation named: '#1'", name)));
		
	return iter->second;
}

	
//---------------------------------------------------------------
//
// XXMLDoc::AddPreProcessInstruction
//
//---------------------------------------------------------------
void XXMLDoc::AddPreProcessInstruction(XXMLProcessInstruction* takeInstruction)
{
	PRECONDITION(takeInstruction != nil);
	
	mPreProcessInstructions.push_back(takeInstruction);
}


//---------------------------------------------------------------
//
// XXMLDoc::AddPostProcessInstruction
//
//---------------------------------------------------------------
void XXMLDoc::AddPostProcessInstruction(XXMLProcessInstruction* takeInstruction)
{
	PRECONDITION(takeInstruction != nil);
	
	mPostProcessInstructions.push_back(takeInstruction);
}

#if __MWERKS__
#pragma mark Ê
#endif

//---------------------------------------------------------------
//
// XXMLDoc::Invariant
//
//---------------------------------------------------------------
void XXMLDoc::Invariant() const
{	
	if (!mElementIDs.empty())
		ASSERT(mRootElement != nil);
}


//---------------------------------------------------------------
//
// XXMLDoc::OnStreamOut
//
//---------------------------------------------------------------
void XXMLDoc::OnStreamOut(XOutStream& stream) const
{	
	stream << 1L;
	
	if (mRootElement != nil) {
		stream << true << *mRootElement;
	
		stream << (uint32) mElementIDs.size();
		IDMap::const_iterator iter = mElementIDs.begin();
		while (iter != mElementIDs.end()) {
			IDMap::value_type value = *iter;
			++iter;
			
			stream << value.first << stream.GetCacheIndex(value.second);
		}

	} else
		stream << false;
		
	stream << (uint32) mPreProcessInstructions.size();
	ProcessInstructions::const_iterator iter = mPreProcessInstructions.begin();
	while (iter != mPreProcessInstructions.end()) {
		const XXMLProcessInstruction* pi = *iter;
		++iter;
		
		XBinaryPersistentMixin::Flatten(stream, pi);
	}
		
	stream << (uint32) mPostProcessInstructions.size();
	iter = mPostProcessInstructions.begin();
	while (iter != mPostProcessInstructions.end()) {
		const XXMLProcessInstruction* pi = *iter;
		++iter;
		
		XBinaryPersistentMixin::Flatten(stream, pi);
	}
}


//---------------------------------------------------------------
//
// XXMLDoc::OnStreamIn
//
//---------------------------------------------------------------
void XXMLDoc::OnStreamIn(XInStream& stream)
{	
	int32 version;
	bool hasRoot;
	stream >> version;

	if (version != 1)
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: Invalid XXMLDoc version.")));
		
	stream >> hasRoot;
	mElementIDs.clear();
		
	uint32 count;
	if (hasRoot) {
		if (mRootElement == nil)
			mRootElement = new XXMLElement(L"dummy name");
			
		stream >> *mRootElement;
		
		uint32 index;
		std::wstring name;
		stream >> count;
		for (uint32 i = 0; i < count; ++i) {
			stream >> name >> index;
			
			XBinaryPersistentMixin* object = stream.GetCachedObject(index);
			XXMLElement* element = dynamic_cast<XXMLElement*>(object);
			ASSERT(element != nil);
		
			mElementIDs.insert(IDMap::value_type(name, element));
		}

	} else {
		delete mRootElement;
		mRootElement = nil;
	}
	
	uint32 i;
	stream >> count;
	mPreProcessInstructions.clear();
	for (i = 0; i < count; ++i) {
		XBinaryPersistentMixin* object = XBinaryPersistentMixin::Unflatten(stream, this);
		XXMLProcessInstruction* pi = dynamic_cast<XXMLProcessInstruction*>(object);
		if (pi == nil) {
			delete object;
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XXMLDoc::OnStreamIn created something that wasn't a XXMLProcessInstruction.")));
		}
		
		mPreProcessInstructions.push_back(pi);
	}

	stream >> count;
	mPostProcessInstructions.clear();
	for (i = 0; i < count; ++i) {
		XBinaryPersistentMixin* object = XBinaryPersistentMixin::Unflatten(stream, this);
		XXMLProcessInstruction* pi = dynamic_cast<XXMLProcessInstruction*>(object);
		if (pi == nil) {
			delete object;
			throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Internal Error: XXMLDoc::OnStreamIn created something that wasn't a XXMLProcessInstruction.")));
		}
		
		mPostProcessInstructions.push_back(pi);
	}
}


}	// namespace Whisper

