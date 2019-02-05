/*
 *  File:       IFormulaState.h
 *  Summary:   	Interface used to swap around the fractal state when a new fractal formula is selected.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IFormulaState.h,v $
 *		Revision 1.1  2001/03/28 09:30:53  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XColorTable.h>
#include <XInterfacePtr.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


//-----------------------------------
//	Forward References
//
class IDocInfo;
typedef XInterfacePtr<IDocInfo> IDocInfoPtr;
typedef XInterfacePtr<const IDocInfo> IConstDocInfoPtr;


// ===================================================================================
//	class IFormulaState
//!		Interface used to swap around the fractal state when a new fractal formula is selected.
// ===================================================================================
class IFormulaState : public XUnknown {

//-----------------------------------
//	API
//
public:
	virtual void 		SetState(IDocInfoPtr& doc, const std::wstring& formula, bool dirtyDoc = false) = 0;
						/**< Updates the document based on the specified formula. */

	virtual void 		ChangeState(IDocInfoPtr& doc, const std::wstring& formula) = 0;
						/**< Like SetState except that an undoable command is used. */

	virtual void 		MakeDefault(const IConstDocInfoPtr& doc) = 0;
						/**< Updates the saved state for the doc's fractal formula
						using the doc's current state. */
};

typedef XInterfacePtr<IFormulaState> IFormulaStatePtr;
typedef XInterfacePtr<const IFormulaState> IConstFormulaStatePtr;

