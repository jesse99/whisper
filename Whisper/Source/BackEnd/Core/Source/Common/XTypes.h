/*
 *  File:		XTypes.h
 *  Summary:	Types used by Whisper.
 *  Written by:	Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XTypes.h,v $
 *		Revision 1.8  2001/05/12 11:14:32  jesjones
 *		Added TakePtr.
 *		
 *		Revision 1.7  2001/03/03 11:11:04  jesjones
 *		Added Windows BlockMoveData functions (from XGeneric.h).
 *		
 *		Revision 1.6  2001/02/28 10:01:56  jesjones
 *		UNUSED is now in the global namespace.
 *		
 *		Revision 1.5  2000/12/10 08:57:08  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.4  2000/11/27 07:23:04  jesjones
 *		Disabled the no side effects warning.
 *		
 *		Revision 1.3  2000/11/20 05:30:25  jesjones
 *		Includes <string>.
 *		
 *		Revision 1.2  2000/11/09 09:25:09  jesjones
 *		Removed the old license code.
 */

#pragma once

#include <memory>
#include <string>

#if MSVC >= 1100
	#include <xutility>
#else
	#include <algorithm>
#endif

#if MAC
	#include <Files.h>
	#include <Menus.h>
	#include <Resources.h>
	#include <Types.h>
#endif

#include <XGeneric.h>

namespace Whisper {


// ===================================================================================
//	Types	
//		Note that the int types (eg int32) are defined in XWhisperHeader.h
// ===================================================================================
#if WIN
	typedef int32				ResID;
	typedef uint32				ResType;
	typedef uint32				OSType;
	
	typedef RGBQUAD 			OSColor;
	typedef RGBQUAD* 			OSColorTable;
	typedef HWND				OSControl;
	typedef FILETIME 			OSDate;					//!< time from Jan 1, 1601 in tenths of a millisecond
	typedef int32				OSDisplay;				//!< $$$ need to update this (and XDisplay.h for multiple monitors)
	typedef HGDIOBJ				OSDrawAttribute;
	typedef HDC					OSDrawContext;
	typedef uint32				OSError;
	typedef std::wstring 		OSFSSpec;
	typedef HMENU	 			OSMenu;
	typedef HPALETTE 			OSPalette;
	typedef HBITMAP				OSPixMap;
	typedef POINT 				OSPoint;
	typedef RECT 				OSRect;
	typedef HRGN 				OSRegion;
	typedef SIZE 				OSSize;
	typedef std::wstring		OSVolume;				//!< normally a single char, but may be longer for network volumes
	typedef HWND				OSWindow;
	typedef MSG					OSEvent;

#elif MAC
	typedef RGBColor 			OSColor;
	typedef CTabHandle 			OSColorTable;
	typedef ControlHandle		OSControl;
	typedef LongDateCvt 		OSDate;					//!< time from Jan 1, 1904 in seconds
	typedef GDHandle			OSDisplay;
	typedef Handle				OSDrawAttribute;		//!< always nil
	typedef GrafPtr				OSDrawContext;
	typedef OSStatus			OSError;
	typedef EventRecord			OSEvent;
	typedef FSSpec 				OSFSSpec;
	typedef MenuHandle 			OSMenu;
	typedef PaletteHandle		OSPalette;
	typedef PixMapHandle		OSPixMap;
	typedef Point 				OSPoint;
	typedef Rect 				OSRect;
	typedef RgnHandle 			OSRegion;
	typedef Point 				OSSize;
	typedef int16 				OSVolume;
	typedef WindowRef			OSWindow;
#endif

typedef int32					MilliSecond;
typedef std::wstring			XMenuCommand;


// ===================================================================================
//	Functions that make it easier to write code with warn_implicitconv on
// ===================================================================================
#if MAC
	inline void 	BlockMoveData(const void *srcPtr, void *destPtr, uint32 byteCount) 	{::BlockMoveData(srcPtr, destPtr, (long) byteCount);}
	inline Handle 	NewHandle(uint32 byteCount)											{return ::NewHandle((long) byteCount);}
	inline Ptr 		NewPtr(uint32 byteCount)											{return ::NewPtr((long) byteCount);}

	inline void 	SetHandleSize(Handle h, uint32 newSize)								{::SetHandleSize(h, (long) newSize);}
	inline void 	SetPtrSize(Ptr p, uint32 newSize)									{::SetPtrSize(p, (long) newSize);}
#endif


//===================================================================================
//	Windows Routines
//===================================================================================
#if WIN
	inline void 	BlockMoveData(const void *srcPtr, void *destPtr, unsigned long byteCount) 	{memmove(destPtr, srcPtr, byteCount);}

	inline void 	BlockMoveData(const void *srcPtr, void *destPtr, long byteCount) 			{memmove(destPtr, srcPtr, (size_t) byteCount);}
#endif

}	// namespace Whisper


// ===================================================================================
//	Misc Folderol
// ===================================================================================
template <class T>					
inline void UNUSED(const volatile T&)	{}		// Note that this generates no code on Mac or Win (if inlining is on).

template <typename T>							// instead of Foo(int* takePtr) you can use foo(TakePtr<int> ptr), this makes the calling code clearer
class TakePtr {
public:
	explicit TakePtr(T* p)		{mPtr = p;}
	
	template <typename U>
	TakePtr(std::auto_ptr<U> p)	{mPtr = p.release();}
	
	T* get() const				{return mPtr;}
	T* release()				{T* temp = mPtr; mPtr = nil; return temp;}
private:
	T* mPtr;
};

#if MSVC >= 1100								// MSVC 5 and 6 include a bunch of stuff that is erroneously in the global namespace
	#define STD		
#else
	#define STD	std
#endif

#if __MWERKS__ >= 0x2300
	#pragma const_strings on					// so string literals can only be assigned to const pointers
#endif

#if __MWERKS__ > 0x2300				
//	#pragma warn_no_side_effect on				// doesn't seem all that useful and Pro 6's MSL still chokes when it's on ($$$ enable in Pro 7? $$$)
//	#pragma warn_resultnotused on				// this one isn't practical (eg every use of a custom assignment operator is flagged as an error)
#endif


