/*
 *	File:		XDrawExtension.cpp
 *	Function:	Default IDrawExtension implementation.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XDrawExtension.cpp,v $
 *		Revision 1.4  2001/04/27 04:32:28  jesjones
 *		Updated for the XTranscode changes and/or replaced ASSERTs with PRECONDITION.
 *		
 *		Revision 1.3  2001/02/07 04:58:03  jesjones
 *		Last bit of the war on implementation inheritance.
 *		
 *		Revision 1.2  2000/11/09 12:33:51  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IDrawExtension.h>

#include <IDrawExtensionHelper.h>

namespace Whisper {


// ===================================================================================
//	class XDrawExtension
//!		Default IDrawExtension implementation.
// ===================================================================================
class XDrawExtension : public IDrawExtension {
			
//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual			   ~XDrawExtension();
	
						XDrawExtension(XBoss* boss);
						
//-----------------------------------
//	Inherited API 
//
public:
	virtual std::wstring GetName() const							{return mName;}
	virtual void 		SetName(const std::wstring& name)			{PRECONDITION(name.length() > 0); mName = name;}
									
	virtual	XRect 		GetIndent() const							{return mIndent;}
	virtual	void 		SetIndent(const XRect& indent);

	virtual bool 		DrawsBeforeOwner() const					{return mDrawBeforeOwner;}
	virtual void 		SetDrawBeforeOwner(bool drawBefore = true)	{mDrawBeforeOwner = drawBefore;}

	virtual	bool		Draw(XDrawContext& canvas, const XRect& ownerExtent, bool enabled);
	
//-----------------------------------
//	Member Data
//
private:
	std::wstring	mName;
	XRect			mIndent;			
	bool			mDrawBeforeOwner;
};

DEFINE_INTERFACE_FACTORY(XDrawExtension)

//---------------------------------------------------------------
//
// XDrawExtension::~XDrawExtension
//
//---------------------------------------------------------------
XDrawExtension::~XDrawExtension()
{
}


//---------------------------------------------------------------
//
// XDrawExtension::XDrawExtension
//
//---------------------------------------------------------------
XDrawExtension::XDrawExtension(XBoss* boss) 
{
	IDrawExtension::DoSetBoss(boss);

	mIndent = kZeroRect;
	mDrawBeforeOwner = false;
}


//---------------------------------------------------------------
//
// XDrawExtension::SetIndent
//
//---------------------------------------------------------------
void XDrawExtension::SetIndent(const XRect& indent)				
{
	PRECONDITION(indent.left >= 0);
	PRECONDITION(indent.right >= 0);
	PRECONDITION(indent.top >= 0);
	PRECONDITION(indent.bottom >= 0);
	
	mIndent = indent;
}


//---------------------------------------------------------------
//
// XDrawExtension::Draw
//
//---------------------------------------------------------------
bool XDrawExtension::Draw(XDrawContext& canvas, const XRect& ownerExtent, bool enabled)
{	
	XRect extent = ownerExtent;
	
	extent.left   += mIndent.left;
	extent.right  += mIndent.right;
	extent.top    += mIndent.top;
	extent.bottom += mIndent.bottom;
	
	if (extent.IsEmpty())
		extent = kZeroRect;
		
	IDrawExtensionHelperPtr helper(this);
	helper->OnDraw(canvas, extent, enabled);
	
	return true;			// $$ get rid of this?
}


}	// namespace Whisper

