/*
 *  File:       XColorTable.h
 *  Summary:	Encapsulates an OS specific array of colors.
 *  Written by: Jesse Jones
 *
 *  Copyright © 1997-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):
 *
 *		$Log: XColorTable.h,v $
 *		Revision 1.5  2001/04/05 04:02:30  jesjones
 *		Next seed is no longer static member data (due to order of initialization problems).
 *		
 *		Revision 1.4  2001/03/05 05:40:45  jesjones
 *		Removed the dollar signs around the Log comment.
 *		
 *		Revision 1.3  2000/12/10 08:50:46  jesjones
 *		Converted header comments to Doxygen format.
 *		
 *		Revision 1.2  2000/11/09 12:28:12  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <4>	 10/07/00	JDJ		Added a ctor that takes an XXMLElement and a CreateElement method. 
 *		 <3>	  5/27/99	JDJ		msNextSeed is now an XAtomicCounter.
 *		 <2>	  1/06/99	JDJ		Can now create an XColorTable from an HPALETTE.
 *		 <1>	 11/27/97	JDJ		Created (from Raven)
 */

#pragma once

#if MAC
	#include <Palettes.h>
	#include <Resources.h>
#endif

#include <XAtomicCounter.h>
#include <XRGBColor.h>

namespace Whisper {

#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export on
#endif


//-----------------------------------
//	Forward References
//
class XXMLElement;


// ===================================================================================
//	class XColorTable
//!		Encapsulates an OS specific array of colors.
// ===================================================================================
class GRAPHICS_EXPORT XColorTable {

//-----------------------------------
//	Initialization/Destruction
//
public:
						~XColorTable();	
	
						XColorTable();	
						
						XColorTable(const OSColor* colors, uint32 numColors);
						/**< numColors has to be 2, 4, 16, or 256 */

	explicit 			XColorTable(ResID rsrcID);
						// Loads a 'clut' resource (this is a custom resource in Windows).
	
						XColorTable(OSPalette palette);	
						
#if MAC
						XColorTable(CTabHandle colorsH);			
#endif

						XColorTable(const XXMLElement& element, const wchar_t* pName = L"Palette", const wchar_t* cName = L"Color");	

						XColorTable(const XColorTable& rhs);	
							
			XColorTable& operator=(const XColorTable& rhs);
			
//-----------------------------------
//	API
//
public:
	//! @name Conversion Operators
	//@{
#if MAC
						operator CTabHandle() const;
	
			PaletteHandle CreatePalette(int16 usage = pmTolerant, int16 tolerance = 0) const;
			
#elif WIN
						operator RGBQUAD*()							{return mColors;}

						operator const RGBQUAD*() const				{return mColors;}
#endif
	//@}

	//! @name Size
	//@{
			uint32 		GetSize() const								{return mSize;}
			
			void 		SetSize(uint32 newSize);
	//@}
				
	//! @name Colors
	//@{
			XRGBColor 	GetColor(uint32 index) const;
						/**< Note that on a Mac the first color is white and the last
						is black. On Windows the first color is black and the
						last is white. */

			void 		SetColor(uint32 index, const XRGBColor& color);
	//@}
				
	//! @name Comparison Operators
	//@{
			bool 		operator==(const XColorTable& rhs) const	{return this->DoCompare(rhs) == 0;}
			bool 		operator!=(const XColorTable& rhs) const	{return this->DoCompare(rhs) != 0;}

			bool 		operator<(const XColorTable& rhs) const		{return this->DoCompare(rhs) < 0;}
			bool 		operator>(const XColorTable& rhs) const		{return this->DoCompare(rhs) > 0;}

			bool 		operator<=(const XColorTable& rhs) const	{return this->DoCompare(rhs) <= 0;}
			bool 		operator>=(const XColorTable& rhs) const	{return this->DoCompare(rhs) >= 0;}
	//@}

	//! @name Misc
	//@{
			XXMLElement* CreateElement(const wchar_t* pName = L"Palette", const wchar_t* cName = L"Color") const;
	//@}
		
//-----------------------------------
//	Internal API
//
public:
#if MAC
			long 		GetSeed() const								{return (**mColors).ctSeed;}
#endif

protected:
			int 		DoCompare(const XColorTable& rhs) const;
						// Returns -1 if lhs < rhs, 0 if lhs == rhs, and
						// +1 if lhs > rhs.
	
//-----------------------------------
//	Member data
//
private:
	OSColorTable	mColors;
	uint32			mSize;
	mutable bool	mDirty;			// if true seed needs to be updated
	
#if WIN
	mutable int32	mSeed;			// used to shortcut comparisons
#endif
};


#if MULTI_FRAGMENT_APP && PRAGMA_EXPORT_SUPPORTED
	#pragma export reset
#endif

}	// namespace Whisper

