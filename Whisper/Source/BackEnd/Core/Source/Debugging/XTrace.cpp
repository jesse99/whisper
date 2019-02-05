/*
 *  File:       XTrace.cpp
 *  Summary:    A class that allows flexible control over what debug output is printed.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTrace.cpp,v $
 *		Revision 1.4  2001/04/27 04:37:22  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/01/05 06:25:40  jesjones
 *		Added DeleteSinks (which the dtor now calls).
 *		
 *		Revision 1.2  2000/11/09 12:56:44  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		  <8>	10/02/00	JDJ		Moved the Trace prefs code back to XTraceExtension.
 *		  <7>	 8/12/00	JDJ		Moved the Trace prefs code from XTraceExtension to XTrace.
 *		  <6>	11/27/99	JDJ		Uses XLogFile.
 *		  <5>	 5/28/99	JDJ		Made thread safe.
 *		  <4>	 4/27/99	JDJ		OutputDebugString is now used with CodeWarrior.
 *		  <3>	 4/19/99	JDJ		ZTraceFile::Print compensates if the file name is too long.
 *		  <2>	 4/18/99	JDJ		Uses a console window with WinCW or when no debugger is present.
 *		  <1>	11/02/97	JDJ		Created (from Raven)
 */
 
#include <XWhisperHeader.h>
#include <XTrace.h>

#include <algorithm>

#include <XThread.h>
#include <XTraceSink.h>

#if DEBUG
namespace Whisper {


// ===================================================================================
//	class ZRegisterTrace
// ===================================================================================
class ZRegisterTrace {

public:
						~ZRegisterTrace();

						ZRegisterTrace();

private:
	static 	void 		DoTrace(const wchar_t* mesg);

	static 	bool 		DoTraceFlow(const char* category);
};

static ZRegisterTrace sRegisterTrace;

//---------------------------------------------------------------
//
// ZRegisterTrace::~ZRegisterTrace	
//
//---------------------------------------------------------------
ZRegisterTrace::~ZRegisterTrace()
{
	gTraceFn       = nil;
	gTraceFlowPred = nil;
}


//---------------------------------------------------------------
//
// ZRegisterTrace::ZRegisterTrace	
//
//---------------------------------------------------------------
ZRegisterTrace::ZRegisterTrace()
{
	gTraceFn       = ZRegisterTrace::DoTrace;
	gTraceFlowPred = ZRegisterTrace::DoTraceFlow;
}


//---------------------------------------------------------------
//
// ZRegisterTrace::DoTrace								[static]		
//
//---------------------------------------------------------------
void ZRegisterTrace::DoTrace(const wchar_t* mesg)
{
	XTrace::Instance()->Trace(mesg);
}


//---------------------------------------------------------------
//
// ZRegisterTrace::DoTraceFlow							[static]		
//
//---------------------------------------------------------------
bool ZRegisterTrace::DoTraceFlow(const char* category)
{
	return XTrace::Instance()->IsEnabled(category);
}

#if __MWERKS__
#pragma mark -
#endif
					
// ===================================================================================
//	XTrace::SCategory
// ===================================================================================

//---------------------------------------------------------------
//
// XTrace::SCategory::SCategory
//
// CW 5 won't inline this...
//
//---------------------------------------------------------------
XTrace::SCategory::SCategory(const SCategory& rhs) : category(rhs.category), enabled(rhs.enabled)	
{
}

#if __MWERKS__
#pragma mark -
#endif

// ===================================================================================
//	class XTrace
// ===================================================================================

XAutoPtr<XTrace>	XTrace::msInstance;		
XCriticalSection 	XTrace::msCreateMutex;

//---------------------------------------------------------------
//
// XTrace::~XTrace			
//
//---------------------------------------------------------------
XTrace::~XTrace()
{
	this->DeleteSinks();
}


//---------------------------------------------------------------
//
// XTrace::XTrace			
//
//---------------------------------------------------------------
XTrace::XTrace()
{
	mCategories.reserve(8);
}


//---------------------------------------------------------------
//
// XTrace::Instance										[static]
//
//---------------------------------------------------------------
XTrace* XTrace::Instance()
{
	if (msInstance.Get() == nil) {
		XEnterCriticalSection enter(msCreateMutex);

		if (msInstance.Get() == nil)
			msInstance.Reset(new XTrace);
	}
	
	return msInstance.Get();
}


//---------------------------------------------------------------
//
// XTrace::HasSink	
//
//---------------------------------------------------------------
bool XTrace::HasSink(const std::wstring& name) const
{
	bool has = false;
	
	Sinks::const_iterator iter = mSinks.begin();
	while (iter != mSinks.end() && !has) {
		XTraceSink* candidate = *iter;
		++iter;
		
		 has = name == candidate->GetName();
	}
		
	return has;
}

			
//---------------------------------------------------------------
//
// XTrace::GetSink	
//
//---------------------------------------------------------------
XTraceSink* XTrace::GetSink(const std::wstring& name)
{
	XTraceSink* sink = nil;
	
	Sinks::iterator iter = mSinks.begin();
	while (iter != mSinks.end() && sink == nil) {
		XTraceSink* candidate = *iter;
		++iter;
		
		if (name == candidate->GetName())
			sink = candidate;
	}
	
	POSTCONDITION(sink != nil);
	
	return sink;
}

			
//---------------------------------------------------------------
//
// XTrace::AddSink	
//
//---------------------------------------------------------------
void XTrace::AddSink(XTraceSink* sink)
{
	PRECONDITION(sink != nil);
	PRECONDITION(std::find(mSinks.begin(), mSinks.end(), sink) == mSinks.end());
	PRECONDITION(XThread::InMainThread());
	
	mSinks.push_back(sink);
}


//---------------------------------------------------------------
//
// XTrace::RemoveSink					
//
//---------------------------------------------------------------
void XTrace::RemoveSink(XTraceSink* sink)
{
	PRECONDITION(sink != nil);
	PRECONDITION(XThread::InMainThread());
	
	Sinks::iterator iter = std::find(mSinks.begin(), mSinks.end(), sink);
	PRECONDITION(iter != mSinks.end());
	
	mSinks.erase(iter);
}


//---------------------------------------------------------------
//
// XTrace::DeleteSinks	
//
//---------------------------------------------------------------
void XTrace::DeleteSinks()
{	
	Sinks::iterator iter = mSinks.begin();
	while (iter != mSinks.end()) {
		XTraceSink* sink = *iter;
		++iter;
		
		delete sink;
	}
	
	mSinks.clear();
}

			
//---------------------------------------------------------------
//
// XTrace::IsEnabled					
//
//---------------------------------------------------------------
bool XTrace::IsEnabled(const char* category) const
{
	PRECONDITION(category != nil);
	PRECONDITION(category[0] != '\0');
	
	bool enabled = false;
		
	XEnterCriticalSection enter(mCategoryMutex);
	
	CategoryMap::iterator iter = std::lower_bound(mCategories.begin(), mCategories.end(), category);

	if (iter != mCategories.end() && iter->category == category)
		enabled = iter->enabled;
	else 
		mCategories.insert(iter, SCategory(category, enabled));	
	
	return enabled;
}


//---------------------------------------------------------------
//
// XTrace::Enable						
//
//---------------------------------------------------------------
void XTrace::Enable(const char* category, bool enable)
{	
	PRECONDITION(category != nil);
	PRECONDITION(category[0] != '\0');
	
	XEnterCriticalSection enter(mCategoryMutex);

	CategoryMap::iterator iter = std::lower_bound(mCategories.begin(), mCategories.end(), category);

	if (iter != mCategories.end() && iter->category == category)
		iter->enabled = enable;
	else 
		mCategories.insert(iter, SCategory(category, enable));	
}


//---------------------------------------------------------------
//
// XTrace::GetNumCategories						
//
//---------------------------------------------------------------
uint32 XTrace::GetNumCategories() const
{
	uint32 count = 0;
	
	{
	XEnterCriticalSection enter(mCategoryMutex);
		count = mCategories.size();
	}
	
	return count;
}


//---------------------------------------------------------------
//
// XTrace::Trace (wchar_t*)							
//
//---------------------------------------------------------------
void XTrace::Trace(const wchar_t* mesg)
{
	PRECONDITION(mesg != nil);
	
	XEnterCriticalSection enter(mTraceMutex);
	
	Sinks::const_iterator iter = mSinks.begin();
	while (iter != mSinks.end()) {
		XTraceSink* sink = *iter;
		++iter;
		
		sink->HandlePrint(mesg);
	}
}


//---------------------------------------------------------------
//
// XTrace::Trace (wchar_t*, wchar_t*)				
//
//---------------------------------------------------------------
void XTrace::Trace(const char* category, const wchar_t* mesg)
{
	PRECONDITION(category != nil);
	PRECONDITION(mesg != nil);
	
	XEnterCriticalSection enter(mTraceMutex);
	
	if (XTrace::IsEnabled(category)) {
		Sinks::const_iterator iter = mSinks.begin();
		while (iter != mSinks.end()) {
			XTraceSink* sink = *iter;
			++iter;
			
			sink->HandlePrint(mesg);
		}
	}
}


}		// namespace Whisper
#endif	// DEBUG
