/*
 *  File:       XTrace.h
 *  Summary:    A class that allows flexible control over what debug output is printed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTrace.h,v $
 *		Revision 1.5  2001/03/05 05:44:07  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.4  2001/01/05 06:25:44  jesjones
 *		Added DeleteSinks (which the dtor now calls).
 *		
 *		Revision 1.3  2000/12/10 08:56:49  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:56:50  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <list>
#include <vector>

#include <XAutoPtr.h>
#include <XCriticalSection.h>
#include <XThreadSafeIter.h>

#if DEBUG
namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif

//-----------------------------------
//	Forward References
//
class XTraceSink;


// ===================================================================================
//	class XTrace
//!		A class that allows flexible control over what debug output is printed.
// ===================================================================================
class CORE_EXPORT XTrace {

//-----------------------------------
//	Types
//
public:		
	struct SCategory {
		std::string	category;
		bool		enabled;
		
					SCategory(const std::string& c = "", bool e = true) : category(c), enabled(e)	{}
					SCategory(const SCategory& rhs);
			
			bool 	operator==(const SCategory& rhs) const	{return category == rhs.category;}
			bool 	operator!=(const SCategory& rhs) const	{return category != rhs.category;}

			bool 	operator==(const char* const rhs) const	{return category == rhs;}
			bool 	operator!=(const char* const rhs) const	{return category != rhs;}
			bool 	operator<(const char* const rhs) const	{return category < rhs;}

			bool 	operator<(const SCategory& rhs) const	{return category < rhs.category;}
	};
		
	typedef std::vector<SCategory> 	CategoryMap;		// traceflow categories are rather static so we can reduce bloat by using a sorted vector instead of a map
	typedef XThreadSafeIter<CategoryMap::const_iterator, XCriticalSection>	cat_iter;

	typedef std::list<XTraceSink*>	Sinks;		
	typedef XThreadSafeIter<Sinks::const_iterator, XCriticalSection>		sink_iter;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XTrace();
						/**< Calls DeleteSinks. */

						XTrace();

	static XTrace* 		Instance();

private:
						XTrace(const XTrace& rhs);
						
			XTrace& 	operator=(const XTrace& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Tracing
	//@{
			void 		Trace(const wchar_t* mesg);
						/**< Writes the message out to zero or more sinks. */

			void 		Trace(const char* category, const wchar_t* mesg);
						/**< Writes the message out only if the category is enabled. */
	//@}

	//! @name Categories
	//@{
			bool 		IsEnabled(const char* category) const;
			
			void 		Enable(const char* category, bool enable = true);
			
			uint32 		GetNumCategories() const;
						/**< Useful for deciding when to call GetCategories. */

			cat_iter 	category_begin() const				{return cat_iter(mCategories.begin(), &mCategoryMutex);}
			cat_iter 	category_end() const				{return cat_iter(mCategories.end(), &mCategoryMutex);}
	//@}
	
	//! @name Sinks
	//@{
			void 		AddSink(XTraceSink* sink);
	
			void 		RemoveSink(XTraceSink* sink);
			void 		DeleteSinks();
			
			bool 		HasSink(const std::wstring& name) const;
			XTraceSink* GetSink(const std::wstring& name);
			
			uint32 		GetNumSinks() const					{return mSinks.size();}

			sink_iter 	sink_begin() const					{return sink_iter(mSinks.begin(), &mSinkMutex);}
			sink_iter 	sink_end() const					{return sink_iter(mSinks.end(), &mSinkMutex);}
	//@}
	
//-----------------------------------
//	Member Data
//
private:				
	Sinks						mSinks;
	mutable CategoryMap			mCategories;
	
	mutable XCriticalSection	mSinkMutex;
	mutable XCriticalSection	mCategoryMutex;
	XCriticalSection			mTraceMutex;
	
#if WIN
	std::wstring				mPrefName;		// cache this so we don't call WSystemInfo at static dtor time
#endif

	static XAutoPtr<XTrace>		msInstance;			
	static XCriticalSection		msCreateMutex;
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}		// namespace Whisper
#endif	// DEBUG
