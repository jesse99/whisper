/*
 *  File:       IColorFormulas.h
 *  Summary:   	Interface used to manage saved color formulas (eg "sin(.5*x)").
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IColorFormulas.h,v $
 *		Revision 1.3  2001/04/03 04:37:38  jesjones
 *		Added Has and notification methods.
 *		
 *		Revision 1.2  2001/03/25 06:08:04  jesjones
 *		Added a comment.
 *		
 *		Revision 1.1  2000/12/28 02:51:05  jesjones
 *		Moved from the Whisper to the Extras module.
 *		
 *		Revision 1.2  2000/11/09 09:00:21  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#pragma once

#include <XCallbacks.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IColorFormulas
//!		Interface used to manage saved color formulas (eg "sin(.5*x)").
// ===================================================================================
class IColorFormulas : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Getters -----
	virtual uint32 		GetCount() const = 0;
	virtual std::wstring Get(uint32 index) const = 0;
	
	virtual std::wstring GetDefault() const = 0;

	virtual bool 		Has(const std::wstring& formula) const = 0;
	
	// ----- Modifiers -----
	virtual void 		Add(const std::wstring& formula) = 0;
		
	virtual void 		Delete(const std::wstring& formula) = 0;
						
	// ----- Callbacks -----
	virtual void 		SetAddCallback(XCallback1<void, std::wstring> callback) = 0;
						/**< Called after Add adds the palette. */

	virtual void 		SetDeleteCallback(XCallback1<void, std::wstring> callback) = 0;
						/**< Called before Delete deletes the palette. */
};

typedef XInterfacePtr<IColorFormulas> IColorFormulasPtr;
typedef XInterfacePtr<const IColorFormulas> IConstColorFormulasPtr;

