/*
 *  File:       XAppFocus.cpp
 *  Summary:   	The interface that handles focusing on the app.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XAppFocus.cpp,v $
 *		Revision 1.3  2001/01/30 07:17:19  jesjones
 *		Added HasFocus.
 *		
 *		Revision 1.2  2000/11/09 12:25:26  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 *		
 *		 <1>	 8/31/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IFocus.h>

#include <ICurrentFocus.h>

namespace Whisper {


// ===================================================================================
//	class XAppFocus
// ===================================================================================
class XAppFocus : public IFocus {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XAppFocus();
	
						XAppFocus(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		WantsFocus() const;
	virtual void		SetFocus();
	virtual bool 		HasFocus() const;
};

DEFINE_INTERFACE_FACTORY(XAppFocus)

//---------------------------------------------------------------
//
// XAppFocus::~XAppFocus
//
//---------------------------------------------------------------
XAppFocus::~XAppFocus()
{
}


//---------------------------------------------------------------
//
// XAppFocus::XAppFocus
//
//---------------------------------------------------------------
XAppFocus::XAppFocus(XBoss* boss)
{	
	IFocus::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XAppFocus::WantsFocus
//
//---------------------------------------------------------------
bool XAppFocus::WantsFocus() const
{
#if MAC
	return true;
#elif WIN
	return false;
#endif
}


//---------------------------------------------------------------
//
// XAppFocus::SetFocus
//
//---------------------------------------------------------------
void XAppFocus::SetFocus()
{
#if WIN
	DEBUGSTR("Can only focus on windows!");
#endif
}


//---------------------------------------------------------------
//
// XAppFocus::HasFocus
//
//---------------------------------------------------------------
bool XAppFocus::HasFocus() const
{
	ICurrentFocusPtr current(L"Application");
	bool has = current->GetFocus() == this;
	
	return has;
}


}	// namespace Whisper
