/*
 *  File:       CDocInfo.cpp
 *  Summary:   	This is the interface that maintains the app specific state of a document.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CDocInfo.cpp,v $
 *		Revision 1.2  2001/04/21 02:24:06  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.1  2000/12/31 04:23:19  jesjones
 *		Checked in
 */

#include "AppHeader.h"
#include "IDocInfo.h"

#include <IDocument.h>


// ===================================================================================
//	class CDocInfo
//!		This is the implementation that maintains the app specific state of a document.
// ===================================================================================
class CDocInfo : public IDocInfo {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~CDocInfo();
	
						CDocInfo(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual int32 		GetPenWidth() const						{return mPenWidth;}
	virtual void 		SetPenWidth(int32 width, bool dirtyDoc = false);
						
	virtual XRGBColor 	GetColor() const						{return mColor;}
	virtual void 		SetColor(const XRGBColor& color, bool dirtyDoc = false);
						
	virtual uint32 		GetNumRects() const						{return mRects.size();}
	virtual XRect 		GetRect(uint32 i) const					{PRECONDITION(i < mRects.size()); return mRects[i];}
						
	virtual void 		AddRect(const XRect& rect, bool dirtyDoc = false);
	virtual void 		RemoveRect(const XRect& rect, bool dirtyDoc = true);

	virtual void 		RemoveRects(bool dirtyDoc = false);
	
//-----------------------------------
//	Member Data
//
private:
	int32			mPenWidth;
	XRGBColor		mColor;
	vector<XRect>	mRects;
};

DEFINE_INTERFACE_FACTORY(CDocInfo)

//---------------------------------------------------------------
//
// CDocInfo::~CDocInfo
//
//---------------------------------------------------------------
CDocInfo::~CDocInfo()
{
}


//---------------------------------------------------------------
//
// CDocInfo::CDocInfo
//
//---------------------------------------------------------------
CDocInfo::CDocInfo(XBoss* boss)
{	
	IDocInfo::DoSetBoss(boss);
		
	mPenWidth = 1;
	mColor = kRGBLightBlue;
}


//---------------------------------------------------------------
//
// CDocInfo::SetPenWidth
//
// When the doc is dirtied it broadcasts so that controls can
// update themselves. dirtyDoc defaults to true which is usually
// what you want but there are times when you may want to disable
// this (eg broadcasting only at the end of a long sequence of changes).
//
//---------------------------------------------------------------
void CDocInfo::SetPenWidth(int32 width, bool dirtyDoc)
{
	if (width != mPenWidth) {
		mPenWidth = width;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}

						
//---------------------------------------------------------------
//
// CDocInfo::SetColor
//
//---------------------------------------------------------------
void CDocInfo::SetColor(const XRGBColor& color, bool dirtyDoc)
{
	if (color != mColor) {
		mColor = color;
		
		if (dirtyDoc) {
			IDocumentPtr doc(this);
			doc->SetDirty();
		}
	}
}

												
//---------------------------------------------------------------
//
// CDocInfo::AddRect
//
//---------------------------------------------------------------
void CDocInfo::AddRect(const XRect& rect, bool dirtyDoc)
{	
	mRects.push_back(rect);
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CDocInfo::RemoveRect
//
//---------------------------------------------------------------
void CDocInfo::RemoveRect(const XRect& rect, bool dirtyDoc)
{	
	vector<XRect>::iterator iter = std::find(mRects.begin(), mRects.end(), rect);
	PRECONDITION(iter != mRects.end());
	
	mRects.erase(iter);
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


//---------------------------------------------------------------
//
// CDocInfo::RemoveRects
//
//---------------------------------------------------------------
void CDocInfo::RemoveRects(bool dirtyDoc)
{
	mRects.clear();
		
	if (dirtyDoc) {
		IDocumentPtr doc(this);
		doc->SetDirty();
	}
}


