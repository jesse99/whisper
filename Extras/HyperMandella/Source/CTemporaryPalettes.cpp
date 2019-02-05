/*
 *  File:       CTemporaryPalettes.cpp
 *  Summary:   	Implementation used to bind palettes to the number keys.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CTemporaryPalettes.cpp,v $
 *		Revision 1.2  2001/04/21 02:25:17  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2001/03/30 04:09:02  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "ITemporaryPalettes.h"

#if MAC
	#include <Sound.h>
#endif


// ===================================================================================
//	class CTemporaryPalettes
//!		Implementation used to bind palettes to the number keys.
// ===================================================================================
class CTemporaryPalettes : public ITemporaryPalettes {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CTemporaryPalettes();
	
						CTemporaryPalettes(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual uint32 		GetCount() const;
	virtual XColorTable GetPalette(uint32 index) const;
	virtual wstring 	GetName(uint32 index) const;
	
	virtual void 		Add(const XColorTable& palette, const wstring& name);
	virtual void 		Clear();
	
//-----------------------------------
//	Internal API
//
private:
			void 		DoPlaySound();

//-----------------------------------
//	Member Data
//
private:
	uint32			mNext;			// next available index (or key - 1)
	XColorTable		mColors[9];
	wstring			mNames[9];
};

DEFINE_INTERFACE_FACTORY(CTemporaryPalettes)

//---------------------------------------------------------------
//
// CTemporaryPalettes::~CTemporaryPalettes
//
//---------------------------------------------------------------
CTemporaryPalettes::~CTemporaryPalettes()
{
}


//---------------------------------------------------------------
//
// CTemporaryPalettes::CTemporaryPalettes
//
//---------------------------------------------------------------
CTemporaryPalettes::CTemporaryPalettes(XBoss* boss) 
{	
	ITemporaryPalettes::DoSetBoss(boss);
	mNext = 0;
}


//---------------------------------------------------------------
//
// CTemporaryPalettes::GetCount
//
//---------------------------------------------------------------
uint32 CTemporaryPalettes::GetCount() const
{
	return mNext;
}


//---------------------------------------------------------------
//
// CTemporaryPalettes::GetPalette
//
//---------------------------------------------------------------
XColorTable CTemporaryPalettes::GetPalette(uint32 index) const
{
	PRECONDITION(index < mNext); 
	
	XColorTable colors = mColors[index];
		
	return colors;
}


//---------------------------------------------------------------
//
// CTemporaryPalettes::GetName
//
//---------------------------------------------------------------
wstring CTemporaryPalettes::GetName(uint32 index) const
{
	PRECONDITION(index < mNext); 
	
	return mNames[index];
}

	
//---------------------------------------------------------------
//
// CTemporaryPalettes::Add
//
//---------------------------------------------------------------
void CTemporaryPalettes::Add(const XColorTable& colors, const wstring& name)
{
	PRECONDITION(mNext < 9);
	
	mColors[mNext] = colors;
	mNames[mNext] = name;
	
	mNext++;
	this->DoPlaySound();
}


//---------------------------------------------------------------
//
// CTemporaryPalettes::Clear
//
//---------------------------------------------------------------
void CTemporaryPalettes::Clear()
{
	mNext = 0;
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CTemporaryPalettes::DoPlaySound
//
//---------------------------------------------------------------
void CTemporaryPalettes::DoPlaySound()
{
#if MAC
	Handle sound = GetResource('snd ', 256);
	if (sound != nil)
		(void) SndPlay(nil, (SndListHandle) sound, true);
#endif
}
