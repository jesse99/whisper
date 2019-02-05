/*
 *  File:		XXMLItems.h
 *  Summary:	Concrete classes for things that can be nested within an XML element..
 *  Written by:	Jesse Jones
 *
 *	Classes:	XXMLCharData			- The item consists of Unicode character data.
 *				XXMLComment				- The item consists of the contents of a comment.
 *				XXMLProcessInstruction	- The item consists of an instruction for a named application.
 *				XXMLElement				- The item consists of an XML element with optional attributes and child XXMLItem's.
 *
 *  Copyright © 1999-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XXMLItems.h,v $
 *		Revision 1.4  2001/03/06 07:33:41  jesjones
 *		Replaced size() with empty() where possible (std::list::size() may take linear time to execute).
 *		
 *		Revision 1.3  2000/11/10 04:47:03  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>

#include <XXMLItem.h>
#include <XXMLAttribute.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


// ===================================================================================
//	class XXMLCharData
// ===================================================================================
class CORE_EXPORT XXMLCharData : public XXMLItem {

	typedef XXMLItem Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLCharData();

						XXMLCharData(const std::wstring& text);

	static XCreateByNameMixin* Create(XCreateByNameMixin* parent);

//-----------------------------------
//	New API
//
public:									
			const std::wstring& GetText() const						{return mText;}
			void 		SetText(const std::wstring& text)			{mText = text;}
						// Note the line ends are always marked with a single '\n' character.
			
//-----------------------------------
//	Inherited API
//
public:									
	virtual bool 		HandleVisit(XXMLDocVisitor& visitor) const;		
			
protected:								
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring		mText;
};


// ===================================================================================
//	class XXMLComment
// ===================================================================================
class CORE_EXPORT XXMLComment : public XXMLItem {

	typedef XXMLItem Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLComment();

						XXMLComment(const std::wstring& contents);

	static XCreateByNameMixin* Create(XCreateByNameMixin* parent);

//-----------------------------------
//	New API
//
public:									
			const std::wstring& GetText() const							{return mContents;}
			void 		SetText(const std::wstring& contents)		{mContents = contents;}
			
//-----------------------------------
//	Inherited API
//
public:									
	virtual bool 		HandleVisit(XXMLDocVisitor& visitor) const;		
			
protected:								
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring		mContents;
};


// ===================================================================================
//	class XXMLProcessInstruction
// ===================================================================================
class CORE_EXPORT XXMLProcessInstruction : public XXMLItem {

	typedef XXMLItem Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLProcessInstruction();

						XXMLProcessInstruction(const std::wstring& target, const std::wstring& data);
						
	static XCreateByNameMixin* Create(XCreateByNameMixin* parent);

//-----------------------------------
//	New API
//
public:									
			const std::wstring& GetTarget() const						{return mTarget;}
			void 		SetTarget(const std::wstring& target)			{ASSERT(target.length() > 0); mTarget = target;}
			
			const std::wstring& GetData() const							{return mData;}
			void 		SetData(const std::wstring& data)				{mData = data;}
			
//-----------------------------------
//	Inherited API
//
public:									
	virtual bool 		HandleVisit(XXMLDocVisitor& visitor) const;		
			
protected:								
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring		mTarget;
	std::wstring		mData;
};


// ===================================================================================
//	class XXMLElement
// ===================================================================================
class CORE_EXPORT XXMLElement : public XXMLItem {

	typedef XXMLItem Inherited;

//-----------------------------------
//	Types
//
public:
	typedef std::list<XXMLAttribute>	Attributes;	
	typedef std::list<XXMLItem*> 		Children;		

	typedef Attributes::iterator 		attr_iterator;	
	typedef Children::iterator 			item_iterator;		

	typedef Attributes::const_iterator 	const_attr_iterator;	
	typedef Children::const_iterator 	const_item_iterator;
	
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XXMLElement();

						XXMLElement(const std::wstring& name);
						
	static XCreateByNameMixin* Create(XCreateByNameMixin* parent);

//-----------------------------------
//	New API
//
public:									
	// ----- Element Name -----
			const std::wstring& GetName() const						{return mName;}
			
	// ----- Attributes -----
			void 		AppendAttribute(const XXMLAttribute& attribute);
			void 		RemoveAttribute(const attr_iterator& iter) 	{mAttributes.erase(iter);}
			
			attr_iterator attr_begin() 							{return mAttributes.begin();}
			attr_iterator attr_end() 							{return mAttributes.end();}
						
			const_attr_iterator attr_begin() const				{return mAttributes.begin();}
			const_attr_iterator attr_end() const				{return mAttributes.end();}
			
			XXMLAttribute attr_front() const					{ASSERT(!mAttributes.empty()); return mAttributes.front();}
			XXMLAttribute attr_back() const						{ASSERT(!mAttributes.empty()); return mAttributes.back();}
			
			uint32 		GetNumAttributes() const				{return mAttributes.size();}

	// ----- Items -----
			void 		AppendItem(XXMLItem* takeItem);
			void 		RemoveItem(const item_iterator& iter)	{mChildren.erase(iter);}
			void 		DeleteItem(const item_iterator& iter)	{delete *iter; mChildren.erase(iter);}

			item_iterator item_begin() 							{return mChildren.begin();}
			item_iterator item_end() 							{return mChildren.end();}
			
			const_item_iterator item_begin() const				{return mChildren.begin();}
			const_item_iterator item_end() const				{return mChildren.end();}
			
			XXMLItem* item_front() 								{ASSERT(!mChildren.empty()); return mChildren.front();}
			XXMLItem* item_back() 								{ASSERT(!mChildren.empty()); return mChildren.back();}

			const XXMLItem* item_front() const					{ASSERT(!mChildren.empty()); return mChildren.front();}
			const XXMLItem* item_back() const					{ASSERT(!mChildren.empty()); return mChildren.back();}

			uint32 		GetNumItems() const						{return mChildren.size();}

	// ----- Searching -----
			const XXMLAttribute& FindAttribute(const std::wstring& name) const;
							// Throws if the item cannot be found.

			const XXMLElement* FindElement(const std::wstring& name, bool recursive = kNonRecursive) const;
							// Returns nil if the element cannot be found.

//-----------------------------------
//	Inherited API
//
public:									
	virtual bool 		HandleVisit(XXMLDocVisitor& visitor) const;		
			
protected:								
	virtual	void 		OnStreamOut(XOutStream& stream) const;

	virtual void 		OnStreamIn(XInStream& stream);

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mName;
	Attributes		mAttributes;
	Children		mChildren;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

