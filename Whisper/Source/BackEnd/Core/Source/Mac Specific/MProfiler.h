/*
 *  File:       MProfiler.h
 *  Summary:   	Stack based class used to profile blocks of code.
 *  Written by: Jesse Jones
 *
 *  Copyright � 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: MProfiler.h,v $
 *		Revision 1.1  2001/08/19 22:53:50  jesjones
 *		Checked in
 *		
 */

#pragma once

#if	__profile__
	#include <Profiler.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//================================================================================
// class MProfiler
//================================================================================
class CORE_EXPORT MProfiler {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~MProfiler();
					   
						MProfiler(const std::wstring& fileName, int32 numFunctions = 2000, int32 stackDepth = 75);
						/**< Destructor will ASSERT if numFunctions or stackDepth isn't 
						large enough. But note that the profiler (up to Pro 3 at 
						least) uses shorts internally: if numFunctions is too large 
						the profiler will start overflowing and the numbers returned
						by ProfilerGetDataSizes will be entirely bogus... */

//-----------------------------------
//	API
//
public:
			bool		IsEnabled() const;
						/**< Defaults to true. */

			void		Enable();
						/**< Turns profiling on. */

			void		Disable();
						/**< Turns profiling off. */

//-----------------------------------
//	Member Data
//
protected:
#if	__profile__
	std::wstring	mFileName;
	int				mNumFunctions;
	int				mStackSize;
	long			mEnableCount;
	
	static bool		msConstructed;
#endif
};


//================================================================================
// class MProfileEnabler
//================================================================================
class CORE_EXPORT MProfileEnabler {

public:
					   ~MProfileEnabler();
					   
						MProfileEnabler(bool enable = true);

protected:
#if	__profile__
	bool	mWasEnabled;
#endif
};


//================================================================================
// Inlines
//================================================================================
#if !__profile__
inline MProfiler::~MProfiler()
{
}
		   
inline MProfiler::MProfiler(const std::wstring&, int32, int32)
{
}

inline bool MProfiler::IsEnabled() const			
{
	return false;
}

inline void	MProfiler::Enable()
{
}

inline void	MProfiler::Disable()
{
}

inline MProfileEnabler::~MProfileEnabler()
{
}

inline MProfileEnabler::MProfileEnabler(bool)
{
}
#endif	// !__profile__


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
