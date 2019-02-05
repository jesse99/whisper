/*
 *  File:       XFragment.h
 *  Summary:   	Wrapper around code fragments/DLLs.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1999 Jesse Jones. All Rights Reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: XFragment.h,v $
 *		Revision 1.4  2001/03/03 11:13:30  jesjones
 *		Added XFragmentException dtor to get rid of a linker warning when building DLL targets.
 *		
 *		Revision 1.3  2000/12/11 01:30:17  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:37:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <stdexcept>

#include <XTypes.h>

#if MAC
	#include <CodeFragments.h>
#endif

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XFileSpec;


//-----------------------------------
//	Types and Constants
//
#if MAC
	typedef CFragConnectionID OSFragment;
#elif WIN
	typedef HINSTANCE OSFragment;
#endif

enum EFragOptions {kUseExistingFragment,	//!< if the fragment is already loaded then use the previously loaded fragment
#if MAC
				   kLoadNewFragment			//!< if the fragment is already loaded then load a new fragment (ie the new fragment will have it's own copy of the fragment's global data)
#endif
				   };	
				   

// ===================================================================================
//	struct SSymbol
//!		A symbol exported by a DLL. Returned by XFragment.
// ===================================================================================
#if MAC
struct COM_EXPORT SSymbol {
	std::wstring	name;
	const void*		ptr;
	bool			isCode;
};
#endif


// ===================================================================================
//	class XLoadFragmentException
//!		This is thrown by the XFragment ctors.
// ===================================================================================
class COM_EXPORT XLoadFragmentException : public std::runtime_error { 
public: 
		virtual		~XLoadFragmentException();
		
					XLoadFragmentException(OSError err, const std::wstring& name, const std::wstring& errName);

					XLoadFragmentException(const std::exception& e, const std::wstring& name, const std::wstring& errName);

public:
	std::wstring	mName;		//!< the library we were trying to load
	std::wstring	mErrName;	//!< the library that failed to load (on Windows this will always be the same as mName)
	OSError			mError;
};


// ===================================================================================
//	class XFragment
//!		Wrapper around code fragments/DLLs.
// ===================================================================================
class COM_EXPORT XFragment {

//-----------------------------------
//	Initialization/Destruction
//
public:
			 			~XFragment();
	
  						XFragment(const std::wstring& name, EFragOptions options = kUseExistingFragment);
  						/**< Searches for the library. On Windows this will search:
  						   1) The application's directory
  						   2) The current directory
  						   3) The Windows system directory
  						   4) The Windows directory
  						   5) The directories listed in the PATH variable.
  						On the Mac this will search:
  						   1) The application's data and resource forks
  						   2) The application's library folder (this is specified in the app's 'cfrg' resource)
  						   3) The application's folder
  						   4) The extensions folder and all sub-folders
  						   5) The ROM registry (this points to fragments burned into ROM)
  						   6) The file and directory registry (this is for special cases like the Component Manager)
  						$$$ I think Apple has tweaked the search order from what was documented in IM */

  						XFragment(const XFileSpec& spec, EFragOptions options = kUseExistingFragment);

#if MAC
  						XFragment(const XFileSpec& spec, uint32 offset, uint32 bytes, EFragOptions options = kUseExistingFragment);

  						XFragment(const std::wstring& name, const void* adr, uint32 bytes, EFragOptions options = kUseExistingFragment);
  						/**< The memory must be locked if it's inside a handle and cannot
  						be unlocked or deleted until the fragment is closed. */
#endif

private:
						XFragment(const XFragment& rhs);
						
			XFragment& 	operator=(const XFragment& rhs);

//-----------------------------------
//	API
//
public:
	//! @name Accessors
	//@{
			std::wstring GetName() const						{return mName;}
			
			OSFragment 	GetOSFragment() const					{return mFragment;}

#if MAC
			void*		GetMainAdr() const						{return mMainAdr;}
						/**< Returns a function pointer to the main entry point. Note that
						this can have an arbitrary signature and it is not called
						automatically. */
#endif
	//@}

	//! @name Symbols
	//@{
			void* 		GetFunction(const std::wstring& name) const;
						/**< Returns the function pointer for the given symbol or nil if
						the function cannot be found. */

#if MAC
			void* 		GetData(const std::wstring& name) const;
						/**< Returns a pointer to the specified global data or nil if
						the data cannot be found. */
			
			uint32 		GetNumSymbols() const;
			SSymbol 	GetSymbol(uint32 index) const;
						/**< Iterates over all symbols in the fragment. */
#endif
	//@}

//-----------------------------------
//	Member Data
//
protected:
	std::wstring	mName;
	OSFragment		mFragment;
#if MAC
	Ptr				mMainAdr;
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper
