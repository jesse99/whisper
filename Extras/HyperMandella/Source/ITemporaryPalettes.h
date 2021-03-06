/*
 *  File:       ITemporaryPalettes.h
 *  Summary:   	Interface used to bind palettes to the number keys.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: ITemporaryPalettes.h,v $
 *		Revision 1.1  2001/03/30 04:09:08  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XColorTable.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class ITemporaryPalettes
//!		Interface used to bind palettes to the number keys.
// ===================================================================================
class ITemporaryPalettes : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Getters -----
	virtual uint32 		GetCount() const = 0;
						/**< The number of temporary palettes in use. */
						// The number of temporary palettes in use.
	
	virtual XColorTable GetPalette(uint32 index) const = 0;
	
	virtual wstring 	GetName(uint32 index) const = 0;
	
	// ----- Modifiers -----
	virtual void 		Add(const XColorTable& palette, const wstring& name) = 0;
		
	virtual void 		Clear() = 0;
};

typedef XInterfacePtr<ITemporaryPalettes> ITemporaryPalettesPtr;
typedef XInterfacePtr<const ITemporaryPalettes> IConstTemporaryPalettesPtr;

