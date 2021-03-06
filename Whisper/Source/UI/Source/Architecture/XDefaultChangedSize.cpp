/*
 *  File:       XDefaultChangedSize.cpp
 *  Summary:   	IParentChangedSize implementation that uses an IBinding.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDefaultChangedSize.cpp,v $
 *		Revision 1.3  2000/12/14 08:34:06  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:31:02  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XDefaultChangedSize.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/14 08:34:06  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 7/06/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IBinding.h>
#include <IParentChangedSize.h>

#include <IGeometry.h>
#include <XNumbers.h>

namespace Whisper {


// ===================================================================================
//	class XDefaultChangedSize
//!		IParentChangedSize implementation that uses an IBinding.
// ===================================================================================
class XDefaultChangedSize : public IParentChangedSize, public IBinding {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDefaultChangedSize();
	
						XDefaultChangedSize(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual void 		HandleParentSizeChange(const XSize& delta, bool redraw = kRedraw);

	virtual SBinding 	GetBinding() const;

	virtual void 		SetBinding(const SBinding& binding);

//-----------------------------------
//	Member Data
//
private:
	SBinding	mBinding;
};

DEFINE_INTERFACE_FACTORY(XDefaultChangedSize)

//---------------------------------------------------------------
//
// XDefaultChangedSize::~XDefaultChangedSize
//
//---------------------------------------------------------------
XDefaultChangedSize::~XDefaultChangedSize()
{
}


//---------------------------------------------------------------
//
// XDefaultChangedSize::XDefaultChangedSize
//
//---------------------------------------------------------------
XDefaultChangedSize::XDefaultChangedSize(XBoss* boss) 
{	
	IBinding::DoSetBoss(boss);
	IParentChangedSize::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDefaultChangedSize::HandleParentSizeChange
//
//---------------------------------------------------------------
void XDefaultChangedSize::HandleParentSizeChange(const XSize& delta, bool redraw)
{
	XSize locDelta  = kZeroSize;
	XSize sizeDelta = kZeroSize;
	
	IParentChangedSize* thisPtr = this;
	IGeometryPtr geometry(thisPtr);

	if (mBinding.right) 
		if (mBinding.left)
			sizeDelta.width = delta.width;
		else
			locDelta.width = delta.width;
	
	if (mBinding.bottom)
		if (mBinding.top)
			sizeDelta.height = delta.height;
		else
			locDelta.height = delta.height;
	
	if (sizeDelta != kZeroSize) {
		XSize oldSize = geometry->GetSize();

		XSize newSize;
		newSize.width  = Max(oldSize.width + sizeDelta.width, 1L);
		newSize.height = Max(oldSize.height + sizeDelta.height, 1L);
		
		if (newSize != oldSize)
			geometry->SetSize(newSize, redraw);
	}
	
	if (locDelta != kZeroSize)
		geometry->SetLocation(geometry->GetLocation() + locDelta, redraw);
}


//---------------------------------------------------------------
//
// XDefaultChangedSize::GetBinding
//
//---------------------------------------------------------------
SBinding XDefaultChangedSize::GetBinding() const
{
	return mBinding;
}


//---------------------------------------------------------------
//
// XDefaultChangedSize::SetBinding
//
//---------------------------------------------------------------
void XDefaultChangedSize::SetBinding(const SBinding& binding)
{
	mBinding = binding;
}


}	// namespace Whisper
