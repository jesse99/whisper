/*
 *  File:		XXMLDTD.cpp
 *  Summary:	A class used to encapsulate an XML DTD (this is the section that describes
 *				what an element can contain, attribute types, and attribute defaults).
 *  Written by:	Jesse Jones
 *
 *  Copyright � 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLDTD.cpp,v $
 *		Revision 1.3  2001/04/27 04:38:53  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.2  2000/11/09 13:01:19  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <9>	 8/11/00	JDJ		XXMLElementDeclaration::GetAttribute returns a pointer instead
 *									of a reference (to allow for better error reporting).
 *		 <8>	 3/26/00	JDJ		Children of ANY elements must be declared.
 *		 <7>	 3/26/00	JDJ		Moved the notation and unparsed entity stuff from XXMLDTD to XXMLDoc.
 *		 <6>	 3/17/00	JDJ		Symbol values can now be over 255.
 *		 <5>	10/07/99	JDJ		mNextSymbol starts at 1 instead of 'a'
 *		 <4>	 6/30/99	JDJ		DoGetSymbol no longer returns '|'.
 *		 <3>	 6/27/99	JDJ		ValidText and ValidChild return true for undeclared elements.
 *		 <2>	 3/19/99	JDJ		Removed AddID and FindElement.
 *		 <1>	 1/30/99	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <XXMLDTD.h>

#include <XNDFA.h>
#include <XRegularExpression.h>
#include <XStringUtils.h>
#include <XTranscode.h>

namespace Whisper { 


//-----------------------------------
//	Constants
//
const int32 kStartState = '_';		// arbitrary, but cannot be in DoGetNextSymbol range


// ===================================================================================
//	class XXMLAttributeDeclaration
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLAttributeDeclaration::~XXMLAttributeDeclaration
//
//---------------------------------------------------------------
XXMLAttributeDeclaration::~XXMLAttributeDeclaration()
{
}
		

//---------------------------------------------------------------
//
// XXMLAttributeDeclaration::XXMLAttributeDeclaration ()
//
//---------------------------------------------------------------
XXMLAttributeDeclaration::XXMLAttributeDeclaration()
{	
	mType = kStringAttribute;
	mDefault = kRequiredAttribute;
	mExternal = false;
}


//---------------------------------------------------------------
//
// XXMLAttributeDeclaration::XXMLAttributeDeclaration
//
//---------------------------------------------------------------
XXMLAttributeDeclaration::XXMLAttributeDeclaration(EAttributeType type, EAttributeDefault deflt, const std::wstring& value) : mValue(value)
{
	ASSERT_IF(value.length() > 0, deflt == kFixedAttribute || deflt == kDefaultAttribute);
	
	mType = type;
	mDefault = deflt;
	mExternal = false;
}


//---------------------------------------------------------------
//
// XXMLAttributeDeclaration::MatchEnum
//
//---------------------------------------------------------------
bool XXMLAttributeDeclaration::MatchEnum(const std::wstring& name) const
{
	PRECONDITION(mType == kEnumAttribute || mType == kNotationAttribute);
	
	EnumList::const_iterator iter = std::find(mEnums.begin(), mEnums.end(), name);

	bool matched = iter != mEnums.end();
	
	return matched; 
}


//---------------------------------------------------------------
//
// XXMLAttributeDeclaration::AddEnum
//
//---------------------------------------------------------------
void XXMLAttributeDeclaration::AddEnum(const std::wstring& name)
{
	PRECONDITION(mType == kEnumAttribute || mType == kNotationAttribute);
	
	if (!this->MatchEnum(name))			// as far as I can tell it's perfectly OK to list the same name in an enum multiple times...
		mEnums.push_back(name);
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLElementDeclaration
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLElementDeclaration::~XXMLElementDeclaration
//
//---------------------------------------------------------------
XXMLElementDeclaration::~XXMLElementDeclaration()
{
	mDTD = nil;
}
		

//---------------------------------------------------------------
//
// XXMLElementDeclaration::XXMLElementDeclaration 
//
//---------------------------------------------------------------
XXMLElementDeclaration::XXMLElementDeclaration(XXMLDTD* dtd, EElementType type) : mDFA(new XDFA)
{
	PRECONDITION(dtd != nil);
	
	mDTD = dtd;
	mType = type;
	mSymbol = mDTD->DoGetSymbol();					// rather than using a static we'll ask the DTD for a code (this is more thread safe and we won't blow any limits if lot's of DTD's are created)
	mExternal = false;
	mHasIDAttribute = false;
	
	if (type != kUndeclaredElement) {
		mDFA->SetStartState(kStartState);
	
		if (mType == kMixedElement)
			mDFA->AddHaltState(kStartState);		// all transitions go from kStartState back to kStartState
	}
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::GetAttribute
//
//---------------------------------------------------------------
const XXMLAttributeDeclaration* XXMLElementDeclaration::GetAttribute(const std::wstring& name) const
{	
	const XXMLAttributeDeclaration* attr = nil;
	
	AttributeMap::const_iterator iter = mAttributes.find(name);
	if (iter != mAttributes.end())
		attr = &(iter->second);
		
	return attr;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::AddAttribute
//
//---------------------------------------------------------------
bool XXMLElementDeclaration::AddAttribute(const std::wstring& name, const XXMLAttributeDeclaration& attribute)
{	
	bool added = mAttributes.insert(AttributeMap::value_type(name, attribute)).second;

	if (added && attribute.GetType() == kIDAttribute) {
		ASSERT(!mHasIDAttribute);
		mHasIDAttribute = true;
	}
	
	return added;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::AddMixedElement
//
//---------------------------------------------------------------
void XXMLElementDeclaration::AddMixedElement(const std::wstring& name)
{
	PRECONDITION(mType == kMixedElement);
	
	const XXMLElementDeclaration* child = mDTD->GetElement(name);
	wchar_t symbol = child->mSymbol;
	
	if (mDFA->HasTransition(symbol))			// VC: No Duplicate Types
		throw std::runtime_error(ToUTF8Str(LoadWhisperString(L"Mixed element '#1' has been included more than once.", name)));

	mDFA->AddTransition(kStartState, symbol, kStartState);
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::AppendChildElement
//
//---------------------------------------------------------------
void XXMLElementDeclaration::AppendChildElement(const std::wstring& name)	
{
	PRECONDITION(mType == kChildrenElement);
	
	const XXMLElementDeclaration* child = mDTD->GetElement(name);
	wchar_t symbol = child->mSymbol;
	
	mExpression += symbol;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::AppendChildSymbol
//
//---------------------------------------------------------------
void XXMLElementDeclaration::AppendChildSymbol(wchar_t symbol)
{
	PRECONDITION(mType == kChildrenElement);
	PRECONDITION(symbol != 0);
	
	mExpression += symbol;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::StartValidating
//
//---------------------------------------------------------------
void XXMLElementDeclaration::StartValidating()
{	
	if (mType == kMixedElement || mType == kChildrenElement) {
		if (mType == kChildrenElement && mExpression.length() > 0) {
			XNDFA ndfa;
			ZConvertRegExpr converter(&ndfa, mExpression);		
		
			mDFA.Reset(ndfa.CreateDFA());
	
			mExpression.clear();
		}
	
		mDFA->Reset();
	}
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::ValidText
//
//---------------------------------------------------------------
bool XXMLElementDeclaration::ValidText(const std::wstring& text)
{
	bool valid = false;
	
	uint32 index;
	switch (mType) {
		case kEmptyElement:
		case kChildrenElement:
			valid = true;
			for (index = 0; index < text.length() && valid; ++index) {	// all white space outside of markup is supposed to be passed to the application so we have to accept whitespace here...
				wchar_t ch = text[index];
				valid = ch == 0x09 || ch == 0x0A || ch == 0x0D || ch == 0x20;
			}
			break;
			
		case kAnyElement:
		case kMixedElement:
			valid = true;
			break;
				
		case kUndeclaredElement:
			valid = true;
			break;
			
		default:
			DEBUGSTR("Bad type in XXMLElementDeclaration::ValidText");
	}
	
	return valid;
}
			

//---------------------------------------------------------------
//
// XXMLElementDeclaration::ValidChild
//
//---------------------------------------------------------------
bool XXMLElementDeclaration::ValidChild(const std::wstring& name)
{
	bool valid = false;
	
	switch (mType) {
		case kEmptyElement:
			valid = false;
			break;
			
		case kAnyElement:
			XXMLElementDeclaration* element = mDTD->GetElement(name);
			valid = element->GetType() != kUndeclaredElement;
			break;
		
		case kMixedElement:
		case kChildrenElement:
			mDFA->NextState(mDTD->GetElement(name)->mSymbol);
			valid = !mDFA->IsBlocked();
			break;
							
		case kUndeclaredElement:
			valid = true;
			break;
			
		default:
			DEBUGSTR("Bad type in XXMLElementDeclaration::ValidChild");
	}
		
	return valid;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::StopValidating
//
//---------------------------------------------------------------
bool XXMLElementDeclaration::StopValidating() const
{
	bool valid = true;
	
	if (mType == kChildrenElement)
		valid = mDFA->CanHalt();
	
	return valid;
}


//---------------------------------------------------------------
//
// XXMLElementDeclaration::DeclareElement
//
// This kind of sucks, but it's needed to handle attribute lists 
// declared before the element. 
//
//---------------------------------------------------------------
void XXMLElementDeclaration::DeclareElement(XXMLElementDeclaration* element)
{
	PRECONDITION(mType == kUndeclaredElement);
	PRECONDITION(element != nil);
	PRECONDITION(element->mType != kUndeclaredElement);
	
	mDTD = element->mDTD;
	mType = element->mType;
	mDFA = element->mDFA;
	mExpression = element->mExpression;

	mAttributes.insert(element->mAttributes.begin(), element->mAttributes.end());
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XXMLDTD
// ===================================================================================

//---------------------------------------------------------------
//
// XXMLDTD::~XXMLDTD
//
//---------------------------------------------------------------
XXMLDTD::~XXMLDTD()
{
}
		

//---------------------------------------------------------------
//
// XXMLDTD::XXMLDTD
//
//---------------------------------------------------------------
XXMLDTD::XXMLDTD()
{
	mNextSymbol = 'a';		// this is arbitrary (except that we don't want to generate a symbol that matches one of the characters special cased by the regular expression code)
}


//---------------------------------------------------------------
//
// XXMLDTD::Clone
//
//---------------------------------------------------------------
XXMLDTD* XXMLDTD::Clone() const
{
	XXMLDTD* dtd = new XXMLDTD;
	dtd->mElements = mElements;
	
	return dtd;
}


//---------------------------------------------------------------
//
// XXMLDTD::GetElement
//
//---------------------------------------------------------------
XXMLElementDeclaration* XXMLDTD::GetElement(const std::wstring& name)
{
	ElementMap::iterator iter = mElements.find(name);
	if (iter == mElements.end()) {
		XXMLElementDeclaration* element = new XXMLElementDeclaration(this, kUndeclaredElement);
		iter = mElements.insert(ElementMap::value_type(name, ElementPtr(element))).first;
	}
	
	return iter->second.Get();
}


//---------------------------------------------------------------
//
// XXMLDTD::GetElement
//
//---------------------------------------------------------------
const XXMLElementDeclaration* XXMLDTD::GetElement(const std::wstring& name) const
{
	ElementMap::const_iterator iter = mElements.find(name);
	if (iter == mElements.end()) {
		XXMLDTD* thisPtr = const_cast<XXMLDTD*>(this);

		XXMLElementDeclaration* element = new XXMLElementDeclaration(thisPtr, kUndeclaredElement);
		iter = thisPtr->mElements.insert(ElementMap::value_type(name, ElementPtr(element))).first;
	}
	
	return iter->second.Get();
}


//---------------------------------------------------------------
//
// XXMLDTD::AddElement
//
//---------------------------------------------------------------
bool XXMLDTD::AddElement(const std::wstring& name, XXMLElementDeclaration* takeElement)
{
	PRECONDITION(takeElement != nil);
	
	bool added = true;
	
	ElementMap::const_iterator iter = mElements.find(name);
	if (iter == mElements.end()) {
		mElements.insert(ElementMap::value_type(name, ElementPtr(takeElement)));
	
	} else if (iter->second->GetType() == kUndeclaredElement) {
		iter->second->DeclareElement(takeElement);
		delete takeElement;
	
	} else {
		added = false;
		delete takeElement;
	}
	
	return added;
}


//---------------------------------------------------------------
//
// XXMLDTD::DoGetSymbol
//
//---------------------------------------------------------------
wchar_t XXMLDTD::DoGetSymbol()
{
	while (mNextSymbol == '.' || mNextSymbol == '*' || mNextSymbol == '+' || 	// can't use characters special cased by the regular expression code
		   mNextSymbol == '(' || mNextSymbol == ')' || mNextSymbol == '[' || 
		   mNextSymbol == ']' || mNextSymbol == '-' || mNextSymbol == '^' || 
		   mNextSymbol == '$' || mNextSymbol == '\\' || mNextSymbol == '?' || 
		   mNextSymbol == '|')
		++mNextSymbol;

	wchar_t ch = mNextSymbol++;
					
	return ch;
}


}	// namespace Whisper

