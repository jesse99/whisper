/*
 *  File:       XDocView.cpp
 *  Summary:   	Interface that allows us to distinguish between doc windows and
 *				other windows in the document layer. 
 *  Written by: Jesse Jones
 *
 *  Copyright � 2000 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):		
 *
 *		$Log: XDocView.cpp,v $
 *		Revision 1.3  2000/12/14 08:34:48  jesjones
 *		More doxyfying.
 *		
 *		Revision 1.2  2000/11/09 12:32:55  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the $Log: XDocView.cpp,v $
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Revision 1.3  2000/12/14 08:34:48  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the More doxyfying.
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the keyword.
 *		
 *		 <1>	 9/03/00	JDJ		Created
 */

#include <XWhisperHeader.h>
#include <IDocView.h>

#include <IDocument.h>
#include <IHierarchy.h>

namespace Whisper {


// ===================================================================================
//	class XDocView
//!		Interface that allows us to distinguish between doc windows and other windows in the document layer. 
// ===================================================================================
class XDocView : public IDocView {

//-----------------------------------
//	Initialization/Destruction
//
public:
	virtual 			~XDocView();
	
  						XDocView(XBoss* boss);
  						
//-----------------------------------
//	Inherited API
//
public:
	virtual IDocumentPtr GetDoc() const;
};

DEFINE_INTERFACE_FACTORY(XDocView)

//---------------------------------------------------------------
//
// XDocView::~XDocView
//
//---------------------------------------------------------------
XDocView::~XDocView()
{
}


//---------------------------------------------------------------
//
// XDocView::XDocView
//
//---------------------------------------------------------------
XDocView::XDocView(XBoss* boss) 
{	
	this->DoSetBoss(boss);
}


//---------------------------------------------------------------
//
// XDocView::GetDoc
//
//---------------------------------------------------------------
IDocumentPtr XDocView::GetDoc() const
{
	IDocumentPtr doc = GetParent<IDocument>(this);	
	
	return doc;
}


}	// namespace Whisper
