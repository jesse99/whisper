/*
 *  File:       XDocFocus.cpp
 *  Summary:   	The interface that handles focusing on documents.
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: XDocFocus.cpp,v $
 *		Revision 1.4  2001/01/30 07:17:46  jesjones
 *		Added HasFocus.
 *		
 *		Revision 1.3  2000/12/14 08:34:26  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:32:30  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <IFocus.h>

#include <ICurrentFocus.h>

namespace Whisper {


// ===================================================================================
//	class XDocFocus
//!		The interface that handles focusing on documents.
// ===================================================================================
class XDocFocus : public IFocus {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual				~XDocFocus();
	
						XDocFocus(XBoss* boss);
						
//-----------------------------------
//	Inherited API
//
public:
	virtual bool 		WantsFocus() const;
	virtual void		SetFocus();
	virtual bool 		HasFocus() const;
};

DEFINE_INTERFACE_FACTORY(XDocFocus)

//---------------------------------------------------------------
//
// XDocFocus::~XDocFocus
//
//---------------------------------------------------------------
XDocFocus::~XDocFocus()
{
}


//---------------------------------------------------------------
//
// XDocFocus::XDocFocus
//
//---------------------------------------------------------------
XDocFocus::XDocFocus(XBoss* boss)
{	
	IFocus::DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDocFocus::WantsFocus
//
//---------------------------------------------------------------
bool XDocFocus::WantsFocus() const
{
	return false;
}


//---------------------------------------------------------------
//
// XDocFocus::SetFocus
//
//---------------------------------------------------------------
void XDocFocus::SetFocus()
{
	DEBUGSTR("Can't focus on documents!");
}


//---------------------------------------------------------------
//
// XDocFocus::HasFocus
//
//---------------------------------------------------------------
bool XDocFocus::HasFocus() const
{
	ICurrentFocusPtr current(L"Application");
	bool has = current->GetFocus() == this;
	
	return has;
}


}	// namespace Whisper
