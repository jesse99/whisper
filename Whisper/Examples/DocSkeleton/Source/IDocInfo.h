/*
 *  File:       IDocInfo.h
 *  Summary:   	This is the interface that maintains the app specific state of a document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: IDocInfo.h,v $
 *		Revision 1.1  2000/12/31 04:24:31  jesjones
 *		Checked in
 *		
 */

#pragma once

#include <XInterfacePtr.h>
#include <XRect.h>
#include <XRGBColor.h>
#include <XUnknown.h>

using namespace Whisper;
using namespace std;


// ===================================================================================
//	class IDocInfo
//!		This is the interface that maintains the app specific state of a document.
// ===================================================================================
class IDocInfo : public XUnknown {

//-----------------------------------
//	API
//
public:
	// ----- Pen Width -----
	virtual int32 		GetPenWidth() const = 0;
			
	virtual void 		SetPenWidth(int32 width, bool dirtyDoc = true) = 0;
						/**< If dirtyDoc is set an kChangedDocument message is sent to 
						all of the callbacks registered with IDocument. The IDocWindow
						implementations listen for this message and invalidate themselves
						if it's received. */
						
	// ----- Color -----
	virtual XRGBColor 	GetColor() const = 0;
			
	virtual void 		SetColor(const XRGBColor& color, bool dirtyDoc = true) = 0;
						
	// ----- Rectangles -----
	virtual uint32 		GetNumRects() const = 0;
	virtual XRect 		GetRect(uint32 index) const = 0;
						
	virtual void 		AddRect(const XRect& rect, bool dirtyDoc = true) = 0;
	virtual void 		RemoveRect(const XRect& rect, bool dirtyDoc = true) = 0;

	virtual void 		RemoveRects(bool dirtyDoc = true) = 0;
};

typedef XInterfacePtr<IDocInfo> IDocInfoPtr;
typedef XInterfacePtr<const IDocInfo> IConstDocInfoPtr;

