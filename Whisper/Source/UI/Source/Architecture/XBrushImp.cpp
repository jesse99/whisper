/*
 *	File:		XBrushImp.h
 *	Function:	Default IBrush implementation.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XBrushImp.cpp,v $
 *		Revision 1.1  2001/02/07 05:01:37  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IBrush.h>

namespace Whisper {


// ===================================================================================
//	class XBrushImp
//!		Default IBrush implementation.
// ===================================================================================
class XBrushImp : public IBrush {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XBrushImp();
	
						XBrushImp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XBrush 		GetEnabledBrush() const					{return mEnabledBrush;}
	virtual void 		SetEnabledBrush(const XBrush& pen)		{mEnabledBrush = pen;}

	virtual XBrush 		GetDisabledBrush() const				{return mDisabledBrush;}
	virtual void 		SetDisabledBrush(const XBrush& pen)		{mDisabledBrush = pen;}
	
//-----------------------------------
//	Member Data
//
private:
	XBrush	mEnabledBrush;
	XBrush	mDisabledBrush;
};

DEFINE_INTERFACE_FACTORY(XBrushImp)

//---------------------------------------------------------------
//
// XBrushImp::~XBrushImp
//
//---------------------------------------------------------------
XBrushImp::~XBrushImp()
{
}


//---------------------------------------------------------------
//
// XBrushImp::XBrushImp
//
//---------------------------------------------------------------
XBrushImp::XBrushImp(XBoss* boss) 
{
	IBrush::DoSetBoss(boss);

	mEnabledBrush = kWhiteBrush;
	mDisabledBrush = kWhiteBrush;
}


}	// namespace Whisper

