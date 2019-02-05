/*
 *  File:       IPalettes.h
 *  Summary:   	Interface used to manage saved palettes.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IPalettes.h,v $
 *		Revision 1.4  2001/04/03 04:38:20  jesjones
 *		Deleted obsolete notification types.
 *		
 *		Revision 1.3  2001/04/01 04:53:20  jesjones
 *		Uses callbacks to notify on add palette and delete palette.
 *		
 *		Revision 1.2  2001/03/25 06:08:26  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:26  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:05:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XColorTable.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IPalettes
//!		Interface used to manage saved palettes.
// ===================================================================================
class IPalettes : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Getters -----
	virtual uint32 		GetCount() const = 0;
	virtual XColorTable Get(uint32 index) const = 0;
	virtual std::wstring GetName(uint32 index) const = 0;

	virtual bool 		Has(const std::wstring& name) const = 0;
	virtual XColorTable Get(const std::wstring& name) const = 0;
	
	virtual XColorTable GetDefault() const = 0;
	virtual std::wstring GetDefaultName() const = 0;
	
	// ----- Modifiers -----
	virtual void 		Add(const std::wstring& name, const XColorTable& palette) = 0;
		
	virtual void 		Delete(const std::wstring& name) = 0;
		
	// ----- Misc -----
	virtual void 		MungeColorTable(XColorTable& palette) const = 0;
						/**< Removes white and black from the color table. Note that the 
						above routines call this automatically. */
						
	virtual void 		SetAddCallback(XCallback1<void, std::wstring> callback) = 0;
						/**< Called after Add adds the palette. */

	virtual void 		SetDeleteCallback(XCallback1<void, std::wstring> callback) = 0;
						/**< Called before Delete deletes the palette. */
};

typedef XInterfacePtr<IPalettes> IPalettesPtr;
typedef XInterfacePtr<const IPalettes> IConstPalettesPtr;

