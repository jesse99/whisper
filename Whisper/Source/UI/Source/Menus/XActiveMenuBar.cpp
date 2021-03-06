/*
 *  File:       XActiveMenuBar.cpp
 *  Summary:   	Provides access to the current menubar.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XActiveMenuBar.cpp,v $
 *		Revision 1.3  2000/12/14 08:32:33  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:24:43  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XActiveMenuBar.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/14 08:32:33  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 4/25/00	JDJ		Created.
 */

#include <XWhisperHeader.h>
#include <IActiveMenuBar.h>

#include <IMenuBar.h>
#include <XInterfaceMacros.h>

namespace Whisper {


// ===================================================================================
//	class XActiveMenuBar
//!		Provides access to the current menubar.
// ===================================================================================
class XActiveMenuBar : public IActiveMenuBar {

	typedef IActiveMenuBar Inherited;

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XActiveMenuBar();
						
						XActiveMenuBar(XBoss* boss);
		
//-----------------------------------
//	Inherited API
//
public:					
	virtual IMenuBarPtr Get() const;
	
	virtual void 		Set(const IMenuBarPtr& menubar);
	
//-----------------------------------
//	Member Data
//
protected:
	IMenuBarPtr	mMenuBar;
};

DEFINE_INTERFACE_FACTORY(XActiveMenuBar)

//---------------------------------------------------------------
//
// XActiveMenuBar::~XActiveMenuBar
//
//---------------------------------------------------------------
XActiveMenuBar::~XActiveMenuBar()
{
}

					
//---------------------------------------------------------------
//
// XActiveMenuBar::XActiveMenuBar
//
//---------------------------------------------------------------
XActiveMenuBar::XActiveMenuBar(XBoss* boss)
{	
	this->DoSetBoss(boss);
}

				
//---------------------------------------------------------------
//
// XActiveMenuBar::Get
//
//---------------------------------------------------------------
IMenuBarPtr XActiveMenuBar::Get() const
{
	return mMenuBar;
}

	
//---------------------------------------------------------------
//
// XActiveMenuBar::Set
//
//---------------------------------------------------------------
void XActiveMenuBar::Set(const IMenuBarPtr& menubar)
{
#if MAC
	mMenuBar = menubar;

#elif WIN
	if (menubar != mMenuBar) {
		if (mMenuBar)
			mMenuBar->DestroyAcceleratorTable();
		
		mMenuBar = menubar;
	
		if (mMenuBar)
			mMenuBar->RebuildAcceleratorTable();
	}
#endif
}


}	// namespace Whisper
