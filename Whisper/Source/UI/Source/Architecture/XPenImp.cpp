/*
 *	File:		XPenImp.cpp
 *	Function:	Default IPen implementation.
 *	Written by:	Jesse Jones
 *
 *  Copyright © 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *	Change History (most recent first):
 *
 *		$Log: XPenImp.cpp,v $
 *		Revision 1.1  2001/02/07 05:01:40  jesjones
 *		Checked in
 *		
 */

#include <XWhisperHeader.h>
#include <IPen.h>

namespace Whisper {


// ===================================================================================
//	class XPenImp
//!		Default IPen implementation.
// ===================================================================================
class XPenImp : public IPen {	

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XPenImp();
	
						XPenImp(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual XPen 		GetEnabledPen() const				{return mEnabledPen;}
	virtual void 		SetEnabledPen(const XPen& pen)		{mEnabledPen = pen;}

	virtual XPen 		GetDisabledPen() const				{return mDisabledPen;}
	virtual void 		SetDisabledPen(const XPen& pen)		{mDisabledPen = pen;}
	
//-----------------------------------
//	Member Data
//
private:
	XPen	mEnabledPen;
	XPen	mDisabledPen;
};

DEFINE_INTERFACE_FACTORY(XPenImp)

//---------------------------------------------------------------
//
// XPenImp::~XPenImp
//
//---------------------------------------------------------------
XPenImp::~XPenImp()
{
}


//---------------------------------------------------------------
//
// XPenImp::XPenImp
//
//---------------------------------------------------------------
XPenImp::XPenImp(XBoss* boss) 
{
	IPen::DoSetBoss(boss);

	mEnabledPen  = kBlackPen;
	mDisabledPen = kGrayPen;
}


}	// namespace Whisper

